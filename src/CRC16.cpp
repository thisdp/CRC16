#include "CRC16.h"

typedef struct uint16_fast {
  uint8_t low;
  uint8_t high;
}uint16_f;

uint16_t *CRC16ReflectCache = 0;
/*Reflect Cache Calculations*/
void CRC16GenerateReflectCache(){
  if(!CRC16ReflectCache){
    CRC16ReflectCache = new uint16_t[256];
    for (int i = 0; i < 256; i++) {
      CRC16ReflectCache[i] = (i * 0x0202020202ULL & 0x010884422010ULL) % 1023;
    }
  }
}

uint8_t CRC16::reflectFast(uint8_t data){
  return CRC16ReflectCache[data];
}

uint16_t CRC16::reflectFast(uint16_t data){
  return CRC16ReflectCache[((uint16_f*)&data)->high] | (CRC16ReflectCache[((uint16_f*)&data)->low] << 8);
}

uint8_t CRC16::reflectRaw(uint8_t data){
  return (data * 0x0202020202ULL & 0x010884422010ULL) % 1023;
}

uint16_t CRC16::reflectRaw(uint16_t data){
  return ((((uint16_f*)&data)->high * 0x0202020202ULL & 0x010884422010ULL) % 1023)|(((((uint16_f*)&data)->low * 0x0202020202ULL & 0x010884422010ULL) % 1023)<<8);
}

/*CRC Calculations*/
uint16_t CRC16::calc(uint8_t data) {
  uint16_t tCRC = data << 8;
  for (int i = 0; i < 8; i++) 
      tCRC = (tCRC << 1) ^ ((tCRC & 0x8000) ? polynomial : 0);
  return tCRC;
}

void CRC16::updateFast(uint8_t data) {
  data = reflectIn ? reflect(data) : data;
  crc = (crc << 8) ^ CRC16CRCCache[(crc >> 8) ^ data];
}

void CRC16::updateFast(uint8_t *data, uint32_t len){
  for(uint32_t i=0; i<len; i++) updateFast(data[i]);
}

void CRC16::updateRaw(uint8_t data) {
  crc ^= (reflectIn ? reflect(data) : data) << 8;
  for (int i = 0; i < 8; i++)
    crc = (crc << 1) ^ ((crc & 0x8000) ? polynomial : 0);
}

void CRC16::updateRaw(uint8_t *data, uint32_t len){
  for(uint32_t i=0; i<len; i++) updateRaw(data[i]);
}

/*CRC Operations*/
CRC16::~CRC16(){
  if(CRC16CRCCache) delete CRC16CRCCache;
}

CRC16::CRC16(CRC16Model cfg, bool useCache){
  config = cfg;
  crc = xorIn;
  CRC16CRCCache = 0;
  CRCUseCache = useCache;
  if(useCache){
    CRC16GenerateReflectCache();
    CRC16CRCCache = new uint16_t[256];
    for(uint16_t i=0; i<=255; i++){
      CRC16CRCCache[i] = calc(i);
    }
  }
}

CRC16::CRC16(uint8_t _reflectIn, uint8_t _reflectOut, uint16_t _polynomial, uint16_t _xorIn, uint16_t _xorOut, bool useCache){
  reflectIn = _reflectIn;
  reflectOut = _reflectOut;
  polynomial = _polynomial;
  xorIn = _xorIn;
  xorOut = _xorOut;
  crc = xorIn;
  CRCUseCache = useCache;
  if(useCache){
    CRC16GenerateReflectCache();
    CRC16CRCCache = new uint16_t[256];
    for(uint16_t i=0; i<=255; i++) CRC16CRCCache[i] = calc(i);
  }
}

void CRC16::setModel(CRC16Model cfg){
  config = cfg;
  if(CRCUseCache){
    for(uint16_t i=0; i<=255; i++) CRC16CRCCache[i] = calc(i);
  }
}

void CRC16::clear(){
  crc = xorIn;
}

uint16_t CRC16::get(){
  return (reflectOut?reflect(crc):crc)^xorOut;
}

void CRC16::update(uint8_t data){
  if(CRCUseCache){
    updateFast(data);
  }else{
    updateRaw(data);
  }
}

void CRC16::update(uint8_t *data, uint32_t length){
  if(CRCUseCache){
    updateFast(data, length);
  }else{
    updateRaw(data, length);
  }
}

uint8_t CRC16::reflect(uint8_t data){
  return CRCUseCache?reflectFast(data):reflectRaw(data);
}

uint16_t CRC16::reflect(uint16_t data){
  return CRCUseCache?reflectFast(data):reflectRaw(data);
}

/*CRC16 Models*/
CRC16Model CRC16IBM = {
  .xorIn = 0x0000,
  .xorOut = 0x0000,
  .polynomial = 0x8005,
  .reflectIn = true,
  .reflectOut = true,
};

CRC16Model CRC16MAXIM = {
  .xorIn = 0x0000,
  .xorOut = 0xFFFF,
  .polynomial = 0x8005,
  .reflectIn = true,
  .reflectOut = true,
};

CRC16Model CRC16USB = {
  .xorIn = 0xFFFF,
  .xorOut = 0xFFFF,
  .polynomial = 0x8005,
  .reflectIn = true,
  .reflectOut = true,
};

CRC16Model CRC16MODBUS = {
  .xorIn = 0xFFFF,
  .xorOut = 0x0000,
  .polynomial = 0x8005,
  .reflectIn = true,
  .reflectOut = true,
};

CRC16Model CRC16CCITT = {
  .xorIn = 0x0000,
  .xorOut = 0x0000,
  .polynomial = 0x1021,
  .reflectIn = true,
  .reflectOut = true,
};

CRC16Model CRC16CCITT_False = {
  .xorIn = 0xFFFF,
  .xorOut = 0x0000,
  .polynomial = 0x1021,
  .reflectIn = false,
  .reflectOut = false,
};

CRC16Model CRC16X25 = {
  .xorIn = 0xFFFF,
  .xorOut = 0xFFFF,
  .polynomial = 0x1021,
  .reflectIn = true,
  .reflectOut = true,
};

CRC16Model CRC16XMODEM = {
  .xorIn = 0x0000,
  .xorOut = 0x0000,
  .polynomial = 0x1021,
  .reflectIn = false,
  .reflectOut = false,
};

CRC16Model CRC16DNP = {
  .xorIn = 0x0000,
  .xorOut = 0xFFFF,
  .polynomial = 0x3D65,
  .reflectIn = true,
  .reflectOut = true,
};