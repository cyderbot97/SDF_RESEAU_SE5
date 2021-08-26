/*
 * appSX1272.h
 *
 *  Created on: 25 ao�t 2020
 *      Author: Arnaud
 */

#ifndef APP_INC_APPSX1272_H_
#define APP_INC_APPSX1272_H_

#include "SX1272.h"

///////////////////////////////////////////////////////////////
// Configuration du module en mode LoRA ou FSK
///////////////////////////////////////////////////////////////

//Main parameters (to be changed)
#define TX_Addr 100//BROADCAST_ADDR
#define RX_Addr 100 //address of the node

#define OutPower POW_14 //min -1, max 14
#define TypeModulation 1 //0: LORA, 1:FSK
#define PeriodTransmission 100 //en ms

//LORA configuration (to be changed)
#define paramBW BW_125 //BW_125, BW_250 or BW_500
#define paramCR CR_5 //CR_5
#define paramSF SF_12 //SF_7 to SF_12

//FSK configuration (to be changed)
#define BitRate BR1200bauds //BR1200bauds or BR4800bauds
#define Fdev FDA5kHz//FDA5kHz

//Other common parameters (do not change!!)
#define PreambLong 12//12
#define freq_centrale CH_868v3
#define MaxNbRetries 3
#define WaitTxMax 500 //en ms
#define WaitRxMax 7000 //en ms

void APP_SX1272_setup();
void APP_SX1272_runTransmit(char Message[6]);
void APP_SX1272_runTransmitSlave(uint8_t l_address);
int APP_SX1272_runReceive();

#endif /* APP_INC_APPSX1272_H_ */
