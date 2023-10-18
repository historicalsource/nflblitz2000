#if 0	// NOT USED -> Nba movie player handles this stuff beautifully now!
/*****************************************************************************/
/*                                                                           */
/* STREAM.C                                                                  */
/*                                                                           */
/* Code for handling streaming audio using the FIFOs and the I/O ASIC.       */
/*                                                                           */
/* Basic operation:                                                          */
/*                                                                           */
/* - Game reads data from disk and fills a large RAM buffer. This buffer is  */
/*   divided into two halves, A and B.                                       */
/*                                                                           */
/* - When sound FIFO's are empty, the game gets an interrupt from the I/O    */
/*   ASIC. An ISR runs that reads from the RAM buffer and fills the FIFO.    */
/*                                                                           */
/* - If the ISR is reading out of A, then the fill proc fills B, and vice    */
/*   versa.                                                                  */
/*                                                                           */
/* The goal is to keep the amount of realtime spent by the game feeding the  */
/* sound system to a minimum.                                                */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 25 Nov 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

// set this to 1 to turn on debugging
// this debugs _everything_
#define STREAM_DBG 0

// this will only print when something that could
// signal a file problem happens - 15 Feb 98 MVB
#define STREAM_ERROR_DBG 0

// low level debugging each time isr is called
#define STREAM_DBG_ISR 0

// prints each time a buffer is filled
#define STREAM_FILL_DBG 0


/***** INCLUDES **************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dir.h>

/* GOOSE and GLIDE stuff */
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

//#include "include/drivers.h"
#include "include/inthand.h"	
#include "/video/biosrom/bios/include/ioctl.h"
	
/* our stuff */
#include "include/id.h"
//#include "include/defines.h"	
//#include "include/srfonts.h"
//#include "include/srcolors.h"
//#include "include/coin.h"
//#include "include/procid.h"
//#include "include/sr.h"
//#include "include/game.h"

#include "include/stream.h"


/***** DEFINES ***************************************************************/
/* these were in space/include/defines.h */
#define OK 0
#define ERROR 1

/* the sound FIFOs are 512 words x 16-bits wide */
#define STREAM_FIFO_SIZE 512

/* this is 32k bytes (16k words) per half of the buffer */
// #define STREAM_FIFOS_PER_BUFFER 32 17 Dec 97 MVB 
//#define STREAM_FIFOS_PER_BUFFER 16
// put this back to 32 - "both empty" message problem
#define STREAM_FIFOS_PER_BUFFER 32

/* this is the size of a "half"-buffer */
#define STREAM_BUFFER_SIZE (STREAM_FIFO_SIZE * STREAM_FIFOS_PER_BUFFER)

/* this is the whole buffer size */
#define STREAM_DOUBLE_BUFFER_SIZE (STREAM_BUFFER_SIZE * 2)

/* these define the states of the A and B host-side buffers */
enum {
	STREAM_BUFFER_EMPTY,		/* ISR has played thru data; needs to be filled */
	STREAM_BUFFER_INUSE,		/* ISR is currently playing thru the data */
	STREAM_BUFFER_FILLED		/* fill proc has updated; ready to be played */
	};

/* these define the state of the streaming process */
enum {	
	STREAM_STATE_INIT,		/* setup in progress */
	STREAM_STATE_READY,		/* game-side buffers have been preloaded */
	STREAM_STATE_PLAYING,	/* streaming in progress */
	STREAM_STATE_EOF,			/* fill proc has reached end of streaming file */
	STREAM_STATE_DONE			/* ISR has played thru all game-side buffers */
	};

/* these tag the two game-side fill buffers */
enum {
	STREAM_A_BUFFER,
	STREAM_B_BUFFER
	};

/* mode flag for stream_fill_proc() */
enum {
	STREAM_MODE_START,
	STREAM_MODE_CHANGE
	};

/* on average we need to fill a buffer every 6 to 7 ticks @ 60 Hz; */
/* if too much time goes by w/o a fill, then the FIFO ISR must be */
/* crapped out */
/* doubled to match bigger buffer size */
#define STREAM_FILL_TIMEOUT 40

/***** GLOBALS (to this file) ************************************************/

/* the FIFOs are 512 words by 16-bits wide == unsigned short */
unsigned short stream_buffer [STREAM_DOUBLE_BUFFER_SIZE];

/* fence-posts for reading and filling */
/* the game-side buffers */
unsigned short *stream_a_ptr;
unsigned short *stream_b_ptr;
unsigned short *stream_end_ptr;
unsigned short *stream_read_ptr;
unsigned short *stream_fill_ptr;
unsigned short *stream_last_ptr;

/* see the STREAM_BUFFER_ enums above */
int stream_a_state;
int stream_b_state;

/* see the STREAM_STATE_ enums above */
int stream_state;

/* number of 16-bit samples sent */
/* to the sound system */
int stream_total_words;

/* file handle */
FILE *stream_fh;

/* save ptr to stream_fill_proc() */
struct process_node *stream_proc_ptr = NULL;

int stream_volume;

/* 4 June 98 MVB - make sure this gets init'ed to OFF */
int stream_active_flag = FALSE;

int	stream_key;			// Used to keep the movie player from colliding with DMA

int	fifo_size = 0;		// Size of the FIFOs (0 = no FIFOs installed)



/***** EXTERNS ***************************************************************/

//extern struct process_node *cur_proc;


/***** FUNCTION PROTOTYPES ***************************************************/

void stream_buffer_init (void);

int stream_open_file (char *filename);

int stream_close_file (void);

int stream_preload_game_buffers (void);

void stream_fill_proc (int *args);

void stream_simulate_isr_proc (int *args);

int stream_preload_dsp_buffers (void);

void stream_enable_fifo (void);

void stream_disable_fifo (void);

void stream_fade_proc (int *args);

void stream_duck_proc (int *args);

int stream_detect_fifos(void);


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: stream_init()                                                   */
/*                                                                           */
/* Main setup for streaming audio.                                           */
/*                                                                           */
/* - initialize and clear out the game-side double buffer                    */
/*                                                                           */
/* - open the audio file to be streamed and fill the game-side buffer        */
/*                                                                           */
/* - enable the FIFO empty interrupt on the I/O ASIC                         */
/*                                                                           */
/* - start up the proc that will watch and fill the game-side double buffer  */
/*                                                                           */
/* Pass in the complete file name (including any extension) of the audio     */
/* to be streamed.                                                           */
/*                                                                           */
/* Assumes audio file is stereo, interleaved, 16-bit, 31250Hz sample rate.   */
/*                                                                           */
/* Audio playback does not actually start until the stream_start() function  */
/* is called.                                                                */
/*                                                                           */
/* The playback volume must be set using the snd_stream_volume() function.   */
/*                                                                           */
/* Returns OK or ERROR.                                                      */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 25 Nov 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

int stream_init (char *filename)

{ /* stream_init() */

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_init(): begin\n");
	#endif

fprintf(stderr, "sound fifo size: %d\n", fifo_size);
	if(!fifo_size)
	{
		return(OK);
	}

	stream_total_words = 0;
	stream_active_flag = FALSE;

	/* make sure that we aren't locked out of using disk */
	stream_release_key();

	/* set up and clear out the game-side buffers */
	stream_buffer_init();

	/* open up the file */
	if (stream_open_file (filename) != OK)
		{
		fprintf (stderr, "stream_init(): ERROR file %s not found\r\n", filename);    
		return ERROR;
		}

	/* preload the game-side buffers */
	if (stream_preload_game_buffers() != OK)
		return ERROR;

	/* setup the fifo and handshaking */
	/* preload the buffers in the sound DSP RAM */
	if (stream_preload_dsp_buffers() != OK)
		return ERROR;

	/* create a proc to keep the buffers filled */
	/* Note! This must be iqcreate() instead of qcreate() so that the */
	/* fill proc is guaranteed to be created and run without a sleep */
	/* in between */

	/* this MUST be indestructible so that the proc lives through */
	/* the game wipeout, so that it can finish up any pending */
	/* disk dma/interrupt stuff - then it kills itself */

	stream_proc_ptr = iqcreate ("str_fil", (STREAM_PID | NODESTRUCT), 
	                            stream_fill_proc, STREAM_MODE_START, 0, 0, 0);

	/* we can't start streaming until */
	/* the stream_fill_proc() is running */
	stream_state = STREAM_STATE_INIT;

	/* external func must set this up */
	stream_volume = 0;
	
	#if STREAM_DBG
		printf ("stream_init(): end\n");
		printf ("\n");
	#endif		

	return OK;

} /* stream_init() */

/***** END of stream_init() **************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: stream_enable_fifo()                                            */
/*                                                                           */
/* - disable I/O ASIC interrupts                                             */
/*                                                                           */
/* - reset and enable the FIFO                                               */
/*                                                                           */
/* - turn on the "FIFO empty" interrupt                                      */
/*                                                                           */
/* - turn back on I/O ASIC interrupts                                        */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 01 Dec 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

void stream_enable_fifo (void)

{ /* stream_enable_fifo() */

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_enable_fifo(): begin\n");
	#endif

	if(!fifo_size)
	{
		return;
	}

	/* !!! DISABLE I/O ASIC INTERRUPTS !!! */
   inthand_disable_ioasic_interrupt();

	/* reset the fifo and set appropriate flags */
	inthand_enable_fifo();

	/* tell FIFO to let us know when it needs feeding */
	inthand_enable_fifo_empty_interrupt();

	/* !!! ENABLE I/O ASIC INTERRUPTS !!! */
	inthand_enable_ioasic_interrupt();


	#if STREAM_DBG
		printf ("stream_enable_fifo(): done\n");
		printf ("\n");
	#endif		

} /* stream_enable_fifo() */

/***** END of stream_enable_fifo() *******************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: stream_disable_fifo()                                           */
/*                                                                           */
/* - disable I/O ASIC interrupts                                             */
/*                                                                           */
/* - reset and enable the FIFO                                               */
/*                                                                           */
/* - turn off the "FIFO empty" interrupt                                     */
/*                                                                           */
/* - turn back on I/O ASIC interrupts                                        */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 01 Dec 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

void stream_disable_fifo (void)

{ /* stream_disable_fifo() */

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_disable_fifo(): begin\n");
	#endif

	if(!fifo_size)
	{
		return;
	}

	/* !!! DISABLE I/O ASIC INTERRUPTS !!! */
   inthand_disable_ioasic_interrupt();

	/* reset the fifo and set appropriate flags */
	inthand_enable_fifo();

	/* tell FIFO to let us know when it needs feeding */
	inthand_enable_fifo_empty_interrupt();

	/* !!! ENABLE I/O ASIC INTERRUPTS !!! */
	inthand_enable_ioasic_interrupt();


	#if STREAM_DBG
		printf ("stream_disable_fifo(): done\n");
		printf ("\n");
	#endif		

} /* stream_disable_fifo() */

/***** END of stream_disable_fifo() ******************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: stream_start()                                                  */
/*                                                                           */
/* Actually starts sending audio data to the sound system.                   */
/*                                                                           */
/* Returns OK or ERROR.                                                      */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 25 Nov 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

void stream_start (void)

{ /* stream_start() */

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_start(): begin\n");
	#endif

	if(!fifo_size)
	{
		return;
	}

	/* make sure that the fill proc is running */
	if (stream_state != STREAM_STATE_READY)
		{
		#if STREAM_DBG
			fprintf (stderr, "stream_start(): stream_fill_proc() is not running yet\n\r");
		#endif		
		return;
		}

	/* we are now going to play out of this one */
	stream_a_state = STREAM_BUFFER_INUSE;

	/* here we go now */
	stream_state = STREAM_STATE_PLAYING;

	/* tell sound system to start playing audio */
	if (snd_send_data (SND_CMD_STREAM_START) != OK)
		{
		#ifdef DEBUG
			fprintf (stderr, "stream_preload_dsp_buffers(): ERROR sending stream start cmd\r\n");    
		#endif
		}

	#if STREAM_DBG
		printf ("stream_start(): end\n");
		printf ("\n");
	#endif		

} /* stream_start() */

/***** END of stream_start() *************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: stream_stop()                                                   */
/*                                                                           */
/* Shuts down streaming. Can be called "mid-stream".                         */
/*                                                                           */
/* Returns OK or ERROR.                                                      */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 01 Dec 97 MVB                                                             */
/* 05 Dec 97 MVB - clean up proc delete                                      */
/* 20 Mar 98 MVB - changed stream stop flow - do not interrupt disk dma      */
/* 04 May 98 MVB - do not release "key" until stream_fill_proc really done   */
/*                                                                           */
/*****************************************************************************/

void stream_stop (void)

{ /* stream_stop() */

	#if (STREAM_DBG || STREAM_ERROR_DBG)
		printf ("\n");
		printf ("stream_stop(): begin\n");
	#endif

	if(!fifo_size)
	{
		return;
	}

	//printf ("stream_stop(): begin\n");
	/* if streaming isn't running, just return */
	//if (stream_proc_ptr == NULL)
	//	return;

	/* tell the ISR to stop */
	stream_state = STREAM_STATE_DONE;


	//if (existp (STREAM_PID, 0xFFFFFFFF))
	//	{
	//	/* kill the stream_fill_proc() */
	//	kill (stream_proc_ptr, 1);
	//	}

	/* null it out */
	//stream_proc_ptr = NULL;

	/* all data has been sent, so shut */
	/* down the FIFO's */	
	stream_disable_fifo();
			

	/* tell sound system to stop playing audio */
	if (snd_send_data (SND_CMD_STREAM_STOP) != OK)
		{
		#ifdef DEBUG
			fprintf (stderr, "stream_stop(): ERROR sending stream stop cmd\r\n");   
		#endif
		}
	
   /* done reading, so close up shop */
   //stream_close_file();

   /* make sure to turn off any background disk stuff */	
   //install_disk_info (NULL, NULL, NULL);

   /* make sure to release the disk DMA */

   /* 04 May 98 MVB */
   /* !!! This can NOT be done here !!! */
   /* The _only_ place that the key can be released is inside */
   /* stream_fill_proc()... if you clear the flag here, the */
   /* stream_fill_proc() could still be suspended waiting for */
   /* a disk interrupt to come back. */
   /* I suspect that this was causing the attract mode crash */
   /* coming out of game play... the stream_fill_proc() was still */
   /* waiting on a disk interrupt to come back, but att_game_func() */
   /* went ahead and did a disk read. */
   //stream_release_key();


	#if STREAM_DBG
		printf ("stream_stop(): end\n");
		printf ("\n");
	#endif		

} /* stream_stop() */

/***** END of stream_stop() **************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: stream_buffer_init()                                            */
/*                                                                           */
/* Sets up the game-side double buffer.                                      */
/*                                                                           */
/*                   stream_buffer                                           */
/*                   -------------                                           */
/*                   +-----------+                                           */
/*                   |           |       A buffer has one of three states:   */
/*  fill_pointer --> | A buffer  |                                           */
/*                   |           |       1. STREAM_BUFFER_EMPTY              */
/*                   | 32k words |          (needs the game to fill it)      */
/*                   |           |                                           */
/*                   +-----------+       2. STREAM_BUFFER_INUSE              */
/*                   |           |          (ISR is reading from that half)  */
/*                   | B buffer  |                                           */
/*                   |           |       3. STREAM_BUFFER_FILLED             */
/*  read_pointer --> | 32k words |          (game has filled it)             */
/*                   |           |                                           */
/*                   +-----------+                                           */
/*                                                                           */
/* The FIFO holds 512 words. The game gets interrupted when the FIFO is      */
/* empty. The ISR reads 512 words from read_pointer each time it services    */
/* the interrupt. When read_pointer crosses from A to B, the game reads from */
/* the disk and fills up A. When read_pointer wraps back to A, the game      */
/* then fills up B.                                                          */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 25 Nov 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

void stream_buffer_init (void)

{ /* stream_buffer_init() */

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_buffer_init(): begin\n");
	#endif

	if(!fifo_size)
	{
		return;
	}


	/* clear out the entire buffer */
	memset (stream_buffer, 0, sizeof(stream_buffer));

	/* set the A and B states to empty */
	stream_a_state = STREAM_BUFFER_EMPTY;
	stream_b_state = STREAM_BUFFER_EMPTY;

	/*** set up the pointers ***/

	/* marker for the start of the A buffer */
	stream_a_ptr = stream_buffer;

	/* marker for the start of the B buffer */
	stream_b_ptr = stream_buffer + STREAM_BUFFER_SIZE;

	/* marker for last location in B buffer */
	stream_end_ptr = stream_buffer + STREAM_DOUBLE_BUFFER_SIZE - 1;

	/*** set up read and fill pointers */
	stream_read_ptr = stream_a_ptr;
	stream_fill_ptr = stream_a_ptr;


	#if STREAM_DBG
		printf ("stream_buffer_init(): end\n");
		printf ("\n");
	#endif		

} /* stream_buffer_init() */

/***** END of stream_buffer_init() *******************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: stream_open_file()                                              */
/*                                                                           */
/* Opens the audio file and sets the global (to this file) file handle.      */
/*                                                                           */
/* Assumes audio file is stereo, interleaved, 16-bit, 31250Hz sample rate.   */
/*                                                                           */
/* Error checks that the file is at least long enough to fill the buffer     */
/* once.                                                                     */
/*                                                                           */
/* Returns OK or ERROR.                                                      */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 25 Nov 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

int stream_open_file (char *filename)

{ /* stream_open_file() */

/* file info block */
struct ffblk ffblk;			  			
int filesize;

/*****************************************************************************/

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_open_file(): begin\n");
	#endif

	if(!fifo_size)
	{
		return(ERROR);
	}

	/* get the file size */
	findfirst (filename, &ffblk, 0);

	/* stream_fh is global to this file */
	stream_fh = fopen (filename, "rb");

	if (stream_fh == NULL)
		{
		printf ("stream_open_file(): ERROR cannot open file %s for reading.\n", 
		        filename);
		fclose (stream_fh);
		return ERROR;
		}

	filesize = ffblk.ff_fsize;

	#if STREAM_DBG
		printf ("stream_open_file: file size: %d bytes\n", filesize);
	#endif		

	/* error check */
	if (filesize < (int)sizeof(stream_buffer))
		{
		printf ("stream_preload_game_buffers(): ERROR file too small (%d bytes)\n",
		         filesize);
		return ERROR;
		}

	#if STREAM_DBG
		printf ("stream_open_file(): end\n");
		printf ("\n");
	#endif		

	return OK;

} /* stream_open_file() */

/***** END of stream_open_file() *********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: stream_close_file()                                             */
/*                                                                           */
/* Closes the file handle stream_fh.                                         */
/*                                                                           */
/* Returns OK or ERROR.                                                      */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 25 Nov 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

int stream_close_file (void)

{ /* stream_close_file() */

/*****************************************************************************/

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_close_file(): begin\n");
	#endif

	if(!fifo_size)
	{
		return(OK);
	}

	fclose (stream_fh);


	#if STREAM_DBG
		printf ("stream_close_file(): end\n");
		printf ("\n");
	#endif		

	return OK;

} /* stream_close_file() */

/***** END of stream_close_file() *********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: stream_preload_game_buffers()                                   */
/*                                                                           */
/* Reads enough from the disk to fill both the A and B game-side buffers.    */
/* Returns OK or ERROR.                                                      */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 25 Nov 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

int stream_preload_game_buffers (void)

{ /* stream_preload_game_buffers() */

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_preload_game_buffers(): begin\n");
	#endif

	if(!fifo_size)
	{
		return(ERROR);
	}


	if (fread (stream_buffer, sizeof (unsigned short), STREAM_DOUBLE_BUFFER_SIZE, 
       stream_fh) != STREAM_DOUBLE_BUFFER_SIZE)
			{
			printf ("stream_preload_game_buffers(): ERROR reading %d words from file.\n", 
			        STREAM_DOUBLE_BUFFER_SIZE);
			fclose (stream_fh);
			return ERROR;
			}

	/* update the running count */
	stream_total_words += STREAM_DOUBLE_BUFFER_SIZE;

	/* both should be filled now */
	stream_a_state = STREAM_BUFFER_FILLED;
	stream_b_state = STREAM_BUFFER_FILLED;


	#if STREAM_DBG
		printf ("stream_preload_game_buffers(): end\n");
		printf ("\n");
	#endif		

	return OK;

} /* stream_preload_game_buffers() */

/***** END of stream_preload_game_buffers() **********************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: stream_preload_dsp_buffers()                                    */
/*                                                                           */
/* - sets up handshaking between game and sound DSP                          */
/*                                                                           */
/* - resets and enables the FIFO's                                           */
/*                                                                           */
/* - turns on the FIFO's, but does not start playing audio, in order to      */
/*   pre-load the RAM buffers on the sound DSPi                              */
/*                                                                           */
/* Returns OK or ERROR.                                                      */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 25 Nov 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

int stream_preload_dsp_buffers (void)

{ /* stream_preload_dsp_buffers() */

int buffers_available;
int wait_time,wait_start;

/*****************************************************************************/

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_preload_dsp_buffers(): begin\n");
	#endif

	if(!fifo_size)
	{
		return(ERROR);
	}

	/* empty out all the streaming buffers */
	/* that are in the sound DSP RAM */
	if (snd_send_data (SND_CMD_STREAM_FLUSH) != OK)
		{
		#ifdef DEBUG
			fprintf (stderr, "stream_preload_dsp_buffers(): ERROR sending flush stream cmd.\r\n");    
		#endif
		return ERROR;
		}

	/* now that the queue is disabled and empty, */
	/* clear any data that might be sitting in the latch */
	/* data can stay latched even through a sound soft reset */
	/* so make sure to clear it */
	snd_clear_latch();


	/*** pre-load streaming buffers that are in in sound DSP RAM */

	/* clear out and turn on the FIFO */
	stream_enable_fifo();

	/* find out how many RAM buffers the sound DSP has */
	/* each buffer is one stereo frame of audio */
	snd_query_buffers (&buffers_available);

	#if STREAM_DBG
		printf ("stream_preload_dsp_buffers(): %d buffers available before preload\n",
		        buffers_available);
		printf ("\n");
	#endif		

	/* tell sound system to start sucking down fifo buffers */
	/* but to not start playing yet - this is "pre-load" */
	if (snd_send_data (SND_CMD_FIFO_ENABLE) != OK)
		{
		#ifdef DEBUG
			fprintf (stderr, "stream_preload_dsp_buffers(): ERROR sending fifo enable cmd.\r\n");    
		#endif
		return ERROR;
		}

	/* wait for the pre-loading to finish */
	/* since we pre-loaded our game-side buffers, and the */
	/* xfer is interrupt-driven, this should happen very fast */
	/* essentially equal to a memcpy of 512*13 words of data */
	#if STREAM_DBG
		printf ("stream_preload_dsp_buffers(): waiting for pre-load to finish\n\n" );
	#endif		

	/* start the timeout timer */	
//   _ioctl(5, FIOCSTARTTIMER1, 0);
	_ioctl (5, FIOCGETTIMER1, (int)&wait_start);

	do {
		snd_query_buffers (&buffers_available);
		wait_time = 0;
		_ioctl (5, FIOCGETTIMER1, (int)&wait_time);
		} while ((buffers_available > 0) && ((wait_time-wait_start) < (500 * 500000)));

		
	#if STREAM_DBG
		printf ("stream_preload_dsp_buffers(): end\n");
		printf ("\n");
	#endif		

	/* stop the timeout timer */	
//	_ioctl(5, FIOCSTOPTIMER1, 0);

	return OK;

} /* stream_preload_dsp_buffers() */

/***** END of stream_preload_dsp_buffers() ***********************************/

/*****************************************************************************/
/*                                                                           */
/* PROCESS: stream_fill_proc()                                               */
/*                                                                           */
/* Once per game tick, checks the game-side buffer and fills the empty half. */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 25 Nov 97 MVB                                                             */
/* 18 Dec 97 MVB - added background disk mode                                */
/* 06 Jan 98 MVB - fix compiler warning from func arg ptr in install_disk_   */
/* 29 Jan 98 MVB - added mode flag to not set state if changing files        */
/* 15 Feb 98 MVB - added watchdog and more error checking                    */
/* 20 Mar 98 MVB - changed stream stop flow - do not interrupt disk dma      */
/*                                                                           */
/*****************************************************************************/

void stream_fill_proc (int *args)

{ /* stream_fill_proc() */

int mode;
int words_read;
int i;
int watchdog_count;

/*****************************************************************************/

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_fill_proc(): begin\n");
	#endif

	if(!fifo_size)
	{
		die(0);
	}

	/* extract the change flag */
	mode = (int) args[0];

	/* don't reset state if changing files */
	if (mode == STREAM_MODE_START)
		{
		/* first, tell rest of system that it's OK to */
		/* start since this proc is now up and running */
		stream_state = STREAM_STATE_READY;
		}
	
	/* this will check to make sure we are filling this */
	/* regularly - in case the FIFO ISR has crapped out - */
	/* on average we need to do a fill every 6 to 7 ticks (100 msec) */
	watchdog_count = 0;
	/* error check to make sure we don't fill same buffer twice */
	stream_last_ptr = NULL;

	/* wait until the stream_start() call is actually made */
	/* - stream_init() pre-loaded the buffers */

	/* if the streaming gets stopped while we are idling, then bail */
	while (stream_state != STREAM_STATE_PLAYING)
		{

		sleep(1);

		/* while we were asleep, streaming go stopped */
		if (stream_state == STREAM_STATE_DONE)
			{
			/* close the file */
			stream_close_file();
			/* flag outside world that we are really done */
			stream_active_flag = FALSE;
			/* we are done - die */
			return;
			}

		/* while we were asleep, someone restarted streaming */
		if (stream_state == STREAM_STATE_INIT)  
			{
			/* case 2: while we were asleep, a new */
			/* stream_start got issued -- just die quickly, no cleanup */
			return;
			}

		} /* while */

	/* read until end of file */

	while ((stream_state != STREAM_STATE_EOF) && (stream_state != STREAM_STATE_INIT))
	{

		stream_active_flag = TRUE;

		/*** error check - only one buffer should be empty ***/
		if ((stream_a_state == STREAM_BUFFER_EMPTY) && 
		    (stream_b_state == STREAM_BUFFER_EMPTY)) 
			{
			fprintf (stderr, "stream_fill_proc(): WARNING both buffers are empty\n\r");
			}


		/*** figure out which buffer needs to be filled ***/


		/* save the last one */
		/* error check to make sure not filling same one twice */
		stream_last_ptr = stream_fill_ptr;

		stream_fill_ptr = NULL;

		if ((stream_a_state == STREAM_BUFFER_EMPTY) && 
		    (stream_a_state != STREAM_BUFFER_INUSE))
			{

			#if STREAM_FILL_DBG
				fprintf (stderr, "stream_fill_proc(): filling A\n");
			#endif		

			stream_fill_ptr = stream_a_ptr;
			stream_a_state = STREAM_BUFFER_FILLED;

			}
		else if ((stream_b_state == STREAM_BUFFER_EMPTY) &&
		         (stream_b_state != STREAM_BUFFER_INUSE))
			{

			#if STREAM_FILL_DBG
				fprintf (stderr, "stream_fill_proc(): filling B\n");
			#endif		

			stream_fill_ptr = stream_b_ptr;
			stream_b_state = STREAM_BUFFER_FILLED;

			}

		/*** error check - only one buffer should be in use ***/
		if ((stream_a_state == STREAM_BUFFER_INUSE) && 
		    (stream_b_state == STREAM_BUFFER_INUSE)) 
			{
			#if STREAM_ERROR_DBG
				fprintf (stderr, "stream_fill_proc(): ERROR both buffers are INUSE\n\r");
			#endif
			}

		/*** error check - somebody set this wrongly ***/
		if (stream_state == STREAM_STATE_DONE) 
			{
			#if STREAM_ERROR_DBG
				fprintf (stderr, "stream_fill_proc(): ERROR fill proc has STREAM_STATE_DONE\n\r");
			#endif
			}

		/*** fill the empty one ***/
		
		if (stream_fill_ptr != NULL)
			{

			/* error check that we are not filling the same buf twice */
			if (stream_fill_ptr == stream_last_ptr)
				{
				#if STREAM_ERROR_DBG
					fprintf (stderr, "stream_fill_proc(): ERROR same buffer filled twice\n\r");
				#endif
				}

			/* enable background disk loading */
			/* if the disk data is not ready (in cache), then this */
			/* process will be put to sleep, and will be woken up */
			/* when the data is ready */	
			
			/* REMOVED 5-4-98 BRE - Unknown use, Messes up non-movie based */
			/* background loading */
			// Wait for key here...
			//if (stream_check_key() == TRUE)
			//    install_disk_info (0,0,0);

			while (stream_check_key() == TRUE)
				sleep(1);

			/* this is a flag to keep other procs */
			/* from doing any disk access while */
			/* we are doing background/interrupt read */
			stream_get_key();

#if defined(SEATTLE)
			install_disk_info ((void **) &cur_proc, suspend_self, resume_process);
#else
			dio_mode(DIO_MODE_NO_BLOCK);
#endif

			/* be sure everyone knows we are waiting for a disk callback */
			stream_active_flag = TRUE;

			/* fill the buffer */
			words_read = fread (stream_fill_ptr, sizeof (unsigned short), 
			                    STREAM_BUFFER_SIZE, stream_fh);


			/* turn off background mode so no other disk stuff is */
			/* hosed up */
#if defined(SEATTLE)
			install_disk_info (NULL, NULL, NULL);
#else
			dio_mode(DIO_MODE_BLOCK);
#endif

			/* flag that it's OK for other procs */
			/* to use the disk */
			stream_release_key();			

			/* check if a stream_stop() command was issued */
			/* while we were asleep waiting for data */
			/* two cases: */
			/* 1. stop issued, this wakes up before any start issued */
			/* 2. stop issued, and then start issued before we wake up */
			if (stream_state == STREAM_STATE_DONE)  
				{
				/* case 1: stop issued, and no streaming going on now */
				/* close the file */
				stream_close_file();
				/* flag outside world that we are really done */
				stream_active_flag = FALSE;
				/* we are done - die */
				return;
				}

			if (stream_state == STREAM_STATE_INIT)  
				{
				/* case 2: while we were asleep, a new */
				/* stream_start got issued -- just die quickly, no cleanup */
				return;
				}

			stream_total_words += words_read;

			/* for debug */
			#if 0
			printf ("wr:%d tr:%d wd:%d\n", words_read, stream_total_words, 
			        watchdog_count);
			#endif

			/* clear the watchdog */
			watchdog_count = 0;

			/* pad with zeros and flag EOF if we run out of data */
			if (words_read < STREAM_BUFFER_SIZE)
				{
				/* flag that we are out of data */
				stream_state = STREAM_STATE_EOF;

				/* move to the last actual audio data stored */
				stream_fill_ptr += words_read;

				/* fill the rest with zeros */
				for (i=0; i < (STREAM_BUFFER_SIZE - words_read); i++)
					{
					*stream_fill_ptr = 0;
					stream_fill_ptr++;
					}

				/* important to do this so rest of system knows */
				/* that streaming is done */
				stream_active_flag = FALSE;

				/* 22 May 98 - must close the file */
   				stream_close_file();

				#if (STREAM_DBG || STREAM_ERROR_DBG)
					fprintf (stderr, "stream_fill_proc(): WARNING stopped on partial read\n\r");
				#endif

				} /* if words_read */

			} /* if stream_fill_ptr */

		/* increment how many ticks have gone by */
		/* since we needed to fill */
		watchdog_count++;

		/* error check - the FIFO ISR has not emptied out a buffer */
		/* in the last N ticks - something must be wrong */
		if (watchdog_count > STREAM_FILL_TIMEOUT)
			{
			#ifdef DEBUG
			fprintf (stderr, "stream_fill_proc(): WARNING no fill in last %d ticks\n\r",
			         watchdog_count);
			#endif
			}

		sleep (1);

		/* check if a stream_stop() command was issued */
		/* while we were asleep  */
		if (stream_state == STREAM_STATE_DONE)
			{
			/* close the file */
			stream_close_file();
			/* flag outside world that we are really done */
			stream_active_flag = FALSE;
			/* we are done - die */
			return;
			}

		/* while we were asleep, someone restarted streaming */
		if (stream_state == STREAM_STATE_INIT)  
			{
			/* case 2: while we were asleep, a new */
			/* stream_start got issued -- just die quickly, no cleanup */
			return;
			}

		} /* while */

	/* error checking */
	#if (STREAM_DBG || STREAM_ERROR_DBG)
		fprintf (stderr, "stream_fill_proc(): WARNING stopped on EOF state\n\r");
	#endif		

	/* wait for the isr to finish reading through all */
	/* the game-side buffers */
	while (stream_state != STREAM_STATE_DONE)
		{
		sleep(1);
		}

	/* all done */
	stream_stop();

	#if (STREAM_DBG || STREAM_ERROR_DBG)
		printf ("stream_fill_proc(): end\n");
		printf ("\n");
	#endif		

} /* stream_fill_proc() */

/***** END of stream_fill_proc() *********************************************/

/*****************************************************************************/
/*                                                                           */
/* PROCESS: stream_simulate_isr_proc()                                       */
/*                                                                           */
/* For debugging only. Simulates the ISR reading 512 words at a time out of  */
/* the game-side buffer.                                                     */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 25 Nov 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

void stream_simulate_isr_proc (int *args)

{ /* stream_simulate_isr_proc() */

/*****************************************************************************/

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_simulate_isr_proc(): begin\n");
	#endif

	if(!fifo_size)
	{
		return;
	}

	while (1)
		{

		/* given an audio frame size of 7.68 msec, the */
		/* FIFO will interrupt the game approx twice */
		/* per 16 msec video frame */

		stream_fifo_isr();

		stream_fifo_isr();

		sleep(1);

		}

	#if STREAM_DBG
		printf ("stream_simulate_isr_proc(): end\n");
		printf ("\n");
	#endif		

} /* stream_simulate_isr_proc() */

/***** END of stream_simulate_isr_proc() *************************************/

/*****************************************************************************/
/*                                                                           */
/* PROCESS: stream_fifo_isr()                                                */
/*                                                                           */
/* Called by interrupt_ioasic_handler(), which is an interrupt service       */
/* routine.                                                                  */
/*                                                                           */
/* Walks through the game-side RAM buffers and sends 512 words of audio to   */
/* the sound FIFO's each time it is called.                                  */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 25 Nov 97 MVB                                                             */
/* 15 Jan 98 MVB - changed case where fill proc is dead - bug fix hopefully  */
/*                                                                           */
/*****************************************************************************/

void stream_fifo_isr (void)

{ /* stream_fifo_isr() */

int buffer_state;

int i;

/* the FIFO's are memory mapped - not going through an */
/* _ioctl() call because we want this to be as fast as */
/* possible */
static unsigned int *fifo = (unsigned int *) IOASIC_FIFO_ADDR;

/*****************************************************************************/

	#if STREAM_DBG_ISR
		printf ("\n");
		printf ("stream_fifo_isr(): begin\n");
	#endif

	if(!fifo_size)
	{
		return;
	}

	/* if the game-side buffers are done being filled */
	/* because the fill proc hit EOF, but the sound system */
	/* has not yet received the stop cmd, then just send */
	/* zero (silence) to the FIFO, until streaming is stopped */

	if (stream_state == STREAM_STATE_DONE)
		{
		/* fill the FIFO with zeroes */
		for (i=0; i < STREAM_FIFO_SIZE; i++)
			{
			*fifo = 0;
			}	
		return;
		}


	/* figure out which buffer we are in */
	if ((stream_read_ptr >= stream_a_ptr) && (stream_read_ptr < stream_b_ptr))
		buffer_state = stream_a_state;
	else
		buffer_state = stream_b_state;


	/* check that the buffer we are about to read from is full; */
	/* if we read into an empty buffer, just send zeroes (silence) */
	if (buffer_state == STREAM_BUFFER_EMPTY)
		{
		/* fill the FIFO with zeroes */
		for (i=0; i < STREAM_FIFO_SIZE; i++)
			{
			*fifo = 0;
			}	
		return;
		}


	/* read 512 words from the buffer */
	/* and send them to the FIFO */
	for (i=0; i < STREAM_FIFO_SIZE; i++)
		{
		/* write the data to the fifo */
		*fifo = *stream_read_ptr;
		stream_read_ptr++;
		}	


	/* update the buffer states for the fill proc */

	if (stream_read_ptr >= stream_end_ptr)
		{
		/* check for wrap around */
		stream_read_ptr = stream_a_ptr;

		/* if we wrapped into a full buffer, then snag it */
		/* and tell fill proc that it's now in use */
		if (stream_a_state == STREAM_BUFFER_FILLED)
			{
			stream_a_state = STREAM_BUFFER_INUSE;
			}
		stream_b_state = STREAM_BUFFER_EMPTY;
		}
	else if (stream_read_ptr >= stream_b_ptr)
		{
		/* then we must be in buffer B */
		if (stream_b_state == STREAM_BUFFER_FILLED)
			{
			stream_b_state = STREAM_BUFFER_INUSE;
			}
		/* if we just left A, tag it as empty */
		if (stream_a_state == STREAM_BUFFER_INUSE)
			{
			stream_a_state = STREAM_BUFFER_EMPTY;
			}
		}


	#if STREAM_DBG_ISR
		printf ("stream_fifo_isr(): end\n");
		printf ("\n");
	#endif		

} /* stream_fifo_isr() */

/***** END of stream_fifo_isr() **********************************************/

/*****************************************************************************/
/*                                                                           */
/* PROCESS: stream_fade_proc()                                               */
/*                                                                           */
/* Pass in the starting volume, the ending volume, and the fade time. This   */
/* can fade up and fade down.                                                */
/*                                                                           */
/* arg[0] - start volume (0=off, 255 = max)                                  */
/*                                                                           */
/* arg[1] - end volume                                                       */
/*                                                                           */
/* arg[2] - fade time                                                        */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 04 Dec 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

void stream_fade_proc (int *args)

{ /* stream_fade_proc() */

int start_volume;
int end_volume;
int fade_time;
float fade_delta;
float current_volume;
int i;

/*****************************************************************************/

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_fade_proc(): begin\n");
	#endif

	if(!fifo_size)
	{
		return;
	}

	/* extract the process args */
	start_volume = (int) args[0];
	end_volume = (int) args[1];
	fade_time = (int) args[2];

	/* figure out the fade delta */
	fade_delta = (float)(end_volume - start_volume) / (float) fade_time;

	current_volume = (float) start_volume;

	/* update each tick */
	for (i=0; i < fade_time; i++)
		{
		snd_stream_volume ((int) current_volume);
		current_volume += fade_delta;
		sleep(1);
		}

	/* force it to the final value */
	snd_stream_volume (end_volume);


	#if STREAM_DBG
		printf ("stream_fade_proc(): end\n");
		printf ("\n");
	#endif		

} /* stream_fade_proc() */

/***** END of stream_fade_proc() *********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: stream_fade()                                                   */
/*                                                                           */
/* Starts up a proc to fade the streaming volume up or down.                 */
/*                                                                           */
/* start_volume and end_volume can be low->high or vice versa (can fade up   */
/* or down).                                                                 */
/*                                                                           */
/* fade_time is in ticks.                                                    */
/*                                                                           */
/* See also stream_fade_to_zero().                                           */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 04 Dec 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

void stream_fade (int start_volume, int end_volume, int fade_time)

{ /* stream_fade() */

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_fade(): begin\n");
	#endif		

	if(!fifo_size)
	{
		return;
	}

	/* no duck or fade if already at zero */
	if (stream_volume == 0)
		return;

	qcreate ("str_fad", FADE_PID, stream_fade_proc, start_volume, 
	         end_volume, fade_time, 0);

	
	#if STREAM_DBG
		printf ("stream_fade(): end\n");
		printf ("\n");
	#endif		

} /* stream_fade() */

/***** END of stream_fade() **************************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: stream_fade_to_zero()                                           */
/*                                                                           */
/* Fades the streaming sound to zero over the N ticks specified. The         */
/* volume is whatever stream_volume is currently set to.                     */
/*                                                                           */
/* See also stream_fade().                                                   */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 15 Feb 98 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

void stream_fade_to_zero (int fade_time)

{ /* stream_fade_to_zero() */

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_fade_to_zero(): begin\n");
	#endif		


	if(!fifo_size)
	{
		return;
	}

	/* stream volume is stored global to this file */
	qcreate ("str_fad", FADE_PID, stream_fade_proc, stream_volume, 
	         0, fade_time, 0);

	
	#if STREAM_DBG
		printf ("stream_fade_to_zero(): end\n");
		printf ("\n");
	#endif		

} /* stream_fade_to_zero() */

/***** END of stream_fade_to_zero() ******************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: stream_duck()                                                   */
/*                                                                           */
/* Starts up a proc to duck the streaming under the music.                   */
/*                                                                           */
/* amount - how much to duck it                                              */
/*                                                                           */
/* duck_time - how long to keep it ducked                                    */
/*                                                                           */
/* ramp_time - how long to ramp it back up (some % of duck time)             */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 04 Dec 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

void stream_duck (int amount, int duck_time, int ramp_time)

{ /* stream_duck() */

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_duck(): begin\n");
	#endif		

	if(!fifo_size)
	{
		return;
	}

	/* no duck or fade if already at zero */
	if (stream_volume == 0)
		return;

	qcreate ("str_duk", FADE_PID, stream_duck_proc, amount, 
	         duck_time, ramp_time, 0);

	
	#if STREAM_DBG
		printf ("stream_duck(): end\n");
		printf ("\n");
	#endif		

} /* stream_duck() */

/***** END of stream_duck() **************************************************/

/*****************************************************************************/
/*                                                                           */
/* PROCESS: stream_duck_proc()                                               */
/*                                                                           */
/* Process started by stream_duck(). Assumes that the streaming volume has   */
/* been set and saved globally.                                              */
/*                                                                           */
/* arg[0] - amount to duck                                                   */
/*                                                                           */
/* arg[1] - how long to stay ducked (total, incl. ramp up time)              */
/*                                                                           */
/* arg[2] - how long the ramp up should last                                 */
/*                                                                           */
/* (c) 1998 Midway Games Inc.                                                */
/*                                                                           */
/* 11 Feb 98 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

void stream_duck_proc (int *args)

{ /* stream_duck_proc() */

int amount;
int duck_time;
int ramp_time;
int wait_time;
float fade_delta;
float current_volume;
int start_volume;
int end_volume;
int i;

/*****************************************************************************/

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_duck_proc(): begin\n");
	#endif		

	if(!fifo_size)
	{
		die(0);
	}

	/* extract the process args */
	amount = (int) args[0];
	duck_time = (int) args[1];
	ramp_time = (int) args[2];

	/* error check */
	if ((stream_volume - amount) < 0)
		{
		printf ("stream_duck_proc(): ERROR duck amount %d, current vol %d\n",
		        amount, stream_volume);
		return;
		}

	/* figure out the ramp-up delta */
	start_volume = stream_volume - amount;
	end_volume = stream_volume;
	fade_delta = (float)(end_volume - start_volume) / (float) ramp_time;


	wait_time = duck_time - ramp_time;
	/* error check */
	if (wait_time <= 0)
		{
		printf ("stream_duck_proc(): ERROR ramp_time less than duck_time\n");
		return;
		}

	/* duck the volume */
	current_volume = (float) start_volume;
	snd_stream_volume ((int) current_volume);

	/* wait until the ramp-up needs to start */
	sleep (wait_time);

	/* fade back in */
	/* update each tick */
	for (i=0; i < ramp_time; i++)
		{
		snd_stream_volume ((int) current_volume);
		current_volume += fade_delta;
		sleep(1);
		}

	/* force it to the final value */
	snd_stream_volume (end_volume);

	#if STREAM_DBG
		printf ("stream_duck_proc(): end\n");
		printf ("\n");
	#endif		

} /* stream_duck_proc() */

/***** END of stream_duck_proc() *********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: stream_set_volume()                                             */
/*                                                                           */
/* Use this function to set the streaming volume. It is saved gloablly in    */
/* this file, and then the duck commands can access it.                      */
/*                                                                           */
/* (c) 1998 Midway Games Inc.                                                */
/*                                                                           */
/* 11 Feb 98 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

void stream_set_volume (int volume)

{ /* stream_set_volume() */

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_set_volume(): begin\n");
	#endif		

	if(!fifo_size)
	{
		return;
	}

	/* send the volume to the sound system */
	snd_stream_volume (volume);

	/* save the volume setting global to this file */
	stream_volume = volume;
	
	#if STREAM_DBG
		printf ("stream_set_volume(): end\n");
		printf ("\n");
	#endif		

} /* stream_set_volume() */

/***** END of stream_set_volume() ********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: stream_change_file()                                            */
/*                                                                           */
/* Function to go from playing tune A to playing tune B.                     */
/*                                                                           */
/* Basic flow:                                                               */
/*                                                                           */
/* 1. kill the current fill proc (ptr is global to this file)                */
/*                                                                           */
/* 2. close the current file (file handle is global to this file)            */
/*                                                                           */
/* 3. open the new file                                                      */
/*                                                                           */
/* 4. restart the fill proc, using the new file                              */
/*                                                                           */
/* Hopefully:                                                                */
/*                                                                           */
/* - the buffers are full enough to give you time to switch files            */
/*                                                                           */
/* - hitting the disk to open the file doesn't bog it out                    */
/*                                                                           */
/* (c) 1998 Midway Games Inc.                                                */
/*                                                                           */
/* 29 Jan 98 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

int stream_change_file (char *filename)

{ /* stream_change_file() */

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_fade(): begin\n");
	#endif		

	if(!fifo_size)
	{
		return(ERROR);
	}

	/* error check */
	if (stream_state != STREAM_STATE_PLAYING)
		{
		#if STREAM_DBG
			fprintf (stderr, "stream_change_file(): ERROR streaming is not playing\n\r");
		#endif		
		return ERROR;
		}


	/*** 1. kill the current fill proc ***/

	if (existp ((STREAM_PID | NODESTRUCT), 0xFFFFFFFF))
		{
		/* kill the stream_fill_proc() */
		kill (stream_proc_ptr, 1);
		}

	/* null it out */
	stream_proc_ptr = NULL;

	/* release the disk DMA */
	stream_release_key();

	/*** 2. close the current file ***/
	stream_close_file();

	/*** 3. open the new file ***/
	stream_open_file (filename);


	/*** 4. restart the fill proc ***/

	/* note the different mode flag */
	stream_proc_ptr = iqcreate ("str_fil", (STREAM_PID | NODESTRUCT), 
	                            stream_fill_proc, STREAM_MODE_CHANGE, 0, 0, 0);

	return OK;

	#if STREAM_DBG
		printf ("stream_fade(): end\n");
		printf ("\n");
	#endif		

} /* stream_change_file() */

/***** END of stream_change_file() *******************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: stream_check_active()                                           */
/*                                                                           */
/* Returns flag for whether or not stream_fill_proc() is waiting or really   */
/* done.                                                                     */
/*                                                                           */
/* 04 May 98 MVB - Note: This should be checked by any other process that    */
/* stops streaming and is then going to do any sort of disk access.          */
/*                                                                           */
/* (c) 1998 Midway Games Inc.                                                */
/*                                                                           */
/* 20 Mar 98 MVB                                                             */
/*                                                                           */
/*****************************************************************************/

int stream_check_active (void)

{ /* stream_check_active() */

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_check_active(): begin\n");
	#endif		

	if(!fifo_size)
	{
		return(FALSE);
	}

	return stream_active_flag;

	
	#if STREAM_DBG
		printf ("stream_check_active(): end\n");
		printf ("\n");
	#endif		

} /* stream_check_active() */

/***** END of stream_check_active() ******************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: stream_get_key()                                                */
/*                                                                           */
/* stream_fill_proc() calls this to indicate that it is waiting for a        */
/* background/interrupt disk transfer to complete. No other procs should     */
/* do any fread() or disk transfer until this flag goes clear.               */
/*                                                                           */
/* (c) 1998 Midway Games Inc.                                                */
/*                                                                           */
/* 26 Apr 98 PGG                                                             */
/*                                                                           */
/*****************************************************************************/

void stream_get_key (void)

{ /* stream_get_key() */

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_get_key(): begin\n");
	#endif		

	if(!fifo_size)
	{
		return;
	}


	stream_key = TRUE;

	
	#if STREAM_DBG
		printf ("stream_get_key(): end\n");
		printf ("\n");
	#endif		

} /* stream_get_key() */

/***** END of stream_get_key() ***********************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: stream_release_key()                                            */
/*                                                                           */
/* stream_fill_proc() calls this to indicate that it completed its use of    */
/* the background/interrupt disk transfer. This means that other procs are   */
/* free to use the background/interrupt disk stuff.                          */
/*                                                                           */
/* (c) 1998 Midway Games Inc.                                                */
/*                                                                           */
/* 26 Apr 98 PGG                                                             */
/*                                                                           */
/*****************************************************************************/

void stream_release_key (void)

{ /* stream_release_key() */

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_release_key(): begin\n");
	#endif		

	if(!fifo_size)
	{
		return;
	}


	stream_key = FALSE;

	
	#if STREAM_DBG
		printf ("stream_release_key(): end\n");
		printf ("\n");
	#endif		

} /* stream_release_key() */

/***** END of stream_release_key() *******************************************/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: stream_check_key()                                              */
/*                                                                           */
/* Returns TRUE if the stream_fill_proc() is waiting for a disk interrupt to */
/* come back, otherwise FALSE. No other procs should do any fread() or       */
/* disk interrupt functions if the streaming is using it.i                   */
/*                                                                           */
/* (c) 1998 Midway Games Inc.                                                */
/*                                                                           */
/* 26 Apr 98 PGG                                                             */
/*                                                                           */
/*****************************************************************************/

int stream_check_key (void)

{ /* stream_check_key() */

	#if STREAM_DBG
		printf ("\n");
		printf ("stream_check_key(): begin\n");
	#endif		

	if(!fifo_size)
	{
		return(FALSE);
	}

	return stream_key;

	
	#if STREAM_DBG
		printf ("stream_check_key(): end\n");
		printf ("\n");
	#endif		

} /* stream_check_key() */

/***** END of stream_check_key() *********************************************/


int stream_detect_fifos(void)
{
	int	i;
	int	val;

#ifndef ENABLE_STREAMING
	fifo_size = 0;
	return 0;
#endif

	// Hold sound system in reset
	_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
	val &= ~(1<<0);
	_ioctl(4, FIOCSETSOUNDCONTROL, val);

	// Enable the FIFOs
	_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
	val |= (1<<1);
	_ioctl(4, FIOCSETSOUNDCONTROL, val);

	// Reset the FIFOs
	_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
	val &= ~(1<<2);
	_ioctl(4, FIOCSETSOUNDCONTROL, val);

	_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
	val |= (1<<2);
	_ioctl(4, FIOCSETSOUNDCONTROL, val);

	// Is status empty, NOT half-full, and NOT full ?
	_ioctl(4, FIOCGETSOUNDSTATUS, (int)&val);
	if(((val >> 3) & 7) == 1)
	{
		// YES - write 1 word to FIFOs
		*((volatile int *)IOASIC_FIFO_ADDR) = 5;

		// Is status NOT empty, NOT half-full, and NOT full ?
		_ioctl(4, FIOCGETSOUNDSTATUS, (int)&val);
		if(((val >> 3) & 7) != 0)
		{
			// NOPE - FIFO don't exist or are broken
			fifo_size = 0;

			// Reset FIFOs
			_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
			val &= ~(1<<2);
			_ioctl(4, FIOCSETSOUNDCONTROL, val);

			_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
			val |= (1<<2);
			_ioctl(4, FIOCSETSOUNDCONTROL, val);

			// Disable FIFOs
			_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
			val &= ~(1<<1);
			_ioctl(4, FIOCSETSOUNDCONTROL, val);

			_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
			val |= (1<<0);
			_ioctl(4, FIOCSETSOUNDCONTROL, val);

			return(fifo_size);
		}

		// Reset the FIFO's
		_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
		val &= ~(1<<2);
		_ioctl(4, FIOCSETSOUNDCONTROL, val);

		_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
		val |= (1<<2);
		_ioctl(4, FIOCSETSOUNDCONTROL, val);

		// Fill FIFO's with 129 words
		for(i = 0; i < 129; i++)
		{
			*((volatile int *)IOASIC_FIFO_ADDR) = 0;
		}

		// Does status indicate FIFO's are half full ?
		_ioctl(4, FIOCGETSOUNDSTATUS, (int)&val);
		if(((val >> 3) & 7) == 2)
		{
			// YES - FIFO's are 256 words
			fifo_size = 256;
		}

		else
		{
			// Fill FIFO's with another 128 words
			for(i = 0; i < 128; i++)
			{
				*((volatile int *)IOASIC_FIFO_ADDR) = 0;
			}

			// Does status indicate FIFO's are half full ?
			_ioctl(4, FIOCGETSOUNDSTATUS, (int)&val);
			if(((val >> 3) & 7) == 2)
			{
				// YES - FIFO's are 512 words
				fifo_size = 512;
			}
			else
			{
				// Fill FIFO's with another 256 words
				for(i = 0; i < 256; i++)
				{
					*((volatile int *)IOASIC_FIFO_ADDR) = 0;
				}

				// Does status indicate FIFO's are half full ?
				_ioctl(4, FIOCGETSOUNDSTATUS, (int)&val);
				if(((val >> 3) & 7) == 2)
				{
					// YES - FIFO's are 512 words
					fifo_size = 1024;
				}
				else
				{
					// NOPE - FIFO's are broken
					fifo_size = 0;

					// Reset FIFOs
					_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
					val &= ~(1<<2);
					_ioctl(4, FIOCSETSOUNDCONTROL, val);

					_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
					val |= (1<<2);
					_ioctl(4, FIOCSETSOUNDCONTROL, val);

					// Disable FIFOs
					_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
					val &= ~(1<<1);
					_ioctl(4, FIOCSETSOUNDCONTROL, val);

					_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
					val |= (1<<0);
					_ioctl(4, FIOCSETSOUNDCONTROL, val);

					return(fifo_size);
				}
			}
		}

		// Fill FIFO's the reset of the way
		for(i = 0; i < ((fifo_size - 1) >> 1); i++)
		{
			*((volatile int *)IOASIC_FIFO_ADDR) = 0;
		}

		// Does status indicate FIFO's are full ?
		_ioctl(4, FIOCGETSOUNDSTATUS, (int)&val);
		if(((val >> 3) & 7) != 6)
		{
			// NOPE
			fifo_size = 0;

			// Reset FIFOs
			_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
			val &= ~(1<<2);
			_ioctl(4, FIOCSETSOUNDCONTROL, val);

			_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
			val |= (1<<2);
			_ioctl(4, FIOCSETSOUNDCONTROL, val);

			// Disable FIFOs
			_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
			val &= ~(1<<1);
			_ioctl(4, FIOCSETSOUNDCONTROL, val);

  			_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
  			val |= (1<<0);
  			_ioctl(4, FIOCSETSOUNDCONTROL, val);

			return(fifo_size);
		}

		// Reset the FIFO's
		_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
		val &= ~(1<<2);
		_ioctl(4, FIOCSETSOUNDCONTROL, val);

		_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
		val |= (1<<2);
		_ioctl(4, FIOCSETSOUNDCONTROL, val);

		// Does status indicate FIFO's empty ?
		_ioctl(4, FIOCGETSOUNDSTATUS, (int)&val);
		if(((val >> 3) & 7) != 1)
		{
			// NOPE
			fifo_size = 0;
		}

		// Disable FIFOs
		_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
		val |= (1<<0);
		_ioctl(4, FIOCSETSOUNDCONTROL, val);

		_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
		val &= ~(1<<1);
		_ioctl(4, FIOCSETSOUNDCONTROL, val);

		return(fifo_size);
	}

	fifo_size = 0;

	// Reset FIFOs
  	_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
  	val &= ~(1<<2);
  	_ioctl(4, FIOCSETSOUNDCONTROL, val);

  	_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
  	val |= (1<<2);
  	_ioctl(4, FIOCSETSOUNDCONTROL, val);

	// Disable FIFOs
  	_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
  	val &= ~(1<<1);
  	_ioctl(4, FIOCSETSOUNDCONTROL, val);

  	_ioctl(4, FIOCGETSOUNDCONTROL, (int)&val);
  	val |= (1<<0);
  	_ioctl(4, FIOCSETSOUNDCONTROL, val);

	return(fifo_size);
}

/***** END of file STREAM.C **************************************************/
#endif	// #if 0