/******************************************************************************/
/* Module Name: can.c        												  */
/* Author: Osama Elhout and Mohamed Atef              */
/* Purpose: CAN-BUS module													  */
/******************************************************************************/
#ifndef APP_H
#define APP_H

#include "Hal.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"

#define ECU_period 0.5 //periodicity in s

void Init(void);
uint32_t Read_SW(void);
void State_Machine(void);

#endif
