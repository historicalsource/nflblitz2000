/*****************************************************************************/
/*                                                                           */
/* STREAM.H                                                                  */
/*                                                                           */
/* Function prototypes for STREAM.C, audio streaming using FIFOs.            */
/*                                                                           */
/* (c) 1997 Midway Games Inc.                                                */
/*                                                                           */
/* 25 Nov 97 MVB                                                             */
/*                                                                           */
/*****************************************************************************/


/***** FUNCTION PROTOTYPES ***************************************************/

int stream_init (char *filename);

void stream_start (void);

void stream_fifo_isr (void);

void stream_stop (void);

void stream_fade (int start_volume, int end_volume, int fade_time);

void stream_fade_to_zero (int fade_time);

int stream_change_file (char *filename);

void stream_set_volume (int volume);

void stream_duck (int amount, int duck_time, int ramp_time);

int stream_check_active (void);


// These suite of functions are used by procs to keep from colliding with disk DMA
void stream_get_key (void);
void stream_release_key (void);
int  stream_check_key (void);


/***** END of STREAM.H *******************************************************/
