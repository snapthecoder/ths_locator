/**
 * \file
 * \brief Main application file
 *
 * This provides the main application entry point.
 */

#include "Platform.h"
#include "Application.h"
#include "CLS1.h"
#include "nano.h"
#include "WAIT1.h"
#include "Shell.h"
#include "ESP8266.h"

static uint8_t APP_EspMsgBuf[500];

void WebProcess(const CLS1_StdIOType *io) {
  if(ESP_IsServerOn()){
	  ESP_ReadCharsUntil(APP_EspMsgBuf, sizeof(APP_EspMsgBuf), '\n', 10);
	  if(UTIL1_strFind(APP_EspMsgBuf, (char *)"Nano_Start") != -1){
		  CLS1_SendStr("Nano started\r\n", io->stdOut);
		  nanoStartReading();
	  }
	  if(UTIL1_strFind(APP_EspMsgBuf, (char *)"Nano_Stop") != -1){
	  		  CLS1_SendStr("Nano stoped\r\n", io->stdOut);
	  		  nanoStopReading();
	  }
  }
}

void nanoProcess(const CLS1_StdIOType *io){

	if(nanoCheck() == 1){
		  uint8_t responseType = parseResponse();

		  if(responseType == RESPONSE_IS_KEEPALIVE){
			  //scanning
		  }
		  else if(responseType == RESPONSE_IS_TAGFOUND){
			  nanoPrintStatus(io);
		  }
	}
}

void openESP(const CLS1_StdIOType *io){
	uint8_t ch_id = 1;
	uint8_t ssid[10];
	uint8_t pwd[10];

	//TODO use macros or params
	uint8_t IPAddrStr[20] = "192.168.0.101";
	uint16_t port = 8000;
	uint16_t msTimeout = 1000;

	UTIL1_strcpy(ssid, sizeof(ssid), "tsh_net");
	UTIL1_strcpy(pwd, sizeof(pwd), "12345678");

	//connect to AP
	if (ESP_JoinAP(ssid, pwd, 4, io) == ERR_OK){
		CLS1_SendStr("AP connected...\r\n", io->stdOut);
	}
	else{
		CLS1_SendStr("Error connecting AP...\r\n", io->stdOut);
	}

	WAIT1_Waitms(2000);

	if (ESP_SelectMode(3) == ERR_OK){
		CLS1_SendStr("Mode 3 Selected...\r\n", io->stdOut);
	}
	else{
		CLS1_SendStr("Error selecting Mode 3...\r\n", io->stdOut);
	}

	WAIT1_Waitms(1000);

	//open connection to server
	if(ESP_OpenConnection(ch_id, 1, IPAddrStr, port, msTimeout, io) == ERR_OK){
		CLS1_SendStr("Server connected...\r\n", io->stdOut);
	}
	else{
		CLS1_SendStr("Error connecting Server..Is the server on?\r\n", io->stdOut);
	}

	CLS1_PrintPrompt(io);

	//TODO create Server (Probleme mit Connection und Server gleichzeitig)
}


void APP_Start(void) {

	  CLS1_ConstStdIOType *io = CLS1_GetStdio();

	  CLS1_SendStr("\r\n------------------------------------------\r\n", io->stdOut);
	  CLS1_SendStr("TSH GATE\r\nFirmware Version 1.0\r\n", io->stdOut);
	  CLS1_SendStr("------------------------------------------\r\n", io->stdOut);
	  CLS1_PrintPrompt(io);

	  NanoInit();
	  ESP_Init();
	  SHELL_Init();

	  WAIT1_Waitms(100);

	  openESP(io);

	  for(;;){

		  //if reader is started search for TSHs
		  if (nanoStatus()) nanoProcess(io);
		  //check for new commands over serial
		  SHELL_Parse();
		  //commands over network
		  WebProcess(io);

		  WAIT1_Waitms(5);
	  }
}


