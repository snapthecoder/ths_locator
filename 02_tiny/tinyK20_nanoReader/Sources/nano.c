/*
 * nano.c
 *
 *  Created on: 03.04.2018
 *      Author: Dominic
 */
/*
  * Shell.c
 *
 *  Created on: 04.08.2011
 *      Author: Erich Styger
 */

#include "Platform.h"
#include "AS1.h"
#include "nano.h"
#include "RTT1.h"
#include "TMOUT1.h"

uint8_t _headnano = 0;

void NanoInit(void){

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
	sendMessage(TMR_SR_OPCODE_SET_BAUD_RATE, data, size, COMMAND_TIME_OUT, 0);
}

/*Begin scanning for tags
*There are many many options and features to the nano, this sets options
*for continuous read of GEN2 type tags
*/
void nanoStartReading(void){
	nanoDisableReadFilter(); //Don't filter for a specific tag, read all tags

	//define "blobb" (command for nano)
	uint8_t configBlob[] = {0x00, 0x00, 0x01, 0x22, 0x00, 0x00, 0x05, 0x07, 0x22, 0x10, 0x00, 0x1B, 0x03, 0xE8, 0x01, 0xFF};

	sendMessage(TMR_SR_OPCODE_MULTI_PROTOCOL_TAG_OP, configBlob, sizeof(configBlob), 10, 0);
}

void nanoStopReading(void){
	//00 00 = Timeout, currently ignored
	//02 = Option - stop continuous reading
	uint8_t configBlob[] = {0x00, 0x00, 0x02};

	sendMessage(TMR_SR_OPCODE_MULTI_PROTOCOL_TAG_OP, configBlob, sizeof(configBlob), 10, 0); //Do not wait for response
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
	sendMessage(TMR_SR_OPCODE_SET_REGION, &region, sizeof(region), 10 , 0);
}

/*Sets the TX and RX antenna ports to 01
Because the Nano module has only one antenna port, it is not user configurable
*/
void nanoSetAntennaPort(void)
{
  uint8_t configBlob[] = {0x01, 0x01}; //TX port = 1, RX port = 1
  sendMessage(TMR_SR_OPCODE_SET_ANTENNA_PORT, configBlob, sizeof(configBlob), 10, 0);
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

  sendMessage(TMR_SR_OPCODE_SET_TAG_PROTOCOL, data, sizeof(data), 10, 0);
}

void nanoEnableReadFilter(void)
{
  nanoSetReaderConfiguration(0x0C, 0x01); //Enable read filter
}

//Disabling the read filter allows continuous reading of tags
void nanoDisableReadFilter(void)
{
  nanoSetReaderConfiguration(0x0C, 0x00); //Diable read filter
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

  sendMessage(TMR_SR_OPCODE_SET_READER_OPTIONAL_PARAMS, data, sizeof(data), 10, 0);
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

  sendMessage(TMR_SR_OPCODE_SET_READ_TX_POWER, data, size, 10, 0);
}

//Get the read TX power
void nanoGetReadPower(void)
{
  uint8_t data[] = {0x00}; //Just return power
  //uint8_t data[] = {0x01}; //Return power with limits

  sendMessage(TMR_SR_OPCODE_GET_READ_TX_POWER, data, sizeof(data), 10, 0);
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

  sendMessage(TMR_SR_OPCODE_SET_WRITE_TX_POWER, data, size, 10, 0);
}

//Get the write TX power
void nanoGetWritePower(void)
{
  uint8_t data[] = {0x00}; //Just return power
  //uint8_t data[] = {0x01}; //Return power with limits

  sendMessage(TMR_SR_OPCODE_GET_WRITE_TX_POWER, data, sizeof(data), 10, 0);
}

//Read a single EPC
//Caller must provide an array for EPC to be stored in
uint8_t nanoReadTagEPC(uint8_t *epc, uint8_t epcLength, uint16_t timeOut)
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
uint8_t nanoReadUserData(uint8_t *userData, uint8_t userDataLength, uint16_t timeOut)
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



//Writes a data array to a given bank and address
//Allows for writing of passwords and user data
//TODO Add support for accessPassword
//TODO Add support for writing to specific tag
uint8_t writeData(uint8_t bank, uint32_t address, uint8_t *dataToRecord, uint8_t dataLengthToRecord, uint16_t timeOut)
{
  //Example: FF  0A  24  03  E8  00  00  00  00  00  03  00  EE  58  9D
  //FF 0A 24 = Header, LEN, Opcode
  //03 E8 = Timeout in ms
  //00 = Option initialize
  //00 00 00 00 = Address
  //03 = Bank
  //00 EE = Data
  //58 9D = CRC

  uint8_t data[8 + dataLengthToRecord];

  //Pre-load array options
  data[0] = timeOut >> 8 & 0xFF; //Timeout msB in ms
  data[1] = timeOut & 0xFF; //Timeout lsB in ms
  data[2] = 0x00; //Option initialize

  //Splice address into array
  for (uint8_t x = 0 ; x < sizeof(address) ; x++)
    data[3 + x] = address >> (8 * (3 - x)) & 0xFF;

  //Bank 0 = Passwords
  //Bank 1 = EPC Memory Bank
  //Bank 2 = TID
  //Bank 3 = User Memory
  data[7] = bank;

  //Splice data into array
  for (uint8_t x = 0 ; x < dataLengthToRecord ; x++)
    data[8 + x] = dataToRecord[x];

  sendMessage(TMR_SR_OPCODE_WRITE_TAG_DATA, data, sizeof(data), timeOut,0);

  if (msg[0] == ALL_GOOD) //We received a good response
  {
    uint16_t status = (msg[3] << 8) | msg[4];

    if (status == 0x0000)
      return (RESPONSE_SUCCESS);
  }

  //Else - msg[0] was timeout or other
  return (RESPONSE_FAIL);
}

//Reads a given bank and address to a data array
//Allows for writing of passwords and user data
//TODO Add support for accessPassword
//TODO Add support for writing to specific tag
uint8_t readData(uint8_t bank, uint32_t address, uint8_t *dataRead, uint8_t dataLengthRead, uint16_t timeOut)
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

  sendMessage(TMR_SR_OPCODE_READ_TAG_DATA, data, sizeof(data), timeOut, 1);

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

//Checks incoming buffer for the start characters
//Returns true if a new message is complete and ready to be cracked
bool nanoCheck()
{
  while (AS1_GetCharsInTxBuf() != 0)
  {
    uint8_t incomingData;
    AS1_RecvChar(&incomingData);

    //Wait for header byte
    if (_headnano == 0 && incomingData != 0xFF)
    {
      //Do nothing. Ignore this byte because we need a start byte
    }
    else
    {
      //Load this value into the array
      msg[_headnano++] = incomingData;

      _headnano %= MAX_MSG_SIZE; //Wrap variable

      if ((_headnano > 0) && (_headnano == msg[1] + 7))
      {
        //We've got a complete sentence!

        //Erase the remainder of the array
        for (uint8_t x = _headnano ; x < MAX_MSG_SIZE ; x++)
          msg[x] = 0;

        _headnano = 0; //Reset

		//Used for debugging: Does the user want us to print the command to serial port?
		// DEGBUG PRINT

        return (1);
      }

    }
  }
}


  //Given an opcode, a piece of data, and the size of that data, package up a sentence and send it
  void sendMessage(uint8_t opcode, uint8_t *data, uint8_t size, uint16_t timeOut, bool waitForResponse)
  {
    msg[1] = size; //Load the length of this operation into msg array
    msg[2] = opcode;

    //Copy the data into msg array
    for (uint8_t x = 0 ; x < size ; x++)
      msg[3 + x] = data[x];

    sendCommand(timeOut, waitForResponse); //Send and wait for response
  }

  //Given an array, calc CRC, assign header, send it out
  //Modifies the caller's msg array
  void sendCommand(uint16_t timeOut, bool waitForResponse)
  {
    msg[0] = 0xFF; //Universal header
    uint8_t messageLength = msg[1];

    uint8_t opcode = msg[2]; //Used to see if response from module has the same opcode

    //Attach CRC
    uint16_t crc = calculateCRC(&msg[1], messageLength + 2); //Calc CRC starting from spot 1, not 0. Add 2 for LEN and OPCODE bytes.
    msg[messageLength + 3] = crc >> 8;
    msg[messageLength + 4] = crc & 0xFF;

    //Used for debugging: Does the user want us to print the command to serial port?
    // DEBUG PRINT

    //Remove anything in the incoming buffer
    //TODO this is a bad idea if we are constantly readings tags
    //while (AS1_GetCharsInTxBuf() != 0) AS1_RecvChar(0);

    //Send the command to the module

    //use sendblock
/*    uint16_t snt;
    uint16_t length = (messageLength +5);

    if (AS1_SendBlock(&msg[0], length, &snt) != ERR_OK) {
        return;
    }
*/
    for (uint8_t x = 0 ; x < (messageLength + 5) ; x++){
    	WAIT1_Waitus(100);
    	AS1_SendChar(msg[x]);
    }

    //There are some commands (setBaud) that we can't or don't want the response
    if (waitForResponse == 0)
    {
    	//Wait for serial sending to complete
    	return;
    }

    //For debugging, probably remove
    //for (uint8_t x = 0 ; x < 100 ; x++) msg[x] = 0;

    //Wait for response with timeout

    TMOUT1_CounterHandle handle;

    uint32_t startTime = TMOUT1_Value(handle);
    while (AS1_GetCharsInRxBuf() == 0)
    {
      if (TMOUT1_Value(handle) - startTime > timeOut)
      {
        //DEBUG PRINT TIMEOUT
        msg[0] = ERROR_COMMAND_RESPONSE_TIMEOUT;
        return;
      }
      //delay
      WAIT1_Waitms(10);
    }

    // Layout of response in data array:
    // [0] [1] [2] [3]      [4]      [5] [6]  ... [LEN+4] [LEN+5] [LEN+6]
    // FF  LEN OP  STATUSHI STATUSLO xx  xx   ... xx      CRCHI   CRCLO
    messageLength = MAX_MSG_SIZE - 1; //Make the max length for now, adjust it when the actual len comes in
    uint8_t spot = 0;
    while (spot < messageLength)
    {
      if (TMOUT1_Value(handle) - startTime > timeOut)
      {
        //DEBUG TIMEOUT 2
        msg[0] = ERROR_COMMAND_RESPONSE_TIMEOUT;
        return;
      }

      if (AS1_GetCharsInRxBuf() != 0)
      {

        AS1_RecvChar(&msg[spot]); //& nötig?

        if (spot == 1) //Grab the length of this response (spot 1)
          messageLength = msg[1] + 7; //Actual length of response is ? + 7 for extra stuff (header, Length, opcode, 2 status bytes, ..., 2 bytes CRC = 7)

        spot++;

        //There's a case were we miss the end of one message and spill into another message.
        //We don't want spot pointing at an illegal spot in the array
        spot %= MAX_MSG_SIZE; //Wrap condition
      }
    }

    //Used for debugging: Does the user want us to print the command to serial port?
    // DEBUG PRINT RESPONSE

    //Check CRC
    crc = calculateCRC(&msg[1], messageLength - 3); //Remove header, remove 2 crc bytes
    if ((msg[messageLength - 2] != (crc >> 8)) || (msg[messageLength - 1] != (crc & 0xFF)))
    {
      msg[0] = ERROR_CORRUPT_RESPONSE;
      // DEBUG PRINT "Corrupt response"
      return;
    }

    //If crc is ok, check that opcode matches (did we get a response to the command we sent or a different one?)
    if (msg[2] != opcode)
    {
      msg[0] = ERROR_WRONG_OPCODE_RESPONSE;
      // DEBUG PRINT "Wrong opcode response"
      return;
    }

    //If everything is ok, load all ok into msg array
    msg[0] = ALL_GOOD;

  }

  //Comes from serial_reader_l3.c
  //ThingMagic-mutated CRC used for messages.
  //Notably, not a CCITT CRC-16, though it looks close.
  static uint16_t crctable[] =
  {
    0x0000, 0x1021, 0x2042, 0x3063,
    0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b,
    0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
  };

  //Calculates the magical CRC value
  uint16_t calculateCRC(uint8_t *u8Buf, uint8_t len)
  {
    uint16_t crc = 0xFFFF;

    for (uint8_t i = 0 ; i < len ; i++)
    {
      crc = ((crc << 4) | (u8Buf[i] >> 4)) ^ crctable[crc >> 12];
      crc = ((crc << 4) | (u8Buf[i] & 0x0F)) ^ crctable[crc >> 12];
    }

    return crc;
  }

  uint8_t parseResponse(void)
  {
    //See http://www.thingmagic.com/images/Downloads/Docs/AutoConfigTool_1.2-UserGuide_v02RevA.pdf
    //for a breakdown of the response packet

    //Example response:
    //FF  28  22  00  00  10  00  1B  01  FF  01  01  C4  11  0E  16
    //40  00  00  01  27  00  00  05  00  00  0F  00  80  30  00  00
    //00  00  00  00  00  00  00  00  00  15  45  E9  4A  56  1D
    //  [0] FF = Header
    //  [1] 28 = Message length
    //  [2] 22 = OpCode
    //  [3, 4] 00 00 = Status
    //  [5 to 11] 10 00 1B 01 FF 01 01 = RFU 7 bytes
    //  [12] C4 = RSSI
    //  [13] 11 = Antenna ID (4MSB = TX, 4LSB = RX)
    //  [14, 15, 16] 0E 16 40 = Frequency in kHz
    //  [17, 18, 19, 20] 00 00 01 27 = Timestamp in ms since last keep alive msg
    //  [21, 22] 00 00 = phase of signal tag was read at (0 to 180)
    //  [23] 05 = Protocol ID
    //  [24, 25] 00 00 = Number of bits of embedded tag data [M bytes]
    //  [26 to M] (none) = Any embedded data
    //  [26 + M] 0F = RFU reserved future use
    //  [27, 28 + M] 00 80 = EPC Length [N bytes]  (bits in EPC including PC and CRC bits). 128 bits = 16 bytes
    //  [29, 30 + M] 30 00 = Tag EPC Protocol Control (PC) bits
    //  [31 to 42 + M + N] 00 00 00 00 00 00 00 00 00 00 15 45 = EPC ID
    //  [43, 44 + M + N] 45 E9 = EPC CRC
    //  [45, 46 + M + N] 56 1D = Message CRC

    uint8_t msgLength = msg[1] + 7; //Add 7 (the header, length, opcode, status, and CRC) to the LEN field to get total bytes
    uint8_t opCode = msg[2];

    //Check the CRC on this response
    uint16_t messageCRC = calculateCRC(&msg[1], msgLength - 3 ); //Ignore header (start spot 1), remove 3 bytes (header + 2 CRC)
    if ((msg[msgLength - 2] != (messageCRC >> 8)) || (msg[msgLength - 1] != (messageCRC & 0xFF)))
    {
      return (ERROR_CORRUPT_RESPONSE);
    }

    if (opCode == TMR_SR_OPCODE_READ_TAG_ID_MULTIPLE) //opCode = 0x22
    {
      //Based on the record length identify if this is a tag record, a temperature sensor record, or a keep-alive?
      if (msg[1] == 0x00) //Keep alive
      {
        //We have a Read cycle reset/keep-alive message
        //Sent once per second
        uint16_t statusMsg = 0;
        for (uint8_t x = 0 ; x < 2 ; x++)
          statusMsg |= (uint32_t)msg[3 + x] << (8 * (1 - x));

        if (statusMsg == 0x0400)
        {
          return (RESPONSE_IS_KEEPALIVE);
        }
        else if (statusMsg == 0x0504)
        {
          return (RESPONSE_IS_TEMPTHROTTLE);
        }
      }
      else if (msg[1] == 0x08) //Unknown
      {
        return (RESPONSE_IS_UNKNOWN);
      }
      else if (msg[1] == 0x0a) //temperature
      {
          return (RESPONSE_IS_TEMPERATURE);
      }
      else //Full tag record
      {
        //This is a full tag response
        //User can now pull out RSSI, frequency of tag, timestamp, EPC, Protocol control bits, EPC CRC, CRC
        return (RESPONSE_IS_TAGFOUND);
      }
    }
    else
    {
      return (ERROR_UNKNOWN_OPCODE);
    }

  }









