/*
 * nano.h
 *
 *  Created on: 03.04.2018
 *      Author: Dominic
 */

#ifndef SOURCES_NANO_H_
#define SOURCES_NANO_H_

#include "CLS1.h"

#define MAX_MSG_SIZE 255

#define TMR_SR_OPCODE_VERSION 0x03
#define TMR_SR_OPCODE_SET_BAUD_RATE 0x06
#define TMR_SR_OPCODE_READ_TAG_ID_SINGLE 0x21
#define TMR_SR_OPCODE_READ_TAG_ID_MULTIPLE 0x22
#define TMR_SR_OPCODE_WRITE_TAG_ID 0x23
#define TMR_SR_OPCODE_WRITE_TAG_DATA 0x24
#define TMR_SR_OPCODE_KILL_TAG 0x26
#define TMR_SR_OPCODE_READ_TAG_DATA 0x28
#define TMR_SR_OPCODE_CLEAR_TAG_ID_BUFFER 0x2A
#define TMR_SR_OPCODE_MULTI_PROTOCOL_TAG_OP 0x2F
#define TMR_SR_OPCODE_GET_READ_TX_POWER 0x62
#define TMR_SR_OPCODE_GET_WRITE_TX_POWER 0x64
#define TMR_SR_OPCODE_GET_POWER_MODE 0x68
#define TMR_SR_OPCODE_GET_READER_OPTIONAL_PARAMS 0x6A
#define TMR_SR_OPCODE_GET_PROTOCOL_PARAM 0x6B
#define TMR_SR_OPCODE_SET_ANTENNA_PORT 0x91
#define TMR_SR_OPCODE_SET_TAG_PROTOCOL 0x93
#define TMR_SR_OPCODE_SET_READ_TX_POWER 0x92
#define TMR_SR_OPCODE_SET_WRITE_TX_POWER 0x94
#define TMR_SR_OPCODE_SET_REGION 0x97
#define TMR_SR_OPCODE_SET_READER_OPTIONAL_PARAMS 0x9A
#define TMR_SR_OPCODE_SET_PROTOCOL_PARAM 0x9B

#define COMMAND_TIME_OUT  2000 //Number of ms before stop waiting for response from module

//Define all the ways functions can return
#define ALL_GOOD                        0
#define ERROR_COMMAND_RESPONSE_TIMEOUT  1
#define ERROR_CORRUPT_RESPONSE          2
#define ERROR_WRONG_OPCODE_RESPONSE     3
#define ERROR_UNKNOWN_OPCODE            4
#define RESPONSE_IS_TEMPERATURE         5
#define RESPONSE_IS_KEEPALIVE           6
#define RESPONSE_IS_TEMPTHROTTLE        7
#define RESPONSE_IS_TAGFOUND            8
#define RESPONSE_IS_NOTAGFOUND          9
#define RESPONSE_IS_UNKNOWN             10
#define RESPONSE_SUCCESS  			    11
#define RESPONSE_FAIL          			12

//Define the allowed regions - these set the internal freq of the module
#define REGION_INDIA        0x04
#define REGION_JAPAN        0x05
#define REGION_CHINA        0x06
#define REGION_EUROPE       0x08
#define REGION_KOREA        0x09
#define REGION_AUSTRALIA    0x0B
#define REGION_NEWZEALAND   0x0C
#define REGION_NORTHAMERICA 0x0D
#define REGION_OPEN         0xFF


//Declare Methods
void NanoInit(void);
void nanoSetBaud(long baudRate);

void nanoStartReading(void);
void nanoStopReading(void);

void nanoSetRegion(uint8_t region);
void nanoSetAntennaPort(void);
void nanoSetTagProtocol(uint8_t protocol);
void nanoEnableReadFilter(void);
void nanoDisableReadFilter(void);
void nanoSetReaderConfiguration(uint8_t option1, uint8_t option2);
void nanoSetReadPower(int16_t powerSetting);
void nanoGetReadPower(void);
void nanoSetWritePower(int16_t powerSetting);
void nanoGetWritePower(void);

uint8_t nanoReadTagEPC(uint8_t *epc, uint8_t epcLength, uint16_t timeOut);
uint8_t nanoWriteTagEPC(char *newID, uint8_t newIDLength, uint16_t timeOut);
uint8_t nanoReadUserData(uint8_t *userData, uint8_t userDataLength, uint16_t timeOut);
uint8_t nanoWriteUserData(uint8_t *userData, uint8_t userDataLength, uint16_t timeOut);

uint8_t writeData(uint8_t bank, uint32_t address, uint8_t *dataToRecord, uint8_t dataLengthToRecord, uint16_t timeOut);
uint8_t readData(uint8_t bank, uint32_t address, uint8_t *dataRead, uint8_t dataLengthRead, uint16_t timeOut);
void sendMessage(uint8_t opcode, uint8_t *data, uint8_t size, uint16_t timeOut, bool waitForResponse);
void sendCommand(uint16_t timeOut, bool waitForResponse);

uint16_t calculateCRC(uint8_t *u8Buf, uint8_t len);
uint8_t parseResponse(void);

uint8_t getTagDataBytes(void);
uint8_t getTagEPCBytes(void);
bool nanoCheck(void);

void nanoPrintStatus(const CLS1_StdIOType *io);
static void Serial_println(const char *msg);
void Serial_print(const char *msg);

uint8_t NANO_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);

uint8_t nanoStatus();


#endif /* SOURCES_NANO_H_ */
