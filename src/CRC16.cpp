#include "CRC16.h"

/*Reflect Calculations*/
uint8_t reflectLookupTable[] = {
  0,128,64,192,32,160,96,224,16,144,80,208,48,176,112,240,
  8,136,72,200,40,168,104,232,24,152,88,216,56,184,120,248,
  4,132,68,196,36,164,100,228,20,148,84,212,52,180,116,244,
  12,140,76,204,44,172,108,236,28,156,92,220,60,188,124,252,
  2,130,66,194,34,162,98,226,18,146,82,210,50,178,114,242,
  10,138,74,202,42,170,106,234,26,154,90,218,58,186,122,250,
  6,134,70,198,38,166,102,230,22,150,86,214,54,182,118,246,
  14,142,78,206,46,174,110,238,30,158,94,222,62,190,126,254,
  1,129,65,193,33,161,97,225,17,145,81,209,49,177,113,241,
  9,137,73,201,41,169,105,233,25,153,89,217,57,185,121,249,
  5,133,69,197,37,165,101,229,21,149,85,213,53,181,117,245,
  13,141,77,205,45,173,109,237,29,157,93,221,61,189,125,253,
  3,131,67,195,35,163,99,227,19,147,83,211,51,179,115,243,
  11,139,75,203,43,171,107,235,27,155,91,219,59,187,123,251,
  7,135,71,199,39,167,103,231,23,151,87,215,55,183,119,
  247,15,143,79,207,47,175,111,239,31,159,95,223,63,191,127,255,
};

uint8_t CRC16::reflectRaw(uint8_t data){
  uint8_t reflection = 0x00;
  for (int8_t bit = 7; bit >= 0; bit--){
      reflection |= (data & 0x01) << bit;
      data >>= 1;
  }
  return reflection;
}

uint16_t CRC16::reflectRaw(uint16_t data){
  uint16_t reflection = 0x0000;
  for (int8_t bit = 15; bit >= 0; bit--){
    reflection |= (data & 0x01) << bit;
    data >>= 1;
  }
  return reflection;
}

uint8_t CRC16::reflect(uint8_t data){
  return reflectLookupTable[data];
}

typedef struct uint16_fast {
  uint8_t low;
  uint8_t high;
}uint16_f;

uint16_t CRC16::reflect(uint16_t data){
  return reflectLookupTable[((uint16_f*)&data)->high] | (reflectLookupTable[((uint16_f*)&data)->low] << 8);
}

/*CRC Calculations*/
uint16_t CRC16::calc(uint8_t data) {
  uint16_t tCRC = data << 8;
  for (int i = 0; i < 8; i++) 
      tCRC = (tCRC << 1) ^ ((tCRC & 0x8000) ? polynomial : 0);
  return tCRC;
}

void CRC16::updateRaw(uint8_t data) {
  crc ^= (reflectIn ? reflect(data) : data) << 8;
  for (int i = 0; i < 8; i++)
      crc = (crc << 1) ^ ((crc & 0x8000) ? polynomial : 0);
}

void CRC16::updateRaw(uint8_t *data, uint32_t len){
  for(uint32_t i=0; i<len; i++)
    updateRaw(data[i]);
}

void CRC16::update(uint8_t data) {
  data = reflectIn ? reflect(data) : data;
  crc = (crc << 8) ^ crcTable[(crc >> 8) ^ data];
}

void CRC16::update(uint8_t *data, uint32_t len){
  for(uint32_t i=0; i<len; i++)
    update(data[i]);
}

/*CRC Operations*/
CRC16::CRC16(CRC16Model cfg){
  config = cfg;
  crc = xorIn;
  genCRCTable();
}

CRC16::CRC16(uint8_t _reflectIn, uint8_t _reflectOut, uint16_t _polynomial, uint16_t _xorIn, uint16_t _xorOut){
  reflectIn = _reflectIn;
  reflectOut = _reflectOut;
  polynomial = _polynomial;
  xorIn = _xorIn;
  xorOut = _xorOut;
  crc = xorIn;
  genCRCTable();
}

void CRC16::setModel(CRC16Model cfg){
  config = cfg;
  genCRCTable();
}

void CRC16::clear(){
  crc = xorIn;
}

uint16_t CRC16::get(){
    return (reflectOut?reflect(crc):crc)^xorOut;
}

void CRC16::genCRCTable(){
  for(uint16_t i=0; i<=255; i++)
    crcTable[i] = calc(i);
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