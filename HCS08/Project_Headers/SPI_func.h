#ifndef __SPI
#define __SPI

/* MODULE SPI. */

/*Include shared modules, which are used for whole project*/
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* Include inherited components */
#include "RF.h"
#include "Cpu.h"

/* Define buffer size depending of your use of it */
#define SizeBuffer 5
/* Buffer Receiver & Transmitter */
typedef struct BufferC {
	unsigned char buffer[SizeBuffer];
	unsigned char *p;
	unsigned char n;
}T_Buffer;

//T_Buffer transmite;
extern T_Buffer receive;
extern T_Buffer transmite;


void SPI_Send(T_Buffer *transmite, unsigned char i);
void SPI_Init_On(void);

/* END SPI. */
#endif
