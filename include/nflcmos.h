#ifndef _NFLCMOS__H
#define _NFLCMOS__H

//****************************************************************************
// Defines and structure definitions for (football) CMOS
//   including player records, audits, adjustments... etc
//****************************************************************************
#include	"/video/nfl/include/audits.h"

#define CMOS_SIZE           32768
//#define CMOS_SIZE           8192
#define NUM_AUDITS          NEXT_AUD
#define NUM_ADJUSTMENTS     100
#define NUM_PLYR_RECORDS    350
#define NUM_WORLD_RECORDS   6

#define LETTERS_IN_NAME     6
#define PIN_NUMBERS         4

#define GEN_REC_ARRAYS      2		// player records and world records

#define PLYR_RECS_ARRAY     0       // generic rec arrays in cmos
#define WORLD_RECS_ARRAY    1       // generic rec arrays in cmos
#define TEAM_RECS_ARRAY     2       // generic rec arrays in cmos

#define REC_MIN_GAMES       4       // nbr. games required before valid for High scores pages
#define HI_SCORE_ENTRIES    10

#define NUM_GSTATS          8		// games of comps/atts/yds/tds/ints data stored per player

// PLAYER record in CMOS
typedef struct player_record
        {
        char    plyr_name[LETTERS_IN_NAME] __attribute__((packed));
        char    plyr_pin_nbr[PIN_NUMBERS] __attribute__((packed));
        short   		 last_used __attribute__((packed));                      // games since last use of record
        short   		 games_played __attribute__((packed));
        short   		 games_won __attribute__((packed));
        char			 winstreak __attribute__((packed));
        signed short	 points_scored __attribute__((packed));
        signed short 	 points_allowed __attribute__((packed));
        signed int   	 teams_defeated __attribute__((packed));     				// specified in bits
        signed short     tackles __attribute__((packed));
        signed short     sacks __attribute__((packed));
        signed int   	 passing_yds __attribute__((packed));							// allow for Negative yardage
        signed int	     rushing_yds __attribute__((packed));							// allow for Negative yardage
        signed short	 world_records __attribute__((packed));						// specified in bits
		signed int       pass_attempts __attribute__((packed));			// just put attribute on as of june 29th
		signed int       pass_completions __attribute__((packed));
		signed int       pass_touchdowns __attribute__((packed));
		signed int       intercepted __attribute__((packed));
		short			 cmos_rec_num __attribute__((packed));
		short			 disk_bank __attribute__((packed));
		short			 plays_allowed __attribute__((packed));
		short			 trivia_pts __attribute__((packed));
        } plyr_record_t;

// WORLD record in CMOS
typedef struct world_record
		{
        char    plyr_name[LETTERS_IN_NAME];
        char    plyr_pin_nbr[PIN_NUMBERS];
		signed short	value;
		} world_record_t;

//
// NFL teams and their DEFEATED bit position (team #'s defined in GAME.H)
//
#define FALCONS	    (1<<TEAM_FALCONS)
#define CARDINALS	(1<<TEAM_CARDINALS)
#define RAVENS		(1<<TEAM_RAVENS)
#define BILLS		(1<<TEAM_BILLS)
#define PANTHERS	(1<<TEAM_PANTHERS)
#define BEARS		(1<<TEAM_BEARS)
#define BENGALS	    (1<<TEAM_BENGALS)
#define COWBOYS	    (1<<TEAM_COWBOYS)
#define BRONCOS	    (1<<TEAM_BRONCOS)
#define LIONS		(1<<TEAM_LIONS)
#define PACKERS	    (1<<TEAM_PACKERS)
#define COLTS		(1<<TEAM_COLTS)
#define JAGUARS	    (1<<TEAM_JAGUARS)
#define CHIEFS		(1<<TEAM_CHIEFS)
#define DOLPHINS	(1<<TEAM_DOLPHINS)
#define VIKINGS	    (1<<TEAM_VIKINGS)
#define PATRIOTS	(1<<TEAM_PATRIOTS)
#define SAINTS		(1<<TEAM_SAINTS)
#define GIANTS		(1<<TEAM_GIANTS)
#define JETS		(1<<TEAM_JETS)
#define RAIDERS	    (1<<TEAM_RAIDERS)
#define EAGLES		(1<<TEAM_EAGLES)
#define STEELERS	(1<<TEAM_STEELERS)
#define CHARGERS	(1<<TEAM_CHARGERS)
#define FORTYNINERS (1<<TEAM_FORTYNINERS)
#define SEAHAWKS	(1<<TEAM_SEAHAWKS)
#define RAMS		(1<<TEAM_RAMS)
#define BUCS		(1<<TEAM_BUCS)
#define TITANS		(1<<TEAM_TITANS)
#define REDSKINS	(1<<TEAM_REDSKINS)
#define BROWNS		(1<<TEAM_BROWNS)

//
// Defines for ALPHA CHARACTERS stored in players name and pin number
//
#define CH_0          0x30
#define CH_1          0x31
#define CH_2          0x32
#define CH_3          0x33
#define CH_4          0x34
#define CH_5          0x35
#define CH_6          0x36
#define CH_7          0x37
#define CH_8          0x38
#define CH_9          0x39

#define CH_A          0x41
#define CH_B          0x42
#define CH_C          0x43
#define CH_D          0x44
#define CH_E          0x45
#define CH_F          0x46
#define CH_G          0x47
#define CH_H          0x48
#define CH_I          0x49
#define CH_J          0x4a
#define CH_K          0x4b
#define CH_L          0x4c
#define CH_M          0x4d
#define CH_N          0x4e
#define CH_O          0x4f
#define CH_P          0x50
#define CH_Q          0x51
#define CH_R          0x52
#define CH_S          0x53
#define CH_T          0x54
#define CH_U          0x55
#define CH_V          0x56
#define CH_W          0x57
#define CH_X          0x58
#define CH_Y          0x59
#define CH_Z          0x5a
#define CH_DSH        0x2d
#define CH_AST        0x2a
#define CH_EXC        0x21
#define CH_SPC        0x20
#define CH_DEL        0x7F
#define CH_END        0x0d


//
// Routine declarations
//
int init_cmos(void);
int  validate_plyr_records(void);
void set_default_plyr_records(void);
void set_default_world_records(void);
void clear_cmos_record(char *rec,int bytes_in_rec);
void get_player_record_from_cmos(char *name_n_pin, plyr_record_t *plyr_rec);
int find_record_in_cmos(char *plyr_name_and_pin);
int find_record_in_ram(char *pname_and_pin);
int compare_record(char *plr_name_n_pin1,char *plr_name_n_pin2, int n);
int get_new_record_into_ram(char *);
int get_free_record_nbr(void);
void save_player_records(void);
void get_all_records(void);
int sort_trivia(plyr_record_t *rec1, plyr_record_t *rec2);
int sort_qb_rating(plyr_record_t *rec1, plyr_record_t *rec2);
int sort_wins(plyr_record_t *rec1, plyr_record_t *rec2);
int sort_win_streaks(plyr_record_t *rec1, plyr_record_t *rec2);
int sort_win_percent(plyr_record_t *rec1, plyr_record_t *rec2);
int sort_games_played(plyr_record_t *rec1, plyr_record_t *rec2);
int sort_teams_defeated(plyr_record_t *rec1, plyr_record_t *rec2);
int sort_points_scored(plyr_record_t *rec1, plyr_record_t *rec2);
int sort_points_allowed(plyr_record_t *rec1, plyr_record_t *rec2);
int sort_passing_yds(plyr_record_t *rec1, plyr_record_t *rec2);
int sort_rushing_yds(plyr_record_t *rec1, plyr_record_t *rec2);
int calc_tms_def(int tms_def);
void check_world_records_2p(void);
void check_world_records_4p(void);
void show_hiscore_tables(int, int);
void show_world_records(int, int);
void inc_cmos_records_used_cnt(void);

#endif