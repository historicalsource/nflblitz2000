/*****************************************************************************/
/*                                                                           */
/* inthand.c - Interrupt handler                                             */
/*             Taken from HYPERDRIVE                                         */
/*                                                                           */
/* Written by:  Michael J. Lynch                                             */
/* Version:     1.00                                                         */
/* Date:        10/11/95                                                     */
/*                                                                           */
/* Copyright (c) 1997 by Midway Video Inc.                                   */
/* All Rights Reserved                                                       */
/*                                                                           */
/* Use, duplication, or disclosure is strictly forbidden unless approved     */
/* in writing by Williams Electronics Games Inc.                             */
/*                                                                           */
/* $Revision: 3 $                                                            */
/*                                                                           */
/*****************************************************************************/

#define INTHAND_DEBUG 0

// turn this on to find Andy's bogus bug
#define INTHAND_STREAM_ERROR_DETECT 1

/***** INCLUDES **************************************************************/

#include <stdio.h>
#include <goose/switch.h>
//#include "include/drivers.h"	
#include "include/inthand.h"	
//#include "include/feedback.h"	

//#include "include/flash.h"

#undef IOASIC_BASE					
#include "/video/biosrom/bios/include/ioctl.h"
													   
#undef O_RDONLY											    
#undef O_WRONLY
#undef O_RDWR
#undef O_CREAT
#undef O_TRUNC
#undef O_APPEND

#include <fcntl.h>
#include <unistd.h>

#include "include/stream.h"	

#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

/***** EXTERNAL PROTOS *******************************************************/

/* the i/o control driver */
//int _ioctl (int, int, int);

/* External functions referenced here */
void galileo_handler (void);


/***** EXTERNS ***************************************************************/

/* Number of ticks per second */
extern int tsec;

/* Flag to tell processor dispatcher that a vertical  */
/* retrace interrupt was received                     */
extern volatile int	got_interrupt;

/* Running tick counter */
extern int	tick_counter;

/* Seconds the program has be running since last reset */
extern int	uptime;

/* Lamp data */
extern int lamp_data;
extern int lamp_fd;

/* LED data */
extern int led_data;
extern int led_fd;

/* Wheel data */
extern int wheel_data;
extern int wheel_fd;

/* A/D  data */
extern int a2d_data[];
extern int a2d_mask;
extern int a2d_fd;


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: inthand_enable_fifo()                                           */
/*                                                                           */
/* This function resets and enables the sound FIFO's.                        */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 24 Aug 97 EJK                                                             */
/*                                                                           */
/*****************************************************************************/

void inthand_enable_fifo(void)

{  /* inthand_enable_fifo() */

int val;

/*****************************************************************************/

	#if INTHAND_DEBUG
    	printf("inthand_enable_fifo(): begin\n");
	#endif        
 
	/* Reset the fifos by bit bang the reset low then high */
    
    /* Get the current sound control register. */
   	_ioctl (4, FIOCGETSOUNDCONTROL, (int)&val);
    
    /* Clear the FIFO flags flow through, FIFO reset, FIFO enable bits */
    val &= ~0x000E;
    
    /* update the sound control register. */
   	_ioctl (4, FIOCSETSOUNDCONTROL, val);
    
    /* Get the current sound control register. */
   	_ioctl (4, FIOCGETSOUNDCONTROL,(int)&val);

    /* Set the FIFO flags flow through, FIFO reset, FIFO enable bits */
    val |= 0x000E;
    
    /* update the sound control register. */
	_ioctl (4, FIOCSETSOUNDCONTROL, val);

	#if INTHAND_DEBUG
    	printf("inthand_enable_fifo(): done\n");
	#endif        
 
} /* inthand_enable_fifo() */

/***** END of inthand_enable_fifo() ******************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: inthand_enable_ioasic_interrupt()                               */
/*                                                                           */
/* Enables the global interrupt from the I/O ASIC.                           */
/* This function allows the I/O ASIC to interrupt the CPU with interrupts.   */
/*                                                                           */
/* 24 Aug 97 EJK                                                             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

void inthand_enable_ioasic_interrupt(void)

{  /* inthand_enable_ioasic_interrupt() */

int val;

/*****************************************************************************/

	#if (INTHAND_DEBUG)
    	printf("inthand_enable_ioasic_interrupt():entering.\n");
	#endif        

	/* get the current I/O ASIC control register */	   
	_ioctl (4, FIOCGETCONTROL, (int)&val);

	/* Turn on Global Interrupt Enable bit */
	val |= IOASIC_GLOBAL_INTERRUPT_ENABLE;

	/* Update the I/O ASIC control register. */
   	_ioctl (4, FIOCSETCONTROL, val);
 
	#if (INTHAND_DEBUG)
    	printf("inthand_enable_ioasic_interrupt():exiting.\n");
	#endif        
 
} /* inthand_enable_ioasic_interrupt() */

/***** END of inthand_enable_ioasic_interrupt() ******************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: inthand_disable_ioasic_interrupt()                              */
/*                                                                           */
/* Disables the global interrupt from the I/O ASIC.                          */
/*                                                                           */
/* 24 Aug 97 EJK                                                             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

void inthand_disable_ioasic_interrupt(void)

{  /* inthand_disable_ioasic_interrupt() */

int val;

/*****************************************************************************/

	#if (INTHAND_DEBUG)
    	printf("inthand_disable_ioasic_interrupt():entering.\n");
	#endif        

	/* get the current I/O ASIC control register */	   
	_ioctl (4, FIOCGETCONTROL, (int)&val);

	/* Turn on Global Interrupt disable bit */
	val &= 0xC000;

	/* Update the I/O ASIC control register. */
   	_ioctl (4, FIOCSETCONTROL, val);
 
	#if (INTHAND_DEBUG)
    	printf("inthand_disable_ioasic_interrupt():exiting.\n");
	#endif        
 
} /* inthand_disable_ioasic_interrupt() */

/***** END of inthand_disable_ioasic_interrupt() ******************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: inthand_enable_fifo_empty_interrupt()                           */
/*                                                                           */
/* Enables the fifo interrupt from the I/O ASIC.                             */
/* This function allows the fifo empty to interrupt the CPU with interrupts. */
/*                                                                           */
/* 24 Aug 97 EJK                                                             */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

void inthand_enable_fifo_empty_interrupt (void)

{  /* inthand_enable_fifo_empty_interrupt() */

int val;

/*****************************************************************************/

	#if (INTHAND_DEBUG)
    	printf("inthand_enable_fifo_empty_interrupt():entering.\n");
	#endif        
 
	/* get the current I/O ASIC control register */	   
	_ioctl (4, FIOCGETCONTROL, (int)&val);

	/* turn on the FIFO empty interrupt */
	val |= IOASIC_FIFO_EMPTY_INTERRUPT_ENABLE;

	/* update the I/O ASIC control register */
   	_ioctl (4, FIOCSETCONTROL, val);
 
	#if (INTHAND_DEBUG)
    	printf("inthand_enable_fifo_empty_interrupt():exiting.\n");
	#endif        
 
} /* inthand_enable_fifo_empty_interrupt() */

/***** END of inthand_enable_fifo_empty_interrupt() **************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: inthand_disable_fifo_empty_interrupt()                          */
/*                                                                           */
/* Turns off the FIFO empty interrupt from the I/O ASIC to the game.         */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 30 Nov 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

void inthand_disable_fifo_empty_interrupt (void)

{  /* inthand_disable_fifo_empty_interrupt() */

int val;

/*****************************************************************************/

	#if (INTHAND_DEBUG)
    	printf("inthand_disable_fifo_empty_interrupt(): begin\n");
	#endif        
 
	/* get the current I/O ASIC control register */	   
	_ioctl (4, FIOCGETCONTROL, (int)&val);

	/* turn off the FIFO empty interrupt */
	val &= ~IOASIC_FIFO_EMPTY_INTERRUPT_ENABLE;

	/* update the I/O ASIC control register */
   	_ioctl (4, FIOCSETCONTROL, val);
 
	#if (INTHAND_DEBUG)
    	printf("inthand_disable_fifo_empty_interrupt(): done\n");
	#endif        
 
} /* inthand_disable_fifo_empty_interrupt() */

/***** END of inthand_disable_fifo_empty_interrupt() *************************/

/*****************************************************************************/
/*                                                                           */
/* ISR: interrupt_ioasic_handler()                                           */
/*                                                                           */
/* This interrupt handler is called whenever there is an interrupt from the  */
/* I/O ASIC. The isr first checks that it has an interrupt from the I/O ASIC */
/* by reading its status register.  Then it checks if this interrupt is the  */
/* FIFO EMPTY interrupt.  Otherwise this handler does NOT know why this      */
/* interrupt happen so just return.                                          */
/*                                                                           */
/* If the code makes it through the first two checks, then we have a valid   */
/* empty FIFO, so then fill the FIFO.                                        */
/*                                                                           */
/* (c) 1997 Midway Games, Inc.                                               */
/*                                                                           */
/* 25 Nov 97 EJK                                                             */
/* 15 Jan 98 MVB - changed case where fill proc is dead - bug fix hopefully  */
/*                                                                           */
/*****************************************************************************/

int interrupt_ioasic_handler (void)

{ /* interrupt_ioasic_handler() */

int val;					/* value returned from the I/O asic registers */

#ifdef DEBUG
	#if INTHAND_STREAM_ERROR_DETECT
		int buffers_free;		/* how many streaming bufs snd sys has free - debug */
	#endif
#endif

/*****************************************************************************/

		#if INTHAND_DEBUG
		printf("interrupt_ioasic_handler():entering\n");
		#endif        

		/* Get the I/O Asic Main Status Register. */
		_ioctl (4, FIOCGETSTATUS, (int)&val);


		/* Check the I/O Asic main status register to see if the interrupt is */
		/* a "Global Interrupt". If not then bitch and return.                */
		/* This will prevent some bogus glitch/code reason why we are in this */
		/* ISR.                                                               */
		if ((val & IOASIC_GLOBAL_INTERRUPT_STATUS) != IOASIC_GLOBAL_INTERRUPT_STATUS)
			{
			#ifdef DEBUG
				fprintf (stderr, "interrupt_ioasic_handler(): ERROR interrupt not 'global'\n\r");
				fprintf (stderr, "I/O ASIC main status register: %x\n\r", val);
				fprintf (stderr, "early exit\n\r");
				fprintf (stderr, "\n\r");
			#endif
			return(0);
			}

		/* Check the I/O Asic main status register to see if the interrupt is */
		/* a "FIFO EMPTY" interrupt. If not then bitch and return.            */
		if ((val & IOASIC_FIFO_EMPTY_STATUS) != IOASIC_FIFO_EMPTY_STATUS)
			{
			#ifdef DEBUG
				fprintf (stderr, "interrupt_ioasic_handler(): ERROR interrupt not 'fifo empty'\n\r");
				fprintf (stderr, "I/O ASIC main status register: %x\n\r", val);
				fprintf (stderr, "early exit\n\r");
				fprintf (stderr, "\n\r");
			#endif
			return(0);
			}

		/* send 512 words to the sound DSP */
//		stream_fifo_isr();

		/* this is a rudimentary check that we did in fact */
		/* get some data into the fifo */    

		/* get the I/O ASIC main status register */
		_ioctl (4, FIOCGETSTATUS,(int)&val);

		if ((val & IOASIC_FIFO_EMPTY_STATUS) == IOASIC_FIFO_EMPTY_STATUS)
			{
			#ifdef DEBUG
				#if INTHAND_STREAM_ERROR_DETECT
					fprintf (stderr, "interrupt_ioasic_handler(): ERROR fifo still empty\n\r");
					fprintf (stderr, "I/O ASIC main status register: %04X\n\r", val);

					/* also get the sound status register */
					_ioctl (4, FIOCGETSOUNDSTATUS,(int)&val);
					fprintf (stderr, "I/O ASIC sound status register: %04X\n\r", val);

					/* for debugging - find out how many of the sound */
					/* system streaming buffers are free */
					snd_query_buffers (&buffers_free);
					fprintf (stderr, "streaming buffers free: %d\n\r", buffers_free);
				#endif
			#endif
			return(0);
			}

        
	#if INTHAND_DEBUG
    	printf("interrupt_ioasic_handler(): done\n");
	#endif        
    
	/* return and tell BIOS that everything's done */
	return (0);
    
} /* interrupt_ioasic_handler() */

/***** END of interrupt_ioasic_handler() *************************************/

/***** END of INTHAND.C ******************************************************/
