/*
  * Shell.c
 *
 *  Created on: 04.08.2011
 *      Author: Erich Styger
 */

#include "Platform.h"
#include "Application.h"
#include "FRTOS1.h"
#include "Shell.h"
#include "CLS1.h"
#include "FAT1.h"
#include "TmDt1.h"
#include "KIN1.h"
#if PL_CONFIG_HAS_SEGGER_RTT
  #include "RTT1.h"
#endif


void NanoInit(void){
	//setup uart
}

/* Sets Baudrate on Nano
 * possible Baudrates
 	9600,
	19200,
	38400,
	115200,
	230400,
	460800,
	921600,
 */
void nanoSetBaud(long baudRate){
	//Copy this setting into a temp data array
	uint8_t size = sizeof(baudRate);
	uint8_t data[size];
	for (uint8_t x = 0 ; x < size ; x++){
	   data[x] = (uint8_t)(baudRate >> (8 * (size - 1 - x)));
	}
	sendMessage(TMR_SR_OPCODE_SET_BAUD_RATE, data, size, COMMAND_TIME_OUT, false);
}

/*Begin scanning for tags
*There are many many options and features to the nano, this sets options
*for continuous read of GEN2 type tags
*/
void nanoStartReading(void){
	disableReadFilter(); //Don't filter for a specific tag, read all tags

	//define "blobb" (command for nano)
	uint8_t configBlob[] = {0x00, 0x00, 0x01, 0x22, 0x00, 0x00, 0x05, 0x07, 0x22, 0x10, 0x00, 0x1B, 0x03, 0xE8, 0x01, 0xFF};

	sendMessage(TMR_SR_OPCODE_MULTI_PROTOCOL_TAG_OP, configBlob, sizeof(configBlob));
}

void nanoStopReading(void){
	//00 00 = Timeout, currently ignored
	//02 = Option - stop continuous reading
	uint8_t configBlob[] = {0x00, 0x00, 0x02};

	sendMessage(TMR_SR_OPCODE_MULTI_PROTOCOL_TAG_OP, configBlob, sizeof(configBlob), false); //Do not wait for response
}

/*Given a region, set the correct freq
0x04 = IN
0x05 = JP
0x06 = PRC
0x08 = EU3
0x09 = KR2
0x0B = AU
0x0C = NZ
0x0D = NAS2 (North America)
0xFF = OPEN
*/
void nanoSetRegion(uint8_t region){
	sendMessage(TMR_SR_OPCODE_SET_REGION, &region, sizeof(region));
}

/*Sets the TX and RX antenna ports to 01
Because the Nano module has only one antenna port, it is not user configurable
*/
void nanoSetAntennaPort(void)
{
  uint8_t configBlob[] = {0x01, 0x01}; //TX port = 1, RX port = 1
  sendMessage(TMR_SR_OPCODE_SET_ANTENNA_PORT, configBlob, sizeof(configBlob));
}

/*Sets the protocol of the module
Currently only GEN2 has been tested and supported but others are listed here for reference
and possible future support
TMR_TAG_PROTOCOL_NONE              = 0x00
TMR_TAG_PROTOCOL_ISO180006B        = 0x03
TMR_TAG_PROTOCOL_GEN2              = 0x05
TMR_TAG_PROTOCOL_ISO180006B_UCODE  = 0x06
TMR_TAG_PROTOCOL_IPX64             = 0x07
TMR_TAG_PROTOCOL_IPX256            = 0x08
TMR_TAG_PROTOCOL_ATA               = 0x1D
*/
void nanoSetTagProtocol(uint8_t protocol)
{
  uint8_t data[2];
  data[0] = 0; 	//Opcode expects 16-bits
  data[1] = protocol;

  sendMessage(TMR_SR_OPCODE_SET_TAG_PROTOCOL, data, sizeof(data));
}

void nanoEnableReadFilter(void)
{
  setReaderConfiguration(0x0C, 0x01); //Enable read filter
}

//Disabling the read filter allows continuous reading of tags
void nanoDisableReadFilter(void)
{
  setReaderConfiguration(0x0C, 0x00); //Diable read filter
}

/*Sends optional parameters to the module
See TMR_SR_Configuration in serial_reader_imp.h for a breakdown of options
*/
void nanoSetReaderConfiguration(uint8_t option1, uint8_t option2)
{
  uint8_t data[3];

  //These are parameters gleaned from inspecting the 'Transport Logs' of the Universal Reader Assistant
  //And from serial_reader_l3.c
  data[0] = 1; //Key value form of command
  data[1] = option1;
  data[2] = option2;

  sendMessage(TMR_SR_OPCODE_SET_READER_OPTIONAL_PARAMS, data, sizeof(data));
}

//getOptionalParameters - OPEN

//getVersion - OPEN

//Set the read TX power
//Maximum power is 2700 = 27.00 dBm
//1005 = 10.05dBm
void nanoSetReadPower(int16_t powerSetting)
{
  if (powerSetting > 2700) powerSetting = 2700; //Limit to 27dBm

  //Copy this setting into a temp data array
  uint8_t size = sizeof(powerSetting);
  uint8_t data[size];
  for (uint8_t x = 0 ; x < size ; x++)
    data[x] = (uint8_t)(powerSetting >> (8 * (size - 1 - x)));

  sendMessage(TMR_SR_OPCODE_SET_READ_TX_POWER, data, size);
}

//Get the read TX power
void nanoGetReadPower(void)
{
  uint8_t data[] = {0x00}; //Just return power
  //uint8_t data[] = {0x01}; //Return power with limits

  sendMessage(TMR_SR_OPCODE_GET_READ_TX_POWER, data, sizeof(data));
}

//Set the write power
//Maximum power is 2700 = 27.00 dBm
//1005 = 10.05dBm
void nanoSetWritePower(int16_t powerSetting)
{
  uint8_t size = sizeof(powerSetting);
  uint8_t data[size];
  for (uint8_t x = 0 ; x < size ; x++)
    data[x] = (uint8_t)(powerSetting >> (8 * (size - 1 - x)));

  sendMessage(TMR_SR_OPCODE_SET_WRITE_TX_POWER, data, size);
}

//Get the write TX power
void nanoGetWritePower(void)
{
  uint8_t data[] = {0x00}; //Just return power
  //uint8_t data[] = {0x01}; //Return power with limits

  sendMessage(TMR_SR_OPCODE_GET_WRITE_TX_POWER, data, sizeof(data));
}

//Read a single EPC
//Caller must provide an array for EPC to be stored in
uint8_t nanoReadTagEPC(uint8_t *epc, uint8_t &epcLength, uint16_t timeOut)
{
  uint8_t bank = 0x01; //User data bank
  uint8_t address = 0x02; //Starts at 2

  return (readData(bank, address, epc, epcLength, timeOut));
}

//This writes a new EPC to the first tag it detects
//Use with caution. This function doesn't control which tag hears the command.
uint8_t nanoWriteTagEPC(char *newID, uint8_t newIDLength, uint16_t timeOut)
{
  uint8_t bank = 0x01; //EPC memory
  uint8_t address = 0x02; //EPC starts at spot 4

  return (writeData(bank, address, newID, newIDLength, timeOut));
}

//This reads the user data area of the tag. 0 to 64 bytes are normally available.
//Use with caution. The module can't control which tag hears the command.
uint8_t nanoReadUserData(uint8_t *userData, uint8_t &userDataLength, uint16_t timeOut)
{
  uint8_t bank = 0x03; //User data bank
  uint8_t address = 0x00; //Starts at 0

  return (readData(bank, address, userData, userDataLength, timeOut));
}

//This writes data to the tag. 0, 4, 16 or 64 bytes may be available.
//Writes to the first spot 0x00 and fills up as much of the bytes as user provides
//Use with caution. Function doesn't control which tag hears the command.
uint8_t nanoWriteUserData(uint8_t *userData, uint8_t userDataLength, uint16_t timeOut)
{
  uint8_t bank = 0x03; //User memory
  uint8_t address = 0x00;

  return (writeData(bank, address, userData, userDataLength, timeOut));
}

//Reads a given bank and address to a data array
//Allows for writing of passwords and user data
//TODO Add support for accessPassword
//TODO Add support for writing to specific tag
uint8_t readData(uint8_t bank, uint32_t address, uint8_t *dataRead, uint8_t &dataLengthRead, uint16_t timeOut)
{
  //Bank 0
  //response: [00] [08] [28] [00] [00] [EE] [FF] [11] [22] [12] [34] [56] [78]
  //[EE] [FF] [11] [22] = Kill pw
  //[12] [34] [56] [78] = Access pw

  //Bank 1
  //response: [00] [08] [28] [00] [00] [28] [F0] [14] [00] [AA] [BB] [CC] [DD]
  //[28] [F0] = CRC
  //[14] [00] = PC
  //[AA] [BB] [CC] [DD] = EPC

  //Bank 2
  //response: [00] [18] [28] [00] [00] [E2] [00] [34] [12] [01] [6E] [FE] [00] [03] [7D] [9A] [A3] [28] [05] [01] [6B] [00] [05] [5F] [FB] [FF] [FF] [DC] [00]
  //[E2] = CIsID
  //[00] [34] [12] = Vendor ID = 003, Model ID == 412
  //[01] [6E] [FE] [00] [03] [7D] [9A] [A3] [28] [05] [01] [69] [10] [05] [5F] [FB] [FF] [FF] [DC] [00] = Unique ID (TID)

  //Bank 3
  //response: [00] [40] [28] [00] [00] [41] [43] [42] [44] [45] [46] [00] [00] [00] [00] [00] [00] ...
  //User data

  uint8_t data[8];

  //Insert timeout
  data[0] = timeOut >> 8 & 0xFF; //Timeout msB in ms
  data[1] = timeOut & 0xFF; //Timeout lsB in ms

  data[2] = bank; //Bank

  //Splice address into array
  for (uint8_t x = 0 ; x < sizeof(address) ; x++)
    data[3 + x] = address >> (8 * (3 - x)) & 0xFF;

  data[7] = dataLengthRead / 2; //Number of 16-bit chunks to read.
  //0x00 will read the entire bank but may be more than we expect (both Kill and Access PW will be returned when reading bank 1 from address 0)

  //When reading the user data area we need to read the entire bank
  if(bank == 0x03) data[7] = 0x00;

  sendMessage(TMR_SR_OPCODE_READ_TAG_DATA, data, sizeof(data), timeOut);

  if (msg[0] == ALL_GOOD) //We received a good response
  {
    uint16_t status = (msg[3] << 8) | msg[4];

    if (status == 0x0000)
    {
      uint8_t responseLength = msg[1];

      if (responseLength < dataLengthRead) //User wants us to read more than we have available
        dataLengthRead = responseLength;

	  //There is a case here where responseLegnth is more than dataLengthRead, in which case we ignore (don't load) the additional bytes
      //Load limited response data into caller's array
	  for (uint8_t x = 0 ; x < dataLengthRead ; x++)
        dataRead[x] = msg[5 + x];

      return (RESPONSE_SUCCESS);
    }
  }

  //Else - msg[0] was timeout or other
  dataLengthRead = 0; //Inform caller that we weren't able to read anything

  return (RESPONSE_FAIL);
}
