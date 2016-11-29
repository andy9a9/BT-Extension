#ifndef BT_H
#define BT_H

//globalne premenne
#define BT_INIT_STATUS 0x02
volatile unsigned char btInitStatus; //stav inicializacie

#define BT_MAX_INIT_TIME 20
#define BT_INIT_COUNT 3
#define BT_MSG_COUNT  12

#define BT_GET_INIT_COUNT(a) a>0?BT_MSG_COUNT:BT_INIT_COUNT
volatile unsigned char btDataCount; //pocet prijatych znakov

// nazov zariadenia
#define BT_NAME        "BT-ext"
#define BT_NAME_LENGTH 8
#define BT_NAME_CMD    "AT+NAME="
#define BT_RESET_CMD   "AT+RESET"

// odpoved od BT modulu
#define BT_RESPOND_OK  "OK"
#define BT_RESPOND_ERR "ERROR"

// definicia prikazov
#define BT_CMD_ON    0x0F
#define BT_CMD_OFF   0xF0

#define BT_IDENT     0xA2

// zoznam podporovanych prikazov
enum eu_bt_commnads {
   BT_CMD_NAME=0x90,
   BT_CMD_FIND,
   BT_CMD_STATE,
   BT_CMD_TIME
};

#define BT_CMD_ERR   0xF1

unsigned char btRXbuffer[12]; // prijimacie buffer pre BT
unsigned char btTXbuffer[10]; // odosielaci buffer pre BT

void initBT(void);
int cmpInitBT(char *data);
int getCmdBT(void);
int setNameBT(unsigned char *newName);
int setTimeBT(unsigned char *newTime);
void sendRespondBT(unsigned char respond);

#include "bt.c"

#endif   /* BT_H */
