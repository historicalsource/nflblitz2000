/*
 *		$Archive: /video/nfl/grab.c $
 *		$Revision: 5 $
 *		$Date: 9/13/99 6:43p $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#if defined(VEGAS)

#ifdef INCLUDE_SSID
char *ss_grab_c = "$Workfile: grab.c $ $Revision: 5 $";
#endif


// set this to 1 to turn on debugging
#define GRAB_DBG 0

/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 *		USER INCLUDES
 */

#include <goose/goose.h>
#include <glide.h>

#include "include/id.h"

#define ERROR -1
#define OK		1

/***** FUNCTION PROTOTYPES ***************************************************/

void wait_vsync(int);

void grab_save_screen (char *filename);
int grab_write_file (unsigned short *ram_ptr, int width, int height, 
                     char *filename);
void grab_install_proc (void);
void grab_start_proc (int *args);


unsigned int grSstScreenWidth(void);
unsigned int grSstScreenHeight(void);


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: grab_save_screen()                                              */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* Saves the current "front buffer" screen out to a .RAW file on the disk.   */
/* Assumes that the filename passed in has the .RAW extension on it.         */
/*                                                                           */
/* Open this in Photoshop as a .RAW file with a 512x384 resolution, with     */
/* 3 channels, interleaved, 16-bit depth, in Macintosh byte order. You will  */
/* then have to flip the picture vertically.                                 */
/*                                                                           */
/* 21 Nov 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

void grab_save_screen (char *filename)

{ /* grab_save_screen() */

unsigned short *vid_ram;

int buf_size;

unsigned int height;
unsigned int width;


/*****************************************************************************/

	#if GRAB_DBG
		printf ("\n");
		printf ("grab_save_screen(): begin\n");
	#endif

	/* allocate ram */

	width = grSstScreenWidth();
	height = grSstScreenHeight();

	printf ("grab_save_screen(): saving %s...  width:%d height:%d\n", filename, width, height);

	buf_size = width * height * sizeof (unsigned short);

	vid_ram = (unsigned short *) malloc (buf_size);

	if (vid_ram == NULL)
		{
		printf ("grab_save_screen(): ERROR cannot allocate %d bytes\n",
		        buf_size);
		return;
		}


	/* grab the screen */
	grLfbReadRegion (GR_BUFFER_FRONTBUFFER,
					0,0,
					width, height,
					width * sizeof(FxU16),
					vid_ram);


	/* write the data */
	grab_write_file (vid_ram, width, height, filename);

	printf ("grab_save_screen(): file %s written OK\n", filename);

	free (vid_ram);

	#if GRAB_DBG
		printf ("grab_save_screen(): end\n");
		printf ("\n");
	#endif

} /* grab_save_screen() */

/***** END of grab_save_screen() *********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: grab_write_file()                                               */
/*                                                                           */
/* Writes out the 3DFX 5:6:5 format as 16:16:16 RGB.                         */
/*                                                                           */
/* Open this in Photoshop as a .RAW file with a 512x384 resolution, with     */
/* 3 channels, interleaved, 16-bit depth, in Macintosh byte order. You will  */
/* then have to flip the picture vertically.                                 */
/*                                                                           */
/* Copyright 1997 Midway Games Inc.                                          */
/*                                                                           */ 
/* 21 Nov 97 MVB                                                             */ 
/*                                                                           */ 
/*****************************************************************************/

int grab_write_file (unsigned short *ram_ptr, int width, int height, 
                     char *filename)

{ /* grab_write_file() */

int x;
int y;
unsigned int addr;

unsigned short red;
unsigned short blue;
unsigned short green;

int pixel;

FILE *fh;                  		

/***************************************************************************/


	#if GRAB_DBG
		printf ("\n");
		printf ("grab_write_file(): begin\n");
	#endif

	/* open the file */
	fh = fopen (filename, "wb");
	if (fh == NULL)
		{
		printf ("grab_write_file(): ERROR cannot open file %s for writing.\n", 
		        filename);
		fclose (fh);
		return ERROR;
		}


	/* write out the pixels */
	/* .RAW format, no header */		

	for (y=0; y < height; y++)
		{

		for (x=0; x < width; x++)
			{

			/* see Glide programming manual */
			/* video buffer is linear */
			addr = y * width + x;

			/*** 16-bit 5:6:5 pixel ***/
			pixel = ram_ptr[addr] & 0x0000FFFF;

			red = (unsigned short) (pixel >> 11) & 0x1F;
			red = red << 3;

			green = (unsigned short) (pixel >> 5) & 0x3F;
			green = green << 2; 

			blue = (unsigned short) (pixel & 0x1F);
			blue = blue << 3;

			/* each byte (each color) is 16-bits */
			fwrite (&red, sizeof(unsigned short), 1, fh);
			fwrite (&green, sizeof(unsigned short), 1, fh);
			fwrite (&blue, sizeof(unsigned short), 1, fh);

			} /* for x */

		} /* for y */

	fclose (fh); 		

	return OK;

	#if GRAB_DBG
		printf ("grab_write_file(): done\n");
		printf ("\n");
	#endif

} /* grab_write_file() */

/***** grab_write_file() *****************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: grab_install_proc()                                             */
/*                                                                           */
/* Installs a proc that runs the screen grabber when you hit the start       */
/* button.                                                                   */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 21 Nov 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

void grab_install_proc (void)

{ /* grab_install_proc() */

	#if GRAB_DBG
		printf ("\n");
		printf ("grab_install_proc(): begin\n");
	#endif

	set_process_run_level( qcreate( "grabpro", NODESTRUCT, grab_start_proc,  0, 0, 0, 0 ), RL_PLAYER);


	#if GRAB_DBG
		printf ("grab_install_proc(): done\n");
		printf ("\n");
	#endif

} /* grab_install_proc() */

/***** END of grab_install_proc() ********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: grab_start_proc()                                               */
/*                                                                           */
/* When you press the start button during gameplay, saves the current        */
/* screen.                                                                   */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 21 Nov 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

void grab_start_proc (int *args)

{ /* grab_start_proc() */

char filename[20];

int number = 0;

/*****************************************************************************/

#if GRAB_DBG
	printf ("\n");
	printf ("grab_start_proc(): begin\n");
#endif

	while(1)
	{
		if( (get_p5_sw_current()&255) == P_UP )
		{
			while( get_p5_sw_current() & P_UP )
			{
				if( get_p5_sw_close() & P_A )
				{
					/* generate the file name */
					number++;
					sprintf( filename, "grab%02d.raw", number );
				
					/* save the screen */
					grab_save_screen( filename );
				}
				wait_vsync(0);
				scan_switches();
			}
		}

		sleep (1);
	}

#if GRAB_DBG
	printf ("grab_start_proc(): done\n");
	printf ("\n");
#endif

} /* grab_start_proc() */

/***** END of grab_start_proc() **********************************************/

/***** END of file GRAB.C ****************************************************/

#endif //VEGAS
