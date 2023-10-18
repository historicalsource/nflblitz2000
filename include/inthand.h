/*****************************************************************************/
/*                                                                           */
/* FILE: INTHAND.H                                                           */
/*                                                                           */
/* Space Race                                                                */
/*                                                                           */
/* This file contains the interrupt handler functions for the MIPS CPU.      */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

/***** DEFINES ***************************************************************/

#define IOASIC_GLOBAL_INTERRUPT_ENABLE 0x0001
#define IOASIC_GLOBAL_INTERRUPT_STATUS 0x0001
#define IOASIC_FIFO_EMPTY_INTERRUPT_ENABLE 0x0008
#define IOASIC_FIFO_EMPTY_STATUS 0x0008
#define IOASIC_FIFO_FILL_STATUS 0x0020
#define IOASIC_FIFO_ADDR 0xB3000000
#define IOASIC_FIFO_SIZE 512


/***** FUNCTIONS *************************************************************/

int interrupt_ioasic_handler (void);

void inthand_disable_ioasic_interrupt (void);
void inthand_enable_ioasic_interrupt (void);

void inthand_enable_fifo_empty_interrupt (void);
void inthand_disable_fifo_empty_interrupt (void);

void inthand_enable_fifo (void);


/***** END of INTHAND.H ******************************************************/
