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

static uint8_t SendWebPage(uint8_t ch_id, bool ledIsOn, uint8_t temperature, const CLS1_StdIOType *io) {
  static uint8_t http[1024];
  uint8_t cmd[24], rxBuf[48], expected[48];
  uint8_t buf[16];
  uint8_t res = ERR_OK;

  /* construct web page content */
  UTIL1_strcpy(http, sizeof(http), (uint8_t*)"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n");
  UTIL1_strcat(http, sizeof(http), (uint8_t*)"<html>\r\n<body>\r\n");
  UTIL1_strcat(http, sizeof(http), (uint8_t*)"<title>ESP8266 Web Server</title>\r\n");
  UTIL1_strcat(http, sizeof(http), (uint8_t*)"<h2>Web Server using ESP8266</h2>\r\n");
  UTIL1_strcat(http, sizeof(http), (uint8_t*)"<br /><hr>\r\n");
  UTIL1_strcat(http, sizeof(http), (uint8_t*)"<p><form method=\"POST\"><strong>Temp: <input type=\"text\" size=2 value=\"");
  UTIL1_strcatNum8s(http, sizeof(http), temperature);
  UTIL1_strcat(http, sizeof(http), (uint8_t*)"\"> <sup>O</sup>C");
  if (ledIsOn) {
    UTIL1_strcat(http, sizeof(http), (uint8_t*)"<p><input type=\"radio\" name=\"radio\" value=\"0\" >Red LED off");
    UTIL1_strcat(http, sizeof(http), (uint8_t*)"<br><input type=\"radio\" name=\"radio\" value=\"1\" checked>Red LED on");
  } else {
    UTIL1_strcat(http, sizeof(http), (uint8_t*)"<p><input type=\"radio\" name=\"radio\" value=\"0\" checked>Red LED off");
    UTIL1_strcat(http, sizeof(http), (uint8_t*)"<br><input type=\"radio\" name=\"radio\" value=\"1\" >Red LED on");
  }
  UTIL1_strcat(http, sizeof(http), (uint8_t*)"</strong><p><input type=\"submit\"></form></span>");
  UTIL1_strcat(http, sizeof(http), (uint8_t*)"</body>\r\n</html>\r\n");

  UTIL1_strcpy(cmd, sizeof(cmd), "AT+CIPSEND="); /* parameters are <ch_id>,<size> */
  UTIL1_strcatNum8u(cmd, sizeof(cmd), ch_id);
  UTIL1_chcat(cmd, sizeof(cmd), ',');
  UTIL1_strcatNum16u(cmd, sizeof(cmd), UTIL1_strlen(http));
  UTIL1_strcpy(expected, sizeof(expected), cmd); /* we expect the echo of our command */
  UTIL1_strcat(expected, sizeof(expected), "\r\r\n> "); /* expect "> " */
  UTIL1_strcat(cmd, sizeof(cmd), "\r\n");
  res = ESP_SendATCommand(cmd, rxBuf, sizeof(rxBuf), expected, ESP_DEFAULT_TIMEOUT_MS, io);
  if (res!=ERR_OK) {
    if (io!=NULL) {
      CLS1_SendStr("INFO: TIMEOUT, closing connection!\r\n", io->stdOut);
    }
  } else {
    if (io!=NULL) {
      CLS1_SendStr("INFO: Sending http page...\r\n", io->stdOut);
    }
    UTIL1_strcat(http, sizeof(http), "\r\n\r\n"); /* need to add this to end the command! */
    res = ESP_SendATCommand(http, NULL, 0, NULL, ESP_DEFAULT_TIMEOUT_MS, io);
    if (res!=ERR_OK) {
      CLS1_SendStr("Sending page failed!\r\n", io->stdErr); /* copy on console */
    } else {
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
  return res;
}

static void WebProcess(const CLS1_StdIOType *io) {
  uint8_t res=ERR_OK;
  bool isGet;
  uint8_t ch_id=0;
  uint16_t size=0;
  const uint8_t *p;
  //const CLS1_StdIOType *io;

  if(ESP_IsServerOn()){
	  res = ESP_ReadCharsUntil(APP_EspMsgBuf, sizeof(APP_EspMsgBuf), '\n', 10);

	  if(UTIL1_strcmp(APP_EspMsgBuf, (char *)"Nano_Start") == ERR_OK){
		  CLS1_SendStr("Nano Start\r\n", io->stdOut);
	  }
	  UTIL1_strcat(APP_EspMsgBuf, sizeof(APP_EspMsgBuf), "\0");
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
	uint8_t res = ERR_OK;
	uint8_t buf[64];
	uint8_t ch_id = 1;

	uint8_t ssid[10];
	uint8_t pwd[10];

	//TODO use macros
	uint8_t IPAddrStr[20] = "192.168.0.103";
	uint16_t port = 8000;
	uint16_t msTimeout = 1000;

	UTIL1_strcpy(ssid, sizeof(ssid), "tsh_net");
	UTIL1_strcpy(pwd, sizeof(pwd), "12345678");

	//reset ESP
	//ESP_SendATCommand("AT+RESTORE", NULL, 0, NULL, ESP_DEFAULT_TIMEOUT_MS, io);

	//WAIT1_Waitms(2000);

	//TODO error handling
	//connect to AP
	if(ESP_JoinAP(ssid, pwd, 4, io) != ERR_OK) CLS1_SendStr("AP not joined -> reset for retry\r\n", io->stdOut);

	if(ESP_SelectMode(3) != ERR_OK) CLS1_SendStr("Select Mode error -> reset for retry\r\n", io->stdOut);

	//open connection to server
	if(ESP_OpenConnection(ch_id, 1, IPAddrStr, port, msTimeout, io) != ERR_OK)
		CLS1_SendStr("Open connection failed -> reset for retry\r\n", io->stdOut);
	/*
	//send gate nr to server //TODO GATE NR in FLASH config over SHELL
	UTIL1_strcpy(buf, sizeof(buf), "GATE01\r\n");
	if(ESP_PrepareMsgSend(ch_id, UTIL1_strlen(buf), 3000, io) != ERR_OK)
		CLS1_SendStr("Prepare message failed -> reset for retry\r\n", io->stdOut);
	if(ESP_SendATCommand(buf, NULL, 0, NULL, ESP_DEFAULT_TIMEOUT_MS, io) != ERR_OK)
		CLS1_SendStr("Send message failed -> reset for retry\r\n", io->stdOut);

	*/
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
	  //ESP_StartWebServer(io);

	  for(;;){

		  //if reader is started search for TSHs
		  if (nanoStatus()) nanoProcess(io);
		  //check for new commands over serial
		  SHELL_Parse();

		  //commands over network
		  //ESP_Parse();

		  WebProcess(io);
		  WAIT1_Waitms(5);
	  }
}


