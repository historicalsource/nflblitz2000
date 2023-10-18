/* $Revision: 19 $ */

#include	<stdio.h>
#include	<fcntl.h>
#include	<unistd.h>
#include	<ioctl.h>
#include	<goose/goose.h>
#include	"include\game.h"

extern int	sysfont_height;
extern int	reload;

void show_pic_info(void);

void show_pic_info(void)
{
	int	serial_number;
	int	game_number;
	int	month;
	int	day;
	int	year;
	int	dom;
	float	y_pos;
	float	x_pos;
	int	fd;

	// Open the pic device
	if((fd = open("pic:", O_RDONLY)) < 0)
	{
		serial_number = -1;
		game_number = -1;
		dom = -1;
	}

	else
	{
		// Get the serial number
		_ioctl(fd, FIOCGETSERIALNUMBER, (int)&serial_number);

		// Get the game number
		_ioctl(fd, FIOCGETGAMENUMBER, (int)&game_number);

		// Get the dom code
		_ioctl(fd, FIOCGETDOM, (int)&dom);
	}

	// Close the pic device
	close(fd);

	// Check the game number
// original blitz
//	if(game_number != 528 && game_number != 444)
// new blitz

#if 1
//	if ((game_number != 528) &&		// dev pic
	if ((game_number != 498) &&		// 39" Cabinets
		(game_number != 494))		// Blitz 2000 Pic
#else
	if ((game_number != 528) &&		// dev pic
		(game_number != 481) &&		// 25" Blitz '99
		(game_number != 484) &&		// 39" Blitz '99
		(game_number != 467) &&		// 25" Showtime
		(game_number != 487))		// 39" Showtime
#endif
	{
		// Do nothing if game number is wrong
		while(1) ;
	}

	// Figure out the month, day, and year of manufacture
	dom--;
	day = (dom % 31) + 1;
	month = ((dom % 372) / 31) + 1;
	year = (dom / 372) + 80;
	
#if defined(VEGAS)
	if (!reload)
#endif
	{
		y_pos = SPRITE_VRES * .6F;
		x_pos = SPRITE_HRES / 2.0F;
		set_text_position(x_pos, y_pos, 1.0F);
		set_text_justification_mode(HJUST_CENTER|VJUST_BOTTOM);

		oprintf("%dcGame Serial Number: %dc%d",
			WHITE,
			LT_GREEN,
			serial_number);
		y_pos -= sysfont_height;
		set_text_position(x_pos, y_pos-1, 1.0F);
		oprintf("%dcDate of Manufacture: %dc%02d/%02d/%d",
			WHITE,
			LT_GREEN,
			month,
			day,
			(year % 100));

		/* Lets display what we have */
		sleep(1);
	}
}
