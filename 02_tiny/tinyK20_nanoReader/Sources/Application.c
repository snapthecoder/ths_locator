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


static uint8_t APP_EspMsgBuf[512]; /* buffer for messages from ESP8266 */

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
	uint8_t res = ERR_OK;
	uint8_t buf[64];
	uint8_t ch_id = 1;

	uint8_t ssid[10];
	uint8_t pwd[10];

	//TODO use macros
	uint8_t IPAddrStr[20] = "192.168.0.101";
	uint16_t port = 8000;
	uint16_t msTimeout = 1000;

	UTIL1_strcpy(ssid, sizeof(ssid), "tsh_net");
	UTIL1_strcpy(pwd, sizeof(pwd), "12345678");

	//connect to AP
	ESP_JoinAP(ssid, pwd, 4, io);

	UTIL1_strcpy(buf, sizeof(buf), "GATE01");

	ESP_SelectMode(3);

	ESP_OpenConnection(ch_id, 1, IPAddrStr, port, msTimeout, io);

	ESP_PrepareMsgSend(ch_id, UTIL1_strlen(buf), 3000, io);

	ESP_SendATCommand(buf, NULL, 0, NULL, ESP_DEFAULT_TIMEOUT_MS, io);
}


void APP_Start(void) {

	  CLS1_ConstStdIOType *io = CLS1_GetStdio();

	  CLS1_SendStr("\r\n------------------------------------------\r\n", io->stdOut);
	  CLS1_SendStr("THS GATE\r\nFirmware Version 1.0\r\n", io->stdOut);
	  CLS1_SendStr("------------------------------------------\r\n", io->stdOut);
	  CLS1_PrintPrompt(io);

	  NanoInit();
	  ESP_Init();
	  SHELL_Init();

	  WAIT1_Waitms(100);

	  //nanoStartReading();

	  openESP(io);
	  //for future starting over webinterface

	  for(;;){

		  //if reader is started search for TSHs
		  if (nanoStatus()) nanoProcess(io);
		  //check for new commands over serial
		  SHELL_Parse();

		  //commands over network
		  //ESP_Parse();
		  WAIT1_Waitms(5);
	  }
}


