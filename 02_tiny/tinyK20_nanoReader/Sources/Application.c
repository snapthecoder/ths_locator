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

#define SERVER_IP_STR "192.168.0.102"
#define SERVER_PORT	(51298)

#define PWD "dwlvg44571"
#define SSID "dlink-4190"

static uint8_t APP_EspMsgBuf[512]; /* buffer for messages from ESP8266 */

void nanoProcess(const CLS1_StdIOType *io){

	if(nanoCheck() == 1){
		  uint8_t responseType = parseResponse();

		  if(responseType == RESPONSE_IS_KEEPALIVE){
			  //scanning
		  }
		  else if(responseType == RESPONSE_IS_TAGFOUND){
			  nanoPrintStatus();
			  netProcess(io);
		  }
	}
}

void openESP(const CLS1_StdIOType *io){
	uint8_t res = ERR_OK;
	uint8_t buf[64];
	uint8_t ch_id = 1;

	uint8_t ssid[10];
	uint8_t pwd[10];

	uint8_t IPAddrStr[20] = "192.168.0.102";
	uint16_t port = 8000;
	uint16_t msTimeout = 3000;

	uint8_t gateNr = 1;

	UTIL1_strcpy(ssid, sizeof(ssid), "ths_net");
	UTIL1_strcpy(pwd, sizeof(pwd), "12345678");

	UTIL1_strcpy(buf, sizeof(buf), "GATE");

	ESP_SelectMode(3);

	ESP_OpenConnection(ch_id, 1, &IPAddrStr, port, msTimeout, io);

	ESP_PrepareMsgSend(ch_id, UTIL1_strlen(buf), 3000, io);

	ESP_SendATCommand(buf, NULL, 0, NULL, ESP_DEFAULT_TIMEOUT_MS, io);

}


void APP_Start(void) {
	  CLS1_ConstStdIOType *io = CLS1_GetStdio();

	  uint8_t buf[30];
	  UTIL1_strcpy(buf, sizeof(buf), "GATE1234567890\r\n");

	  CLS1_SendStr("\r\n------------------------------------------\r\n", io->stdOut);
	  CLS1_SendStr("THS GATE\r\nFirmware Version 1.0\r\n", io->stdOut);
	  CLS1_SendStr("------------------------------------------\r\n", io->stdOut);
	  CLS1_PrintPrompt(io);

	  //NanoInit();
	  ESP_Init();
	  SHELL_Init();

	  WAIT1_Waitms(100);

	  //open TCP/IP connection
	  openESP(io);

	  for(;;){

		  ESP_PrepareMsgSend(0, UTIL1_strlen(buf), 3000, io);
		  ESP_SendATCommand(buf, NULL, 0, NULL, ESP_DEFAULT_TIMEOUT_MS, io);
		  //check for tags filter response type
		  //nanoProcess(io);
		  //netProcess(io);
		  SHELL_Parse();
		  WAIT1_Waitms(100);
	  }
}


