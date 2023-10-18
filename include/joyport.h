/*****************************************************************************/
/*                                                                           */
/* FILE: JOYPORT.H                                                           */
/*                                                                           */
/* Functions and defines for interfacing to the Nintendo joyport.            */
/*                                                                           */
/* CREATED : July 17, 1998 Peter Giokaris									 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

#ifndef __JOYPORT_H__
#define __JOYPORT_H__

/****** INCLUDES *************************************************************/

/****** DEFINES  *************************************************************/

#define PAGE_SIZE	256
#define NUM_PAGES	128

#define JP_ERR	0
#define JP_OK	1

#define JP_STATUS_DOOR (1<<2)		// If bit is on, door is open...
#define JP_STATUS_CARD (1<<3)		// If bit is on, card is in...
#define JP_OPTO_CLOSE  (1<<4)		// If bit is on, OPTO blocked (door is closed)
#define JP_OPTO_OPEN   (1<<5)		// If bit is on, OPTO blocked (door is open)

#define COMMAND_SOFT_RESET	0x80
#define COMMAND_STATUS		0x81
#define COMMAND_LED_ON		0x82
#define COMMAND_LED_OFF	  	0x83
#define COMMAND_LED_FAST	0x84
#define COMMAND_LED_SLOW  	0x85
#define COMMAND_DOOR_OPEN	0x86
#define COMMAND_DOOR_CLOSE	0x87
#define COMMAND_READ_PAGE	0x88
#define COMMAND_WRITE_PAGE	0x89
#define COMMAND_DOOR_CAL1	0x8A
#define COMMAND_DOOR_CAL2	0x8B
#define COMMAND_DOOR_CAL3	0x8C
#define COMMAND_DOOR_CAL4	0x8D
#define COMMAND_DOOR_OPEN2	0x8E
#define COMMAND_CODE_REV	0x8F

/****** TYPEDEFS *************************************************************/

/***** FUNCTION PROTOTYPES ***************************************************/

int jp_soft_reset ( void );
int jp_status     ( unsigned char *status );

/* * * * */

int jp_LED_on     ( void );
int jp_LED_off    ( void );
int jp_LED_fast   ( void );
int jp_LED_slow   ( void );
int jp_door_open  ( void );
int jp_door_open2 ( void );
int jp_door_close ( void );
int jp_code_rev ( unsigned char * );
int jp_door_open_command (int);

/* * * * */

int jp_read_page   ( unsigned int   page_number, 
					 unsigned char *page );
int jp_write_page  ( unsigned int   page_number, 
				     unsigned char *page );



#endif /* __JOYPORT_H__ */
