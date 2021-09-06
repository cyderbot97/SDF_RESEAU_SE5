/*
 * main.c
 *
 *  Created on: 23 aout 2020
 *      Author: Arnaud
 */

#include "main.h"
#include "stm32f0xx.h"
#include "bsp.h"
#include "comSX1272.h"
#include "SX1272.h"
#include "appSX1272.h"


static void SystemClock_Config();

int main()
{
	// Initialisation des variable locales
	uint8_t l_cpt=0;
	uint8_t l_esclave_num = PREMIER_ESCLAVE;

	uint32_t curtime=0;
	uint32_t i=0;

	uint16_t l_period; // max 60 secondes

	char l_retour_StatusRXMessage=0;
	char *l_msgTest[] ={"A0","B4TEMP","C5LUMI"};




	// Initialize System clock to 48MHz from external clock
	SystemClock_Config();
	// Initialize timebase
	BSP_TIMER_Timebase_Init();
	// Initialize NVIC
	BSP_NVIC_Init();
	// Initialize SPI interface
	BSP_SPI1_Init();
	// Initialize Debug Console
	BSP_Console_Init();

	my_printf("Console ready!\r\n");

	///////////////////////////////////////////
	//setup SX1272
	APP_SX1272_setup();

#if(MODE_MAITRE_ESCLAVE == 1)
	l_period = 10000;
#else
	l_period = 500;
#endif


	while(1)
	{
		curtime=BSP_millis();


		if((curtime%l_period)==0)//send every 100ms
		{

#if(MODE_MAITRE_ESCLAVE == 1)

			//MAITRE
//			APP_SX1272_runTransmit(l_esclave_num, &l_msgTest[0][0]);
			APP_SX1272_runTransmit(l_esclave_num, &l_msgTest[i][0]);

			l_esclave_num +=1;
			l_retour_StatusRXMessage = APP_SX1272_runReceive();

			my_printf("\r\n\r\n Requete => %s a l'esclave n %d", &l_msgTest[i][0],l_esclave_num);
			my_printf("\r\n l_cpt = %d / l_retour_StatusRXMessage = %c \r\n\r\n",l_cpt,l_retour_StatusRXMessage);

			//BSP_DELAY_ms(500);
			AnalyseTramesRecues();

			i++;
			if(i==3)
				i=0;
			if(l_esclave_num > DERNIER_ESCLAVE - 1)
							l_esclave_num = PREMIER_ESCLAVE;

#else
			//ESCLAVE
			l_retour_StatusRXMessage = APP_SX1272_runReceive();
			AnalyseTramesRecues();

#endif

		}



//#if(MODE_MAITRE_ESCLAVE == 1)
//		BSP_DELAY_ms(500);
//
//		if(strncmp("salut",currentstate.packet_received.data,strlen("salut")) == 0)
//			return 0;
//		else
//			return 1;
//
//
//#endif
	}
}

/*
 * Analyse trame
 */
void AnalyseTramesRecues(void)
{
	char l_message[50];
#if(SX1272_debug_mode > 0)
	my_printf("\r\n AnalyseTramesRecues : ");
#endif
#if(MODE_MAITRE_ESCLAVE == 1)
	switch(currentstate.packet_received.data[0])
	{
	case 65:
		if(strncmp("A0present",currentstate.packet_received.data,strlen(currentstate.packet_received.data)) == 0)
			my_printf("\r\n\r\n Presence de l'esclave n %d \r\n\r\n",currentstate.packet_received.src);
		else
			my_printf("\r\n\r\n Requete presence en erreur de l'esclave n %d \r\n\r\n",currentstate.packet_sent.dst);
		break;
	case 66:
		my_printf("\r\n\r\n registre maj de l'esclave n %d => %s \r\n\r\n",currentstate.packet_received.src, currentstate.packet_received.data);
		break;
	case 67:
		my_printf("\r\n\r\n donnees de l'esclave n %d => %s \r\n\r\n",currentstate.packet_received.src, currentstate.packet_received.data);
		break;
	default:
		my_printf("\r\n\r\n octet de reserve aucune action / esclave n %d => %s \r\n\r\n",currentstate.packet_received.src, currentstate.packet_received.data);
		break;
	}
#else
	my_printf("\r\n\r\n Switch case => %d => %c \r\n\r\n",currentstate.packet_received.data[0], currentstate.packet_received.data[0]);
	switch(currentstate.packet_received.data[0])
	{
	case 65:
		sprintf(l_message, "%s", "present");
		APP_SX1272_runTransmit(MAITRE, "A0present");
		break;
	case 66:
		ConfigurationCapteurs(&l_message);
		APP_SX1272_runTransmit(MAITRE, &l_message);
		break;
	case 67:
		LectureCapteur(&l_message[0]);
		APP_SX1272_runTransmit(MAITRE, &l_message);
		break;
	default:
		my_printf("\r\n\r\n octet de reserve aucune action n %d => %s \r\n\r\n",currentstate.packet_received.src, currentstate.packet_received.data);
		break;
	}
	APP_SX1272_runTransmit(MAITRE, &l_message[0]);
#endif


}

/*
 * Configuration des capteurs
 */
void ConfigurationCapteurs(char p_donnee[])
{
	if(strncmp("B4TEMP",currentstate.packet_received.data,strlen(currentstate.packet_received.data)) == 0)
		p_donnee = "B6okTEMP";
	else if(strncmp("B4LUMI",currentstate.packet_received.data,strlen(currentstate.packet_received.data)) == 0)
		p_donnee = "B6okLUMI";
	else
		p_donnee = "B3nok";
		my_printf("\r\n\r\n Configuration capteur \r\n\r\n");
}

/*
 * Lecture d'un capteur
 */
void LectureCapteur(char p_donnee[])
{
	if(strncmp("C4TEMP",currentstate.packet_received.data,strlen(currentstate.packet_received.data)) == 0)
		p_donnee = "C3239";
	else if(strncmp("C4LUMI",currentstate.packet_received.data,strlen(currentstate.packet_received.data)) == 0)
			p_donnee = "C42309";
	else
		p_donnee = "C4none";
	my_printf("\r\n\r\n retour capteur \r\n\r\n");
}

/*
 * 	Clock configuration for the Nucleo STM32F072RB board
 * 	HSE input Bypass Mode 			-> 8MHz
 * 	SYSCLK, AHB, APB1 				-> 48MHz
 *  	PA8 as MCO with /16 prescaler 		-> 3MHz
 *
 *  Laurent Latorre - 05/08/2017
 */

static void SystemClock_Config()
{
	uint32_t	HSE_Status;
	uint32_t	PLL_Status;
	uint32_t	SW_Status;
	uint32_t	timeout = 0;

	timeout = 1000000;

	// Start HSE in Bypass Mode
	RCC->CR |= RCC_CR_HSEBYP;
	RCC->CR |= RCC_CR_HSEON;

	// Wait until HSE is ready
	do
	{
		HSE_Status = RCC->CR & RCC_CR_HSERDY_Msk;
		timeout--;
	} while ((HSE_Status == 0) && (timeout > 0));

	// Select HSE as PLL input source
	RCC->CFGR &= ~RCC_CFGR_PLLSRC_Msk;
	RCC->CFGR |= (0x02 <<RCC_CFGR_PLLSRC_Pos);

	// Set PLL PREDIV to /1
	RCC->CFGR2 = 0x00000000;

	// Set PLL MUL to x6
	RCC->CFGR &= ~RCC_CFGR_PLLMUL_Msk;
	RCC->CFGR |= (0x04 <<RCC_CFGR_PLLMUL_Pos);

	// Enable the main PLL
	RCC-> CR |= RCC_CR_PLLON;

	// Wait until PLL is ready
	do
	{
		PLL_Status = RCC->CR & RCC_CR_PLLRDY_Msk;
		timeout--;
	} while ((PLL_Status == 0) && (timeout > 0));

	// Set AHB prescaler to /1
	RCC->CFGR &= ~RCC_CFGR_HPRE_Msk;
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

	//Set APB1 prescaler to /1
	RCC->CFGR &= ~RCC_CFGR_PPRE_Msk;
	RCC->CFGR |= RCC_CFGR_PPRE_DIV1;

	// Enable FLASH Prefetch Buffer and set Flash Latency
	FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;

	/* --- Until this point, MCU was still clocked by HSI at 8MHz ---*/
	/* --- Switching to PLL at 48MHz Now!  Fasten your seat belt! ---*/

	// Select the main PLL as system clock source
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	// Wait until PLL becomes main switch input
	do
	{
		SW_Status = (RCC->CFGR & RCC_CFGR_SWS_Msk);
		timeout--;
	} while ((SW_Status != RCC_CFGR_SWS_PLL) && (timeout > 0));

	/* --- Here we go! ---*/

	/*--- Use PA8 as MCO output at 48/16 = 3MHz ---*/

	// Set MCO source as SYSCLK (48MHz)
	RCC->CFGR &= ~RCC_CFGR_MCO_Msk;
	RCC->CFGR |=  RCC_CFGR_MCOSEL_SYSCLK;

	// Set MCO prescaler to /16 -> 3MHz
	RCC->CFGR &= ~RCC_CFGR_MCOPRE_Msk;
	RCC->CFGR |=  RCC_CFGR_MCOPRE_DIV16;

	// Enable GPIOA clock
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// Configure PA8 as Alternate function
	GPIOA->MODER &= ~GPIO_MODER_MODER8_Msk;
	GPIOA->MODER |= (0x02 <<GPIO_MODER_MODER8_Pos);

	// Set to AF0 (MCO output)
	GPIOA->AFR[1] &= ~(0x0000000F);
	GPIOA->AFR[1] |=  (0x00000000);

	// Update SystemCoreClock global variable
	SystemCoreClockUpdate();
}
