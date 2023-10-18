#include <stdio.h>
#include <stdlib.h>
#include	<string.h>
#include <dos.h>
#include	<dir.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

#include "include/fontid.h"
#include "include/game.h"
#include "include/nflcmos.h"
#include "include/select.h"
#include "include/sndcalls.h"
#include "include/id.h"
#include "include/gameadj.h"

extern font_info_t bast25_font;

#define	MAX_OP_LINES	16

static char	*op_line[MAX_OP_LINES];
static char	buffer[128];

void do_operator_message(int min, int max);

void do_operator_message(int min, int max)
{
	struct ffblk	ffblk;
	FILE				*fp;
	int				num_op_lines;
	int				val;
	int				i;
	char				*tmp;
	float				line_inc;
	float				space;
	float				ypos;
//	int				timeout;
	font_node_t		*fn = NULL;

	// Does the operator message file exist ?
	if(findfirst("operator.msg", &ffblk, 0))
	{
		// NOPE - Nothing to do
		return;
	}

	// Are we supposed to display the operator message ?
	if(get_adjustment(OPERATOR_MSG_ADJ, &val))
	{
		// CMOS FAIL - Don't display
		return;
	}
	else if(!val)
	{
		// Adjustment says don't display
		return;
	}

	// Open the operator message file
	if((fp = fopen("operator.msg", "rb")) == (FILE *)0)
	{
		// ERROR - do nothing
		return;
	}

	// Loop reading lines from the operator message file
	num_op_lines = 0;
	while(fgets(buffer, sizeof(buffer), fp))
	{
		// Allocate memory for the line
		op_line[num_op_lines] = (char *)JMALLOC(strlen(buffer) + 1);

		// Did we get memory ?
		if(!op_line[num_op_lines])
		{
			// NOPE - Free all of the allocations made thus far
			while(num_op_lines--)
			{
				JFREE(op_line[num_op_lines]);
			}

			// Close the file
			fclose(fp);

			// Done
			return;
		}
		else
		{
			// Copy the line
			strcpy(op_line[num_op_lines], buffer);

			// NULL terminate it at the <CR>
			tmp = strchr(op_line[num_op_lines], 0xa);
			if(tmp)
			{
				*tmp = 0;
			}

			// Increment line count
			++num_op_lines;

			// Max lines reached ?
			if(num_op_lines >= MAX_OP_LINES)
			{
				// YES - Only allow maximum lines
				break;
			}
		}
	}

	// Close the file
	fclose(fp);

	if((ffblk.ff_fsize % 4))
	{
		JFREE(op_line[num_op_lines-1]);
		num_op_lines--;
	}

	fn = create_font(&bast25_font, FONTID_BAST25);

	// Put up the background
	draw_backdrop();

	// Figure out the line spacing
	line_inc = get_font_height(FONTID_BAST10);
	line_inc += 3.0f;

	// Figure out how much space is needed
	if(fn)
	{
		space = line_inc * (float)(num_op_lines - 1);

		space += get_font_height(FONTID_BAST25);
		space += 15.0f;
	}
	else
	{
		space = line_inc * (float)num_op_lines;
	}

	// Figure the position of the first line
	ypos = (SPRITE_VRES + space) / 2.0f;

	if(fn)
	{
		set_text_font(FONTID_BAST25);
		set_text_justification_mode(HJUST_CENTER|VJUST_CENTER);
		set_string_id(0x55aa);
		set_text_color(LT_GREEN);
		set_text_z_inc(0.5f);
		set_text_position(SPRITE_HRES / 2.0f, ypos, 1.0f);
		oprintf("%s", op_line[0]);
		set_text_z_inc(0.0f);
		ypos -= get_font_height(FONTID_BAST25);
		ypos -= 15.0f;
	}

	// Set up all of the text drawing stuff
	set_text_font(FONTID_BAST10);
	set_text_justification_mode(HJUST_CENTER|VJUST_CENTER);
	set_string_id(0x55aa);
	set_text_color(WHITE);

	if(!fn)
	{
		set_text_position(SPRITE_HRES / 2.0f, ypos, 1.0f);
		oprintf("%s", op_line[0]);
		ypos -= line_inc;
	}

	// Put up all of the lines
	for(i = 1; i < num_op_lines; i++)
	{
		set_text_position(SPRITE_HRES / 2.0f, ypos, 1.0f);
		oprintf("%s", op_line[i]);
		ypos -= line_inc;
	}

	// Fade it up
	fade(1.0f, tsec / 2, NULL);

	// Let it be seen for at least the minimum
	sleep(min);

	// Let it be seen for up to the maximum
//	timeout = (tsec * 4) + (tsec / 2);
	while(--max && !get_player_sw_current())
	{
		sleep(1);
	}

	// Fade it down
	fade(0.0f, 15, NULL);
	sleep(15);

	// Delete the whole pile of crap
	delete_multiple_strings(0x55aa, -1);

	// Delete the backgound
	del1c(0, 0);

	// Free up the memory used
	for(i = 0; i < num_op_lines; i++)
	{
		JFREE(op_line[i]);
	}

	if(fn)
	{
		// Delete the big ass font
		delete_font(fn);
	}
}
