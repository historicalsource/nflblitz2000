/*****************************************************************************/
/*                                                                           */
/* FILE: JOYPLAY.C                                                           */
/*                                                                           */
/* This file hooks into the joyport to read the play data					 */
/*                                                                           */
/* CREATED : July 22, 1998 Peter Giokaris									 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

/* make this 1 to turn on verbose debugging info */
#define JOYPLAY_DBG 0

/***** INCLUDES **************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
	
/* GOOSE and GLIDE stuff */
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

/* our stuff 			 */
#include "include/cap.h"
#include "include/ncode.h"		// For translating ninasci to asci
#include "/video/nfl/include/joyplay.h"

/***** DEFINES ***************************************************************/

/* Based on the playfrom, this needs to be set to TRUE if the fields in the 
   file need to be swapped */

#define BYTE_SWAP TRUE


/* The pages on the memory card that contain 
   the I-Node blocks 						*/
#define NIN_INODE_BLOCK  	1
#define NIN_INODE_BLOCK_M  	2

/* The pages on the memory card that contain 
   the file descriptors to game notes 		*/
#define NIN_FILE_BLOCK_A	3
#define NIN_FILE_BLOCK_B    4

/***** TYPEDEFS **************************************************************/

/* These structures fill up the table of contents on a Nintendo memory
   pack */
typedef struct GameNote_tag
		{
			unsigned int 	game_code			PACKED;
			unsigned short 	comp_code			PACKED;
			unsigned short 	start_page			PACKED;
			char			status				PACKED;
			char			reserved			PACKED;
			unsigned short	chksum				PACKED;
			int				extension			PACKED;
			unsigned char	note_name[16]		PACKED;

		} GameNote, *pGameNote;

/***** EXTERNALS *************************************************************/

/***** GLOBALS   *************************************************************/

static unsigned int		MWY_GAME_CODE = ('N'<<24)|('B'<<16)|('Z'<<8)|'E';
static unsigned short 	MWY_COMPANY_CODE = 	('5'<<8)|('D');

#define MWY_GAME_NAME_SIZE  9
static unsigned char	MWY_GAME_NAME[16] = "NFL BLITZ";


unsigned short INodes[NUM_PAGES];	// Holds the way files are broken up in
									// Memory


/***** PROTOTYPES ************************************************************/

int joyplay_read ( char *name, char *pin, pPlayBook playbook );

int joyplay_file_pass 	( pGameNote note );
int  get_inodes_for_playbook ( int file_handle, int location, short *nodes  );

void  n64c2asc 			(char *src, char *dest, int length);
unsigned short swap_order_short 	( unsigned short value );
int   swap_order_int 	( int value );
int   compare_strings 	( char *str_a, char *str_b, int num );

/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  joyplay_test()                                                 */
/*                                                                           */
/* This function tests out the joyport driver. The game never starts.		 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
void joyplay_test ( void )

{ /* joyplay_test() */

// int i;
 int switches, old_switches;
 PlayBook playbook;

 int num_times, num_pass, num_fail;
 
	/* init the switches       */
	switches = old_switches = 0; 

    /* Go into infinite loop   */
	while ( 1 )
	{

		switches = get_player_sw_current();

		/* Input from game */
		if ( ( switches &  P1_A ) && (!(old_switches & P1_A )) )
		{
			joyplay_do_function ( JPFUNC_DOOR_OPEN );
		} 
		if ( ( switches &  P1_B ) && (!(old_switches & P1_B )) )
		{
			joyplay_do_function ( JPFUNC_DOOR_CLOSE );

		} 
		if ( ( switches &  P1_C ) && (!(old_switches & P1_C )) )
		{

			num_times = 0;
			num_pass  = 0;
			num_fail  = 0;
			

			while ( 1 )
			{

				if ( joyplay_read(
					"AAAAAA", "1111", 
					&playbook ) == FALSE )
				{
					printf("\nCARD READ FAILED\n");
					num_fail++;

				}
				else
				{
					printf("\nCARD READ PASSED\n");
					num_pass++;

				} /* else */

				num_times++;

				/* break out of condition */
				switches = get_player_sw_current();
				if ( switches &  P1_C )
				{
					break;

				} /* if */

			} /* while */

			// PRINT OUT STATS...
			printf("\n\nTEST STATS : NUM TRIES %d NUM PASS %d NUM FAIL %d\n",
									num_times, num_pass, num_fail);

		} /* if */

		if ( ( switches &  P2_A ) && (!(old_switches & P2_A )) )
		{

			joyplay_do_function ( JPFUNC_LED_FLASH_FAST );

		} 
		if ( ( switches &  P2_B ) && (!(old_switches & P2_B )) )
		{

			joyplay_do_function ( JPFUNC_LED_FLASH_SLOW );

		} 
		if ( ( switches &  P2_C ) && (!(old_switches & P2_C )) )
		{


		} 

		/* get the old input */
	 	old_switches = switches;

	} /* while */

}  /* joyplay_test() */

/***** END of joyplay_test() *************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  joyplay_read()                                                 */
/*                                                                           */
/* This function tests out the joyport driver. 								 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

int joyplay_read ( char *name, char *pin, pPlayBook playbook )

{ /* joyplay_read() */

 int i;
 int result = FALSE;

#ifdef NO_CARD_READER
	return FALSE;
#endif

	/* Check to see if the joyport mechanism exists */
	if ( joyplay_init() == FALSE )
	{
		printf("\nJOYPORT DOES NOT EXIST!!!\n");
	}
	else
	{
		printf("\nJOYPORT AVAILABLE...\n");
		/* Check to see if a card is in the slot    */
		if ( joyplay_check_card() == TRUE )
		{
	     int file_handle; 

			printf("\nCARD IS IN SLOT\n");
					
			/* Check for the game file on card 		*/
	     	file_handle = joyplay_check_file();

			if ( file_handle == -1 )
			{
				printf("File was not found on card\n");
			}
			else
			{
				// Read the playbook off of the card...
			    if ( joyplay_get_playbook( file_handle, name, pin, playbook) == TRUE )
				{

					printf("\nPLAYBOOK FOUND!\n");

					/* Look through playbook here... */
					for ( i = 0; i < 9; i++ )
					{
						printf("Play %d : N1: %s N2: %s\n", (i+1),
						playbook->play[i].name1,
						playbook->play[i].name2);
	
					} /* for i */

					// Good result!
					result = TRUE;

				}
				else
				{
					printf("\nPLAYBOOK NOT FOUND.\n");
	
				} /* else */

			} /* else */

		}
		else
		{
			printf("\nCARD IS NOT IN SLOT\n");
						
		} /* else */
			
	} /* else */

 return result;

} /* joyplay_read() */

/***** END of joyplay_read() *************************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  joyplay_init()                                                 */
/*                                                                           */
/* This is the first function that gets called when reading from the joyport */
/* It resets the joyport and reports back to the caller if the joyport 		 */
/* exists.																	 */
/*                                                                           */
/* RETURNS : 	TRUE  - Joyport exists 										 */
/* RETURNS : 	FALSE - Joyport does not exist.								 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

int joyplay_init ( void )

{ /* joyplay_init() */

 int result = TRUE;
 unsigned char status;
 int i;

#if defined(VEGAS)
	return(FALSE);
#endif

 /* Init the INodes 									 */
 for ( i = 0; i < NUM_PAGES; i++ )
 {
	INodes[i] = 0;

 } /* for i */

 /* Do a hard reset here (if we get a chance to do this) */

 /* do a soft reset, and check the return 				 */
 if ( jp_status(&status) == JP_ERR )
	result = FALSE;

 return result;

} /* joyplay_init() */

/***** END of joyplay_init() *************************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  joyplay_check_card()                                           */
/*                                                                           */
/* This function checks to see if a memory card is in the joyport.			 */
/*                                                                           */
/* RETURNS : 	TRUE  - Memory card exists 			  						 */
/* RETURNS : 	FALSE - Memory card does not exist.	  						 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

int joyplay_check_card ( void )

{ /* joyplay_check_card() */

 int result = FALSE;
 unsigned char status;

	/* read the status byte */
	if ( jp_status(&status) == JP_OK )
	{
		/* Is the card in place? */
		if ( status & JP_STATUS_CARD )
		{
			result = TRUE;

		} /* if */

	} /* if */

 return result;

} /* joyplay_check_card() */

/***** END of joyplay_check_card() *******************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  joyplay_check_file() 		                                     */
/*                                                                           */
/* This function checks to see if the game file (defined above) exists.		 */
/*                                                                           */
/* If the entry exists, the function will load up the I-Node file block		 */
/* structure and check it's integrity 										 */
/*                                                                           */
/* RETURNS : 	positive number  - File handle.		  						 */
/* RETURNS : 	(-1) 			 - File does not exist.		  				 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

int joyplay_check_file ( void )

{ /* joyplay_check_file() */

 int i;
 int result = -1;
 unsigned char page[PAGE_SIZE];

 /* Read file block A    */
 if ( jp_read_page ( NIN_FILE_BLOCK_A, page ) == JP_ERR )
 	goto CHECK_FILE_LOAD_INODE;

 /* Parse the file block */
 for ( i = 0; i < 8; i++ )
 {
 	pGameNote note = ((pGameNote)(&page[sizeof(GameNote)*i]));

 	if ( joyplay_file_pass ( note ) == TRUE )
	{
		/* get the right page number to start grabbing file from */
		result = swap_order_short(note->start_page);

		goto CHECK_FILE_LOAD_INODE;
	
	} /* if  */

 } /* for i */


 /* Read file block B    */
 if ( jp_read_page ( NIN_FILE_BLOCK_B, page ) == JP_ERR )
 	goto CHECK_FILE_LOAD_INODE;

 /* Parse the file block */
 for ( i = 0; i < 8; i++ )
 {
 	pGameNote note = ((pGameNote)(&page[sizeof(GameNote)*i]));

 	if ( joyplay_file_pass ( note ) == TRUE )
	{
		/* get the right page number to start grabbing file from */
		result = swap_order_short(note->start_page);
		goto CHECK_FILE_LOAD_INODE;
	
	} /* if  */

 } /* for i */
	
CHECK_FILE_LOAD_INODE:

	if ( result != (-1) )
	{

	  short *inodes;

		/* Load in the INODE table   		 */
 		if ( jp_read_page ( NIN_INODE_BLOCK, page ) == JP_ERR )
		{
			// ERROR! Return the error value...
			result = -1;
			printf("\n1\n");
		 	goto CHECK_FILE_END;

		} /* if */

		/* Drop the INodes into the table... */
		inodes = ((short*)page); 
		for ( i = 0; i <  NUM_PAGES; i++ )
		{
		
			INodes[i] = swap_order_short ( inodes[i] );

		} /* for */		

		/* check the integrity of the INodes using the Mirror */
 		if ( jp_read_page ( NIN_INODE_BLOCK_M, page ) == JP_ERR )
		{
			// ERROR! Return the error value...
			result = -1;
			printf("\n2\n");
		 	goto CHECK_FILE_END;

		} /* if */

		inodes = ((short*)page); 
		for ( i = 0; i <  NUM_PAGES; i++ )
		{
		
			/* BAD NODE VALUE ON CARD. ABORT!!!! */
			if ( INodes[i] != swap_order_short ( inodes[i] ) )
			{

				// ERROR! Return the error value...
				result = -1;
			 	goto CHECK_FILE_END;
	
			} /* if */

		} /* for */		

	} /* if */

CHECK_FILE_END:

 return result;

} /* joyplay_check_file() */

/***** END of joyplay_check_file() *******************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  joyplay_get_playbook()                                         */
/*                                                                           */
/* This function takes a file handle, a player name and a pin and returns 	 */
/* a filled playbook block.	  												 */
/*                                                                           */
/* NOTE : A NAME That is less then 6 characters MUST be padded with spaces.  */
/*                                                                           */
/* RETURNS : 	TRUE  - play data for this player exists.					 */
/* RETURNS : 	FALSE - play data for this player does not exist.			 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

int joyplay_get_playbook ( int file_handle, 
						   char *name, char *pin, 
						   pPlayBook playbook )

{ /* joyplay_get_playbook() */

 int i;
 int result = FALSE;
 unsigned char page[PAGE_SIZE];
 char player_name[6];
 char player_pin[4];

 /* Read first block for player names and pins    */
 if ( jp_read_page ( file_handle, page ) == JP_ERR )
 	goto GET_PLAYER_BLOCK_DONE;

 /* Print out the player names 					  */
 for ( i = 0; i < 16; i++ )
 {
  int j;
  char *player_entry;

	/* Point to player entry */
	player_entry = &page[16*i];

	/* Copy over name        */
	for ( j = 0; j < 6; j++ )
	{
		player_name[j] = player_entry[j];

	} /* for */

	/* Copy over pin         */
	for ( j = 0; j < 4; j++ )
	{
		player_pin[j] = player_entry[j+6];

	} /* for */

	if ( ( compare_strings ( player_name, name, 6 ) == TRUE ) &&
	     ( compare_strings ( player_pin,  pin,  4 ) == TRUE ) )
	{

		break;

	} /* if */

 } /* for */

 /* Did we get a hit? */
 if ( i < 16 )
 {
	short nodes[4];

	/* Fill up the four slots to read from the INode table */
	if ( get_inodes_for_playbook(file_handle, i, nodes) == TRUE )
	{
 	 unsigned char *ptr;

		/* Finally, fill up the playbook file */
		ptr = (unsigned char*)playbook;

		for ( i = 0; i < 4; i++ )
		{
 			if ( jp_read_page ( nodes[i], ptr ) == JP_ERR )
				break;
			else
			{
				/* increment into the playbook */
				ptr+=PAGE_SIZE;

			} /* else */

		} /* for i */

		/* If we've made it here, proclaim it to the world! */
		if ( i == 4 )
			result = TRUE;

	} /* if */

 } /* if */


GET_PLAYER_BLOCK_DONE:

 return result;

} /* joyplay_get_playbook() */

/***** END of joyplay_get_playbook() *****************************************/



/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  joyplay_do_function()                                          */
/*                                                                           */
/* This function takes an enumerated function value and sends the proper 	 */
/* command to the joyport. 													 */
/*                                                                           */
/* The following functions are available :									 */
/*                                                                           */
/* JPFUNC_LED_ON															 */
/* JPFUNC_LED_OFF															 */
/* JPFUNC_LED_FLASH_FAST													 */
/* JPFUNC_LED_FLASH_SLOW													 */
/* JPFUNC_DOOR_OPEN															 */
/* JPFUNC_DOOR_CLOSE														 */
/*                                                                           */
/* RETURNS : 	TRUE  - function executed properly.							 */
/* RETURNS : 	FALSE - function did not execute.							 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/

int  joyplay_do_function ( int function )

{ /* joyplay_do_function() */

 int result = JP_ERR;
 unsigned char status;
 static int state0, state1;

	switch ( function ) 
	{

		case(JPFUNC_LED_ON):
							result = jp_LED_on();
							break;
		case(JPFUNC_LED_OFF):
							result = jp_LED_off();
							break;
		case(JPFUNC_LED_FLASH_FAST):
							result = jp_LED_fast();
							break;
		case(JPFUNC_LED_FLASH_SLOW):
							result = jp_LED_slow();
							break;
		case(JPFUNC_DOOR_OPEN):
							fprintf (stderr,"\nOPEN: CHECK, STATE0 = %X, STATE1 = %X\n",state0,state1);
							if (state0 == 255 && state1 == 255) {
								result = jp_door_open();
								fprintf (stderr,"\nOPEN: OPEN, STATE0 = %X, STATE1 = %X\n",state0,state1);
							} else if (state0 == JP_OPTO_OPEN && state1 == 255) {
								fprintf (stderr,"\nOPEN: CORRECT, STATE0 = %X, STATE1 = %X\n",state0,state1);

								if (joyplay_do_function(JPFUNC_DOOR_CLOSE) == JP_ERR)
									result = JP_ERR;

								else {
									sleep(CLOSE_WAIT);
									fprintf (stderr,"\nOPEN: CLOSE, STATE0 = %X, STATE1 = %X\n",state0,state1);
									result = jp_door_open();
									fprintf (stderr,"\nOPEN: OPEN, STATE0 = %X, STATE1 = %X\n",state0,state1);
								}
							} else
								result = JP_ERR;
							break;
		case(JPFUNC_DOOR_OPEN2):
							if (jp_status(&status) == JP_ERR)
								result = JP_ERR;
							else {
								state0 = status & (JP_OPTO_CLOSE|JP_OPTO_OPEN);
								fprintf (stderr,"\nOPEN2: CHECK, STATE0 = %X, STATE1 = %X\n",state0,state1);
								if (state0 == JP_OPTO_OPEN) {
									result = jp_door_open2();
									fprintf (stderr,"\nOPEN2: OPEN2, STATE0 = %X, STATE1 = %X\n",state0,state1);
								}
							}
							break;
		case(JPFUNC_DOOR_CLOSE):
							if (jp_status(&status) == JP_ERR)
								result = JP_ERR;
							else {
								state1 = status & (JP_OPTO_CLOSE|JP_OPTO_OPEN);
								fprintf (stderr,"\nCLOSE: CHECK, STATE0 = %X, STATE1 = %X\n",state0,state1);
								if (state0 == JP_OPTO_OPEN && state1 == JP_OPTO_CLOSE) {
									result = jp_door_close();
									fprintf (stderr,"\nCLOSE: CLOSE, STATE0 = %X, STATE1 = %X\n",state0,state1);
									state0 = state1 = 255;
								} else {
									state1 = 255;
									result = JP_ERR;
								}
							}
							break;
		case(JPFUNC_DOOR_CLOSE2):
							result = jp_door_close();
							break;
		case(JPFUNC_RESET_STATES):
							state0 = state1 = 255;
							result = JP_OK;
							break;
		default:
							result = JP_ERR;
							break;

	} /* switch */

 return result;

} /* joyplay_do_function() */

/***** END of joyplay_do_function() ******************************************/


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  joyplay_file_pass() 		                                     */
/*                                                                           */
/* Takes a GameNote record and sees if it matches the one being looked for.  */
/*                                                                           */
/*****************************************************************************/

int joyplay_file_pass ( pGameNote note )

{ /* joyplay_file_pass() */

// int i;
 int result = FALSE;

 unsigned int   gcode;
 unsigned short ccode;
 unsigned char  nname[17];

		gcode = swap_order_int (   note->game_code );
		ccode = swap_order_short ( note->comp_code );

		/* Convert the note name over to asci... */
		n64c2asc(note->note_name, nname, 16); nname[16] = 0;

		/* Check the values...					 */
		if ( gcode == MWY_GAME_CODE )
		{
			if ( ccode == MWY_COMPANY_CODE )
			{
				/* Parse the name to see if it matches the game name */
				result = compare_strings ( nname, 
										   MWY_GAME_NAME, 
										   MWY_GAME_NAME_SIZE );

			} /* if */

		} /* if */

 return result;

} /* joyplay_file_pass() */


/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  get_inodes_for_playbook()                                      */
/*                                                                           */
/* Takes a file_handle (start of file page) and fills up the four pages  	 */
/* where the playbook lies. Returns TRUE if successful, FALSE if not...		 */
/*                                                                           */
/*****************************************************************************/

int  get_inodes_for_playbook ( int file_handle, int location, short *nodes  )

{ /* get_inodes_for_playbook() */

 int result = FALSE;
 int i;

 short first_node;
 short start_node;
	
	first_node = INodes[file_handle];

    if (( first_node < NUM_PAGES ) && ( first_node > 4 ))
	{

		/* Get the first node in the file for this playbook   */
		start_node = location * 4;

		/* Start parsing the file until we hit the first page */
		for ( i = 0; i < start_node; i++ )
		{
			first_node = INodes[first_node];

			if (( first_node >= NUM_PAGES ) || ( first_node < 5 ))
				break;

		} /* for */

		/* Did we get to the first page for the file?  		  */
		if ( i == start_node )
		{

			for ( i = 0; i < 4; i++ )
			{
				nodes[i] = first_node;

				first_node = INodes[first_node];

				if (( first_node >= NUM_PAGES ) || ( first_node < 5 ))
					break;

			} /* for */

			if ( i == 4 )
			{
				result = TRUE;

			} /* if */

		} /* if */

	} /* if */

 return result;

} /* get_inodes_for_playbook() */


/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  n64c2asc()		 		                                     */
/*                                                                           */
/* Converts ninASCI to ASCI													 */
/*                                                                           */
/*****************************************************************************/

void n64c2asc(char *src, char *dest, int length) 
{
	int	i;

	for (i = 0 ; i < length ; i ++ ) 
	{
		if ( src[i] >= __0 && src[i] <= __9) 
		{
			dest[i] = src[i] + ('0' - __0);
		} 
		else if ( src[i] >= __A && src[i] <= __Z) 
		{
			dest[i] = src[i] + ('A' - __A);
		} 
		else if (src[i] == __NUL && i < length-1) 
		{
			dest[i] = ' ';
		} else {
			switch(src[i]) 
			{
				case __NUL:
					dest[i] = NULL;
					break;
				case __SPC:
					dest[i] = ' ';
					break;
				case __EXCLAM:
					dest[i] = '!';
					break;
				case __2QUOTE:
					dest[i] = '\"';
					break;
				case __NUMBR:
					dest[i] = '#';
					break;
				case __1QUOTE:
					dest[i] = '\'';
					break;
				case __ASTER:
					dest[i] = '*';
					break;
				case __PLUS:
					dest[i] = '+';
					break;
				case __COMMA:
					dest[i] = ',';
					break;
				case __MINUS:
					dest[i] = '-';
					break;
				case __PERIOD:
					dest[i] = '.';
					break;
				case __SLASH:
					dest[i] = '/';
					break;
				case __COLON:
					dest[i] = ':';
					break;
				case __EQUAL:
					dest[i] = '=';
					break;
				case __QUEST:
					dest[i] = '?';
					break;
				case __AT:
					dest[i] = '@';
					break;
				default:
					dest[i] = ' ';
			}
		}
	}    

}  /* n64c2asc() */


/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  swap_order_short()  		                                     */
/*                                                                           */
/* swaps the endian order of a short 										 */
/*                                                                           */
/*****************************************************************************/

unsigned short swap_order_short ( unsigned short value )
{

 short result = value;
 char  tmp, *ptmp;


 if ( BYTE_SWAP == TRUE )	
 {
   ptmp 	= (char*)&result; /* point to byte */
   tmp  	= *ptmp;	/* byte A        */
  *ptmp 	= *(ptmp+1);	/* swap : B to A */
  *(ptmp+1) = tmp;		/* swap : A to B */
   
 } /* if */

 return result;

} /* swap_order_short */


/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  swap_order_int()  		                                     */
/*                                                                           */
/* swaps the endian order of an int 										 */
/*                                                                           */
/*****************************************************************************/

int  swap_order_int ( int value )

{ /* swap_order_int () */

 int   result = value;
 short tmpA, tmpB, *ptmp;
 
 if ( BYTE_SWAP == TRUE )
 {
  
   ptmp      = (short*)&result;				/* point to word 	  */
   tmpA      = swap_order_short(*ptmp);		/* word A in proper order */
   tmpB      = swap_order_short(*(ptmp+1));	/* word B in proper order */
  *ptmp      = tmpB;						/* swap : B to A 	  */
  *(ptmp+1)  = tmpA;						/* swap : A to B	  */

 } /* if */

 return result;

} /* swap_order_int */


/*****************************************************************************/
/*                                                                           */
/* FUNCTION:  compare_strings()  		                                     */
/*                                                                           */
/* Compares the strings; if the same, then BOOOOYAAAA!!!!					 */
/*                                                                           */
/*****************************************************************************/

int compare_strings ( char *str_a, char *str_b, int num )

{ /* compare_strings() */

 int i;
 int result = TRUE;

	for ( i = 0; i < num; i++ )
	{
		if ( str_a[i] != str_b[i] )
		{
			result = FALSE;
			break;
		} /* if */
		
	} /* for */

 return result;

} /* compare_strings() */



/***** END of joyplay.c ******************************************************/
