#pragma once
#include "Arduino.h"

class CRC16Model{
public:
  uint16_t xorIn;
  uint16_t xorOut;
  uint16_t polynomial;
  uint8_t reflectIn;
  uint8_t reflectOut;
};

extern CRC16Model CRC16IBM;
extern CRC16Model CRC16MAXIM;
extern CRC16Model CRC16USB;
extern CRC16Model CRC16MODBUS;
extern CRC16Model CRC16CCITT;
extern CRC16Model CRC16CCITT_False;
extern CRC16Model CRC16X25;
extern CRC16Model CRC16XMODEM;
extern CRC16Model CRC16DNP;

extern uint16_t *CRC16ReflectCache;

class CRC16{
public:
  CRC16(CRC16Model cfg, bool useCache = true);
  ~CRC16();
  CRC16(uint8_t reflectIn, uint8_t reflectOut, uint16_t polynomial, uint16_t xorIn, uint16_t xorOut, bool useCache = true);
  void setModel(CRC16Model cfg);
  void updateRaw(uint8_t data);
  void updateRaw(uint8_t *data, uint32_t len);
  void update(uint8_t data);
  void update(uint8_t *data, uint32_t len);
  uint8_t reflectRaw(uint8_t data);
  uint16_t reflectRaw(uint16_t data);
  uint8_t reflect(uint8_t data);
  uint16_t reflect(uint16_t data);
  uint16_t get();
  void clear();
private:
  uint16_t *CRC16CRCCache;
  bool CRCUseCache;
  void updateFast(uint8_t data);
  void updateFast(uint8_t *data, uint32_t len);
  uint8_t reflectFast(uint8_t data);
  uint16_t reflectFast(uint16_t data);
  uint16_t calc(uint8_t data);
  union{
    CRC16Model config;
    struct{
      uint16_t xorIn;
      uint16_t xorOut;
      uint16_t polynomial;
      uint8_t reflectIn;
      uint8_t reflectOut;
    };
  };
  uint16_t crc;
};
