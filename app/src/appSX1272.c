/*
 * appSX1272.c
 *
 *  Created on: 25 ao�t 2020
 *      Author: Arnaud
 */

#include "main.h"
#include "appSX1272.h"
#include "SX1272.h"
#include "comSX1272.h"
#include "string.h"
#include "delay.h"



///////////////////////////////////////////////////////////////
// D�claration variables globales
///////////////////////////////////////////////////////////////
static char LgMsg = 0;
static char Message[] = "Salut esclave n";
static float waitPeriod = 0; //en ms
static int cp = 0;  //compteur de paquets transmis
static int type_modulation=TypeModulation;
static uint16_t RegBitRate = BitRate;
static uint16_t RegFdev = Fdev;

// status variables
static int8_t e;
static uint8_t ConfigOK = 1;

///////////////////////////////////////////////////////////////
// Setup function
///////////////////////////////////////////////////////////////
void APP_SX1272_setup()
{
	// Power ON the module
	e = BSP_SX1272_ON(type_modulation);
	if (e == 0)
	{
		my_printf("SX1272 Module on\r\n");
	}
	else
	{
		//my_printfln(F("Problem of activation of SX1272 Module !"));
		ConfigOK = 0;
	}
	// Select frequency channel
	e = BSP_SX1272_setChannel(freq_centrale);
	my_printf("Frequency channel ");
	my_printf("%d",freq_centrale);
	if (e == 0)
	{
		my_printf(" has been successfully set.\r\n");
	}
	else
	{
		my_printf(" has not been set !\r\n");
		ConfigOK = 0;
	}
	// Select output power
	e = BSP_SX1272_setPower(OutPower);
	my_printf("Output power ");
	my_printf("%d",OutPower);
	if (e == 0)
	{
		my_printf(" has been successfully set.\r\n");
	}
	else
	{
		my_printf(" has not been set !\r\n");
		ConfigOK = 0;
	}

	if (ConfigOK == 1)
	{
		//////////////////////////////////////////////////////////////////////
		//config suppl�mentaire mode LORA
		//////////////////////////////////////////////////////////////////////
		if(type_modulation==0)
		{
			// Set header
			e = BSP_SX1272_setHeaderON();
			// Set transmission mode
			e = BSP_SX1272_setCR(paramCR);    // CR_5 : CR = 4/5
			e = BSP_SX1272_setSF(paramSF);   // SF = 12
			e = BSP_SX1272_setBW(paramBW);    // BW = 125 KHz
			// Set CRC
			e = BSP_SX1272_setCRC_ON();
			// Set the node address
			e = BSP_SX1272_setNodeAddress(RX_Addr);
			// Set the length of preamble
			e = BSP_SX1272_setPreambleLength(PreambLong);
			// Set the number of transmission retries
			currentstate._maxRetries = MaxNbRetries;
		}
		//////////////////////////////////////////////////////////////////////
		//config suppl�mentaire mode FSK
		//////////////////////////////////////////////////////////////////////
		else
		{
			// Set CRC
			e = BSP_SX1272_setCRC_ON();
			// Set the node address
			e = BSP_SX1272_setNodeAddress(RX_Addr);
			// Set the length of preamble
			e = BSP_SX1272_setPreambleLength(PreambLong);
			// Set the number of transmission retries
			currentstate._maxRetries = MaxNbRetries;

			BSP_SX1272_Write(REG_SYNC_VALUE1,0x05);
			BSP_SX1272_Write(REG_SYNC_VALUE2,0x05);
			BSP_SX1272_Write(REG_SYNC_VALUE3,0x05);
			BSP_SX1272_Write(REG_SYNC_VALUE4,0x05);

			//Set the frequency deviation an bit rate parameters
			BSP_SX1272_Write(REG_FDEV_MSB,(RegFdev>>8)&0x00FF);// FDA = Fstep*FDEV = 61Hz*Fdev : ex: 0x7FF*61 = 125kHz ex2: 0X52*61=5kHz
			BSP_SX1272_Write(REG_FDEV_LSB,RegFdev&0x00FF);//...
			BSP_SX1272_Write(REG_BITRATE_MSB,(RegBitRate>>8)&0x00FF);//FXOSC=32Mz, BR = FXOSC/(Bitrate + BitrateFrac/16), ex: FXOSC/0x682B = 1200 bauds, ex2: FXOSC/0x200=62.5 kbauds
			BSP_SX1272_Write(REG_BITRATE_LSB,RegBitRate&0x00FF);//...

		}
		my_printf("SX1272 successfully configured !\r\n");
	}
	else
	{
		my_printf("SX1272 initialization failed !\r\n");
	}

	waitPeriod = PeriodTransmission;

	BSP_DELAY_ms(1000);
}

//void APP_SX1272_runTransmit(char Message[6])
//{
//  uint8_t dest_address = TX_Addr;
//
//  //////////////////////////////////////////////////////////////////////////////////
//  // Transmit a packet continuously with a pause of "waitPeriod"
//  if (ConfigOK == 1)
//  {
//
//	my_printf("\r\n START SEND\r\n\n");
//    LgMsg=strlen(Message);
//    e = BSP_SX1272_sendPacketTimeout(dest_address,Message,WaitTxMax);
//    my_printf("\r\n END SEND\r\n\n");
//    if(type_modulation)
//    {
//      BSP_SX1272_Write(REG_OP_MODE, FSK_STANDBY_MODE); // FSK standby mode to switch off the RF field
//    }
//
//    if (e == 0)
//    {
//      my_printf("\n Packet number ");
//      my_printf("%d",cp);
//	  my_printf(" ;Rx node address ");
//	  my_printf("%d\r\n",dest_address);
//      cp++;
//    }
//    else
//    {
//      my_printf("\n Trasmission problem !\r\n");
//    }
//    BSP_DELAY_ms(waitPeriod); //delay to send packet every PeriodTransmission
//  }
//}


void APP_SX1272_runTransmit(uint8_t l_address, char l_message[])
{

	char str[4];
	sprintf(str, "%d", l_address);

	char l_msgStr[strlen(l_message)+3];

	strcpy(l_msgStr,l_message);
	//strcat(l_msgStr,  str);


	//////////////////////////////////////////////////////////////////////////////////
	// Transmit a packet continuously with a pause of "waitPeriod"
	if (ConfigOK == 1)
	{

		LgMsg=strlen(l_msgStr);
		e = BSP_SX1272_sendPacketTimeout(l_address,l_msgStr,WaitTxMax);

		if(type_modulation)
		{
			BSP_SX1272_Write(REG_OP_MODE, FSK_STANDBY_MODE); // FSK standby mode to switch off the RF field
		}
#if (SX1272_debug_mode > 0) // Affichage d'informations sur les paquets dans le terminal
		if (e == 0)
		{
			my_printf("\n Packet number ");
			my_printf("%d",cp);
			my_printf(" ;Rx node address ");
			my_printf("%d\r\n",l_address);
			cp++;
		}
		else
		{
			my_printf("\n Trasmission problem !\r\n");
		}
#endif

		BSP_DELAY_ms(waitPeriod); //delay to send packet every PeriodTransmission
	}
}


char APP_SX1272_runReceive()
{
	char l_StatusRXMessage='0';

	//////////////////////////////////////////////////////////////////////////////////
	// Receive packets continuously
	if (ConfigOK == 1)
	{

		e = BSP_SX1272_receivePacketTimeout(WaitRxMax);

		//paquet reçu, correct ou non
		if (e == 0)
		{
			l_StatusRXMessage = '0';
			if (currentstate._reception == CORRECT_PACKET)
			{
				// Check if the received packet is correct
				// The length and the content of the packet is checked
				// if it is valid, the cpok counter is incremented
				LgMsg=strlen(Message);
				if(currentstate.packet_received.length>=LgMsg)//check the length
				{
					if(memcmp(Message,currentstate.packet_received.data,LgMsg)==0)//check the content
					{
						l_StatusRXMessage = '1';
					}
				}
			}
		}
		// RX Timeout !! No packet received
		else
		{
			l_StatusRXMessage = '2';
		}


#if(SX1272_debug_mode > 0)

		// LORA mode
		if(TypeModulation == 0)
		{
			e = BSP_SX1272_getRSSIpacket();
			my_printf("%d\r\n",currentstate._RSSIpacket);
		}
		// FSK mode
		else
		{
			e = BSP_SX1272_getRSSI(); //done during RX, no packet RSSI available in FSK mode;
			my_printf("RSSI = %d\r\n",currentstate._RSSI);
		}
		//////////////////////////////////////////////////////////////////////////////////
		// Plot receive packets in the serial monitor
		my_printf("StatRXMsg = %d",l_StatusRXMessage);
		my_printf(" ; ");
		my_printf("pckt packnum = %d",currentstate.packet_received.packnum);
		my_printf(" ; ");
		my_printf("pckt length = %d",currentstate.packet_received.length);
		my_printf(" ; ");
		my_printf("\r\n################\r\n msg de %d pour %d => %s\r\n################\r\n",currentstate.packet_received.src, currentstate.packet_received.dst, currentstate.packet_received.data);
		my_printf("\r\n Octet 0 => %c \r\n################\r\n", currentstate.packet_received.data[0]);
#endif
	}

	return l_StatusRXMessage;
}
