/*
 * Shell.c
 *
 *      Author: Erich Styger and Dominic Meierhans
 */

#include "Shell.h"
#include "CLS1.h"
#include "nano.h"
#include "ESP8266.h"


/* table with shell parser/handler */
static const CLS1_ParseCommandCallback CmdParserTable[] =
{
  CLS1_ParseCommand,
  ESP_ParseCommand,
  NANO_ParseCommand,
  NULL /* sentinel */
};

static unsigned char localConsole_buf[96]; /* buffer for command line */

void SHELL_Parse(void) {
  (void)CLS1_ReadAndParseWithCommandTable(localConsole_buf, sizeof(localConsole_buf), CLS1_GetStdio(), CmdParserTable);
}

void SHELL_Init(void) {
  localConsole_buf[0] = '\0'; /* initialize buffer */
}
