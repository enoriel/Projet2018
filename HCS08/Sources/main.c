/* ###################################################################
**     Filename    : main.c
**     Project     : nrf24l01_qe128
**     Processor   : MC9S08QE128CLK
**     Version     : Driver 01.12
**     Compiler    : CodeWarrior HCS08 C Compiler
**     Date/Time   : 2018-06-13, 16:56, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.12
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "SPI1.h"
#include "IIC2.h"
#include "SPI_func.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "RF.h"
T_Buffer TxBuff;
T_Buffer transmite;
T_Buffer receive;
unsigned char Status_check;

/* User includes (#include below this line is not maintained by Processor Expert) */

void main(void)
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  /* For example: for(;;) { } */
	TxBuff.buffer[0] = 0x55;
	TxBuff.buffer[1] = 0x25;
	TxBuff.buffer[2] = 0x34;
	TxBuff.buffer[4] = 0xFF;
	TxBuff.p = &(TxBuff.buffer[0]);
//	TxBuff.n = sizeof(transmite.buffer);
	TxBuff.n = 4;
	Status_check = 0x00;
	/*
	 * Initialisation de la communication SPI
	 */
	SPI_Init_On();

	/*
	 * Configuration du module radio nRF24L01+
	 * --------------------------------------------------------------
	 * Description: Initialisation des paramètres du module
	 * 			- Vitesse de communication: 2Mbps;
	 * 			- Fréquence radio: 2.43GHz
	 * 			- CRC 16 bits
	 * 			- Adresse des canaux Tx et Rx
	 */
	Init_RF();
	configuration_Tx();
	configuration_Rx();
	for(;;){
		Rx_check();
		configuration_reg(W_STATUS,0x70);
		Tx_Send(&TxBuff.buffer[0],2);
	}
  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale HCS08 series of microcontrollers.
**
** ###################################################################
*/
