/*****************************************************************************/
/*                                                                           */
/* FILE: JOYPLAY.H                                                           */
/*                                                                           */
/* Functions and defines for reading playbook data from the Nintendo joyport */
/*                                                                           */
/* CREATED : July 22, 1998 Peter Giokaris									 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

#ifndef __JOYPLAY_H__
#define __JOYPLAY_H__

/****** INCLUDES *************************************************************/

#include "/video/nfl/include/joyport.h"	// Functions for interfacing to the Nintendo joyport.

/****** DEFINES  *************************************************************/

//	If NO_CARD_READER is defined, N64 card reader will be disabled
#define NO_CARD_READER		//	7/8/99 PERRIN: No card reader for Blitz 2000

/* Values that are used by the function 'joyplay_do_function' */
enum {
		JPFUNC_LED_ON = 0,
		JPFUNC_LED_OFF,
		JPFUNC_LED_FLASH_FAST,
		JPFUNC_LED_FLASH_SLOW,
		JPFUNC_DOOR_OPEN,
		JPFUNC_DOOR_OPEN2,
		JPFUNC_DOOR_CLOSE,
		JPFUNC_DOOR_CLOSE2,
		JPFUNC_RESET_STATES
	 };

#define	OPEN_WAIT			60*2
#define	OPEN2_WAIT			60*2
#define	CLOSE_WAIT			60*3

// When creating structures, we must make sure that they are packed
#define PACKED __attribute__((packed))

//#define CP_COUNT			9
//#define CP_NAMELEN			9

/****** TYPEDEFS *************************************************************/

// Control points in a play...
//typedef struct _CapPoint
//{
//	unsigned char	x;
//	unsigned char	y;
//	unsigned char	flags;
//
//} CapPoint, *pCapPoint;

 
// A play structure...
//typedef struct _CapPlay
//	{
//		char			name1[CP_NAMELEN];
//		char			name2[CP_NAMELEN];
//		unsigned char	motion;
//		unsigned char	flags;
//		CapPoint		wpn[3][CP_COUNT];
//
//	} CapPlay, *pCapPlay;


// A package of plays...
typedef struct _PlayBook
	{
//		CapPlay			play[9];
//		char			pad[(((((sizeof(CapPlay)*9)/256)+1)*256)-(sizeof(CapPlay)*9))];
		cap_play_t		play[9];
		char			pad[(((((sizeof(cap_play_t)*9)/256)+1)*256)-(sizeof(cap_play_t)*9))];

	} PlayBook, *pPlayBook;


/***** FUNCTION PROTOTYPES ***************************************************/

/* The first function to get called before doing any joyport reads.			 */
int joyplay_init ( void );	

/* Check to see if a card is in the joyport.								 */
int joyplay_check_card ( void );

/* Checks for a given file on the card.	Returns a file handle on success.	 */
int joyplay_check_file ( void );

/* This function gets a playbook from the memory card. 						 */
int joyplay_get_playbook ( int file_handle, char *name, char *pin, pPlayBook playbook );

/* This function controls the joyports lights and door.						 */
int  joyplay_do_function ( int function );


/*************************/
void joyplay_test ( void );
/*************************/

#endif /* __JOYPLAY_H__ */
