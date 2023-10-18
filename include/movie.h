#ifndef __MOVIE_H__
#define __MOVIE_H__
 
/*****************************************************************************/
/*                                                                           */
/* FILE: MOVIE.H                                                             */
/*                                                                           */
/* Plays movies; ripped off from NFL (thanx guys!)							 */                                                                          
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/* 22 Apr 98 PGG                                                             */
/*                                                                           */
/*****************************************************************************/


/***** DEFINES ***************************************************************/

enum {
		MOVIE_STATE_DONE = 0,
		MOVIE_STATE_IDLE,
		MOVIE_STATE_INIT,
		MOVIE_STATE_RUN,
		MOVIE_STATE_PAUSED,
		MOVIE_STATE_HOLD,
		MOVIE_STATE_TOTAL

	};

/***** TYPEDEFS **************************************************************/

typedef struct control_file_data
{
	unsigned long	num_frames;		// Number of frames in the movie
	unsigned long	partition;		// Parition on which movie resides
	unsigned long	start_block;	// Starting block number of movie
	unsigned long	zbuffer;		// ?
	unsigned long	size;			// 0 = 384x256x8  1 = 512x256x8
} control_file_data_t;

extern control_file_data_t		control;


/***** FUNCTION PROTOTYPES ***************************************************/

void movie_config_player(float w, float h, float x, float y, float z, int fps);
void movie_proc(int *args);
void movie_pause(void);
void movie_resume(void);
int  movie_get_state(void);
void movie_abort(void);
void movie_set_chromakey(int ckey);
void movie_set_chromakey_mode(int flag);
void movie_set_transparency(float percent);

void movie_hide(void);
void movie_unhide(void);
void movie_debug_proc(int * pargs);

/****** END of MOVIE.H *******************************************************/

#endif // __MOVIE_H__
