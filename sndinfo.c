/* $Revision: 8 $ */

#include	<goose/goose.h>
#include	"include/game.h"

#define ERROR 0xEEEE

#define SND_CMD_SRAM_TEST    0x3A
#define SND_CMD_DRAM0_TEST   0x4A
#define SND_CMD_DRAM1_TEST   0x5A
#define SND_CMD_ROM_VERSION  0x6A
#define SND_CMD_ASIC_VER     0x7A
#define SND_CMD_ECHO         0x8A
#define SND_CMD_PMINT_SUM    0x9A  
#define SND_CMD_BONG         0xAA

/* completion codes (CC) for above diags */
#define SND_RTN_SRAM_PASSED   0xCC01
#define SND_RTN_DRAM0_PASSED  0xCC02
#define SND_RTN_DRAM1_PASSED  0xCC03
#define SND_RTN_BONG_FINISHED 0xCC04
#define SND_RTN_SRAM1_FAILED  0xEE01 
#define SND_RTN_SRAM2_FAILED  0xEE02
#define SND_RTN_SRAM3_FAILED  0xEE03
#define SND_RTN_DRAM0_FAILED  0xEE04 
#define SND_RTN_DRAM1_FAILED  0xEE05 

extern int	sysfont_height;
extern int	reload;

void show_sound_version(void);


void show_sound_version(void)
{
	int	boot_version;
	int	sdrc_version;
	int	opsys_version;
	int	port_status;
	int	pmint_sum;
	int	sram_status;
	int	dram_status;
	int	bong_status;
	float	y_pos;
	float	x_pos;

	snd_reset();

#ifndef NOTESTS
	y_pos = SPRITE_VRES * 0.4F;
	x_pos = SPRITE_HRES / 2.0F;

#if defined(VEGAS)
	if (!reload)
#endif
	{
		boot_version = snd_get_boot_version();
		boot_version &= 0xffff;

		set_text_position(x_pos, y_pos, 1.0F);
		set_text_justification_mode(HJUST_CENTER|VJUST_BOTTOM);

		oprintf("%dcSound Boot Version: %dc%X.%02X",
			WHITE,
			LT_GREEN,
			boot_version >> 8,
			boot_version & 0xff);
		sleep(1);

		sdrc_version = snd_get_sdrc_version();
		sdrc_version &= 0xffff;

		y_pos -= 15.0f;
		set_text_position(x_pos, y_pos, 1.0f);
		oprintf("%dcSound SDRC Version: %dc%X",
			WHITE,
			LT_GREEN,
			sdrc_version);
		sleep(1);

		port_status = snd_test_port();
		port_status &= 0xffff;

		y_pos -= 15.0f;
		set_text_position(x_pos, y_pos, 1.0f);
		oprintf("%dcSound Port Status: %dc%s",
			WHITE,
			(port_status == ERROR ? LT_RED: LT_GREEN),
			(port_status == ERROR ? "BAD" : "GOOD"));
		sleep(1);

		pmint_sum = snd_get_pmint_sum();
		pmint_sum &= 0xffff;

		y_pos -= 15.0f;
		set_text_position(x_pos, y_pos, 1.0f);
		oprintf("%dcSound Checksum: %dc%x",
			WHITE,
			LT_GREEN,
			pmint_sum);
		sleep(1);

		snd_send_command(SND_CMD_SRAM_TEST);
		sram_status = snd_wait_for_completion();
		sram_status &= 0xffff;

		y_pos -= 15.0f;
		set_text_position(x_pos, y_pos, 1.0f);
		switch(sram_status)
		{
			case SND_RTN_SRAM_PASSED:
			{
				oprintf("%dcSound SRAM:  %dcOK", WHITE, LT_GREEN);
				break;
			}
			case SND_RTN_SRAM1_FAILED:
			{
				oprintf("%dcSound SRAM:  %dcSRAM 1 failed", WHITE, LT_RED);
				break;
			}
			case SND_RTN_SRAM2_FAILED:
			{
				oprintf("%dcSound SRAM:  %dcSRAM 2 failed", WHITE, LT_RED);
				break;
			}
			case SND_RTN_SRAM3_FAILED:
			{
				oprintf("%dcSound SRAM:  %dcSRAM 3 failed", WHITE, LT_RED);
				break;
			}
			default:
			{
				oprintf("%dcSound static RAM:  %dcUNKNOWN ERROR", WHITE, LT_RED);
				break;
			}
		}
		sleep(1);

		snd_send_command(SND_CMD_DRAM0_TEST);
		dram_status = snd_wait_for_completion();
		dram_status &= 0xffff;

		y_pos -= 15.0f;
		set_text_position(x_pos, y_pos, 1.0f);
		switch(dram_status)
		{
			case SND_RTN_DRAM0_PASSED:
			{
				oprintf("%dcSound DRAM:  %dcOK", WHITE, LT_GREEN);
				break;
			}
			case SND_RTN_DRAM0_FAILED:
			{
				oprintf("%dcSound DRAM:  %dcDRAM failed", WHITE, LT_RED);
				break;
			}
			default:
			{
				oprintf("%dcSound DRAM:  %dcUNKNOWN ERROR", WHITE, LT_RED);
				break;
			}
		}
		sleep(1);

		snd_send_command(SND_CMD_BONG);
		bong_status = snd_wait_for_completion();
		bong_status &= 0xffff;

		y_pos -= 15.0f;
		set_text_position(x_pos, y_pos, 1.0f);
		oprintf("%dcSound Tone Status: %dc%s",
			WHITE,
			(bong_status != SND_RTN_BONG_FINISHED ? LT_RED: LT_GREEN),
			(bong_status != SND_RTN_BONG_FINISHED ? "BAD" : "GOOD"));
		sleep(1);
	}
#endif
	
	snd_load_opsys();

#ifndef NOTESTS

	opsys_version = snd_get_opsys_ver();
	opsys_version &= 0xffff;

#if defined(VEGAS)
	if (!reload)
#endif
	{
		y_pos -= 15.0f;
		set_text_position(x_pos, y_pos, 1.0f);
		oprintf("%dcSound OS Version: %dc%X.%02X",
			WHITE,
			LT_GREEN,
			opsys_version >> 8,
			opsys_version & 0xff);

		/* Lets display what we have */
		sleep(1);
	}
#endif
}
