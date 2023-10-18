/*****************************************************************************/
/*                                                                           */
/* FILE: JOYPORT.C                                                           */
/*                                                                           */
/* Thif file is the loweset level interface to the Nintendo joyport			 */
/*                                                                           */
/* CREATED : July 17, 1998 Peter Giokaris									 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

/* make this 1 to turn on verbose debugging info */
#define JOYPORT_DBG 0

/***** INCLUDES **************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include	<ioctl.h>
	
/* GOOSE and GLIDE stuff */
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

/* our stuff 			 */
#include "/video/nfl/include/joyport.h"
#include "/video/nfl/include/gameadj.h"

/***** DEFINES ***************************************************************/


/* * * * * * * * * * * * * * */

/* NOTE : THESE DEFINES ARE GOOD FOR BLITZ99 ONLY. ANY OTHER GAME
		  THAT USES THIS CODE WILL REQUIRE CO-ORDINATION WITH 
		  THE RIGHT MEMORY MAP */
#define IOASIC_BASE 			0xb6000000
#define	IOASIC_STATUS			(IOASIC_BASE + (0xd<<2))
#define	IOASIC_UART_RX			(IOASIC_BASE + (0x2<<2))
#define IOASIC_UART_RCV_CHAR	0x1000
/* * * * * * * * * * * * * * */


#define JP_ECHO				0		// Set to zero when card has no echo on...

#define JP_TIMEOUT			200000	// Used for timing out the read...
//#define JP_TIMEOUT			20000000	// Used for timing out the read...

/* * * * * * * * * * * * * * */
#define FLUSH_CHAR		 	'Z'
#define FLUSH_MAX		 	12
/* * * * * * * * * * * * * * */
#define MAX_MESSAGE_OUT 	20
#define READ_MESSAGE_SIZE 	265
/* * * * * * * * * * * * * * */
/* ZERO OUT TO USE DEBUG COMMANDS */
#if 1

/* Header bytes  */
#define HEADER_B1			0xCD
#define HEADER_B2			0xD4
#define HEADER_B3			0xF9

#else

///* Header bytes  */
//#define HEADER_B1			'M'
//#define HEADER_B2			'D'
//#define HEADER_B3			'Y'
//
///* Command bytes */
//#define COMMAND_SOFT_RESET	'1'
//#define COMMAND_STATUS		'2'
//#define COMMAND_LED_ON		'3'
//#define COMMAND_LED_OFF	  	'4'
//#define COMMAND_LED_FAST	'5'
//#define COMMAND_LED_SLOW  	'6'
//#define COMMAND_DOOR_OPEN	'7'
//#define COMMAND_DOOR_CLOSE	'8'
//#define COMMAND_READ_PAGE	'9'
//#define COMMAND_WRITE_PAGE	('9' + 1)
//// #define COMMAND_DOOR_OPEN2	('9' + 2)
//#define COMMAND_CODE_REV	0x8F

#endif 

/***** TYPEDEFS **************************************************************/

/***** EXTERNALS *************************************************************/

extern int _read  ( int fd, char *buf, int cnt );
extern int _write ( int fd, char *buf, int cnt );

/***** GLOBALS   *************************************************************/


/***** PROTOTYPES ************************************************************/

void flush_jp ( void );			// Flushes the jp recieve buffer
void flush_game ( void );		// Flushes the games recieve buffer

void generate_checksum ( unsigned char *msg, 
						 int num_bytes, 
						 unsigned short *chksum );
void send_msg ( unsigned char *msg, int num_bytes );
int recv_msg ( unsigned char *msg, int num_bytes );


static int jp_getc(void);
static int jp_putc(char c);


/*****************************************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  jp_soft_reset()                                                */
/*                                                                           */
/* This function sends a soft reset command 								 */
/*                                                                           */
/* RETURN : 																 */
/*			 JP_OK  : Command was successful								 */
/* 			 JP_ERR : Command failed										 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
int jp_soft_reset ( void )
{
 unsigned char  msg[MAX_MESSAGE_OUT];
 unsigned char  *pchksum;
 unsigned short chksum;
// int   i;
 
 int result = JP_ERR;

 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

 /* Hard-code the message       */
 msg[0] = HEADER_B1;
 msg[1] = HEADER_B2;
 msg[2] = HEADER_B3;
 msg[3] = COMMAND_SOFT_RESET;

 /* generate the checksum       */
 generate_checksum ( msg, 4, &chksum);

 /* put checksum int msg 		*/
 pchksum = (char*)&chksum;
 msg[4]  = pchksum[1];
 msg[5]  = pchksum[0];

 /* send msg out! 				*/
 send_msg(msg, 6);

 /* Wait for ack message 		 */
 result = recv_msg(msg, 7);

 /* Test the results coming back */
 if ( result == JP_OK )
 {

  unsigned short chksum2;
  unsigned char  *pchksum2;

	 pchksum2 = (char*)&chksum2;
	 pchksum2[1] = msg[5];
	 pchksum2[0] = msg[6];
	 generate_checksum ( msg, 5, &chksum);

	 if ( chksum != chksum2 )
	 {
	 	result = JP_ERR;	 

	 } /* if */

 } /* if */

 return result;

} /* jp_soft_reset() */ 

/***** END of jp_soft_reset() ************************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  jp_status()                                                    */
/*                                                                           */
/* This function sends a command for the joyports status.					 */
/*                                                                           */
/* ARG    :  unsigned int *status											 */
/*                                                                           */
/* RETURN : 																 */
/*			 JP_OK  : Command was successful								 */
/* 			 JP_ERR : Command failed										 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
int jp_status ( unsigned char *status )
{
 unsigned char  msg[MAX_MESSAGE_OUT];
 unsigned char  *pchksum;
 unsigned short chksum;
// int   i;
 
 int result = JP_ERR;

 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

 /* Hard-code the message       */
 msg[0] = HEADER_B1;
 msg[1] = HEADER_B2;
 msg[2] = HEADER_B3;
 msg[3] = COMMAND_STATUS;

 /* generate the checksum       */
 generate_checksum ( msg, 4, &chksum);

 /* put checksum int msg 		*/
 pchksum = (char*)&chksum;
 msg[4]  = pchksum[1];
 msg[5]  = pchksum[0];

 /* send msg out! 				*/
 send_msg(msg, 6);

 /* Wait for ack message 		 */
 result = recv_msg(msg, 7);

 /* Test the results coming back */
 if ( result == JP_OK )
 {

  unsigned short chksum2;
  unsigned char  *pchksum2;

	 pchksum2 = (char*)&chksum2;
	 pchksum2[1] = msg[5];
	 pchksum2[0] = msg[6];
	 generate_checksum ( msg, 5, &chksum);

	 if ( chksum != chksum2 )
	 {
	 	result = JP_ERR;	 

	 } /* if */

 } /* if */

 /* Fill the status bit 		*/
 if ( result == JP_OK )
	 *status = msg[4];

 return result;

} /* jp_status() */ 

/***** END of jp_status() ***************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  jp_code_rev()                                                    */
/*                                                                           */
/* This function sends a command to retrieve code revision.						 */
/*                                                                           */
/* ARG    :  unsigned int *status											 */
/*                                                                           */
/* RETURN : 																 */
/*			 JP_OK  : Command was successful								 */
/* 			 JP_ERR : Command failed										 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
int jp_code_rev ( unsigned char *status )
{
 unsigned char  msg[MAX_MESSAGE_OUT];
 unsigned char  *pchksum;
 unsigned short chksum;
// int   i;
 
 int result = JP_ERR;

 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

 /* Hard-code the message       */
 msg[0] = HEADER_B1;
 msg[1] = HEADER_B2;
 msg[2] = HEADER_B3;
 msg[3] = COMMAND_CODE_REV;

 /* generate the checksum       */
 generate_checksum ( msg, 4, &chksum);

 /* put checksum int msg 		*/
 pchksum = (char*)&chksum;
 msg[4]  = pchksum[1];
 msg[5]  = pchksum[0];

 /* send msg out! 				*/
 send_msg(msg, 6);

 /* Wait for ack message 		 */
 result = recv_msg(msg, 7);

 /* Test the results coming back */
 if ( result == JP_OK )
 {

  unsigned short chksum2;
  unsigned char  *pchksum2;

	 pchksum2 = (char*)&chksum2;
	 pchksum2[1] = msg[5];
	 pchksum2[0] = msg[6];
	 generate_checksum ( msg, 5, &chksum);

	 if ( chksum != chksum2 )
	 {
	 	result = JP_ERR;	 

	 } /* if */

 } /* if */

 /* Fill the status bit 		*/
 if ( result == JP_OK )
	 *status = msg[4];

 return result;

} /* jp_code_rev() */ 

/***** END of jp_code_rev() ***************************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  jp_LED_on()	                                                 */
/*                                                                           */
/* This function turns the joyports LED on 									 */
/*                                                                           */
/* RETURN : 																 */
/*			 JP_OK  : Command was successful								 */
/* 			 JP_ERR : Command failed										 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
int jp_LED_on ( void )
{
 unsigned char  msg[MAX_MESSAGE_OUT];
 unsigned char  *pchksum;
 unsigned short chksum;
// int   i;
 
 int result = JP_ERR;

 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

 /* Hard-code the message       */
 msg[0] = HEADER_B1;
 msg[1] = HEADER_B2;
 msg[2] = HEADER_B3;
 msg[3] = COMMAND_LED_ON;

 /* generate the checksum       */
 generate_checksum ( msg, 4, &chksum);

 /* put checksum int msg 		*/
 pchksum = (char*)&chksum;
 msg[4]  = pchksum[1];
 msg[5]  = pchksum[0];

 /* send msg out! 				*/
 send_msg(msg, 6);

 /* Wait for ack message 		 */
 result = recv_msg(msg, 7);

 /* Test the results coming back */
 if ( result == JP_OK )
 {

  unsigned short chksum2;
  unsigned char  *pchksum2;

	 pchksum2 = (char*)&chksum2;
	 pchksum2[1] = msg[5];
	 pchksum2[0] = msg[6];
	 generate_checksum ( msg, 5, &chksum);

	 if ( chksum != chksum2 )
	 {
	 	result = JP_ERR;	 

	 } /* if */

 } /* if */

 return result;

} /* jp_LED_on() */ 

/***** END of jp_LED_on() ****************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  jp_LED_off()	                                                 */
/*                                                                           */
/* This function turns the joyports LED off									 */
/*                                                                           */
/* RETURN : 																 */
/*			 JP_OK  : Command was successful								 */
/* 			 JP_ERR : Command failed										 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
int jp_LED_off ( void )
{
 unsigned char  msg[MAX_MESSAGE_OUT];
 unsigned char  *pchksum;
 unsigned short chksum;
// int   i;
 
 int result = JP_ERR;

 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

 /* Hard-code the message       */
 msg[0] = HEADER_B1;
 msg[1] = HEADER_B2;
 msg[2] = HEADER_B3;
 msg[3] = COMMAND_LED_OFF;

 /* generate the checksum       */
 generate_checksum ( msg, 4, &chksum);

 /* put checksum int msg 		*/
 pchksum = (char*)&chksum;
 msg[4]  = pchksum[1];
 msg[5]  = pchksum[0];

 /* send msg out! 				*/
 send_msg(msg, 6);

 /* Wait for ack message 		 */
 result = recv_msg(msg, 7);

 /* Test the results coming back */
 if ( result == JP_OK )
 {

  unsigned short chksum2;
  unsigned char  *pchksum2;

	 pchksum2 = (char*)&chksum2;
	 pchksum2[1] = msg[5];
	 pchksum2[0] = msg[6];
	 generate_checksum ( msg, 5, &chksum);

	 if ( chksum != chksum2 )
	 {
	 	result = JP_ERR;	 

	 } /* if */

 } /* if */

 return result;

} /* jp_LED_off() */ 

/***** END of jp_LED_off() ***************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  jp_LED_fast()	                                                 */
/*                                                                           */
/* This function makse the joyport LED flash fast.							 */
/*                                                                           */
/* RETURN : 																 */
/*			 JP_OK  : Command was successful								 */
/* 			 JP_ERR : Command failed										 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
int jp_LED_fast ( void )
{
 unsigned char  msg[MAX_MESSAGE_OUT];
 unsigned char  *pchksum;
 unsigned short chksum;
// int   i;
 
 int result = JP_ERR;

// unsigned char msg2[MAX_MESSAGE_OUT];
// int tries;

 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

 /* Hard-code the message       */
 msg[0] = HEADER_B1;
 msg[1] = HEADER_B2;
 msg[2] = HEADER_B3;
 msg[3] = COMMAND_LED_FAST;

 /* generate the checksum       */
 generate_checksum ( msg, 4, &chksum);

 /* put checksum int msg 		*/
 pchksum = (char*)&chksum;
 msg[4]  = pchksum[1];
 msg[5]  = pchksum[0];

 /* send msg out! 				 */
 send_msg(msg, 6);

 /* Wait for ack message 		 */
 result = recv_msg(msg, 7);

 /* Test the results coming back */
 if ( result == JP_OK )
 {

  unsigned short chksum2;
  unsigned char  *pchksum2;

	 pchksum2 = (char*)&chksum2;
	 pchksum2[1] = msg[5];
	 pchksum2[0] = msg[6];
	 generate_checksum ( msg, 5, &chksum);

	 if ( chksum != chksum2 )
	 {
	 	result = JP_ERR;	 

	 } /* if */

 } /* if */

 return result;

} /* jp_LED_fast() */ 

/***** END of jp_LED_fast() **************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  jp_LED_slow()	                                                 */
/*                                                                           */
/* This function makes the joyport LED flash slow.							 */
/*                                                                           */
/* RETURN : 																 */
/*			 JP_OK  : Command was successful								 */
/* 			 JP_ERR : Command failed										 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
int jp_LED_slow ( void )
{
 unsigned char  msg[MAX_MESSAGE_OUT];
 unsigned char  *pchksum;
 unsigned short chksum;
// int   i;
 
 int result = JP_ERR;

 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

 /* Hard-code the message       */
 msg[0] = HEADER_B1;
 msg[1] = HEADER_B2;
 msg[2] = HEADER_B3;
 msg[3] = COMMAND_LED_SLOW;

 /* generate the checksum        */
 generate_checksum ( msg, 4, &chksum);

 /* put checksum int msg 		 */
 pchksum = (char*)&chksum;
 msg[4]  = pchksum[1];
 msg[5]  = pchksum[0];

 /* send msg out! 				 */
 send_msg(msg, 6);

 /* Wait for ack message 		 */
 result = recv_msg(msg, 7);

 /* Test the results coming back */
 if ( result == JP_OK )
 {

  unsigned short chksum2;
  unsigned char  *pchksum2;

	 pchksum2 = (char*)&chksum2;
	 pchksum2[1] = msg[5];
	 pchksum2[0] = msg[6];
	 generate_checksum ( msg, 5, &chksum);

	 if ( chksum != chksum2 )
	 {
	 	result = JP_ERR;	 

	 } /* if */

 } /* if */

 return result;

} /* jp_LED_slow() */ 

/***** END of jp_LED_slow() **************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  jp_door_open()                                                 */
/*                                                                           */
/* This function opens the joyport door.									 */
/*                                                                           */
/* RETURN : 																 */
/*			 JP_OK  : Command was successful								 */
/* 			 JP_ERR : Command failed										 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
int jp_door_open ( void )
{
 unsigned char  msg[MAX_MESSAGE_OUT];
 unsigned char  *pchksum;
 unsigned short chksum;
 int   i;
 
 int result = JP_ERR;

 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

 /* Hard-code the message       */
 msg[0] = HEADER_B1;
 msg[1] = HEADER_B2;
 msg[2] = HEADER_B3;

 if (get_adjustment(DOOR_OPEN_ADJ, &i))
// 	msg[3] = COMMAND_DOOR_OPEN;
 	i= COMMAND_DOOR_OPEN;
 else if ((i != COMMAND_DOOR_OPEN) &&
 			 (i < COMMAND_DOOR_CAL1) && (i > COMMAND_DOOR_CAL4))
 	i= COMMAND_DOOR_OPEN;

  msg[3] = (unsigned char)i;

 fprintf (stderr,"\nOPEN COMMAND: %X\n", msg[3]);


 /* generate the checksum       */
 generate_checksum ( msg, 4, &chksum);

 /* put checksum int msg 		*/
 pchksum = (char*)&chksum;
 msg[4]  = pchksum[1];
 msg[5]  = pchksum[0];

 /* send msg out! 				*/
 send_msg(msg, 6);

 /* Wait for ack message 		 */
 result = recv_msg(msg, 7);

 /* Test the results coming back */
 if ( result == JP_OK)
 {

  unsigned short chksum2;
  unsigned char  *pchksum2;

	 pchksum2 = (char*)&chksum2;
	 pchksum2[1] = msg[5];
	 pchksum2[0] = msg[6];
	 generate_checksum ( msg, 5, &chksum);

	 if ( chksum != chksum2 )
	 {
	 	result = JP_ERR;	 

	 } /* if */

 } /* if */

 return result;

} /* jp_door_open() */ 

/***** END of jp_door_open() *************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  jp_door_open2()                                                 */
/*                                                                           */
/* This function opens the joyport door farther to release magnet.			 */
/*                                                                           */
/* RETURN : 																 */
/*			 JP_OK  : Command was successful								 */
/* 			 JP_ERR : Command failed										 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
int jp_door_open2 ( void )
{
 unsigned char  msg[MAX_MESSAGE_OUT];
 unsigned char  *pchksum;
 unsigned short chksum;
// int   i;
 
 int result = JP_ERR;

 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

 /* Hard-code the message       */
 msg[0] = HEADER_B1;
 msg[1] = HEADER_B2;
 msg[2] = HEADER_B3;
 msg[3] = COMMAND_DOOR_OPEN2;

 /* generate the checksum       */
 generate_checksum ( msg, 4, &chksum);

 /* put checksum int msg 		*/
 pchksum = (char*)&chksum;
 msg[4]  = pchksum[1];
 msg[5]  = pchksum[0];

 /* send msg out! 				*/
 send_msg(msg, 6);

 /* Wait for ack message 		 */
 result = recv_msg(msg, 7);

 /* Test the results coming back */
 if ( result == JP_OK )
 {

  unsigned short chksum2;
  unsigned char  *pchksum2;

	 pchksum2 = (char*)&chksum2;
	 pchksum2[1] = msg[5];
	 pchksum2[0] = msg[6];
	 generate_checksum ( msg, 5, &chksum);

	 if ( chksum != chksum2 )
	 {
	 	result = JP_ERR;	 

	 } /* if */

 } /* if */

 return result;

} /* jp_door_open2() */ 

/***** END of jp_door_open2() *************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  jp_door_open_command()                                                 */
/*                                                                           */
/* This function sends an open command the joyport door.							 */
/*                                                                           */
/* RETURN : 																 */
/*			 JP_OK  : Command was successful								 */
/* 			 JP_ERR : Command failed										 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
int jp_door_open_command (int command)
{
 unsigned char  msg[MAX_MESSAGE_OUT];
 unsigned char  *pchksum;
 unsigned short chksum;
// int   i;
 
 int result = JP_ERR;

 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

 /* Hard-code the message       */
 msg[0] = HEADER_B1;
 msg[1] = HEADER_B2;
 msg[2] = HEADER_B3;
 msg[3] = command;

 /* generate the checksum       */
 generate_checksum ( msg, 4, &chksum);

 /* put checksum int msg 		*/
 pchksum = (char*)&chksum;
 msg[4]  = pchksum[1];
 msg[5]  = pchksum[0];

 /* send msg out! 				*/
 send_msg(msg, 6);

 /* Wait for ack message 		 */
 result = recv_msg(msg, 7);

 /* Test the results coming back */
 if ( result == JP_OK)
 {

  unsigned short chksum2;
  unsigned char  *pchksum2;

	 pchksum2 = (char*)&chksum2;
	 pchksum2[1] = msg[5];
	 pchksum2[0] = msg[6];
	 generate_checksum ( msg, 5, &chksum);

	 if ( chksum != chksum2 )
	 {
	 	result = JP_ERR;	 

	 } /* if */

 } /* if */

 return result;

} /* jp_door_open_command() */ 

/***** END of jp_door_open_command() *************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  jp_door_close()                                                */
/*                                                                           */
/* This function closes the joyport door.									 */
/*                                                                           */
/* RETURN : 																 */
/*			 JP_OK  : Command was successful								 */
/* 			 JP_ERR : Command failed										 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
int jp_door_close ( void )
{
 unsigned char  msg[MAX_MESSAGE_OUT];
 unsigned char  *pchksum;
 unsigned short chksum;
// int   i;
 
 int result = JP_ERR;

 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

 /* Hard-code the message       */
 msg[0] = HEADER_B1;
 msg[1] = HEADER_B2;
 msg[2] = HEADER_B3;
 msg[3] = COMMAND_DOOR_CLOSE;

 /* generate the checksum       */
 generate_checksum ( msg, 4, &chksum);

 /* put checksum int msg 		*/
 pchksum = (char*)&chksum;
 msg[4]  = pchksum[1];
 msg[5]  = pchksum[0];

 /* send msg out! 				*/
 send_msg(msg, 6);

 /* Wait for ack message 		 */
 result = recv_msg(msg, 7);

 /* Test the results coming back */
 if ( result == JP_OK )
 {

  unsigned short chksum2;
  unsigned char  *pchksum2;

	 pchksum2 = (char*)&chksum2;
	 pchksum2[1] = msg[5];
	 pchksum2[0] = msg[6];
	 generate_checksum ( msg, 5, &chksum);

	 if ( chksum != chksum2 )
	 {
	 	result = JP_ERR;	 

	 } /* if */

 } /* if */


 return result;

} /* jp_door_close() */ 

/***** END of jp_door_close() ************************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  jp_read_page()                                                 */
/*                                                                           */
/* This function reads a page out of the card.								 */
/*                                                                           */
/* RETURN : 																 */
/*			 JP_OK  : Command was successful								 */
/* 			 JP_ERR : Command failed										 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
int jp_read_page ( unsigned int page_number, unsigned char *page )
{
 unsigned char  msg[MAX_MESSAGE_OUT];
 unsigned char  msg2[READ_MESSAGE_SIZE];
 unsigned char  *pchksum;
 unsigned short chksum;
 int   i;
 
 int result = JP_ERR;

 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

 /* make sure that the page is valid */
 if ( page_number >= NUM_PAGES )
	return result;

 /* Hard-code the message       */
 msg[0] = HEADER_B1;
 msg[1] = HEADER_B2;
 msg[2] = HEADER_B3;
 msg[3] = COMMAND_READ_PAGE;
 msg[4] = (unsigned int)page_number;

 /* generate the checksum       */
 generate_checksum ( msg, 5, &chksum);

 /* put checksum int msg 		*/
 pchksum = (char*)&chksum;
 msg[5]  = pchksum[1];
 msg[6]  = pchksum[0];

 /* send msg out! 				*/
 send_msg(msg, 7);

 /* Wait for ack message 		*/
 result = recv_msg(msg2, READ_MESSAGE_SIZE);

 /* Test the results coming back */
 if ( result == JP_OK )
 {

  unsigned short chksum2;
  unsigned char  *pchksum2;

	 pchksum2 = (char*)&chksum2;
	 pchksum2[1] = msg2[READ_MESSAGE_SIZE-2];
	 pchksum2[0] = msg2[READ_MESSAGE_SIZE-1];
	 generate_checksum ( msg2, (READ_MESSAGE_SIZE-2), &chksum);

	 if ( chksum != chksum2 )
	 {
	 	result = JP_ERR;	 

	 } /* if */

 } /* if */
 

 /* Copy page 					*/
 for ( i = 0; i < PAGE_SIZE; i++ )
	page[i] = msg2[i+6];

 return result;

} /* jp_read_page() */ 

/***** END of jp_read_page() *************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  jp_write_page()                                                */
/*                                                                           */
/* This function writes a page out to the card.								 */
/*                                                                           */
/* RETURN : 																 */
/*			 JP_OK  : Command was successful								 */
/* 			 JP_ERR : Command failed										 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
int jp_write_page ( unsigned int page_number, unsigned char *page )
{
 unsigned char  msg[MAX_MESSAGE_OUT+PAGE_SIZE];
 unsigned char  *pchksum;
 unsigned short chksum;
 int   i;
 
 int result = JP_ERR;

 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

 /* make sure that the page is valid   */
 if ( page_number >= NUM_PAGES )
	return result;

 /* Hard-code the message       */
 msg[0] = HEADER_B1;
 msg[1] = HEADER_B2;
 msg[2] = HEADER_B3;
 msg[3] = COMMAND_WRITE_PAGE;
 msg[4] = (unsigned int)page_number;

 /* generate the checksum       */
 generate_checksum ( msg, 5, &chksum);

 /* put checksum int msg 		*/
 pchksum = (char*)&chksum;
 msg[5]  = pchksum[1];
 msg[6]  = pchksum[0];

 /* send msg out! 				*/
 send_msg(msg, 7);

 /* Wait for ack message 		*/
 result = JP_OK;

 /* Send page over 				*/
 for ( i = 0; i < PAGE_SIZE; i++ )
 {
	jp_putc(page[i]);
 }

 /* Wait for ack message 		*/


 return result;


return result;

} /* jp_read_page() */ 

/***** END of jp_read_page() *************************************************/


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  flush_jp()                                                     */
/*                                                                           */
/* Flushes the joyports recieve buffer 										 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

void flush_jp ( void )

{ /* flush_jp() */

 int i;

 for ( i = 0; i < FLUSH_MAX; i++ )
 {
	 jp_putc(FLUSH_CHAR);
 
 } /* for */
 
} /* flush_jp() */

/***** END of flush_jp() *****************************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  flush_game()                                                   */
/*                                                                           */
/* Flushes the games receive buffer. 										 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

void flush_game ( void )

{ /* flush_game() */

	while ( jp_getc() != (-1) ) ;

} /* flush_game() */

/***** END of flush_game() ***************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  generate_checksum()                                            */
/*                                                                           */
/* Create a checksum value to put into msg									 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

void generate_checksum ( unsigned char *msg, int num_bytes, unsigned short *chksum )

{ /* generate_checksum() */

 int i;

 /* clear the checksum value */
 *chksum = 0;

 /* generate checksum        */
 for ( i = 0; i < num_bytes; i++ )
 {
	*chksum += msg[i];

 } /* for */

} /* generate_checksum() */

/***** END of generate_checksum() *********************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  send_msg()                                                     */
/*                                                                           */
/* Sends out the message 													 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

void send_msg ( unsigned char *msg, int num_bytes )

{ /* send_msg() */

 int i,j;

 /* Flush out the joyports recieve buffer */
 flush_jp();

 /* Flush the games internal UART buffers */
 flush_game();

 /* Send out the message 				  */ 
 for ( i = 0; i < num_bytes; i++ )
 {

	/* Case : if i == (MAX_MESSAGE_OUT-1), break!         */
	if ( i == (MAX_MESSAGE_OUT-1) )
	{
		break;	

	} /* if */

	/* Send out one char of the message 				  */
	jp_putc(msg[i]);

    /* If echo on card is turned on 					  */
    if ( JP_ECHO == 1 ) 
	{

		/* Delay! */
		for(j= 0; j < 200000; j++) ;

		while(1)
		{

			if ( jp_getc() != (-1) )
				break;

		} /* while */

	} /* if */
	
 } /* for */

} /* send_msg() */

/***** END of send_msg() *****************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  recv_msg()                                                     */
/*                                                                           */
/* Recieves a message from the card.										 */
/*                                                                           */
/* RETURN : 																 */
/*			 JP_OK  : Command was successful								 */
/* 			 JP_ERR : Command failed										 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

int recv_msg ( unsigned char *msg, int num_bytes )

{ /* recv_msg() */
 
 int result = JP_OK;
 int i, j;
 int timeout;

 	i = 0;
 	timeout = JP_TIMEOUT;

 	/* go into loop      */
 	while ( i < num_bytes )
 	{

  	 int c;

  	 	msg[i] = 0;

		/* get a character */
		c = jp_getc();
	
		/* Are we still waiting for a character to come into buffer? */
		if ( c == -1 )
		{
			timeout--;
			if ( timeout <= 0 )
			{
				// Underrun...
				result = JP_ERR;				
				break;

			} /* if timeout */

		} /* if */
		else
		{
			msg[i] = (unsigned char)c;
			timeout = JP_TIMEOUT;
			i++;

		} /* else */

 	} /* while */

    // Delay (for possible last character)
	for(j= 0; j < 100000; j++) ;

	// Overrun...
	if (( jp_getc() != -1 ) && ( result != JP_ERR ))
	{

		/* Flush EVERRRYYYTHINGGGGG.... */
		flush_jp();
		flush_game();
		result = JP_ERR;				

	} /* if */

 return result;

} /* recv_msg() */

/***** END of recv_msg() *****************************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  jp_getc()                                                      */
/*                                                                           */
/* Gets a character from the serial port.									 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

static int UARTgetc(void);

static int jp_getc(void)

{ /* int jp_getc() */

 return UARTgetc();

} /* jp_getc() */

/***** END of jp_getc() *****************************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  jp_getc()                                                      */
/*                                                                           */
/* Sends a character to the serial port.									 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

static int jp_putc(char c)

{ /* int jp_putc() */

	_write ( 1, &c, 1);

 return JP_OK;

} /* jp_putc() */

/***** END of jp_putc() ******************************************************/




/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  UARTgetc()                                                     */
/*                                                                           */
/* low-level function to read directly from the UART 						 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

static int UARTgetc(void)
{
	unsigned short val;

	// Get UART status  
	val = *((volatile short *)IOASIC_STATUS);
  
	// Is data available ?
	if(val & IOASIC_UART_RCV_CHAR) 
	{
		val = *((volatile short *)IOASIC_UART_RX);
		return(val & 0x00ff);
	}

	// No data - return -1
	return(-1);

}


/***** END of joyport.c ******************************************************/
