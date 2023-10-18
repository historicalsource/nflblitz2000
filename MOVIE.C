/*
 *		$Archive: /video/nfl/MOVIE.C $
 *		$Revision: 32 $
 *		$Date: 10/05/99 7:54p $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifdef INCLUDE_SSID
char *ss_movie_c = "$Workfile: MOVIE.C $ $Revision: 32 $";
#endif

#include	<stdio.h>
#include	<stdlib.h>

/* GOOSE and GLIDE stuff */

#include	<goose/goose.h>
#if defined(VEGAS)
#include	<fxglide.h>
#else
#include	<glide/fxglide.h>
#endif

/* our stuff */
#if 0
#include	"stream.h"
#endif
#include	"/video/nfl/include/movie.h"
#include	"/video/nfl/include/id.h"
#include	"/video/nfl/include/externs.h"


/***** DEFINES ***************************************************************/

#if defined(VEGAS)
#define	MAX_FRAME_BUFFERS	 	2
#else
#define	MAX_FRAME_BUFFERS	 	4
#endif

#define	LOW_WATER_MARK		 	2
#define	HIGH_WATER_MARK		 	3
#define	SECTOR_SIZE			 	512

// Partition type definitions
#define	RAW_PARTITION		0			// Raw partition (No filesystem)
#define	FATFS_PARTITION		1			// FAT Filesystem partition
#define	EXT2FS_PARTITION	2			// Extended 2 Filesystem partition
#define	SWAP_PARTITION		3			// Swap Partition

#define	PART_MAGIC	(('P'<<24)|('A'<<16)|('R'<<8)|('T'<<0))


/***** TYPEDEFS **************************************************************/

// NOTES about partitions:
// If a partition table exists on the drive:
// Partition number 0 ALWAYS starts at block 0 and includes ALL blocks.
// This partition is reserved for raw access to the drive.
//
// All other partitions are usable by the user
//
// Format of the partition info structure
typedef struct partition_info
{
	int	starting_block;				// Logical starting block number
	int	num_blocks;					// Number of blocks in partition
	int	partition_type;				// Type of partition
} partition_info_t;


// Format of the partition information stored on the disk at sector 0,
// head 0, cylinder 0.  This table occupies exactly 1 sector of the disk.
typedef struct partition_table
{
	int					magic_number; 		// The magic number 'PART'
	int					num_partitions;	// Number of partitions
	partition_info_t	partition[0];		// Partition info
} partition_table_t;


typedef struct palette_data
{
	unsigned short	color[256];
} palette_data_t;


typedef struct movie_control
{
	struct movie_control	*next;
	int						current_sector;
	int						number_of_sectors;
	int						number_of_frames;
} movie_control_t;


typedef struct movie_buffer
{
	int	head;
	int	tail;
	int	num_in_queue;
	int	*frame_buffer[MAX_FRAME_BUFFERS];
	int	*frame_buffer_save[MAX_FRAME_BUFFERS];
} movie_buffer_t;



/***** VARIABLES *************************************************************/

/* EXTERNAL */


/* GLOBALS */

control_file_data_t		control;


/* INTERNAL */

static volatile movie_buffer_t    mb;
static volatile movie_control_t * mc_list = (movie_control_t *)0;
static volatile movie_control_t * cur_mc;
static volatile int movie_shutdown = 0;

static process_node_t * mrp;
static process_node_t * mdp;
static process_node_t * sig_proc = (process_node_t *)0;

static sprite_info_t  * mov_sprite[2] = {NULL, NULL};
static texture_node_t * mov_texture[4];
#if defined(VEGAS)
static Texture_node_t * tnode[4];
static volatile int     movie_reading_frame = 0;
#else
static GrMipMapInfo * mminfo[4];
static volatile int __read_issued = 0;
register int * dma_ptr asm("$28");
#endif
static unsigned short * conversion_buffer;
static unsigned short * convert_buf_save;

static float movie_x;
static float movie_y;
static float movie_z;

static int sectors_per_frame;
static int last_frame_hold;
static int frames = 0;
static int movie_proc_done;
static int movie_fps;
static int movie_state = MOVIE_STATE_DONE;

static char texture_name1[16];
static char texture_name2[16];

static image_info_t	movie_part1 =
{texture_name1, 338.0f, 384.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f};

static image_info_t	movie_part2 =
{texture_name2, 174.0f, 384.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f};


/***** FUNCTION PROTOTYPES (EXTERNAL) ****************************************/

short _SecReads(int, void *, int);
partition_table_t * ide_get_partition_table(void);
void flush_disk_queue(void);

/***** FUNCTION PROTOTYPES (PRIVATE) *****************************************/

static int  movie_init(char *basename[]);
static void movie_display_proc(int *);
#if defined(VEGAS)
static int movie_player_read_frame(void);
static void movie_download_it(Texture_node_t *, Texture_node_t *);
#else
static void movie_player_interrupt(int status);
static void movie_download_it(GrMipMapInfo *, GrMipMapInfo *);
#endif
static void movie_cleanup(void);


/***** FUNCTIONS (PUBLIC) ****************************************************/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: movie_config_player()                                           */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
void movie_config_player(float w, float h, float x, float y, float z, int fps)
{
	movie_part1.w = w * 0.6666f;
	movie_part1.h = h;
	movie_part2.w = w * 0.3333f;
	movie_part2.h = h;
	movie_x = x;
	movie_y = y;
	movie_z = z;
	movie_state = MOVIE_STATE_IDLE;
	movie_fps = fps;

}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: movie_set_chromakey()                                           */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
void movie_set_chromakey(int ckey)
{
	 mov_sprite[0]->state.chroma_color = ckey;
	 mov_sprite[1]->state.chroma_color = ckey;
}

void movie_set_chromakey_mode(int flag)
{
	if(flag)
	{
	 	mov_sprite[0]->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;
	 	mov_sprite[1]->state.chroma_key_mode = GR_CHROMAKEY_ENABLE;
	} 
	else
	{
		mov_sprite[0]->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
		mov_sprite[1]->state.chroma_key_mode = GR_CHROMAKEY_DISABLE;
	}
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: movie_set_transparency()                                        */
/*                                                                           */
/* Sets alpha blending modes to make movie transparent						     */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
void movie_set_transparency(float percent)
{
	unsigned char scale = (char)(255 * percent);

	mov_sprite[0]->state.alpha_source = GR_ALPHASOURCE_CC_ALPHA;
	mov_sprite[0]->state.alpha_rgb_src_function = GR_BLEND_SRC_ALPHA;
	mov_sprite[0]->state.alpha_rgb_dst_function = GR_BLEND_ONE_MINUS_SRC_ALPHA;
	mov_sprite[0]->state.alpha_a_src_function = GR_BLEND_ONE;
	mov_sprite[0]->state.alpha_a_dst_function = GR_BLEND_ZERO;
	mov_sprite[0]->state.constant_color = scale<<24;
	mov_sprite[1]->state.alpha_source = GR_ALPHASOURCE_CC_ALPHA;
	mov_sprite[1]->state.alpha_rgb_src_function = GR_BLEND_SRC_ALPHA;
	mov_sprite[1]->state.alpha_rgb_dst_function = GR_BLEND_ONE_MINUS_SRC_ALPHA;
	mov_sprite[1]->state.alpha_a_src_function = GR_BLEND_ONE;
	mov_sprite[1]->state.alpha_a_dst_function = GR_BLEND_ZERO;
	mov_sprite[1]->state.constant_color = scale<<24;
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: movie_proc()                                                    */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
void movie_proc(int *args)
{
	int	timeout;

#if defined(VEGAS)
	if (args[3])
		dio_mode(DIO_MODE_NO_BLOCK);
#endif

	// Initialize the movie player stuff
	if(!movie_init((char**)*args))
	{
		fprintf(stderr, "movie_reader_proc(): movie initialization failure\r\n");

#if defined(VEGAS)
		if (args[3])
			dio_mode(DIO_MODE_BLOCK);
#endif
		movie_state = MOVIE_STATE_DONE;
		die(0);
	}

#if defined(VEGAS)
	if (args[3])
		dio_mode(DIO_MODE_BLOCK);

	movie_reading_frame = 0;
#endif

	// Initialize the process pointer
	mrp = cur_proc;

	// Initialize the pointer to the process to signal when done
	sig_proc = (process_node_t *)*(args+1);

	// Initialize the last frame hold
	last_frame_hold = *(args+2);

	// Start up the movie displayer process
	mdp = qcreate("mdisp", 0, movie_display_proc, 0, 0, 0, 0);

#ifdef DEBUG
	// Start up the movie debug process
	qcreate("mdbg", 0, movie_debug_proc, 0, 0, 0, 0);
#endif

	// Set flag saying movie process is running
	movie_proc_done = 0;

	// Go into a loop reading movie frames into the movie frame ring buffer
	while(cur_mc)
	{
		while(cur_mc->number_of_frames > 0 && !movie_shutdown)
		{
#if defined(VEGAS)
			// Read a frame - did the read timeout ?
			if(!movie_player_read_frame())
			{
				// YES - abort the movie

				// Kill the displayer process
				kill(mdp, 0);

				// Delete all of the resources
				movie_cleanup();

				// Done
				die(0);
			}
#else
#if 0
			// Wait for key here...
			if(stream_check_key() == TRUE)
			{
				install_disk_callback(0);
			}
			while(stream_check_key() == TRUE)
			{
				sleep(1);
			}
			// Get key here...
			stream_get_key();
#endif
			// ... and install the disk interrupt...
			install_disk_callback(movie_player_interrupt);

			// Set read issued flag
			__read_issued = 1;

			// Prime the movie frame reader pump
			_SecReads(cur_mc->current_sector, mb.frame_buffer[mb.head], cur_mc->number_of_sectors);
	
			// Suspend until the movie reader disk interrupt signal handler detects
			// that the movie frame ring buffer full.  This basically means the
			// movie frame reader is able to fill the buffer faster than the movie
			// frame displayer is able to display the frames.  If the read
			// issued flag does not go not true within 11 ticks (192ms) it means
			// that for whatever reason the interrupt from the read above did
			// not occur.  In this case we abort the movie.
			timeout = 11;
			while(__read_issued && --timeout)
			{
				// Have all of the frames been read ?
				if(cur_mc->number_of_frames <= 0)
				{
					// YES - Done
					break;
				}
				sleep(1);
			}

			// Did we timeout ?
			if(!timeout)
			{
				// YES - Abort the movie
				__read_issued = 0;
				cur_mc->number_of_frames = 0;
				flush_disk_queue();
			}

			// Uninstall the disk interrupt...
			install_disk_callback(0);
#if 0
			// ...and release key here...
			stream_release_key();
#endif
			// If we get here it is because the movie frame reader is filling the
			// the movie frame ring buffer faster than the movie player process
			// is displaying the movie frames.  This is good and what we want to
			// happen.  At this point we wait for the movie frame displayer process
			// to empty the movie frame ring buffer down to the low water mark and
			// then re-prime the movie frame reader pump.
	
			// Loop waiting for the number of frames available to reach the low
			// water mark.
			while(mb.num_in_queue > LOW_WATER_MARK)
			{
				sleep(1);
			}
#endif
		}

		// Get next movie to read
		cur_mc = cur_mc->next;
		if(cur_mc)
		{
			frames += cur_mc->number_of_frames;
		}
		else
		{
			break;
		}
	}

	// Set flag saying movie process is done
	movie_proc_done = 1;

	// Suspend until killed by the displayer process	
	suspend_self();
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: movie_cleanup()                                                 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
static void movie_cleanup(void)
{
	movie_control_t * mc;
	movie_control_t * mc_save;
	int i;

	// Tell the reader process we are shutting down
	movie_shutdown = 1;

#if defined(VEGAS)
	// Don't kill the reader process unless there are NO frames being read
	while(movie_reading_frame) sleep(1);
#else
	// When done with the movie - uninstall the disk interrupt signal handler
	install_disk_callback(0);
#endif

	// Make sure the movie reader process is dead
	if(mrp)
	{
		kill(mrp, 0);
	}

	// Delete all of the movie control nodes
	mc = (movie_control_t *)mc_list;
	while(mc)
	{
		mc_save = mc->next;
		JFREE(mc);
		mc = mc_save;
	}
	mc_list = 0;

	// Delete the extra textures
	if(mov_sprite[0]->tn == mov_texture[0])
	{
		delete_texture(mov_texture[2]);
		delete_texture(mov_texture[3]);
	}
	else
	{
		delete_texture(mov_texture[0]);
		delete_texture(mov_texture[1]);
	}

	// Delete the sprites used for the movie
	delobj(mov_sprite[0]);
	delobj(mov_sprite[1]);
	mov_sprite[0] = NULL;
	mov_sprite[1] = NULL;

	// Free up the frame buffer memory
	for(i = 0; i < MAX_FRAME_BUFFERS; i++)
	{
		JFREE(mb.frame_buffer_save[i]);
		mb.frame_buffer[i] = NULL;
	}

	// Free up the conversion buffer memory
	JFREE(convert_buf_save);
	convert_buf_save = NULL;

	// Is there a process to be resumed ?
	if(sig_proc)
	{
		resume_process(sig_proc);
	}

	// Set the movie state to done
	movie_state = MOVIE_STATE_DONE;

#if defined(VEGAS)
	// Set disk driver to blocking mode
	dio_mode(DIO_MODE_BLOCK);
#else
	// Sleep for a tick to allow last texture transfer in DMA buffer to occur
	// and NOT collide with subsequent process texture loads.
	suspend_all();
	sleep(1);
	resume_all();
#endif
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: movie_pause()	                                                 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
void movie_pause(void)
{
	// Set the movie state to paused
	movie_state = MOVIE_STATE_PAUSED;

	// Suspend the movie reader and movie display processes
	suspend_process(mrp);
	suspend_process(mdp);
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: movie_resume()	                                                 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
void movie_resume(void)
{
	// Set the movie state to run
	movie_state = MOVIE_STATE_RUN;

	// Resume the movie reader and movie display processes
	resume_process(mrp);
	resume_process(mdp);
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: movie_get_state()                                               */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
int movie_get_state(void)
{
	return(movie_state);
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: movie_abort()	                                                 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
void movie_abort(void)
{
	// Make sure the movie player is shut down if it is running
	if(movie_get_state() > MOVIE_STATE_IDLE)
	{
		// Suspend the movie reader and display processes
		suspend_process(mrp);
		suspend_process(mdp);

#if defined(SEATTLE)
		// Wait for the interrupt handler to finish with what it has queued
		while(__read_issued)
		{
			sleep(1);
		}
#endif
#if 0
		/* gotta give back the key */
		stream_release_key();
#endif
		// Kill off the movie display process
		kill(mdp, 0);

		// Call the movie clean up
		movie_cleanup();
	}
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: movie_init()	                                                 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
static int movie_init(char *basename[])
{
	int								i;
	char								file_name[20];
	FILE								*fp;
	partition_table_t				*part_table;
	volatile movie_control_t	*mc;
	unsigned long					stored_crc;
	unsigned long					gen_crc;

	// Reset the movie shutdown flag
	movie_shutdown = 0;

#if defined(VEGAS)
	// Reset the reading frame flag
	movie_reading_frame = 0;

	// Set disk driver to blocking mode
//	dio_mode(DIO_MODE_BLOCK);
#else
	// Set disk driver to blocking mode
//	install_disk_callback(0);
#endif

	// Set the movie state to init
	movie_state = MOVIE_STATE_INIT;

	// Clear the process pointers
	mrp = (process_node_t *)0;
	mdp = (process_node_t *)0;

	// Set the starting texture names
	sprintf(texture_name1, "%s1.wms", basename[0]);
	sprintf(texture_name2, "%s2.wms", basename[0]);

	// Get the partition table
	part_table = ide_get_partition_table();

	// Allocate memory for the first movie sequence
	mc = (movie_control_t *)JMALLOC(sizeof(movie_control_t));
	if(!mc)
	{
#ifdef DEBUG
		fprintf(stderr, "movie_init(): Can not allocate memory for movie control node\r\n");
#endif
		movie_state = MOVIE_STATE_DONE;
		return(0);
	}

	// Set list pointer
	mc_list = mc;

	// Set this nodes next
	mc->next = (movie_control_t *)0;

	// Generate the control file name
	sprintf(file_name, "%s.cnt", basename[0]);

	// Open the control file
	if((fp = fopen(file_name, "rb")) == (FILE *)0)
	{
#ifdef DEBUG
		fprintf(stderr, "movie_init(): Can not open movie control file: %s\r\n", file_name);
#endif
		JFREE((void *)mc);
		movie_state = MOVIE_STATE_DONE;
		return(0);
	}

	// Read the CRC from the control file
	if(fread(&stored_crc, sizeof(unsigned int), 1, fp) != 1)
	{
#ifdef DEBUG
		fprintf(stderr, "movie_init():  Can not read CRC from control file: %s\r\n", file_name);
#endif
		fclose(fp);
		JFREE((void *)mc);
		movie_state = MOVIE_STATE_DONE;
		return(0);
	}

	// Read the movie control file
	if(fread(&control, sizeof(control_file_data_t), 1, fp) != 1)
	{
#ifdef DEBUG
		fprintf(stderr, "movie_init(): Can not read control file: %s\r\n", file_name);
#endif
		fclose(fp);
		JFREE((void *)mc);
		movie_state = MOVIE_STATE_DONE;
		return(0);
	}

	// Close the control file
	fclose(fp);

	// Generate a CRC on the data read from the file
	gen_crc = crc((void *)&control, sizeof(control_file_data_t));

	// Check the generated CRC against the stored CRC
	if(gen_crc != stored_crc)
	{
#ifdef DEBUG
		fprintf(stderr, "movie_init():  CRC error detected in movie control file: %s\r\n", file_name);
#endif
		JFREE((void *)mc);
		movie_state = MOVIE_STATE_DONE;
		return(0);
	}

	// Check to make sure movie is on a valid partition
	if(control.partition > (unsigned int)part_table->num_partitions)
	{
#ifdef DEBUG
		fprintf(stderr, "movie_init(): movie %s is not on a valid partition\r\n", file_name);
#endif
		JFREE((void *)mc);
		movie_state = MOVIE_STATE_DONE;
		return(0);
	}

	// Adjust the start block of the movie for the start of the partition
	control.start_block += part_table->partition[control.partition].starting_block;

	// Calculate number of sectors/frame
	sectors_per_frame = (control.size ? 384 : 192);

	// Initialize this nodes information
	mc->current_sector = control.start_block;
	mc->number_of_sectors = sectors_per_frame + 1;
	mc->number_of_frames = control.num_frames;

	// Set the control info from the first movie on the list
	control.start_block = mc_list->current_sector;
	control.num_frames = control.num_frames;
	frames = mc_list->number_of_frames - 1;

	// Set the current movie control pointer
	cur_mc = mc_list;

	// Allocate memory for the conversion buffer
	conversion_buffer = (unsigned short *)JMALLOC((sectors_per_frame * SECTOR_SIZE * 2) + 64);
	if(!conversion_buffer)
	{
#ifdef DEBUG
		fprintf(stderr, "movie_init(): Can not allocate memory for conversion buffer\r\n");
#endif
		JFREE((void *)mc);
		movie_state = MOVIE_STATE_DONE;
		return(0);
	}

	// Save the pointer
	convert_buf_save = conversion_buffer;

	// Cache page align the conversion buffer
	while((int)conversion_buffer & 0x1f)
	{
		++conversion_buffer;
	}

	// Create the sprites used to display the movie frames on screen
	// 2 sprites per movie frame
	mov_sprite[0] = _beginobj(NULL,&movie_part1, movie_x, movie_y, movie_z, 0x666);
	if(!mov_sprite[0])
	{
#ifdef DEBUG
		fprintf(stderr, "movie_init(): Can not create first sprite\r\n");
#endif
		JFREE(convert_buf_save);
		JFREE((void *)mc);
		convert_buf_save = NULL;
		movie_state = MOVIE_STATE_DONE;
		return(0);
	}
	else
	{
		hide_sprite(mov_sprite[0]);
	}

	mov_sprite[1] = _beginobj(NULL,&movie_part2, movie_x + movie_part1.w, movie_y, movie_z, 0x666);
	if(!mov_sprite[1])
	{
#ifdef DEBUG
		fprintf(stderr, "movie_init(): Can not create second sprite\r\n");
#endif
		delobj(mov_sprite[0]);
		mov_sprite[0] = NULL;
		JFREE(convert_buf_save);
		JFREE((void *)mc);
		convert_buf_save = NULL;
		movie_state = MOVIE_STATE_DONE;
		return(0);
	}
	else
	{
		hide_sprite(mov_sprite[1]);
	}

	// Create the textures used to display the movie frames
	// 2 textures per movie frame

	// A 256x256x16 texture
	mov_texture[0] = mov_sprite[0]->tn;

	// A 128x256x16 texture
	mov_texture[1] = mov_sprite[1]->tn;

	// A 256x256 texture
	mov_texture[2] = create_texture(texture_name1,
		0x666,
		0,
		CREATE_NORMAL_TEXTURE,
		GR_TEXTURECLAMP_CLAMP,
		GR_TEXTURECLAMP_CLAMP,
		GR_TEXTUREFILTER_BILINEAR,
		GR_TEXTUREFILTER_BILINEAR);

	if(!mov_texture[2])
	{
#ifdef DEBUG
		fprintf(stderr, "movie_init(): Can not create third texture node\r\n");
#endif
		delobj(mov_sprite[0]);
		delobj(mov_sprite[1]);
		mov_sprite[0] = NULL;
		mov_sprite[1] = NULL;
		JFREE(convert_buf_save);
		JFREE((void *)mc);
		convert_buf_save = NULL;
		movie_state = MOVIE_STATE_DONE;
		return(0);
	}

	// A 128x256x8 texture
	mov_texture[3] = create_texture(texture_name2,
		0x666,
		0,
		CREATE_NORMAL_TEXTURE,
		GR_TEXTURECLAMP_CLAMP,
		GR_TEXTURECLAMP_CLAMP,
		GR_TEXTUREFILTER_BILINEAR,
		GR_TEXTUREFILTER_BILINEAR);

	if(!mov_texture[3])
	{
#ifdef DEBUG
		fprintf(stderr, "movie_init(): Can not create forth texture node\r\n");
#endif
		delete_texture(mov_texture[2]);
		delobj(mov_sprite[0]);
		delobj(mov_sprite[1]);
		mov_sprite[0] = NULL;
		mov_sprite[1] = NULL;
		JFREE(convert_buf_save);
		JFREE((void *)mc);
		convert_buf_save = NULL;
		movie_state = MOVIE_STATE_DONE;
		return(0);
	}

#if defined(VEGAS)
	// Initialize the mip map info pointers
	tnode[0] = mov_texture[0]->texture_handle;
	tnode[1] = mov_texture[1]->texture_handle;
	tnode[2] = mov_texture[2]->texture_handle;
	tnode[3] = mov_texture[3]->texture_handle;
#else
	// Initialize the mip map info pointers
	mminfo[0] = &_GCs[0].mm_table.data[mov_texture[0]->texture_handle];
	mminfo[1] = &_GCs[0].mm_table.data[mov_texture[1]->texture_handle];
	mminfo[2] = &_GCs[0].mm_table.data[mov_texture[2]->texture_handle];
	mminfo[3] = &_GCs[0].mm_table.data[mov_texture[3]->texture_handle];
#endif

	// Initialize the circular queue of movie frames
	mb.head = mb.tail = 0;
	mb.num_in_queue = 0;

	// Allocate memory for the movie frames
	for(i = 0; i < MAX_FRAME_BUFFERS; i++)
	{
		mb.frame_buffer[i] = (int *)JMALLOC(((sectors_per_frame + 1) * SECTOR_SIZE) + 32);
		mb.frame_buffer_save[i] = mb.frame_buffer[i];
		if(!mb.frame_buffer[i])
		{
#ifdef DEBUG
			fprintf(stderr, "movie_init(): Can not allocate memory for movie frame buffer: %d\r\n", i);
#endif
			i--;
			while(i >= 0)
			{
				JFREE(mb.frame_buffer_save[i]);
				mb.frame_buffer[i] = NULL;
				--i;
			}
			delete_texture(mov_texture[2]);
			delete_texture(mov_texture[3]);
			delobj(mov_sprite[0]);
			delobj(mov_sprite[1]);
			mov_sprite[0] = NULL;
			mov_sprite[1] = NULL;
			JFREE(convert_buf_save);
			JFREE((void *)mc);
			convert_buf_save = NULL;
			movie_state = MOVIE_STATE_DONE;
			return(0);
		}
#if defined(VEGAS)
		// Make sure the frame buffers are cache page aligned
		while((int)mb.frame_buffer[i] & 0x1f)
		{
			mb.frame_buffer[i]++;
		}
#endif
	}

#if defined(VEGAS)
	// Read movie frames until the queue is filled
	while(mb.num_in_queue < MAX_FRAME_BUFFERS)
	{
		if(!movie_player_read_frame())
		{
			movie_state = MOVIE_STATE_DONE;
			return(0);
		}
	}
#endif

	// Return success
	return(1);
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: movie_display_proc()                                            */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
static void movie_display_proc(int *args)
{
	register int 				i;
	register int				frame_num;
#if defined(VEGAS)
	register unsigned short	*pal_data;
	register unsigned char	*pix_data;
	register unsigned int	*to;
#else
	register unsigned short	*pal_data asm("$9");
	register unsigned char	*pix_data asm("$10");
	register unsigned int	*to asm("$11");

	// Wait for the movie frame buffer to fill up before starting to display
	// movie frames.  Also abort if the movie process finishes and we never
	// see frames in the queue.
	while(mb.num_in_queue < MAX_FRAME_BUFFERS && !movie_proc_done)
	{
		sleep(1);
	}
#endif

	// Is the movie process done ?
	if(movie_proc_done)
	{
		// YES - then something went wrong with the movie and we abort
		frames = 0;
	}

	// Set the movie state to run
	movie_state = MOVIE_STATE_RUN;

	// At this point we can unhide the sprites
	unhide_sprite(mov_sprite[0]);
	unhide_sprite(mov_sprite[1]);

	// Loop to display all of the movie frames
	for(frame_num = 0; frame_num <= frames; frame_num++)
	{
		// Wait for frames to get into the queue
		// If none show up within 5 ticks something is wrong and we abort
#if defined(VEGAS)
		i = 5;
#else
		i = 3;
#endif
		while(!mb.num_in_queue && i--)
		{
			sleep(1);
		}

		// Did we ever get any frames ?
		if(i <= 0)
		{
			// NOPE - abort
#ifdef DEBUG
			fprintf(stderr, "movie_display_proc(): timeout waiting for frames\r\n");
#endif
			break;
		}

		// Is there a frame in the queue ?
		if(mb.num_in_queue)
		{
			// YES - Set the palette data pointer
			pal_data = (unsigned short *)mb.frame_buffer[mb.tail];

			// Set the pixel data pointer
			pix_data = (unsigned char *)(pal_data + 256);

			// Set the destination buffer pointer
			to = (int *)conversion_buffer;

			// Depalettize the pix data to a buffer
			i = (sectors_per_frame * SECTOR_SIZE) / 2;
			while(i)
			{
#if defined(SEATTLE)
				__asm__("	cache	0x11,0($10)");
				__asm__("	cache	0xd,0($11)");
#endif
				*to++ = (pal_data[pix_data[0]] | (pal_data[pix_data[1]] << 16));
				*to++ = (pal_data[pix_data[2]] | (pal_data[pix_data[3]] << 16));
				*to++ = (pal_data[pix_data[4]] | (pal_data[pix_data[5]] << 16));
				*to++ = (pal_data[pix_data[6]] | (pal_data[pix_data[7]] << 16));
				*to++ = (pal_data[pix_data[8]] | (pal_data[pix_data[9]] << 16));
				*to++ = (pal_data[pix_data[10]] | (pal_data[pix_data[11]] << 16));
				*to++ = (pal_data[pix_data[12]] | (pal_data[pix_data[13]] << 16));
				*to++ = (pal_data[pix_data[14]] | (pal_data[pix_data[15]] << 16));
#if defined(SEATTLE)
				__asm__("	cache	0x19,-32($11)");

				__asm__("	cache	0xd,0($11)");
#endif
				*to++ = (pal_data[pix_data[16]] | (pal_data[pix_data[17]] << 16));
				*to++ = (pal_data[pix_data[18]] | (pal_data[pix_data[19]] << 16));
				*to++ = (pal_data[pix_data[20]] | (pal_data[pix_data[21]] << 16));
				*to++ = (pal_data[pix_data[22]] | (pal_data[pix_data[23]] << 16));
				*to++ = (pal_data[pix_data[24]] | (pal_data[pix_data[25]] << 16));
				*to++ = (pal_data[pix_data[26]] | (pal_data[pix_data[27]] << 16));
				*to++ = (pal_data[pix_data[28]] | (pal_data[pix_data[29]] << 16));
				*to++ = (pal_data[pix_data[30]] | (pal_data[pix_data[31]] << 16));
#if defined(SEATTLE)
				__asm__("	cache	0x19,-32($11)");
#endif

				pix_data += 32;

				i -= 16;
			}

			// Set the sprites to point at their new textures
			if(!(frame_num & 1))
			{
#if defined(SEATTLE)
				movie_download_it(mminfo[0], mminfo[1]);
#endif
				mov_sprite[0]->tn = mov_texture[0];
				mov_sprite[1]->tn = mov_texture[1];
			}
			else
			{
#if defined(SEATTLE)
				movie_download_it(mminfo[2], mminfo[3]);
#endif
				mov_sprite[0]->tn = mov_texture[2];
				mov_sprite[1]->tn = mov_texture[3];
			}

#if defined(VEGAS)
			// Download the texture data for the sprites to the TMU
			movie_download_it(mov_sprite[0]->tn->texture_handle, mov_sprite[1]->tn->texture_handle);
#endif

			// Adjust the tail pointer
			mb.tail++;
			mb.tail &= (MAX_FRAME_BUFFERS - 1);

			// Decrement count of number in queue
			--mb.num_in_queue;

			// Let the frame display for 2 ticks (30fps)
			if(movie_fps == 30)
			{
				sleep(2);
			}
			else if(movie_fps == 60)
			{
				sleep(1);
			}
			else if(movie_fps == 45)
			{
				if(!(frame_num & 1))
				{
					sleep(1);
				}
				else
				{
					sleep(2);
				}
			}
			else
			{
				if(!(frame_num % 3))
				{
					sleep(1);
				}
				else
				{
					sleep(2);
				}
			}
		}
	}

	// Should the last frame be held ?
	if(last_frame_hold)
	{
		// Set the movie state to paused
		movie_state = MOVIE_STATE_HOLD;

#if defined(VEGAS)
		// Done anyway - uninstall the disk interrupt signal handler
		dio_mode(DIO_MODE_BLOCK);
#else
		// Done anyway - uninstall the disk interrupt signal handler
		install_disk_callback(0);
#endif

		// Let the last frame be seen for a bit
		sleep(last_frame_hold);
	}

	// Clean up all of the resources
	movie_cleanup();
}

/*===========================*/
/* The Really Big Conditional*/
/*                           */
#if defined(VEGAS)
/*===========================*/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: movie_player_read_frame()                                       */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
static int movie_player_read_frame(void)
{
	register int	timeout = 5;

	// Wait for a slot to open up in the queue
	// Timeout if one does not open up within 5 ticks
	while(mb.num_in_queue == MAX_FRAME_BUFFERS && --timeout)
	{
		sleep(1);
	}

	// Did we timeout ?
	if(!timeout)
	{
		// YES - return error
#ifdef DEBUG
		fprintf(stderr, "movie_player_read_frame(): timeout waiting for q space\r\n");
#endif
		return(0);
	}

#if 0
	// Wait until we get the key
	while(stream_check_key() == TRUE)
	{
		sleep(1);
	}

	// Get the key
	stream_get_key();
#endif

#if defined(VOODOO2)
	// Set disk driver to non-blocking mode
	dio_mode(DIO_MODE_NO_BLOCK);
#endif

	// Set flag saying read is in progress
	movie_reading_frame = 1;

	// Read the movie frame from the disk
	_SecReads(cur_mc->current_sector, mb.frame_buffer[mb.head], cur_mc->number_of_sectors);

	// Reset flag saying read is in progress
	movie_reading_frame = 0;

	// Adjust the number in the queue
	mb.num_in_queue++;

	// Adjust the head pointer
	mb.head++;
	mb.head &= (MAX_FRAME_BUFFERS - 1);

	// Adjust the current sector number
	cur_mc->current_sector += cur_mc->number_of_sectors;

	// Adjust the number of frames left
	cur_mc->number_of_frames--;

#if defined(VOODOO2)
	// Set the disk driver back to blocking mode
	dio_mode(DIO_MODE_BLOCK);
#endif

#if 0
	// Release the key
	stream_release_key();
#endif

	// Return success
	return(1);
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: movie_download_int()                                            */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
static void movie_download_it(Texture_node_t *t1, Texture_node_t *t2)
{
	t1->info.data = conversion_buffer;
	t2->info.data = conversion_buffer + (256*256);
	grTexDownloadMipMap(t1->attrib.tmu,
								t1->attrib.start_addr,
								t1->attrib.evenOdd,
								&t1->info);
	grTexDownloadMipMap(t2->attrib.tmu,
								t2->attrib.start_addr,
								t2->attrib.evenOdd,
								&t2->info);
}

/*===========================*/
/* The Really Big Conditional*/
/*                           */
#else //defined(VEGAS)
/*===========================*/

/*****************************************************************************/
/*                                                                           */
/* FUNCTION: movie_player_interrupt()                                        */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
static void movie_player_interrupt(int status)
{
	// Adjust the number in the queue
	mb.num_in_queue++;

	// Adjust the head pointer
	mb.head++;
	mb.head &= (MAX_FRAME_BUFFERS - 1);

	// Adjust the current sector number
	cur_mc->current_sector += cur_mc->number_of_sectors;

	// Adjust the number of frames left
	cur_mc->number_of_frames--;

	// Abort if no more to read
	if(!cur_mc->number_of_frames)
	{
		__read_issued = 0;
		return;
	}

	// Queue is NOT full
	if(mb.num_in_queue < MAX_FRAME_BUFFERS) 
	{
		__read_issued = 1;
		_SecReads(cur_mc->current_sector, mb.frame_buffer[mb.head], cur_mc->number_of_sectors);
	}
	else
	{
		__read_issued = 0;
	}
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: movie_download_int()                                            */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
static void movie_download_it(GrMipMapInfo *mminfo1, GrMipMapInfo *mminfo2)
{
	register int				y asm("$8");

	// Set up info for download
	__asm__("	cache	0xd,0($28)");
	dma_ptr[0] = 16;
	dma_ptr[1] = (int)&dma_ptr[4];
	dma_ptr[2] = 0x08000300;
	dma_ptr[3] = (int)&dma_ptr[8];
	dma_ptr[4] = mminfo1->tTextureMode;
	dma_ptr[5] = mminfo1->tLOD;
	dma_ptr[6] = 0;
	dma_ptr[7] = mminfo1->tmu_base_address;
	__asm__("	cache	0x19,0($28)");
	dma_ptr += 8;

	// Download first 65280 of 131072 bytes
	__asm__("	cache	0xd,0($28)");
	dma_ptr[0] = (256 * 255);
	dma_ptr[1] = (int)conversion_buffer & 0x1fffffff;
	dma_ptr[2] = 0x08800000;
	dma_ptr[3] = (int)&dma_ptr[4];

	// Download second 65280 of 131072 bytes
	dma_ptr[4] = (256*255);
	dma_ptr[5] = ((int)conversion_buffer + (256*255)) & 0x1fffffff;
	dma_ptr[6] = 0x08800000 + (256 * 255);
	dma_ptr[7] = (int)&dma_ptr[8];
	__asm__("	cache	0x19,0($28)");
	dma_ptr += 8;

	// Download last 512 of 131072 bytes
	__asm__("	cache	0xd,0($28)");
	dma_ptr[0] = 512;
	dma_ptr[1] = ((int)conversion_buffer + (256*255 * 2)) & 0x1fffffff;
	dma_ptr[2] = 0x08800000 + (256 * 255 * 2);
	dma_ptr[3] = (int)&dma_ptr[8];
	__asm__("	cache	0x19,0($28)");
	dma_ptr += 8;

	// Set up info for download
	__asm__("	cache	0xd,0($28)");
	dma_ptr[0] = 16;
	dma_ptr[1] = (int)&dma_ptr[4];
	dma_ptr[2] = 0x08000300;
	dma_ptr[3] = (int)&dma_ptr[8];
	dma_ptr[4] = mminfo2->tTextureMode;
	dma_ptr[5] = mminfo2->tLOD;
	dma_ptr[6] = 0;
	dma_ptr[7] = mminfo2->tmu_base_address;
	__asm__("	cache	0x19,0($28)");
	dma_ptr += 8;

	// Download 256 lines of 128 pixels
	for(y = 0; y < 256; y++)
	{
		if(!(y & 1))
		{
			__asm__("	cache	0xd,0($28)");
		}
		// Download last 128 pixels
		dma_ptr[0] = 256;
		dma_ptr[1] = ((int)conversion_buffer + (256*256*2) + (y*256)) & 0x1fffffff;
		dma_ptr[2] = 0x08800000 + (y*512);
		dma_ptr[3] = (int)&dma_ptr[4];
		if(y & 1)
		{
			__asm__("	cache	0x19,-16($28)");
		}
		dma_ptr += 4;
	}
}

/*===========================*/
/* The Really Big Conditional*/
/*                           */
#endif //defined(VEGAS)
/*===========================*/


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: movie_hide()	                                                 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
void movie_hide(void)
{
	if (mov_sprite[0]) hide_sprite(mov_sprite[0]);
	if (mov_sprite[1]) hide_sprite(mov_sprite[1]);
}


/*****************************************************************************/
/*                                                                           */
/* FUNCTION: movie_unhide()	                                                 */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
void movie_unhide(void)
{
	if (mov_sprite[0]) unhide_sprite(mov_sprite[0]);
	if (mov_sprite[1]) unhide_sprite(mov_sprite[1]);
}


#ifdef DEBUG
extern int halt;
/*****************************************************************************/
/*                                                                           */
/* FUNCTION: movie_debug_proc()                                              */
/*                                                                           */
/* (c) 1998 Midway Games, Inc.                                               */
/*                                                                           */
/*****************************************************************************/
void movie_debug_proc(int * pargs)
{
	float transparency = 1.0f;
	int use_chroma_key = 0;
	int last = 0;
	int step = 0;

	while (movie_get_state() != MOVIE_STATE_DONE)
	{
		int swnew = get_p5_sw_close() & 255;
		int swcur = get_p5_sw_current() & 255;

		if (step)
			movie_pause();
		step = FALSE;

		if (!existp(DEBUG_SW_PID,-1) && swnew == P_D && swcur == P_D)
			halt = !halt;

		if ( halt && halt != last && movie_state == MOVIE_STATE_RUN)
			movie_pause();

		if (!halt && halt != last && movie_state == MOVIE_STATE_PAUSED)
			movie_resume();

		if (movie_state == MOVIE_STATE_PAUSED)
		{
			if (swnew == P_RIGHT && swcur == P_RIGHT)
			{
				printf("Movie step\n");
				movie_resume();
				step = TRUE;
			}
			else
			if (swnew == P_LEFT && swcur == P_LEFT)
			{
				use_chroma_key = !use_chroma_key;
				printf("Movie chromakey: %s\n", use_chroma_key ? "ON":"OFF");
				movie_set_chromakey_mode(use_chroma_key); 
			}
			else
			if (swnew == P_UP && swcur == P_UP)
			{
				transparency += 0.1f;
				if (transparency > 1.0f)
					transparency = 1.0f;
				printf("Movie transparency: %4.1f%%\n", transparency*100.0f);
				movie_set_transparency(transparency);
			}
			else
			if (swnew == P_DOWN && swcur == P_DOWN)
			{
				transparency -= 0.1f;
				if (transparency < 0.0f)
					transparency = 0.0f;
				printf("Movie transparency: %4.1f%%\n", transparency*100.0f);
				movie_set_transparency(transparency);
			}
		}
		last = halt;

		sleep(1);
	}
}
#endif //DEBUG
