#ifndef GL_DTE_H
#define GL_DTE_H

extern unsigned char tmtMsg; // timeout na prichadzajucu spravu
extern unsigned char cmdState;
extern unsigned char sysState;
extern unsigned int16 Ks;
extern struct stTime glStTime;

#define MODE_NORMAL   1
#define MODE_SERVICE  2
#define MODE_TIME     4
#define MODE_SLEEP    8

#include "gl_hd.h"

#endif   /* GL_DTE_H */
