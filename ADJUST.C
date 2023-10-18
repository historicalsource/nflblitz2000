//
// adjust.c - Source for game specific adjustments in CMOS memory
//
// $Revision: 26 $
//
// $Author: Danielt $
//
#include	<goose/goose.h>
#include	"include/gameadj.h"

static void write_adjust_file(void);

//
// Table used to check the game specific adjustments
//
adjustment_info_t	nfl_adjustment_info[] = {
{CREDITS_FOR_FULL_GAME_ADJ, 4, 40, 10},
{DIFFICULTY_ADJ, 0, 10, 5},
{MOVIE_SND_ADJ, 0, 1, 1},
{ATTRACT_VOLUME_LEVEL_ADJ, 0, 255, 40},
{TIME_ADJ, 0, 4, 2},
{OPERATOR_MSG_ADJ, 0, 1, 0},
{H4_FREE_GAME_ADJ, 0, 1, 1},
{HH_FREE_GAME_ADJ, 0, 1, 0},
{HC_FREE_GAME_ADJ, 0, 1, 0},
{H4_FREE_QUARTER_ADJ, 0, 1, 0},
{HH_FREE_QUARTER_ADJ, 0, 1, 0},
{HC_FREE_QUARTER_ADJ, 0, 1, 0},
{VIOLENCE_ADJ, 0, 1, 0},
{SWEAR_ADJ, 0, 1, 1},
{CLEAR_RECORDS_ADJ, 0, 1, 1},
{TOURNAMENT_MODE_ADJ, 0, 1, 0},
{DOOR_OPEN_ADJ, 0x86, 0x8D, 0x86},
};

int check_all_adjustments(void)
{
	write_adjust_file();
	return(
		check_system_adjustments(
			sizeof(nfl_adjustment_info)/sizeof(adjustment_info_t),
			nfl_adjustment_info));
}

void restore_all_adjustments(void)
{
	restore_factory_adjustments(
		sizeof(nfl_adjustment_info)/sizeof(adjustment_info_t),
		nfl_adjustment_info);
}


static void write_adjust_file(void)
{
	FILE	*fp;

	if((fp = fopen("adjust.fmt", "wb")) == (FILE *)0)
	{
		fprintf(stderr, "Can not open ADJUST.FMT for writing\r\n");
		return;
	}

	ADD_HIDDEN_ADJUST(fp, ATTRACT_VOLUME_LEVEL_ADJ, nfl_adjustment_info);

	ADD_LIST_ADJUST(fp, DIFFICULTY_ADJ, "GAME DIFFICULTY", nfl_adjustment_info);
	ADJUST_LIST_NAME(fp, "DIFFICULTY LEVEL");
	ADJUST_LIST_ITEM(fp, "SUPER EASY");
	ADJUST_LIST_ITEM(fp, "VERY EASY");
	ADJUST_LIST_ITEM(fp, "EASIEST");
	ADJUST_LIST_ITEM(fp, "EASIER");
	ADJUST_LIST_ITEM(fp, "EASY");
	ADJUST_LIST_ITEM(fp, "NORMAL");
	ADJUST_LIST_ITEM(fp, "HARD");
	ADJUST_LIST_ITEM(fp, "HARDER");
	ADJUST_LIST_ITEM(fp, "HARDEST");
	ADJUST_LIST_ITEM(fp, "VERY HARD");
	ADJUST_LIST_ITEM(fp, "SUPER HARD");
	LIST_ADJUST_DONE(fp);

	ADD_DECIMAL_ADJUST(fp, CREDITS_FOR_FULL_GAME_ADJ, "DISCOUNT PRICE/CREDITS", nfl_adjustment_info);

	ADD_LIST_ADJUST(fp, TIME_ADJ, "CLOCK SPEED", nfl_adjustment_info);
	ADJUST_LIST_NAME(fp, "TIME PER QUARTER");
	ADJUST_LIST_ITEM(fp, "EXTRA SLOW");				//   APPROX. 3:30
	ADJUST_LIST_ITEM(fp, "SLOW");							//   APPROX. 3:00
	ADJUST_LIST_ITEM(fp, "MEDIUM");						//   APPROX. 2:30
	ADJUST_LIST_ITEM(fp, "FAST");							//   APPROX. 2:00
	ADJUST_LIST_ITEM(fp, "EXTRA FAST");				//   APPROX. 1:45
	LIST_ADJUST_DONE(fp);

//	ADD_ENABLE_DISABLE_ADJUST(fp, OPERATOR_MSG_ADJ, "OPERATOR MESSAGE", nfl_adjustment_info);
//	ADD_ENABLE_DISABLE_ADJUST(fp, HH_FREE_QUARTER_ADJ, "H VS. H FREE PERIODS", nfl_adjustment_info);
//	ADD_ENABLE_DISABLE_ADJUST(fp, HC_FREE_QUARTER_ADJ, "H VS. CPU FREE PERIODS", nfl_adjustment_info);
//	ADD_ENABLE_DISABLE_ADJUST(fp, HH_FREE_GAME_ADJ, "H VS. H FREE GAMES", nfl_adjustment_info);
//	ADD_ENABLE_DISABLE_ADJUST(fp, HC_FREE_GAME_ADJ, "H VS. CPU FREE GAMES", nfl_adjustment_info);

	ADD_ENABLE_DISABLE_ADJUST(fp, H4_FREE_GAME_ADJ,    "FREE GAME- FULL 4 PLR.", nfl_adjustment_info);
	ADD_ENABLE_DISABLE_ADJUST(fp, HH_FREE_GAME_ADJ,    "FREE GAME- FULL 1 ON 1", nfl_adjustment_info);
	ADD_ENABLE_DISABLE_ADJUST(fp, HC_FREE_GAME_ADJ,    "FREE GAME- FULL V. CPU", nfl_adjustment_info);

	ADD_ENABLE_DISABLE_ADJUST(fp, H4_FREE_QUARTER_ADJ, "FREE QUARTER- 4 PLR.", nfl_adjustment_info);
	ADD_ENABLE_DISABLE_ADJUST(fp, HH_FREE_QUARTER_ADJ, "FREE QUARTER- 1 ON 1", nfl_adjustment_info);
	ADD_ENABLE_DISABLE_ADJUST(fp, HC_FREE_QUARTER_ADJ, "FREE QUARTER- V. CPU", nfl_adjustment_info);

	ADD_ENABLE_DISABLE_ADJUST(fp, VIOLENCE_ADJ, "VIOLENCE", nfl_adjustment_info);

	ADD_ENABLE_DISABLE_ADJUST(fp, SWEAR_ADJ, "BAD LANGUAGE", nfl_adjustment_info);

	ADD_YES_NO_ADJUST(fp, CLEAR_RECORDS_ADJ, "CLEAR PLAYER RECORDS", nfl_adjustment_info);

	ADD_ENABLE_DISABLE_ADJUST(fp, TOURNAMENT_MODE_ADJ, "TOURNAMENT MODE", nfl_adjustment_info);

	ADD_ENABLE_DISABLE_ADJUST(fp, MOVIE_SND_ADJ, "ATTRACT MOVIE SOUND", nfl_adjustment_info);

	ADD_HIDDEN_ADJUST(fp, DOOR_OPEN_ADJ, nfl_adjustment_info);

 	fclose(fp);
}

