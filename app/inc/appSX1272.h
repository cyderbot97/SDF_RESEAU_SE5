/*
 * appSX1272.h
 *
 *  Created on: 25 ao�t 2020
 *      Author: Arnaud
 */

#ifndef APP_INC_APPSX1272_H_
#define APP_INC_APPSX1272_H_

#include "SX1272.h"

/******************************************************************************
 * Definitions & Declarations
 *****************************************************************************/

#define MODE_MAITRE_ESCLAVE	1 // Maitre = 1 et Esclave = 0

#define MAITRE 				100
#define PREMIER_ESCLAVE 	101
#define DERNIER_ESCLAVE 	103

///////////////////////////////////////////////////////////////
// Configuration du module en mode LoRA ou FSK
///////////////////////////////////////////////////////////////

#if(MODE_MAITRE_ESCLAVE == 1)

#define TX_Addr 101 //address of the first slave
#define RX_Addr MAITRE //address of the master

#else

#define TX_Addr MAITRE //address of the master
//#define RX_Addr 101 //address of the slave
#define RX_Addr 102 //address of the slave
//#define RX_Addr 103 //address of the slave
//#define RX_Addr 104 //address of the slave
//#define RX_Addr 105 //address of the slave
//#define RX_Addr 106 //address of the slave
//#define RX_Addr 107 //address of the slave
//#define RX_Addr 108 //address of the slave
//#define RX_Addr 109 //address of the slave
//#define RX_Addr 110 //address of the slave

#endif


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

extern SX1272status currentstate;

void APP_SX1272_setup();
//void APP_SX1272_runTransmit(char Message[6]);
void APP_SX1272_runTransmit(uint8_t l_address, char l_message[]);
//void APP_SX1272_runTransmitSlave(uint8_t l_address);
char APP_SX1272_runReceive();

#endif /* APP_INC_APPSX1272_H_ */
