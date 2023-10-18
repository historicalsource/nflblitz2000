#if 0	// Not used

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<dir.h>
#include	<fcntl.h>
#include	<unistd.h>
#include	<ioctl.h>
#include	<goose/goose.h>
#include "include/fontid.h"
#include	"include/game.h"

#define	USER_OBJ_ID	0x55aa

//extern unsigned char	anim_data[];

//unsigned long crc(unsigned char *data, int len);

#define	NO_FILE		1
#define	OPEN_FAIL	2
#define	ALLOC_FAIL	4
#define	READ_FAIL	8
#define	CRC_FAIL		16

typedef struct	fcheck_data
{
	struct ffblk	ffblk __attribute__((packed));
	unsigned long	crc __attribute__((packed));
} fcheck_data_t;

static fcheck_data_t	*fc_array;

void do_file_check(void);

void do_file_check(void)
{
	struct ffblk	ffblk;
	unsigned long	crc32;
	unsigned long	*crc_ptr;
	int				files_not_found;
	int				file_open_errors;
	int				allocation_errors;
	int				read_errors;
	int				data_errors;
	FILE				*fp;
	int				i;
	int				num;
	int				num_files;
	unsigned char	*buffer;
	float				ypos;
	float				ypos_save;
	float				xpos;
	int				num_checked;

	// Set the justification mode
	set_text_justification_mode(HJUST_CENTER|VJUST_CENTER);

	// Set the string id
	set_string_id(0);

	// Get the info about the check file
	if(findfirst("filesys.chk", &ffblk, 0))
	{
		set_text_color(LT_RED);
		set_text_position(SPRITE_HRES / 2.0f, SPRITE_VRES / 2.0f, 1.0f);
		oprintf("FILE SYSTEM CHECK FILE NOT FOUND");
		return;
	}

	// Allocate memory for the file check file
	if((fc_array = (fcheck_data_t *)malloc(ffblk.ff_fsize)) == NULL)
	{
		set_text_color(LT_RED);
		set_text_position(SPRITE_HRES / 2.0f, SPRITE_VRES / 2.0f, 1.0f);
		oprintf("MEMORY ALLOCATION FAILURE");
		return;
	}

	// Open the check file
	if((fp = fopen("filesys.chk", "rb")) == (FILE *)0)
	{
		set_text_color(LT_RED);
		set_text_position(SPRITE_HRES / 2.0f, SPRITE_VRES / 2.0f, 1.0f);
		oprintf("CAN NOT OPEN FILE SYSTEM CHECK FILE");
		free(fc_array);
		return;
	}

	// Read the file check control file
	if(fread((void *)fc_array, sizeof(char), ffblk.ff_fsize, fp) != ffblk.ff_fsize)
	{
		// Close the check file
		fclose(fp);

		// ERROR
		set_text_color(LT_RED);
		set_text_position(SPRITE_HRES / 2.0f, SPRITE_VRES / 2.0f, 1.0f);
		oprintf("CAN NOT READ FILE SYSTEM CHECK FILE");
		return;
	}

	// Close the check data file
	fclose(fp);

	// Generate the crc for the file check file
	crc32 = crc((unsigned char *)fc_array, ffblk.ff_fsize - 4);

	// Check generated crc against the stored crc
	crc_ptr = (unsigned long *)fc_array + ((ffblk.ff_fsize / 4) - 1);
	if(crc32 != *crc_ptr)
	{
		// ERROR
		set_text_color(LT_RED);
		set_text_position(SPRITE_HRES / 2.0f, SPRITE_VRES / 2.0f, 1.0f);
		oprintf("CRC CHECK FAILURE ON FILESYS.CHK");
		free(fc_array);
		return;
	}
	
	// Figure out how many entries there are in the check data array
	num = ffblk.ff_fsize / sizeof(fcheck_data_t);
	num_files = num;

	set_text_position(SPRITE_HRES / 2.0f, SPRITE_VRES / 2.0f - 10.0f, 1.0f);
	set_text_color(WHITE);
	set_string_id(USER_OBJ_ID+1);
	oprintf("CHECKING %d FILE%s", num, (num>1?"S":""));
	set_string_id(USER_OBJ_ID);

	// Walk the check data array and check each file listed
	files_not_found = 0;
	file_open_errors = 0;
	allocation_errors = 0;
	read_errors = 0;
	data_errors = 0;

	// Check each file
	for(i = 0; i < num; i++)
	{
		// Show what is going on
		delete_multiple_strings(USER_OBJ_ID, -1);
		set_string_id(USER_OBJ_ID);
		set_text_font(FONTID_BAST10);
		set_text_position(SPRITE_HRES / 2.0f, SPRITE_VRES / 2.0f - 22.0f, 1.0f);
		set_text_color(WHITE);
		oprintf("%d - %s", i, fc_array[i].ffblk.ff_name);
		sleep(1);

		// Assume file is bad
		fc_array[i].ffblk.ff_attrib = 1;

		// Locate the file
		if(!findfirst(fc_array[i].ffblk.ff_name, &ffblk, 0))
		{
			// Open the file
			if((fp = fopen(fc_array[i].ffblk.ff_name, "rb")) != NULL)
			{
				// Allocate memory for the file
				if((buffer = (unsigned char *)malloc(ffblk.ff_fsize)) != NULL)
				{
					// Read the file
					if(fread(buffer, sizeof(char), ffblk.ff_fsize, fp) == ffblk.ff_fsize)
					{
						// Generate the CRC based on the size specified by the check file
						crc32 = crc(buffer, fc_array[i].ffblk.ff_fsize);

						// Is it good ?
						if(crc32 != fc_array[i].crc)
						{
							data_errors++;
						}
						else
						{
							fc_array[i].ffblk.ff_attrib = 0;
						}
					}
					else
					{
						read_errors++;
					}

					// Deallocate buffer memory
					free(buffer);
				}
				else
				{
					allocation_errors++;
				}

				// Close the file
				fclose(fp);
			}
			else
			{
				file_open_errors++;
			}
		}
		else
		{
			files_not_found++;
		}
	}

	num_checked = i;

	delete_multiple_strings(USER_OBJ_ID, 0xfffffffe);
	set_text_position(SPRITE_HRES / 2.0f, SPRITE_VRES / 2.0f - 10.0f, 1.0f);
	set_text_color(WHITE);
	oprintf("%d FILE%s CHECKED", num_checked, (num_checked>1?"S":""));

	if((files_not_found|file_open_errors|read_errors|data_errors))
	{
		// How many files have errors ?
		num = files_not_found + file_open_errors + allocation_errors + read_errors + data_errors;

		// Show the results
		ypos = SPRITE_VRES / 2.0f - 22.0f;
		set_text_position(SPRITE_HRES / 2.0f, ypos, 1.0f);
		set_text_color(LT_RED);
		oprintf("ERRORS DETECTED IN %d FILE%s", num, (num>1?"S":""));
		set_text_color(LT_YELLOW);
	}
	else
	{
		// Show the results
		set_text_position(SPRITE_HRES / 2.0f, SPRITE_VRES / 2.0f - 22.0f, 1.0f);
		set_text_color(LT_GREEN);
		oprintf("%d FILE%s OK", num_checked, (num_checked>1?"S":""));
	}

	// Delete all of the objects
	free(fc_array);
}
#endif
