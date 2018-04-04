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

#define SERVER_IP_STR "192.168.1.0"
#define SERVER_PORT	(8000)

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
	uint8_t ch_id = 4;

	res = ESP_SetNumberOfConnections(1, io, ESP_DEFAULT_TIMEOUT_MS);
	if (res==ERR_OK) {
		res = ESP_OpenConnection(ch_id, TRUE, SERVER_IP_STR, SERVER_PORT, 5000, io);
		if (res==ERR_OK) {
			//sing up at server
			UTIL1_strcpy(buf, sizeof(buf), "GATE_0001"); //send gate ID
			UTIL1_strcat(buf, sizeof(buf), "\r\n");
			res = ESP_PrepareMsgSend(ch_id, UTIL1_strlen(buf), 3000, io);
			if (res==ERR_OK) {
				res = ESP_SendATCommand(buf, NULL, 0, NULL, ESP_DEFAULT_TIMEOUT_MS, io);
				if (res!=ERR_OK) {
					CLS1_SendStr("Sending page failed!\r\n", io->stdErr); /* copy on console */
				}
				else {
					for(;;) { /* breaks */
						res = ESP_ReadCharsUntil(buf, sizeof(buf), '\n', 1000);
				        if (res==ERR_OK) { /* line read */
				        	if (io!=NULL) {
				        		CLS1_SendStr(buf, io->stdOut); /* copy on console */
				            }
				        }
				        if (UTIL1_strncmp(buf, "SEND OK\r\n", sizeof("SEND OK\r\n")-1)==0) { /* ok from module */
				            break;
				        }
					}
				}
			}
		}
	}
}


void APP_Start(void) {
	CLS1_ConstStdIOType *io = CLS1_GetStdio();

	  CLS1_SendStr("\r\n------------------------------------------\r\n", io->stdOut);
	  CLS1_SendStr("THS GATE\r\nFirmware Version 1.0\r\n", io->stdOut);
	  CLS1_SendStr("------------------------------------------\r\n", io->stdOut);
	  CLS1_PrintPrompt(io);

	  NanoInit();
	  //ESP_Init();
	  SHELL_Init();

	  WAIT1_Waitms(100);

	  //open TCP/IP connection
	  openESP(io);

	  for(;;){

		  //check for tags filter response type
		  nanoProcess(io);
		  netProcess(io);
		  SHELL_Parse();
		  WAIT1_Waitms(10);
	  }
}


