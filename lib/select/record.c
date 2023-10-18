#if defined(VEGAS)

#define USE_DISK_CMOS
void update_other_cmos_to_disk(int sleep_allowed);

#endif

/*******************************************************************
 *
 *    DESCRIPTION: Record handling
 *
 *    AUTHOR: Jeff Johnson
 *
 *    HISTORY: started Oct 3. 1996
 *
 * $Revision: 74 $
 * $Archive: /video/nfl/lib/select/record.c $
 * $Modtime: 9/16/99 7:13p $
 *
 *******************************************************************/

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

#include	"/video/nfl/include/fontid.h"
#include	"/video/nfl/include/game.h"
#include	"/video/nfl/include/audits.h"
#include	"/video/nfl/include/record.h"

#define	 NULL	0

static int team_tier( int );

//
// Global ram definitions
//
plyr_record_t	*rec_sort_ram = NULL;	// pointer to the array of PLAYER RECORDs from cmos into ram
extern const int team_ladder[8][5];
																													// pass completions
extern float pass_rating(int, int, int, int, int);	//								   tackles \\					   // pass touchdowns
//																							sacks \\					 // interceptions
//																						passing yards \\					// record number
// Default player record table...this is read into cmos at FACTORY RESTORE.					rushing yards \\				   // disk bank
//																							   world records \\					 // custome plays
static plyr_record_t  default_records[] = {	//													 pass attempts \\				   // trivia pts.
	{{'T','U','R','M','E','L'},{'0','3','2','2'},0,5,4,3,121,49+145,FALCONS|COLTS|BEARS|TITANS,	50,11,300,215,0,55,30, 9,5, 0,-1,9,2},
	{{'S','A','L',' ',' ',' '},{'0','2','0','1'},0,5,3,0,121,42+145,FALCONS|COLTS|BEARS,		52,12,300,221,0,44,32, 8,5, 1,-1,9,0},
	{{'J','A','P','P','L','E'},{'6','6','6','0'},0,5,4,4,126,56+145,FALCONS|COLTS|BEARS|RAIDERS,38,14,170,250,0,30,12,10,6, 2,-1,9,2},
	{{'J','E','N','I','F','R'},{'3','3','3','3'},0,5,3,1,124,72+145,FALCONS|COLTS|BEARS,		42,15,275,264,0,52,29,11,5, 3,-1,9,2},
	{{'L','U','I','S',' ',' '},{'3','3','3','3'},0,5,3,1,117,64+145,FALCONS|COLTS|BEARS,		40,20,295,214,0,45,34, 9,6, 4,-1,9,1},
	{{'D','A','N','I','E','L'},{'0','6','0','4'},0,5,3,2,122,55+145,FALCONS|COLTS|BEARS,		38,12,325,253,0,45,32, 9,6, 5,-1,9,3},
	{{'J','T',' ',' ',' ',' '},{'1','2','2','1'},0,5,4,1,110,59+145,FALCONS|COLTS|BEARS|COWBOYS,38,14,365,219,0,40,24,10,5, 6,-1,9,2},
	{{'M','I','K','E',' ',' '},{'3','3','3','3'},0,5,3,1,118,56+145,FALCONS|COLTS|BEARS,		36,12,120,223,0,43,23,12,6, 7,-1,9,2},
	{{'R','O','O','T',' ',' '},{'6','0','0','0'},0,5,3,1,121,59+145,FALCONS|COLTS|BEARS,		34,12,360,213,0,46,15,13,7, 8,-1,9,1},
	{{'P','A','U','L','O',' '},{'0','5','1','7'},0,5,3,1,116,75+145,FALCONS|COLTS|BEARS,		32,12,345,212,0,50,19,12,8, 9,-1,9,2},
	{{'G','R','I','N','C','H'},{'0','2','2','2'},0,5,3,1,116,75+145,FALCONS|COLTS|BEARS,		30,12,345,211,0,35,10, 5,6,10,-1,9,1},
	{{'G','U','I','D','O',' '},{'6','7','6','5'},0,5,3,0,107,74+145,COWBOYS|RAIDERS|CHARGERS,	34,10,300,210,0,40,15,10,6,11,-1,9,2},
	{{'J','A','S','O','N',' '},{'3','1','4','1'},0,5,3,1,118,56+145,FALCONS|COLTS|BEARS,		63,11,120,225,0,43,23,12,6, 7,-1,9,1},
	{{'M','X','V',' ',' ',' '},{'1','0','1','4'},0,5,4,2,120,56+145,FALCONS|COLTS|BEARS,		30,20,120,227,0,43,23,12,6, 7,-1,9,1},
	{{'E','D','D','I','E',' '},{'3','3','3','3'},0,5,3,1,119,56+145,FALCONS|COLTS|BEARS,		30,20,220,225,0,43,23,12,6, 7,-1,9,1}
};

static world_record_t  default_world_records[] = {
	{{'T','U','R','M','E','L'},{'0','3','2','2'},2},					// most sacks
	{{'S','A','L',' ',' ',' '},{'0','2','0','1'},5},					// most touchdowns in game
	{{'J','A','P','P','L','E'},{'6','6','6','0'},10},					// most tackles
	{{'D','A','N','I','E','L'},{'0','6','0','4'},2},					// most interceptions
	{{'J','E','N','I','F','R'},{'3','3','3','3'},21},					// most points scored in game
	{{'J','T',' ',' ',' ',' '},{'3','1','4','1'},10},					// largest victory margin (points)
};

//
// Variables
//
plyr_record_t		tmp_plyr_record;
plyr_record_t		tmp_default_rec;				// used for initializing record array
world_record_t		tmp_world_rec;

plyr_record_t		player1_data;
plyr_record_t		player2_data;
plyr_record_t		player3_data;
plyr_record_t		player4_data;
plyr_record_t *		plyr_data_ptrs[] = {&player1_data,&player2_data,&player3_data,&player4_data};

//externs
extern int					old_game_purchased;


//-------------------------------------------------------------------------------------------------
// This routine sets up cmos.. for the AUDITS, ADJUSTMENTS
//     HIGH SCORES TABLES...and then the PLAYER RECORDS
//
// INPUT:	nothing
// RETURNS: error code (see cmos.h for error codes)
//-------------------------------------------------------------------------------------------------
int init_cmos(void)
{
	struct cmos_config_info	cmos_init_struct;
	int	i;
	FILE	*fp;

	cmos_init_struct.cmos_size = CMOS_SIZE;
	cmos_init_struct.max_audits = NEXT_AUD + 1;
	cmos_init_struct.max_adjustments = NUM_ADJUSTMENTS;
	cmos_init_struct.num_gcr_tables = GEN_REC_ARRAYS;	// just one record field type (player recs)
	cmos_init_struct.user_data_size = 0;
	cmos_init_struct.gcr[PLYR_RECS_ARRAY].entry_size = sizeof(plyr_record_t);
	cmos_init_struct.gcr[PLYR_RECS_ARRAY].num_entries = NUM_PLYR_RECORDS;

	cmos_init_struct.gcr[WORLD_RECS_ARRAY].entry_size = sizeof(world_record_t);
	cmos_init_struct.gcr[WORLD_RECS_ARRAY].num_entries = NUM_WORLD_RECORDS;

	if((fp = fopen("audits.fmt", "wb")) == (FILE *)0)
	{
		fprintf(stderr, "Can not open file AUDITS.FMT\r\n");
#ifdef DEBUG
		lockup();
#endif
	}
	if(fp)
	{
		fprintf(fp, "cmos size %d audits %d adjustments %d gcrtables %d cmosdone\r\n",
			cmos_init_struct.cmos_size,
			cmos_init_struct.max_audits,
			cmos_init_struct.max_adjustments,
			cmos_init_struct.num_gcr_tables);

		for(i = 0; i < cmos_init_struct.num_gcr_tables; i++)
		{
			fprintf(fp, "cmosgcr entrysize %d numentries %d cmosgcrdone\r\n",
				cmos_init_struct.gcr[i].entry_size,
				cmos_init_struct.gcr[i].num_entries);
		}

		fprintf(fp, "\r\n");

		write_audit_info(fp);
		fflush(fp);
		fclose(fp);
	}

 	return (config_cmos(&cmos_init_struct));					// return error code.
}

//-------------------------------------------------------------------------------------------------
// 								This routine checks the player records for validity
//
// INPUT:		nothing
// RETURNS: nothing
//-------------------------------------------------------------------------------------------------
int validate_plyr_records(void)
{
	int		rec_num;
	int		status = 0;

 	for (rec_num=0; rec_num < NUM_PLYR_RECORDS; rec_num++)
	{
		if (get_generic_record(PLYR_RECS_ARRAY,rec_num,&tmp_plyr_record))
	  	{																														// bad record...clear it
			status = 1;
			clear_cmos_record((char *)&tmp_plyr_record,sizeof(plyr_record_t));
			if (set_generic_record(PLYR_RECS_ARRAY,rec_num,&tmp_plyr_record))
			{
#ifdef DEBUG
				fprintf(stderr, "Error occured in trying to set cmos record.\r\n");
				lockup();																									// record didnt save...
#endif
			}
	  	}
	}
	if(status)
	{
		set_default_plyr_records();
		set_default_world_records();
	}
	return(status);
}

//-------------------------------------------------------------------------------------------------
//								Sets up the first X records in cmos to some default values
//
// INPUT:	  nothing
// RETURNS: nothing
//-------------------------------------------------------------------------------------------------
void set_default_plyr_records(void)
{
	int rec_num;
	
	for (rec_num=0; rec_num < (sizeof(default_records)/sizeof(plyr_record_t)); rec_num++)
	{
		if (set_generic_record(PLYR_RECS_ARRAY,rec_num,&default_records[rec_num]))
		{
#ifdef DEBUG
			fprintf(stderr, "Error occured when trying to setup default player records.\r\n");
			lockup();																// write to cmos failed just lockup
#endif
		}
	}

	// Clear the remainder of the records (factory reset purposes)
	clear_cmos_record((char *)&tmp_plyr_record,sizeof(plyr_record_t));
	while(rec_num < NUM_PLYR_RECORDS)
	{
		if (set_generic_record(PLYR_RECS_ARRAY,rec_num,&tmp_plyr_record))
		{
#ifdef DEBUG
			fprintf(stderr, "Error occured in trying to set cmos record.\r\n");
			lockup();																									// record didnt save...
#endif
		}
		++rec_num;
	}
}

//-------------------------------------------------------------------------------------------------
//								Sets up the first X world records in cmos to some default values
//
// INPUT:	  nothing
// RETURNS: nothing
//-------------------------------------------------------------------------------------------------
void set_default_world_records(void)
{
	int rec_num;
	
	for (rec_num=0; rec_num < NUM_WORLD_RECORDS; rec_num++)
	{
		if (set_generic_record(WORLD_RECS_ARRAY,rec_num,&default_world_records[rec_num]))
		{
#ifdef DEBUG
			fprintf(stderr, "Error occured when trying to setup default world records.\r\n");
			lockup();																// write to cmos failed just lockup
#endif
		}
	}
}

//-------------------------------------------------------------------------------------------------
//					This routine clears (sets to 0) every element in the player record.
//
// INPUT:  ptr. to rec to clear, and number of bytes to clear
// RETURNS: nothing
//-------------------------------------------------------------------------------------------------
void clear_cmos_record(char *rec, int bytes_in_rec)
{
	while (bytes_in_rec)							//clear SIZE bytes in record
	{
		*rec++ = 0;
		bytes_in_rec--;
	}
}

//-------------------------------------------------------------------------------------------------
//										Get player record into ram from cmos
//
//   INPUT: ptr. to players entered name and pin number, and DESTINATION of player record data
// RETURNS: -1 if no record found
//-------------------------------------------------------------------------------------------------
//int get_player_record_from_cmos(char *name_n_pin, plyr_record_t *plyr_rec)
//{
//int rec_num;
//
//	rec_num = find_record_in_cmos(name_n_pin);
//	if (rec_num >= 0)																		// record found
//	{
//		get_generic_record(PLYR_RECS_ARRAY,rec_num,plyr_rec);			// get record from cmos into ram
//		return(rec_num);
//	}
//	else
//		return(-1);
//}

//-------------------------------------------------------------------------------------------------
//										Put player record into cmos from ram
//
//   INPUT: nothing
// RETURNS: nothing
//-------------------------------------------------------------------------------------------------
void save_player_records(void)
{
	plyr_record_t * prec;
	tdata_t * trec;
	int rec_num, i, x;
	int gmsk;
	int team_x, oppo_x;
	int score[2];

	gmsk = (1 << game_info.game_quarter) - 1;
	score[0] = compute_score(0);
	score[1] = compute_score(1);

	for (x = 0; x < (four_plr_ver ? MAX_PLYRS : 2); x++)
	{
		team_x = !(oppo_x = !((x > 1) || (!four_plr_ver && x)));

		prec = plyr_data_ptrs[x];
		trec = &tdata_blocks[team_x];

		if (prec->plyr_name[0] != '\0' &&
			prec->plyr_name[0] != '-' &&
			qtr_pstatus[x] == gmsk)
		{
			rec_num = find_record_in_cmos(prec->plyr_name);

			if (rec_num >= 0)
	 		{
				// get found record!!
				get_generic_record(PLYR_RECS_ARRAY, rec_num, &tmp_plyr_record);
				tmp_plyr_record.games_played++;
				tmp_plyr_record.last_used = 0;
			}
			else
			{
				// no record found; create new one or use 'least' used record
				rec_num = get_new_record_into_ram(prec->plyr_name);

				tmp_plyr_record.games_played = 1;
				tmp_plyr_record.last_used    = 1;
			}

			tmp_plyr_record.points_scored  += score[team_x];
			tmp_plyr_record.points_allowed += score[oppo_x];

			if (score[team_x] > score[oppo_x])
			{
				tmp_plyr_record.games_won++;
				tmp_plyr_record.winstreak++;
				
				if ((!four_plr_ver && !qtr_pstatus[x^1]) ||
					 (four_plr_ver && !qtr_pstatus[x^2] && !qtr_pstatus[x^3]) ||
					 (team_tier(game_info.team[oppo_x]) < 7 ))
				{
					i = tmp_plyr_record.teams_defeated;
					tmp_plyr_record.teams_defeated |= 1 << game_info.team[oppo_x];
					if (tmp_plyr_record.teams_defeated == ALLTEAMS_MASK &&
							i != ALLTEAMS_MASK)
						do_grand_champ |= (1 << x);
				}
			}
			else
				tmp_plyr_record.winstreak = 0;

			tmp_plyr_record.tackles          += trec->tackles;
			tmp_plyr_record.passing_yds      += trec->passing_yards;
			tmp_plyr_record.rushing_yds      += trec->rushing_yards;
			tmp_plyr_record.sacks            += trec->sacks;
			tmp_plyr_record.pass_attempts    += trec->passes;
			tmp_plyr_record.pass_completions += trec->completions;
			tmp_plyr_record.pass_touchdowns  += trec->pass_touchdowns;
			tmp_plyr_record.intercepted      += tdata_blocks[oppo_x].interceptions;
			tmp_plyr_record.trivia_pts        = prec->trivia_pts;

			set_generic_record(PLYR_RECS_ARRAY, rec_num, &tmp_plyr_record);
		}
	}
	//!!! FIX Is this happening?
	//inc_cmos_records_used_cnt();
#ifdef USE_DISK_CMOS
	/* write out the CMOS to disk */
	write_cmos_to_disk(TRUE);
	update_other_cmos_to_disk(TRUE);
#endif
}

//			rec_num = get_free_record_nbr();
//			// didnt find record...create new one or use 'least' used record
//			clear_cmos_record((char *)&tmp_plyr_record,sizeof(plyr_record_t));		// init. record
//
//			// copy over name and pin
//			for (i=0; i < LETTERS_IN_NAME+PIN_NUMBERS; i++)
//				tmp_plyr_record.plyr_name[i] = prec->plyr_name[i];
//
//			tmp_plyr_record.cmos_rec_num = rec_num;
//			tmp_plyr_record.disk_bank = 0;
//			tmp_plyr_record.plays_allowed = 5;


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int get_new_record_into_ram(char *name_and_pin)
{
	int rec_num,i;

	rec_num = get_free_record_nbr();

	clear_cmos_record((char *)&tmp_plyr_record,sizeof(plyr_record_t));		// init. record

	// copy over name and pin
	for (i=0; i < LETTERS_IN_NAME+PIN_NUMBERS; i++)
		tmp_plyr_record.plyr_name[i] = name_and_pin[i];

	// set some default stuff
	tmp_plyr_record.cmos_rec_num = rec_num;
	tmp_plyr_record.disk_bank = -1;
	tmp_plyr_record.plays_allowed = 3;

	return(rec_num);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void inc_cmos_records_used_cnt(void)
{
int rec_num;

	// inc all records 'last_used' count
	for (rec_num=0; rec_num < NUM_PLYR_RECORDS; rec_num++)
	{
		if (get_generic_record(PLYR_RECS_ARRAY,rec_num,&tmp_plyr_record))	  	 // get cmos record into ram
		{
			if (tmp_plyr_record.games_played > 0)
			{
				tmp_plyr_record.last_used += 1;
				set_generic_record(PLYR_RECS_ARRAY,rec_num,&tmp_plyr_record);			// save record
			}
		}
	}
}
	
//-------------------------------------------------------------------------------------------------
//										Try to find a matching record in cmos
//
//   INPUT:  ptr. to players entered name and pin number
//  RETURN: matching record number else -1
//-------------------------------------------------------------------------------------------------
int find_record_in_cmos(char *pname_and_pin)
{
int rec_num;
	
	for (rec_num=0; rec_num < NUM_PLYR_RECORDS; rec_num++)
	{
		if (get_generic_record(PLYR_RECS_ARRAY,rec_num,&tmp_plyr_record))	  	 // get cmos record into ram
			clear_cmos_record((char *)&tmp_plyr_record,sizeof(plyr_record_t));    // record was corrupt
		else if (!(compare_record((char *)&tmp_plyr_record.plyr_name,pname_and_pin,LETTERS_IN_NAME+PIN_NUMBERS)))
			return(rec_num);																												// match found
	}
	return(-1);																																	// no match found
}

//-------------------------------------------------------------------------------------------------
//									Try to find a matching record in RECORD SORT ram
//
//   INPUT:  ptr. to players entered name and pin number
//  RETURN: matching record number else -1
//-------------------------------------------------------------------------------------------------
int find_record_in_ram(char *pname_and_pin)
{
	int rec_num;
	plyr_record_t		*plr_rec;

	if(rec_sort_ram)
	{	
		plr_rec = rec_sort_ram;
		for (rec_num=0; rec_num < NUM_PLYR_RECORDS; rec_num++,plr_rec++)
	 	{
			if (!(compare_record((char *)plr_rec->plyr_name,pname_and_pin,LETTERS_IN_NAME+PIN_NUMBERS)))
			{
				return(rec_num+1);																												// match found
			}
		}
	}
	return(-1);																																	// no match found
}

//-------------------------------------------------------------------------------------------------
//					Check to determine if the player has beaten any world records, if
// record was broken the new info is stored and 1 is returned.
//-------------------------------------------------------------------------------------------------
void check_world_records_2p(void)
{
plyr_record_t	*prec;
int	diff;

	//
	// Player 1
	//
	prec = &player1_data;
	if (prec->plyr_name[0] != NULL)
	{
		// most sacks
		if (!(get_generic_record(WORLD_RECS_ARRAY,0,&tmp_world_rec)))
		{
			if (tmp_world_rec.value < tdata_blocks[0].sacks)
			{
				tmp_world_rec.value = tdata_blocks[0].sacks;
				strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
				set_generic_record(WORLD_RECS_ARRAY,0,&tmp_world_rec);
			}
		}
		// most TD's in game
		if (!(get_generic_record(WORLD_RECS_ARRAY,1,&tmp_world_rec)))
		{
			if (tmp_world_rec.value < tdata_blocks[0].touchdowns)
			{
				tmp_world_rec.value = tdata_blocks[0].touchdowns;
				strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
				set_generic_record(WORLD_RECS_ARRAY,1,&tmp_world_rec);
			}
		}
		// most tackles
		if (!(get_generic_record(WORLD_RECS_ARRAY,2,&tmp_world_rec)))
		{
			if (tmp_world_rec.value < tdata_blocks[0].tackles)
			{
				tmp_world_rec.value = tdata_blocks[0].tackles;
				strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
				set_generic_record(WORLD_RECS_ARRAY,2,&tmp_world_rec);
			}
		}
		// most interceptions
		if (!(get_generic_record(WORLD_RECS_ARRAY,3,&tmp_world_rec)))
		{
			if (tmp_world_rec.value < tdata_blocks[0].interceptions)
			{
				tmp_world_rec.value = tdata_blocks[0].interceptions;
				strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
				set_generic_record(WORLD_RECS_ARRAY,3,&tmp_world_rec);
			}
		}
		// most points scored in game
		if (!(get_generic_record(WORLD_RECS_ARRAY,4,&tmp_world_rec)))
		{
			if (tmp_world_rec.value < compute_score(0))
			{
				tmp_world_rec.value = compute_score(0);
				strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
				set_generic_record(WORLD_RECS_ARRAY,4,&tmp_world_rec);
			}
		}
		// largest victory margin
		if (!(get_generic_record(WORLD_RECS_ARRAY,5,&tmp_world_rec)))
		{
			diff = compute_score(0) - compute_score(1);
			if ((diff > 0) && (tmp_world_rec.value < diff))
			{
				tmp_world_rec.value = diff;
				strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
				set_generic_record(WORLD_RECS_ARRAY,5,&tmp_world_rec);
			}
		}
	}	

	//
	// Player 2
	//
	prec = &player2_data;
	if (prec->plyr_name[0] != NULL)
	{
		// most sacks
		if (!(get_generic_record(WORLD_RECS_ARRAY,0,&tmp_world_rec)))
		{
			if (tmp_world_rec.value < tdata_blocks[1].sacks)
			{
				tmp_world_rec.value = tdata_blocks[1].sacks;
				strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
				set_generic_record(WORLD_RECS_ARRAY,0,&tmp_world_rec);
			}
		}
		// most TD's in game
		if (!(get_generic_record(WORLD_RECS_ARRAY,1,&tmp_world_rec)))
		{
			if (tmp_world_rec.value < tdata_blocks[1].touchdowns)
			{
				tmp_world_rec.value = tdata_blocks[1].touchdowns;
				strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
				set_generic_record(WORLD_RECS_ARRAY,1,&tmp_world_rec);
			}
		}
		// most tackles
		if (!(get_generic_record(WORLD_RECS_ARRAY,2,&tmp_world_rec)))
		{
			if (tmp_world_rec.value < tdata_blocks[1].tackles)
			{
				tmp_world_rec.value = tdata_blocks[1].tackles;
				strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
				set_generic_record(WORLD_RECS_ARRAY,2,&tmp_world_rec);
			}
		}
		// most interceptions
		if (!(get_generic_record(WORLD_RECS_ARRAY,3,&tmp_world_rec)))
		{
			if (tmp_world_rec.value < tdata_blocks[1].interceptions)
			{
				tmp_world_rec.value = tdata_blocks[1].interceptions;
				strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
				set_generic_record(WORLD_RECS_ARRAY,3,&tmp_world_rec);
			}
		}
		// most points scored in game
		if (!(get_generic_record(WORLD_RECS_ARRAY,4,&tmp_world_rec)))
		{
			if (tmp_world_rec.value < compute_score(1))
			{
				tmp_world_rec.value = compute_score(1);
				strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
				set_generic_record(WORLD_RECS_ARRAY,4,&tmp_world_rec);
			}
		}
		// largest victory margin
		if (!(get_generic_record(WORLD_RECS_ARRAY,5,&tmp_world_rec)))
		{
			diff = compute_score(1) - compute_score(0);
			if ((diff > 0) && (tmp_world_rec.value < diff))
			{
				tmp_world_rec.value = diff;
				strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
				set_generic_record(WORLD_RECS_ARRAY,5,&tmp_world_rec);
			}
		}
	}	
}

//-------------------------------------------------------------------------------------------------
//					Check to determine if the player has beaten any world records, if
// record was broken the new info is stored and 1 is returned.
//-------------------------------------------------------------------------------------------------
void check_world_records_4p(void)
{
	plyr_record_t	*prec;
	int	diff,i;

	//
	// Player 1 and Player 2 (team 1)
	//
	for (i=0; i < 2; i++)
	{
		prec = plyr_data_ptrs[i];
//		prec = &player1_data;
		if (prec->plyr_name[0] != NULL)
		{
			// most sacks
			if (!(get_generic_record(WORLD_RECS_ARRAY,0,&tmp_world_rec)))
			{
				if (tmp_world_rec.value < tdata_blocks[0].sacks)
				{
					tmp_world_rec.value = tdata_blocks[0].sacks;
					strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
					set_generic_record(WORLD_RECS_ARRAY,0,&tmp_world_rec);
				}
			}
			// most TD's in game
			if (!(get_generic_record(WORLD_RECS_ARRAY,1,&tmp_world_rec)))
			{
				if (tmp_world_rec.value < tdata_blocks[0].touchdowns)
				{
					tmp_world_rec.value = tdata_blocks[0].touchdowns;
					strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
					set_generic_record(WORLD_RECS_ARRAY,1,&tmp_world_rec);
				}
			}
			// most tackles
			if (!(get_generic_record(WORLD_RECS_ARRAY,2,&tmp_world_rec)))
			{
				if (tmp_world_rec.value < tdata_blocks[0].tackles)
				{
					tmp_world_rec.value = tdata_blocks[0].tackles;
					strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
					set_generic_record(WORLD_RECS_ARRAY,2,&tmp_world_rec);
				}
			}
			// most interceptions
			if (!(get_generic_record(WORLD_RECS_ARRAY,3,&tmp_world_rec)))
			{
				if (tmp_world_rec.value < tdata_blocks[0].interceptions)
				{
					tmp_world_rec.value = tdata_blocks[0].interceptions;
					strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
					set_generic_record(WORLD_RECS_ARRAY,3,&tmp_world_rec);
				}
			}
			// most points scored in game
			if (!(get_generic_record(WORLD_RECS_ARRAY,4,&tmp_world_rec)))
			{
				if (tmp_world_rec.value < compute_score(0))
				{
					tmp_world_rec.value = compute_score(0);
					strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
					set_generic_record(WORLD_RECS_ARRAY,4,&tmp_world_rec);
				}
			}
			// largest victory margin
			if (!(get_generic_record(WORLD_RECS_ARRAY,5,&tmp_world_rec)))
			{
				diff = compute_score(0) - compute_score(1);
				if ((diff > 0) && (tmp_world_rec.value < diff))
				{
					tmp_world_rec.value = diff;
					strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
					set_generic_record(WORLD_RECS_ARRAY,5,&tmp_world_rec);
				}
			}
		}
	}


	//
	// Player 3 and Player 4 (team 2)
	//
	for (i=2; i < 4; i++)
	{
		prec = plyr_data_ptrs[i];
		if (prec->plyr_name[0] != NULL)
		{
			// most sacks
			if (!(get_generic_record(WORLD_RECS_ARRAY,0,&tmp_world_rec)))
			{
				if (tmp_world_rec.value < tdata_blocks[1].sacks)
				{
					tmp_world_rec.value = tdata_blocks[1].sacks;
					strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
					set_generic_record(WORLD_RECS_ARRAY,0,&tmp_world_rec);
				}
			}
			// most TD's in game
			if (!(get_generic_record(WORLD_RECS_ARRAY,1,&tmp_world_rec)))
			{
				if (tmp_world_rec.value < tdata_blocks[1].touchdowns)
				{
					tmp_world_rec.value = tdata_blocks[1].touchdowns;
					strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
					set_generic_record(WORLD_RECS_ARRAY,1,&tmp_world_rec);
				}
			}
			// most tackles
			if (!(get_generic_record(WORLD_RECS_ARRAY,2,&tmp_world_rec)))
			{
				if (tmp_world_rec.value < tdata_blocks[1].tackles)
				{
					tmp_world_rec.value = tdata_blocks[1].tackles;
					strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
					set_generic_record(WORLD_RECS_ARRAY,2,&tmp_world_rec);
				}
			}
			// most interceptions
			if (!(get_generic_record(WORLD_RECS_ARRAY,3,&tmp_world_rec)))
			{
				if (tmp_world_rec.value < tdata_blocks[1].interceptions)
				{
					tmp_world_rec.value = tdata_blocks[1].interceptions;
					strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
					set_generic_record(WORLD_RECS_ARRAY,3,&tmp_world_rec);
				}
			}
			// most points scored in game
			if (!(get_generic_record(WORLD_RECS_ARRAY,4,&tmp_world_rec)))
			{
				if (tmp_world_rec.value < compute_score(1))
				{
					tmp_world_rec.value = compute_score(1);
					strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
					set_generic_record(WORLD_RECS_ARRAY,4,&tmp_world_rec);
				}
			}
			// largest victory margin
			if (!(get_generic_record(WORLD_RECS_ARRAY,5,&tmp_world_rec)))
			{
				diff = compute_score(1) - compute_score(0);
				if ((diff > 0) && (tmp_world_rec.value < diff))
				{
					tmp_world_rec.value = diff;
					strncpy(tmp_world_rec.plyr_name,prec->plyr_name,LETTERS_IN_NAME+PIN_NUMBERS);
					set_generic_record(WORLD_RECS_ARRAY,5,&tmp_world_rec);
				}
			}
		}
	}

}


//-------------------------------------------------------------------------------------------------
//				    Determines wheter current cmos record matches players record
//
//	This routine assumes that the data to compare resides at the start of the pointer (structs)
//
//   INPUT:  ptr to cmos record, ptr. to player record data in ram, size of data
//  RETURN: 0 = match else non-zero
//-------------------------------------------------------------------------------------------------
int compare_record(char *plr_name_n_pin1,char *plr_name_n_pin2, int n)
{

	while (n)
	{
		if (*plr_name_n_pin1++ != *plr_name_n_pin2++)
			return(1);									// records are different
		n--;
	}
	return(0);											// records are the same
}

//-------------------------------------------------------------------------------------------------
//									Gets the "least" used record number
//   INPUT: none
//  RETURN: free record number
//-------------------------------------------------------------------------------------------------
int get_free_record_nbr(void)
{
int rec_num;
int last_used_cnt;
int least_used_rec = 0;
	
	// this will FIND the first available record...or allow the LEAST used record to be over written
	last_used_cnt = -1;
	for (rec_num=0; rec_num < NUM_PLYR_RECORDS; rec_num++)
	{
		if (get_generic_record(PLYR_RECS_ARRAY,rec_num,&tmp_plyr_record))	 			// get cmos record into ram
			clear_cmos_record((char *)&tmp_plyr_record,sizeof(plyr_record_t));			// record was corrupt
		// an empty record...use it
//		if (tmp_plyr_record.games_played <= 0)
		if ((tmp_plyr_record.games_played <= 0) &&
			(tmp_plyr_record.plays_allowed == 0))
		{
			return(rec_num);																			// record is free
		}

		// find the least used rec.	
		if (tmp_plyr_record.last_used >= last_used_cnt)
		{
			last_used_cnt = tmp_plyr_record.last_used;						// new least used record
			least_used_rec = rec_num;
		}
	}
	return(least_used_rec);
}

//-------------------------------------------------------------------------------------------------
//						This routine gets all the players records into MALLOC'd ram
//
//   INPUT: none
// RETURNS: ptr. to JMALLOC'd ram
//-------------------------------------------------------------------------------------------------
void get_all_records(void)
{
	int 	rec_num;
	plyr_record_t	*rec_ram;

	if (!rec_sort_ram)
		rec_sort_ram = (plyr_record_t *)JMALLOC((sizeof(plyr_record_t)*NUM_PLYR_RECORDS));

	if(rec_sort_ram)
	{
		rec_ram = rec_sort_ram;

		for(rec_num=0; rec_num < NUM_PLYR_RECORDS; rec_num++,rec_ram++)
		{
			get_generic_record(PLYR_RECS_ARRAY,rec_num,rec_ram);	 			// get cmos record into ram
		}
	}
#ifdef DEBUG
	else
	{
		fprintf(stderr, "get_all_records() - Can not allocate memory for player records\r\n");
		lockup();
	}
#endif
}

//-------------------------------------------------------------------------------------------------
//							This routine sorts the player records according to TRIVIA POINTS
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_trivia(plyr_record_t *rec1, plyr_record_t *rec2)
{
int tmp;

	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;
		
	tmp = (int)(rec2->trivia_pts - rec1->trivia_pts);
	if (!tmp)
		return ((int)(rec2->games_played - rec1->games_played));
	return (tmp);
}

//-------------------------------------------------------------------------------------------------
//							This routine sorts the player records according to WINS
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_wins(plyr_record_t *rec1, plyr_record_t *rec2)
{
int tmp;

	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;
		
	tmp = (int)(rec2->games_won - rec1->games_won);
	if (!tmp)
		return ((int)(rec2->games_played - rec1->games_played));
	return (tmp);
}

//-------------------------------------------------------------------------------------------------
//							This routine sorts the player records according to QB RATINGS
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_qb_rating(plyr_record_t *rec1, plyr_record_t *rec2)
{
	int tmp,pr1,pr2;

	// reject record with less than 4 games of experience
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;
		
	pr1 = pass_rating(rec1->pass_attempts,rec1->pass_completions,rec1->passing_yds,rec1->pass_touchdowns,rec1->intercepted);
	pr2 = pass_rating(rec2->pass_attempts,rec2->pass_completions,rec2->passing_yds,rec2->pass_touchdowns,rec2->intercepted);
		
	tmp = (int)(pr2 - pr1);
//	tmp = (int)(rec2->games_won - rec1->games_won);
	if (!tmp)
		return ((int)(rec2->games_played - rec1->games_played));
	return (tmp);
}

//-------------------------------------------------------------------------------------------------
//							This routine sorts the player records according to WINS
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_win_streaks(plyr_record_t *rec1, plyr_record_t *rec2)
{
int tmp;

	// reject record with less than 4 games of experience
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;
		
	tmp = (int)(rec2->winstreak - rec1->winstreak);
	if (!tmp)
		return ((int)(rec2->games_played - rec1->games_played));
	return (tmp);
}

//-------------------------------------------------------------------------------------------------
//							This routine sorts the player records according to WINS
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_win_percent(plyr_record_t *rec1, plyr_record_t *rec2)
{
int tmp, prcnt1, prcnt2;

	// reject record with less than 4 games of experience
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;
	
//	if (rec1->games_played <= 0)
//		prcnt1 = 0;
//	else
	prcnt1 = ((rec1->games_won * 1000)/rec1->games_played);
	
//	if (rec2->games_played <= 0)
//		prcnt2 = 0;
//	else
	prcnt2 = ((rec2->games_won * 1000)/rec2->games_played);
		
	tmp = (int)(prcnt2 - prcnt1);
	if (!tmp)
		return ((int)(rec2->games_played - rec1->games_played));
	return (tmp);
}

//-------------------------------------------------------------------------------------------------
//							This routine sorts the player records according to WINS
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_games_played(plyr_record_t *rec1, plyr_record_t *rec2)
{
int tmp;

	// reject record with less than 4 games of experience
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;
		
	tmp = (int)(rec2->games_played - rec1->games_played);
	if (!tmp)
	{
		tmp = (int)rec2->games_won - rec1->games_won;
		if (!tmp)
			return ((int)(rec2->winstreak - rec1->winstreak));
	}
	return (tmp);
}

//-------------------------------------------------------------------------------------------------
//				This routine sorts the player records according to MOST POINTS SCORED PER GAME
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_points_scored(plyr_record_t *rec1, plyr_record_t *rec2)
{
	int tmp,gms_plyd1,gms_plyd2;


	// reject record with less than 4 games of experience
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;
		
	gms_plyd1 = (int)rec1->games_played;
	if (!gms_plyd1)																				// player hasn't played a game (empty record) put on bottom
		gms_plyd1 = 500;																		// Just trick the sort into thinking this player sucks.

	gms_plyd2 = (int)rec2->games_played;
	if (!gms_plyd2)																				// player hasn't played a game (empty record) put on bottom
		gms_plyd2 = 500;																		// Just trick the sort into thinking this player sucks.

	tmp = ((rec2->points_scored*100) / gms_plyd2) - ((rec1->points_scored*100) / gms_plyd1);
	if (!tmp)
	{
		tmp = (int)(rec2->games_won - rec1->games_won);
		if (!tmp)
			return ((int)(rec2->winstreak - rec1->winstreak));
	}
	return (tmp);
}

//-------------------------------------------------------------------------------------------------
//				This routine sorts the player records according to LEAST POINTS ALLOWED PER GAME
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_points_allowed(plyr_record_t *rec1, plyr_record_t *rec2)
{
int tmp,gms_plyd1,gms_plyd2,pts_alwd1,pts_alwd2;

	// reject record with less than 4 games of experience
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;

	gms_plyd1 = (int)rec1->games_played;
	pts_alwd1 = (int)rec1->points_allowed;
	if (!gms_plyd1)
	{																									// player hasn't played a game (empty record) put on bottom
		gms_plyd1 = 1;																		// Just trick the sort into thinking this player sucks.
		pts_alwd1 = 500;																	// Worst player ever
	}

	gms_plyd2 = (int)rec2->games_played;
	pts_alwd2 = (int)rec2->points_allowed;
	if (!gms_plyd2)
	{																									// player hasn't played a game (empty record) put on bottom
		gms_plyd2 = 1;																		// Just trick the sort into thinking this player sucks.
		pts_alwd2 = 500;																	// Worst player ever
	}

	tmp = (int)(((pts_alwd1*100) / gms_plyd1) - ((pts_alwd2*100) / gms_plyd2));
	if (!tmp)
	{
		tmp = (int)(rec1->games_won - rec2->games_won);
		if (!tmp)
			return ((int)(rec1->winstreak - rec2->winstreak));
	}
	return (tmp);
}


//-------------------------------------------------------------------------------------------------
//							This routine sorts the player records according to WINS
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_teams_defeated(plyr_record_t *rec1, plyr_record_t *rec2)
{
int tmp;

	// reject record with less than 4 games of experience
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;

	tmp = (calc_tms_def(rec2->teams_defeated) - calc_tms_def(rec1->teams_defeated));
	if (!tmp)
	{
		tmp = (int)(rec2->games_won - rec1->games_won);
		if (!tmp)
			return ((int)(rec2->winstreak - rec1->winstreak));
	}
	return (tmp);
}

//-------------------------------------------------------------------------------------------------
//							This routine sorts the player records according to WINS
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_passing_yds(plyr_record_t *rec1, plyr_record_t *rec2)
{
	float	r1 ,r2;
	int		tmp;

	// reject record with less than 4 games of experience
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;

	r1 = ((float)rec1->passing_yds)/((float)rec1->games_played);
	r2 = ((float)rec2->passing_yds)/((float)rec2->games_played);
		
	tmp = (int)(1000.0f * (r2 - r1));
	
	if (!tmp)
	{
		tmp = (int)(rec2->games_won - rec1->games_won);
		if (!tmp)
			return ((int)(rec2->winstreak - rec1->winstreak));
	}
	return (tmp);
}

//-------------------------------------------------------------------------------------------------
//							This routine sorts the player records according to WINS
//
// called from qsort
//-------------------------------------------------------------------------------------------------
int sort_rushing_yds(plyr_record_t *rec1, plyr_record_t *rec2)
{
	int		tmp;
	float	r1, r2;

	// reject record with less than 4 games of experience
	if ((rec1->games_played < 4) && (rec2->games_played < 4))
		return 0;
	else if (rec1->games_played < 4)	
		return 1;
	else if (rec2->games_played < 4)	
		return -1;

	r1 = ((float)(rec1->rushing_yds))/((float)(rec1->games_played));
	r2 = ((float)(rec2->rushing_yds))/((float)(rec2->games_played));
		
	tmp = (int)(1000.0f * (r2 - r1));
	
	if (!tmp)
	{
		tmp = (int)(rec2->games_won - rec1->games_won);
		if (!tmp)
			return ((int)(rec2->winstreak - rec1->winstreak));
	}
	return (tmp);
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int calc_tms_def(int tms_def)
{
	int bit_num,cnt;

	cnt = 0;
	for (bit_num=0; bit_num < NUM_TEAMS; bit_num++)
		if (tms_def & (1<<bit_num))			// bit set ?
			cnt++;												// yes
	return(cnt);
}


//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
static int team_tier( int team )
{
	int		tier,i;
	
	for (tier = 0; tier < 9; tier++)
	{
		for (i = 0; team_ladder[tier][i] != -1; i++)
		{
			if (team_ladder[tier][i] == team)
				return tier;
		}
	}
	
	return -1;
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------


