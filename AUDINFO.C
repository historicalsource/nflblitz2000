#include	<stdio.h>
#include	<goose/goose.h>
#include	"/video/nfl/include/game.h"
#include	"/video/nfl/include/audits.h"

void do_team_percent_aud(int);
void do_off_play_percent_aud(int);
void do_def_play_percent_aud(int);
void clear_coin_audits(void);
void clear_credits_audits(void);
void clear_game_audits(void);
void clear_team_audits(void);
void clear_reset_audits(void);
void clear_off_play_audits(void);
void clear_def_play_audits(void);
void clear_cause_pc_audits(void);
int  check_coin_audits(void);
int  check_credits_audits(void);
int  check_game_audits(void);
int  check_team_audits(void);
int  check_reset_audits(void);
int  check_off_play_audits(void);
int  check_def_play_audits(void);
int  check_cause_pc_audits(void);
int  check_all_audits(void);

// Table used to clear out coin audits
static int	coin_audits[] = {
LEFT_COIN_AUDIT_NUM,
RIGHT_COIN_AUDIT_NUM,
BILLS_COIN_AUDIT_NUM,
CENTER_COIN_AUDIT_NUM,
EXTRA_COIN_AUDIT_NUM,
GAME_START_AUD,
MID_GAME_START_AUD,
CONTINUE_AUD,
FREE_QUARTERS_AUD,
FREE_GAME_AUD,
SERVICE_CREDITS_AUD,
TOTAL_PLAYS_AUD,
TOTAL_PAID_CREDITS_AUD,
TOTAL_MONEY_AUDIT_NUM,
};

// Table used to clear the credits audits
static int	credits_audits[] = {
CREDITS_AUD,
};

// Table used to clear out game audits
static int	game_audits[] = {
UPTIME_AUDIT_NUM,
PLAYTIME_AUD,
PLAYTIME_PERCENT_AUD,
FULL_GAME_PURCHASE_AUD,
FULL_GAME_PURCHASE_PCT_AUD,

INITIALS_ENTERED_AUD,
INITIALS_ENTERED_PERCENT_AUD,

CREATE_PLAY_AUD,
FINISHED_GAMES_AUD,
QUARTERS_PLAYED_AUD,

ONE_PLAYER_AUD,
TWO_PLAYER_AUD,
THREE_PLAYER_AUD,
FOUR_PLAYER_AUD,
TWOVCPU_PLAYER_AUD,

ONE_PLAYER_PERCENT_AUD,
TWO_PLAYER_PERCENT_AUD,
THREE_PLAYER_PERCENT_AUD,
FOUR_PLAYER_PERCENT_AUD,
TWOVCPU_PLAYER_PERCENT_AUD,

ONE_QUARTER_AUD,
TWO_QUARTER_AUD,
THREE_QUARTER_AUD,
FOUR_QUARTER_AUD,
ONE_OVERTIME_AUD,
TWO_OVERTIME_AUD,
THREE_OVERTIME_AUD,

ONE_QUARTER_PERCENT_AUD,
TWO_QUARTER_PERCENT_AUD,
THREE_QUARTER_PERCENT_AUD,
FOUR_QUARTER_PERCENT_AUD,
ONE_OVERTIME_PERCENT_AUD,
TWO_OVERTIME_PERCENT_AUD,
THREE_OVERTIME_PERCENT_AUD,

ONE_PLAYER_FINISH_AUD,
TWO_PLAYER_FINISH_AUD,
THREE_PLAYER_FINISH_AUD,
FOUR_PLAYER_FINISH_AUD,
TWOVCPU_PLAYER_FINISH_AUD,

ONE_PLAYER_FINISH_PERCENT_AUD,
TWO_PLAYER_FINISH_PERCENT_AUD,
THREE_PLAYER_FINISH_PERCENT_AUD,
FOUR_PLAYER_FINISH_PERCENT_AUD,
TWOVCPU_PLAYER_FINISH_PERCENT_AUD,

HVH_FINISH_AUD,
HVC_FINISH_AUD,
HVH_FINISH_PERCENT_AUD,

TOTAL_HVSH_POINTS_AUD,
TOTAL_HVSC_POINTS_AUD,
AVERAGE_HVSH_POINTS_AUD,
AVERAGE_HVSC_POINTS_AUD,
TOTAL_WINNING_POINTS_AUD,
TOTAL_LOSING_POINTS_AUD,
AVERAGE_WINNING_SCORE_AUD,
AVERAGE_LOSING_SCORE_AUD,
TOTAL_CPU_VICTORIES_AUD,
CPU_VICTORY_PERCENT_AUD,
LARGEST_CPU_VICTORY_AUD,
LARGEST_CPU_LOSS_AUD,

LEFT_SIDE_WINS_AUD,
LEFT_SIDE_WINS_PERCENT_AUD,
TEX_LOAD_FAIL_AUD,
SND_LOAD_FAIL_AUD,
//N64CARD_PLUGINS_AUD,
//N64CARD_BLITZED_AUD,
//N64CARD_PLAY_OK_AUD,
//N64CARD_PLAY_BD_AUD
};

#define FIRST_TEAM_AUD		ARIZONA_CARDINALS_AUD
#define LAST_TEAM_AUD		WASHINGTON_REDSKINS_AUD

// Table used to clear out team audits
int	team_audits[] = {
ARIZONA_CARDINALS_AUD,		//	make sure the 'FIRST_TEAM_AUD' define matches this first entry
ATLANTA_FALCONS_AUD,
BALTIMORE_RAVENS_AUD,
BUFFALO_BILLS_AUD,
CAROLINA_PANTHERS_AUD,
CHICAGO_BEARS_AUD,
CINCINNATI_BENGALS_AUD,
CLEVELAND_BROWNS_AUD,
DALLAS_COWBOYS_AUD,
DENVER_BRONCOS_AUD,
DETROIT_LIONS_AUD,
GREEN_BAY_PACKERS_AUD,
INDIANAPOLIS_COLTS_AUD,
JACKSONVILLE_JAGUARS_AUD,
KANSAS_CITY_CHIEFS_AUD,
MIAMI_DOLPHINS_AUD,
MINNESOTA_VIKINGS_AUD,
NEW_ENGLAND_PATRIOTS_AUD,
NEW_ORLEANS_SAINTS_AUD,
NEW_YORK_GIANTS_AUD,
NEW_YORK_JETS_AUD,
OAKLAND_RAIDERS_AUD,
PHILADEPHIA_EAGLES_AUD,
PITTSBURGH_STEELERS_AUD,
SAN_DIEGO_CHARGERS_AUD,
SAN_FRANCISCO_AUD,
SEATTLE_SEAHAWKS_AUD,
ST_LOUIS_RAMS_AUD,
TAMPA_BAY_BUCS_AUD,
TENNESSEE_TITANS_AUD,
WASHINGTON_REDSKINS_AUD,	//	make sure the 'LAST_TEAM_AUD' define matches this last (well, last of AUD list) entry

ARIZONA_CARDINALS_PERCENT_AUD,
ATLANTA_FALCONS_PERCENT_AUD,
BALTIMORE_RAVENS_PERCENT_AUD,
BUFFALO_BILLS_PERCENT_AUD,
CAROLINA_PANTHERS_PERCENT_AUD,
CHICAGO_BEARS_PERCENT_AUD,
CINCINNATI_BENGALS_PERCENT_AUD,
CLEVELAND_BROWNS_PERCENT_AUD,
DALLAS_COWBOYS_PERCENT_AUD,
DENVER_BRONCOS_PERCENT_AUD,
DETROIT_LIONS_PERCENT_AUD,
GREEN_BAY_PACKERS_PERCENT_AUD,
INDIANAPOLIS_COLTS_PERCENT_AUD,
JACKSONVILLE_JAGUARS_PERCENT_AUD,
KANSAS_CITY_CHIEFS_PERCENT_AUD,
MIAMI_DOLPHINS_PERCENT_AUD,
MINNESOTA_VIKINGS_PERCENT_AUD,
NEW_ENGLAND_PATRIOTS_PERCENT_AUD,
NEW_ORLEANS_SAINTS_PERCENT_AUD,
NEW_YORK_GIANTS_PERCENT_AUD,
NEW_YORK_JETS_PERCENT_AUD,
OAKLAND_RAIDERS_PERCENT_AUD,
PHILADEPHIA_EAGLES_PERCENT_AUD,
PITTSBURGH_STEELERS_PERCENT_AUD,
SAN_DIEGO_CHARGERS_PERCENT_AUD,
SAN_FRANCISCO_PERCENT_AUD,
SEATTLE_SEAHAWKS_PERCENT_AUD,
ST_LOUIS_RAMS_PERCENT_AUD,
TAMPA_BAY_BUCS_PERCENT_AUD,
TENNESSEE_TITANS_PERCENT_AUD,
WASHINGTON_REDSKINS_PERCENT_AUD,
};

static int	reset_audits[] = {
WATCHDOG_AUDIT_NUM,
TLB_MOD_AUD,
TLB_LOAD_AUD,
TLB_STORE_AUD,
ADDR_LOAD_AUD,
ADDR_STORE_AUD,
BUS_FETCH_AUD,
BUS_DATA_AUD,
SYSCALL_AUD,
RSVD_INST_AUD,
COP_USE_AUD,
INT_ARITH_AUD,
TRAP_AUD,
RSVD_EXC_AUD,
FPU_EXC_AUD,
INTEGER_DIV0_AUD,
INTEGER_OVERFLOW_AUD,
RESTART_AUD,
BUFFER_SWAP_TIMEOUT_AUD,
DMA_TIMEOUT_AUD,
DIV0_UNHANDLED_AUD,
VBLANK_FREEZE_AUD,
TLB_REFILL_AUD,
};

static int	def_play_audits[] = {
SAFE_COVER_AUD,
ONE_MAN_BLITZ_AUD,
TWO_MAN_BLITZ_AUD,
SUICIDE_BLITZ_AUD,
ZONE_BLITZ_AUD,
NEAR_ZONE_AUD,
MEDIUM_ZONE_AUD,
DEEP_ZONE_AUD,
GOAL_LINE_AUD,
DEF_EXTRA_1_AUD,
DEF_EXTRA_2_AUD,
DEF_EXTRA_3_AUD,
DEF_EXTRA_4_AUD,
DEF_EXTRA_5_AUD,
DEF_EXTRA_6_AUD,
DEF_EXTRA_7_AUD,
DEF_EXTRA_8_AUD,
DEF_EXTRA_9_AUD,
BLOCK_FG_AUD,
BLOCK_PUNT_AUD,
PUNT_RETURN_AUD,

SAFE_COVER_PERCENT_AUD,
ONE_MAN_BLITZ_PERCENT_AUD,
TWO_MAN_BLITZ_PERCENT_AUD,
SUICIDE_BLITZ_PERCENT_AUD,
ZONE_BLITZ_PERCENT_AUD,
NEAR_ZONE_PERCENT_AUD,
MEDIUM_ZONE_PERCENT_AUD,
DEEP_ZONE_PERCENT_AUD,
GOAL_LINE_PERCENT_AUD,
DEF_EXTRA_1_PERCENT_AUD,
DEF_EXTRA_2_PERCENT_AUD,
DEF_EXTRA_3_PERCENT_AUD,
DEF_EXTRA_4_PERCENT_AUD,
DEF_EXTRA_5_PERCENT_AUD,
DEF_EXTRA_6_PERCENT_AUD,
DEF_EXTRA_7_PERCENT_AUD,
DEF_EXTRA_8_PERCENT_AUD,
DEF_EXTRA_9_PERCENT_AUD,
BLOCK_FG_PERCENT_AUD,
BLOCK_PUNT_PERCENT_AUD,
PUNT_RETURN_PERCENT_AUD,
};

static int	off_play_audits[] = {
TEAMPLAY_1_AUD,
TEAMPLAY_2_AUD,
TEAMPLAY_3_AUD,
ZIG_ZAG_AUD,
SPIDER_LEGS_AUD,
MONKEY_AUD,
SLIP_SLIDE_AUD,
QB_POST_AUD,
QUICK_DISH_AUD,
UPPER_CUT_AUD,
DA_BOMB_AUD,
HAIL_MARY2_AUD,
TURMOIL_AUD,
BACK_SPLIT_AUD,
SUB_ZERO_AUD,
DAWG_HOOK_AUD,
UTB_DEEP_AUD,
SLANT_AUD,
BLIZZARD_AUD,
CRISS_CROSS_AUD,
UP_THE_GUT_AUD,
SCREEN_AUD,
SUPER_FLY_AUD,
MIDDLE_PICK_AUD,
SWEEP_AUD,
REVERSE_ZIP_AUD,
HB_BLOCK_AUD,
OFF_EXTRA_1_AUD,
OFF_EXTRA_2_AUD,
OFF_EXTRA_3_AUD,
OFF_EXTRA_4_AUD,
OFF_EXTRA_5_AUD,
OFF_EXTRA_6_AUD,
OFF_EXTRA_7_AUD,
OFF_EXTRA_8_AUD,
OFF_EXTRA_9_AUD,
PUNT_AUD,
FAKE_PUNT_AUD,
FIELD_GOAL_AUD,
FAKE_FG_AUD,
EXTRA_POINT_AUD,

TEAMPLAY_1_PERCENT_AUD,
TEAMPLAY_2_PERCENT_AUD,
TEAMPLAY_3_PERCENT_AUD,
ZIG_ZAG_PERCENT_AUD,
SPIDER_LEGS_PERCENT_AUD,
MONKEY_PERCENT_AUD,
SLIP_SLIDE_PERCENT_AUD,
QB_POST_PERCENT_AUD,
QUICK_DISH_PERCENT_AUD,
UPPER_CUT_PERCENT_AUD,
DA_BOMB_PERCENT_AUD,
HAIL_MARY2_PERCENT_AUD,
TURMOIL_PERCENT_AUD,
BACK_SPLIT_PERCENT_AUD,
SUB_ZERO_PERCENT_AUD,
DAWG_HOOK_PERCENT_AUD,
UTB_DEEP_PERCENT_AUD,
SLANT_PERCENT_AUD,
BLIZZARD_PERCENT_AUD,
CRISS_CROSS_PERCENT_AUD,
UP_THE_GUT_PERCENT_AUD,
SCREEN_PERCENT_AUD,
SUPER_FLY_PERCENT_AUD,
MIDDLE_PICK_PERCENT_AUD,
SWEEP_PERCENT_AUD,
REVERSE_ZIP_PERCENT_AUD,
HB_BLOCK_PERCENT_AUD,
OFF_EXTRA_1_PERCENT_AUD,
OFF_EXTRA_2_PERCENT_AUD,
OFF_EXTRA_3_PERCENT_AUD,
OFF_EXTRA_4_PERCENT_AUD,
OFF_EXTRA_5_PERCENT_AUD,
OFF_EXTRA_6_PERCENT_AUD,
OFF_EXTRA_7_PERCENT_AUD,
OFF_EXTRA_8_PERCENT_AUD,
OFF_EXTRA_9_PERCENT_AUD,
PUNT_PERCENT_AUD,
FAKE_PUNT_PERCENT_AUD,
FIELD_GOAL_PERCENT_AUD,
FAKE_FG_PERCENT_AUD,
EXTRA_POINT_PERCENT_AUD,
};

static int	cause_pc_audits[] = {
//AT_AUD,
V0_AUD,
V1_AUD,
A0_AUD,
A1_AUD,
A2_AUD,
A3_AUD,
T0_AUD,
T1_AUD,
T2_AUD,
T3_AUD,
T4_AUD,
T5_AUD,
T6_AUD,
T7_AUD,
T8_AUD,
T9_AUD,
S0_AUD,
S1_AUD,
S2_AUD,
S3_AUD,
S4_AUD,
S5_AUD,
S6_AUD,
S7_AUD,
S8_AUD,
//K0_AUD,
//K1_AUD,
//GP_AUD,
SP_AUD,
RA_AUD,
CURPROC_AUD,
//
CAUSE_AUD,
PC_AUD,
//
CAUSE_AUD2,
PC_AUD2,
CAUSE_AUD3,
PC_AUD3,
CAUSE_AUD4,
PC_AUD4,
CAUSE_AUD5,
PC_AUD5,
CAUSE_AUD6,
PC_AUD6,
//CAUSE_AUD+12,
//PC_AUD+12,
//CAUSE_AUD+14,
//PC_AUD+14,
//CAUSE_AUD+16,
//PC_AUD+16,
//CAUSE_AUD+18,
//PC_AUD+18,
};


void write_audit_info(FILE *fp)
{
	// Coin Audits
	ADD_MENU_ITEM("COIN AUDITS", "SHOW THE COINS TAKEN");
	// Coin Audit Page(s)
	ADD_AUDIT(LEFT_COIN_AUDIT_NUM,    MEDIUM_FONT, LT_YELLOW, "LEFT SLOT COINS",       DECIMAL, LT_YELLOW);
	ADD_AUDIT(RIGHT_COIN_AUDIT_NUM,   MEDIUM_FONT, LT_CYAN,   "RIGHT SLOT COINS",      DECIMAL, LT_CYAN  );
	ADD_AUDIT(BILLS_COIN_AUDIT_NUM,   MEDIUM_FONT, LT_YELLOW, "BILLS",                 DECIMAL, LT_YELLOW);
	ADD_AUDIT(CENTER_COIN_AUDIT_NUM,  MEDIUM_FONT, LT_CYAN,   "CENTER SLOT COINS",     DECIMAL, LT_CYAN  );
	ADD_AUDIT(EXTRA_COIN_AUDIT_NUM,   MEDIUM_FONT, LT_YELLOW, "EXTRA SLOT COINS",      DECIMAL, LT_YELLOW);
	ADD_AUDIT(GAME_START_AUD,         MEDIUM_FONT, LT_CYAN,   "GAME STARTS",           DECIMAL, LT_CYAN  );
	ADD_AUDIT(MID_GAME_START_AUD,     MEDIUM_FONT, LT_YELLOW, "MID-GAME STARTS",       DECIMAL, LT_YELLOW);
	ADD_AUDIT(CONTINUE_AUD,           MEDIUM_FONT, LT_CYAN,   "CONTINUES",             DECIMAL, LT_CYAN  );
	ADD_AUDIT(FREE_QUARTERS_AUD,      MEDIUM_FONT, LT_YELLOW, "FREE QUARTERS AWARDED", DECIMAL, LT_YELLOW);
	ADD_AUDIT(FREE_GAME_AUD,          MEDIUM_FONT, LT_CYAN,   "FREE GAMES AWARDED",    DECIMAL, LT_CYAN  );
	ADD_AUDIT(SERVICE_CREDITS_AUD,    MEDIUM_FONT, LT_YELLOW, "SERVICE CREDITS",       DECIMAL, LT_YELLOW);
	ADD_AUDIT(TOTAL_PLAYS_AUD,        MEDIUM_FONT, LT_CYAN,   "TOTAL PLAYS",           DECIMAL, LT_CYAN  );
	ADD_AUDIT(TOTAL_PAID_CREDITS_AUD, MEDIUM_FONT, LT_YELLOW, "TOTAL PAID CREDITS",    DECIMAL, LT_YELLOW);
	ADD_AUDIT(TOTAL_MONEY_AUDIT_NUM,  LARGE_FONT,  LT_CYAN,   "TOTAL",                 MONEY,   LT_CYAN  );

	// Credits Audits
	ADD_MENU_ITEM("CREDITS AUDITS", "SHOW THE AVAILABLE CREDITS");
	// Credits Audit Page(s)
	ADD_AUDIT(CREDITS_AUD, LARGE_FONT, LT_YELLOW, "CREDITS AVAILABLE", DECIMAL, WHITE);

	// Game Audits
	ADD_MENU_ITEM("GAME AUDITS", "SHOW STATISTICS ABOUT THE GAME");
	COLUMNS(2);	
	// Game Audit Page(s)
	ADD_AUDIT(UPTIME_AUDIT_NUM,                  SMALL_FONT, LT_YELLOW, "TOTAL UPTIME",                  DURATION, LT_YELLOW);
	ADD_AUDIT(PLAYTIME_PERCENT_AUD,              SMALL_FONT, LT_CYAN,   "TOTAL PLAYTIME",                PERCENT,  LT_CYAN  );
	ADD_AUDIT(GAME_START_AUD,                    SMALL_FONT, LT_YELLOW, "GAMES STARTED",                 DECIMAL,  LT_YELLOW);
	ADD_AUDIT(INITIALS_ENTERED_PERCENT_AUD,      SMALL_FONT, LT_CYAN,   "INITIALS ENTERED",              PERCENT,  LT_CYAN  );
	ADD_AUDIT(FULL_GAME_PURCHASE_PCT_AUD,        SMALL_FONT, LT_YELLOW, "(FG) PURCHASED AT START",       PERCENT,  LT_YELLOW);
	ADD_AUDIT(CREATE_PLAY_AUD,                   SMALL_FONT, LT_CYAN,   "CREATE PLAY SESSIONS",          DECIMAL,  LT_CYAN  );
	ADD_AUDIT(ONE_PLAYER_PERCENT_AUD,            SMALL_FONT, LT_YELLOW, "1 PLAYER",                      PERCENT, LT_YELLOW);
	ADD_AUDIT(TWO_PLAYER_PERCENT_AUD,            SMALL_FONT, LT_CYAN,   "2 PLAYER",                      PERCENT, LT_CYAN  );
	ADD_AUDIT(THREE_PLAYER_PERCENT_AUD,          SMALL_FONT, LT_YELLOW, "3 PLAYER",                      PERCENT, LT_YELLOW);
	ADD_AUDIT(FOUR_PLAYER_PERCENT_AUD,           SMALL_FONT, LT_CYAN,   "4 PLAYER",                      PERCENT, LT_CYAN  );
	ADD_AUDIT(TWOVCPU_PLAYER_PERCENT_AUD,        SMALL_FONT, LT_YELLOW, "2P V CPU",                      PERCENT, LT_YELLOW);
	ADD_AUDIT(ONE_QUARTER_PERCENT_AUD,           SMALL_FONT, LT_CYAN,   "1 QUARTER GAMES",               PERCENT, LT_CYAN  );
	ADD_AUDIT(TWO_QUARTER_PERCENT_AUD,           SMALL_FONT, LT_YELLOW, "2 QUARTER GAMES",               PERCENT, LT_YELLOW);
	ADD_AUDIT(THREE_QUARTER_PERCENT_AUD,         SMALL_FONT, LT_CYAN,   "3 QUARTER GAMES",               PERCENT, LT_CYAN  );
	ADD_AUDIT(FOUR_QUARTER_PERCENT_AUD,          SMALL_FONT, LT_YELLOW, "4 QUARTER GAMES",               PERCENT, LT_YELLOW);
	ADD_AUDIT(ONE_OVERTIME_PERCENT_AUD,          SMALL_FONT, LT_CYAN,   "WENT INTO OVERTIME 1",          PERCENT, LT_CYAN  );
	ADD_AUDIT(TWO_OVERTIME_PERCENT_AUD,          SMALL_FONT, LT_YELLOW, "WENT INTO OVERTIME 2",          PERCENT, LT_YELLOW);
	ADD_AUDIT(THREE_OVERTIME_PERCENT_AUD,        SMALL_FONT, LT_CYAN,   "WENT INTO OVERTIME 3",          PERCENT, LT_CYAN  );
	ADD_AUDIT(ONE_PLAYER_FINISH_PERCENT_AUD,     SMALL_FONT, LT_YELLOW, "1 PLAYER FINISHES",             PERCENT, LT_YELLOW);
	ADD_AUDIT(TWO_PLAYER_FINISH_PERCENT_AUD,     SMALL_FONT, LT_CYAN,   "2 PLAYER FINISHES",             PERCENT, LT_CYAN  );
	ADD_AUDIT(THREE_PLAYER_FINISH_PERCENT_AUD,   SMALL_FONT, LT_YELLOW, "3 PLAYER FINISHES",             PERCENT, LT_YELLOW);
	ADD_AUDIT(FOUR_PLAYER_FINISH_PERCENT_AUD,    SMALL_FONT, LT_CYAN,   "4 PLAYER FINISHES",             PERCENT, LT_CYAN  );
	ADD_AUDIT(TWOVCPU_PLAYER_FINISH_PERCENT_AUD, SMALL_FONT, LT_YELLOW, "2P V CPU FINISHES",             PERCENT, LT_YELLOW);
	ADD_AUDIT(LEFT_SIDE_WINS_PERCENT_AUD,        SMALL_FONT, LT_CYAN,   "H VS H LSW'S",                  PERCENT, LT_CYAN  );
	ADD_AUDIT(CPU_VICTORY_PERCENT_AUD,           SMALL_FONT, LT_YELLOW, "CPU WINS (FG)",                 PERCENT, LT_YELLOW);
	ADD_AUDIT(AVERAGE_HVSH_POINTS_AUD,           SMALL_FONT, LT_CYAN,   "AVG H VS H TOTAL SCORE (FG)",   DECIMAL, LT_CYAN  );
	ADD_AUDIT(AVERAGE_HVSC_POINTS_AUD,           SMALL_FONT, LT_YELLOW, "AVG H VS CPU TOTAL SCORE (FG)", DECIMAL, LT_YELLOW);
	ADD_AUDIT(AVERAGE_WINNING_SCORE_AUD,         SMALL_FONT, LT_CYAN,   "AVG H VS H WIN SCORE (FG)",     DECIMAL, LT_CYAN  );
	ADD_AUDIT(AVERAGE_LOSING_SCORE_AUD,          SMALL_FONT, LT_YELLOW, "AVG H VS H LOSS SCORE (FG)",    DECIMAL, LT_YELLOW);
	ADD_AUDIT(LARGEST_CPU_VICTORY_AUD,           SMALL_FONT, LT_CYAN,   "BIGGEST CPU WIN MARGIN (FG)",   DECIMAL, LT_CYAN  );
	ADD_AUDIT(LARGEST_CPU_LOSS_AUD,              SMALL_FONT, LT_YELLOW, "BIGGEST CPU LOSS MARGIN (FG)",  DECIMAL, LT_YELLOW);
	ADD_AUDIT(TEX_LOAD_FAIL_AUD,                 SMALL_FONT, LT_CYAN,   "TLF'S",                         DECIMAL, LT_CYAN  );
	ADD_AUDIT(SND_LOAD_FAIL_AUD,                 SMALL_FONT, LT_YELLOW, "SBLF'S",                        DECIMAL, LT_YELLOW);
//	ADD_AUDIT(N64CARD_PLUGINS_AUD, SMALL_FONT, LT_YELLOW, "N64 CARDS PLUGGED IN", DECIMAL, LT_YELLOW);
//	ADD_AUDIT(N64CARD_BLITZED_AUD, SMALL_FONT, LT_CYAN,   "N64 CARDS WITH PLAYS", DECIMAL, LT_CYAN);
//	ADD_AUDIT(N64CARD_PLAY_OK_AUD, SMALL_FONT, LT_YELLOW, "N64 CARD PLAYS READ IN", DECIMAL, LT_YELLOW);
//	ADD_AUDIT(N64CARD_PLAY_BD_AUD, SMALL_FONT, LT_CYAN,   "N64 CARD INVALID PLAYS", DECIMAL, LT_CYAN);
//	ADD_AUDIT(TWO_PLAYER_AUD, SMALL_FONT, LT_CYAN, "H VS H GAMES", DECIMAL, LT_CYAN);

	// Team Selection Audits
	ADD_MENU_ITEM("TEAM STATS", "SHOW STATISTICS ABOUT EACH NFL TEAM");
	COLUMNS(2);	
	// Team Selection Audit Page(s)
	ADD_AUDIT(ARIZONA_CARDINALS_PERCENT_AUD,    SMALL_FONT, LT_CYAN,   "ARIZONA CARDINALS",    PERCENT, LT_CYAN  );
	ADD_AUDIT(ATLANTA_FALCONS_PERCENT_AUD,      SMALL_FONT, LT_YELLOW, "ATLANTA FALCONS",      PERCENT, LT_YELLOW);
	ADD_AUDIT(BALTIMORE_RAVENS_PERCENT_AUD,     SMALL_FONT, LT_CYAN,   "BALTIMORE RAVENS",     PERCENT, LT_CYAN  );
	ADD_AUDIT(BUFFALO_BILLS_PERCENT_AUD,        SMALL_FONT, LT_YELLOW, "BUFFALO BILLS",        PERCENT, LT_YELLOW);
	ADD_AUDIT(CAROLINA_PANTHERS_PERCENT_AUD,    SMALL_FONT, LT_CYAN,   "CAROLINA PANTHERS",    PERCENT, LT_CYAN  );
	ADD_AUDIT(CHICAGO_BEARS_PERCENT_AUD,        SMALL_FONT, LT_YELLOW, "CHICAGO BEARS",        PERCENT, LT_YELLOW);
	ADD_AUDIT(CINCINNATI_BENGALS_PERCENT_AUD,   SMALL_FONT, LT_CYAN,   "CINCINNATI BENGALS",   PERCENT, LT_CYAN  );
	ADD_AUDIT(CLEVELAND_BROWNS_PERCENT_AUD,     SMALL_FONT, LT_YELLOW, "CLEVELAND BROWNS",     PERCENT, LT_YELLOW);
	ADD_AUDIT(DALLAS_COWBOYS_PERCENT_AUD,       SMALL_FONT, LT_CYAN,   "DALLAS COWBOYS",       PERCENT, LT_CYAN  );
	ADD_AUDIT(DENVER_BRONCOS_PERCENT_AUD,       SMALL_FONT, LT_YELLOW, "DENVER BRONCOS",       PERCENT, LT_YELLOW);
	ADD_AUDIT(DETROIT_LIONS_PERCENT_AUD,        SMALL_FONT, LT_CYAN,   "DETROIT LIONS",        PERCENT, LT_CYAN  );
	ADD_AUDIT(GREEN_BAY_PACKERS_PERCENT_AUD,    SMALL_FONT, LT_YELLOW, "GREEN BAY PACKERS",    PERCENT, LT_YELLOW);
	ADD_AUDIT(INDIANAPOLIS_COLTS_PERCENT_AUD,   SMALL_FONT, LT_CYAN,   "INDIANAPOLIS COLTS",   PERCENT, LT_CYAN  );
	ADD_AUDIT(JACKSONVILLE_JAGUARS_PERCENT_AUD, SMALL_FONT, LT_YELLOW, "JACKSONVILLE JAGUARS", PERCENT, LT_YELLOW);
	ADD_AUDIT(KANSAS_CITY_CHIEFS_PERCENT_AUD,   SMALL_FONT, LT_CYAN,   "KANSAS CITY CHIEFS",   PERCENT, LT_CYAN  );
	ADD_AUDIT(MIAMI_DOLPHINS_PERCENT_AUD,       SMALL_FONT, LT_YELLOW, "MIAMI DOLPHINS",       PERCENT, LT_YELLOW);
	ADD_AUDIT(MINNESOTA_VIKINGS_PERCENT_AUD,    SMALL_FONT, LT_CYAN,   "MINNESOTA VIKINGS",    PERCENT, LT_CYAN  );
	ADD_AUDIT(NEW_ENGLAND_PATRIOTS_PERCENT_AUD, SMALL_FONT, LT_YELLOW, "NEW ENGLAND PATRIOTS", PERCENT, LT_YELLOW);
	ADD_AUDIT(NEW_ORLEANS_SAINTS_PERCENT_AUD,   SMALL_FONT, LT_CYAN,   "NEW ORLEANS SAINTS",   PERCENT, LT_CYAN  );
	ADD_AUDIT(NEW_YORK_GIANTS_PERCENT_AUD,      SMALL_FONT, LT_YELLOW, "NEW YORK GIANTS",      PERCENT, LT_YELLOW);
	ADD_AUDIT(NEW_YORK_JETS_PERCENT_AUD,        SMALL_FONT, LT_CYAN,   "NEW YORK JETS",        PERCENT, LT_CYAN  );
	ADD_AUDIT(OAKLAND_RAIDERS_PERCENT_AUD,      SMALL_FONT, LT_YELLOW, "OAKLAND RAIDERS",      PERCENT, LT_YELLOW);
	ADD_AUDIT(PHILADEPHIA_EAGLES_PERCENT_AUD,   SMALL_FONT, LT_CYAN,   "PHILADEPHIA EAGLES",   PERCENT, LT_CYAN  );
	ADD_AUDIT(PITTSBURGH_STEELERS_PERCENT_AUD,  SMALL_FONT, LT_YELLOW, "PITTSBURGH STEELERS",  PERCENT, LT_YELLOW);
	ADD_AUDIT(SAN_DIEGO_CHARGERS_PERCENT_AUD,   SMALL_FONT, LT_CYAN,   "SAN DIEGO CHARGERS",   PERCENT, LT_CYAN  );
	ADD_AUDIT(SAN_FRANCISCO_PERCENT_AUD,        SMALL_FONT, LT_YELLOW, "SAN FRANCISCO",        PERCENT, LT_YELLOW);
	ADD_AUDIT(SEATTLE_SEAHAWKS_PERCENT_AUD,     SMALL_FONT, LT_CYAN,   "SEATTLE SEAHAWKS",     PERCENT, LT_CYAN  );
	ADD_AUDIT(ST_LOUIS_RAMS_PERCENT_AUD,        SMALL_FONT, LT_YELLOW, "ST LOUIS RAMS",        PERCENT, LT_YELLOW);
	ADD_AUDIT(TAMPA_BAY_BUCS_PERCENT_AUD,       SMALL_FONT, LT_CYAN,   "TAMPA BAY BUCS",       PERCENT, LT_CYAN  );
	ADD_AUDIT(TENNESSEE_TITANS_PERCENT_AUD,     SMALL_FONT, LT_YELLOW, "TENNESSEE TITANS",     PERCENT, LT_YELLOW);
	ADD_AUDIT(WASHINGTON_REDSKINS_PERCENT_AUD,  SMALL_FONT, LT_CYAN,   "WASHINGTON REDSKINS",  PERCENT, LT_CYAN  );

	// Offensive Play Audits
	ADD_MENU_ITEM("OFFENSIVE PLAYS", "SHOW STATISTICS ABOUT OFFENSIVE PLAYS PICKED");
	COLUMNS(2);	
	// Offensive Play Audit Page(s)
	ADD_AUDIT(TEAMPLAY_1_PERCENT_AUD,	 SMALL_FONT, LT_CYAN, "TEAMPLAY 1",       PERCENT, LT_CYAN);
	ADD_AUDIT(TEAMPLAY_2_PERCENT_AUD,	 SMALL_FONT, LT_YELLOW, "TEAMPLAY 2",       PERCENT, LT_YELLOW);
	ADD_AUDIT(TEAMPLAY_3_PERCENT_AUD,	 SMALL_FONT, LT_CYAN, "TEAMPLAY 3",       PERCENT, LT_CYAN);

	ADD_AUDIT(ZIG_ZAG_PERCENT_AUD,       SMALL_FONT, LT_YELLOW, "ZIG ZAG",       PERCENT, LT_YELLOW);
	ADD_AUDIT(SPIDER_LEGS_PERCENT_AUD,   SMALL_FONT, LT_CYAN,   "SPIDER LEGS",   PERCENT, LT_CYAN  );
	ADD_AUDIT(MONKEY_PERCENT_AUD,        SMALL_FONT, LT_YELLOW, "MONKEY",        PERCENT, LT_YELLOW);
	ADD_AUDIT(SLIP_SLIDE_PERCENT_AUD,    SMALL_FONT, LT_CYAN,   "SLIP SLIDE",    PERCENT, LT_CYAN  );
	ADD_AUDIT(QB_POST_PERCENT_AUD,       SMALL_FONT, LT_YELLOW, "QB POST",       PERCENT, LT_YELLOW);
	ADD_AUDIT(QUICK_DISH_PERCENT_AUD,    SMALL_FONT, LT_CYAN,   "QUICK DISH",    PERCENT, LT_CYAN  );
	ADD_AUDIT(UPPER_CUT_PERCENT_AUD,     SMALL_FONT, LT_YELLOW, "UPPER CUT",     PERCENT, LT_YELLOW);
	ADD_AUDIT(DA_BOMB_PERCENT_AUD,       SMALL_FONT, LT_CYAN,   "DA BOMB",       PERCENT, LT_CYAN  );
	ADD_AUDIT(HAIL_MARY2_PERCENT_AUD,    SMALL_FONT, LT_YELLOW, "HAIL MARY 2",   PERCENT, LT_YELLOW);
	ADD_AUDIT(TURMOIL_PERCENT_AUD,       SMALL_FONT, LT_CYAN,   "TURMOIL",       PERCENT, LT_CYAN  );
	ADD_AUDIT(BACK_SPLIT_PERCENT_AUD,    SMALL_FONT, LT_YELLOW, "BACK SPLIT",    PERCENT, LT_YELLOW);
	ADD_AUDIT(SUB_ZERO_PERCENT_AUD,      SMALL_FONT, LT_CYAN,   "SUBZERO",       PERCENT, LT_CYAN  );
	ADD_AUDIT(DAWG_HOOK_PERCENT_AUD,     SMALL_FONT, LT_YELLOW, "DAWG HOOK",     PERCENT, LT_YELLOW);
	ADD_AUDIT(UTB_DEEP_PERCENT_AUD,      SMALL_FONT, LT_CYAN,   "U.T.B. DEEP",   PERCENT, LT_CYAN  );
	ADD_AUDIT(SLANT_PERCENT_AUD,         SMALL_FONT, LT_YELLOW, "CROSS SLANT",   PERCENT, LT_YELLOW);
	ADD_AUDIT(BLIZZARD_PERCENT_AUD,      SMALL_FONT, LT_CYAN,   "BLIZZARD",      PERCENT, LT_CYAN  );
	ADD_AUDIT(CRISS_CROSS_PERCENT_AUD,   SMALL_FONT, LT_YELLOW, "CRISS CROSS",   PERCENT, LT_YELLOW);
	ADD_AUDIT(UP_THE_GUT_PERCENT_AUD,    SMALL_FONT, LT_CYAN,   "UP THE GUT",    PERCENT, LT_CYAN  );
	ADD_AUDIT(SCREEN_PERCENT_AUD,        SMALL_FONT, LT_YELLOW, "SCREEN RIGHT",  PERCENT, LT_YELLOW);
	ADD_AUDIT(SUPER_FLY_PERCENT_AUD,     SMALL_FONT, LT_CYAN,   "SUPER FLY",     PERCENT, LT_CYAN  );
	ADD_AUDIT(MIDDLE_PICK_PERCENT_AUD,   SMALL_FONT, LT_YELLOW, "MIDDLE PICK",   PERCENT, LT_YELLOW);
	ADD_AUDIT(SWEEP_PERCENT_AUD,         SMALL_FONT, LT_CYAN,   "SWEEP RIGHT",   PERCENT, LT_CYAN  );
	ADD_AUDIT(REVERSE_ZIP_PERCENT_AUD,   SMALL_FONT, LT_YELLOW, "REVERSE ZIP",   PERCENT, LT_YELLOW);
	ADD_AUDIT(HB_BLOCK_PERCENT_AUD,      SMALL_FONT, LT_CYAN,   "HB BLOCK",      PERCENT, LT_CYAN  );
	ADD_AUDIT(OFF_EXTRA_1_PERCENT_AUD,   SMALL_FONT, LT_YELLOW, "EXTRA PLAY 1",  PERCENT, LT_YELLOW);
	ADD_AUDIT(OFF_EXTRA_2_PERCENT_AUD,   SMALL_FONT, LT_CYAN,   "EXTRA PLAY 2",  PERCENT, LT_CYAN  );
	ADD_AUDIT(OFF_EXTRA_3_PERCENT_AUD,   SMALL_FONT, LT_YELLOW, "EXTRA PLAY 3",  PERCENT, LT_YELLOW);
	ADD_AUDIT(OFF_EXTRA_4_PERCENT_AUD,   SMALL_FONT, LT_CYAN,   "EXTRA PLAY 4",  PERCENT, LT_CYAN  );
	ADD_AUDIT(OFF_EXTRA_5_PERCENT_AUD,   SMALL_FONT, LT_YELLOW, "EXTRA PLAY 5",  PERCENT, LT_YELLOW);
	ADD_AUDIT(OFF_EXTRA_6_PERCENT_AUD,   SMALL_FONT, LT_CYAN,   "EXTRA PLAY 6",  PERCENT, LT_CYAN  );
	ADD_AUDIT(OFF_EXTRA_7_PERCENT_AUD,   SMALL_FONT, LT_YELLOW, "EXTRA PLAY 7",  PERCENT, LT_YELLOW);
	ADD_AUDIT(OFF_EXTRA_8_PERCENT_AUD,   SMALL_FONT, LT_CYAN,   "EXTRA PLAY 8",  PERCENT, LT_CYAN  );
	ADD_AUDIT(OFF_EXTRA_9_PERCENT_AUD,   SMALL_FONT, LT_YELLOW, "EXTRA PLAY 9",  PERCENT, LT_YELLOW);
	ADD_AUDIT(PUNT_PERCENT_AUD,          SMALL_FONT, LT_CYAN,   "PUNT",          PERCENT, LT_CYAN  );
	ADD_AUDIT(FAKE_PUNT_PERCENT_AUD,     SMALL_FONT, LT_YELLOW, "FAKE PUNT",     PERCENT, LT_YELLOW);
	ADD_AUDIT(FIELD_GOAL_PERCENT_AUD,    SMALL_FONT, LT_CYAN,   "FIELD GOAL",    PERCENT, LT_CYAN  );
	ADD_AUDIT(FAKE_FG_PERCENT_AUD,       SMALL_FONT, LT_YELLOW, "FAKE F.G.",     PERCENT, LT_YELLOW);
	ADD_AUDIT(EXTRA_POINT_PERCENT_AUD,   SMALL_FONT, LT_CYAN,   "EXTRA POINT",   PERCENT, LT_CYAN  );

	// Defensive Play Audits
	ADD_MENU_ITEM("DEFENSIVE PLAYS", "SHOW STATISTICS ABOUT DEFENSIVE PLAYS PICKED");
	COLUMNS(2);	
	// Defensive Play Audit Page(s)
	ADD_AUDIT(SAFE_COVER_PERCENT_AUD,    SMALL_FONT, LT_YELLOW, "SAFE COVER",    PERCENT, LT_YELLOW);
	ADD_AUDIT(ONE_MAN_BLITZ_PERCENT_AUD, SMALL_FONT, LT_CYAN,   "1 MAN BLITZ",   PERCENT, LT_CYAN  );
	ADD_AUDIT(TWO_MAN_BLITZ_PERCENT_AUD, SMALL_FONT, LT_YELLOW, "2 MAN BLITZ",   PERCENT, LT_YELLOW);
	ADD_AUDIT(SUICIDE_BLITZ_PERCENT_AUD, SMALL_FONT, LT_CYAN,   "SUICIDE BLITZ", PERCENT, LT_CYAN  );
	ADD_AUDIT(ZONE_BLITZ_PERCENT_AUD,    SMALL_FONT, LT_YELLOW, "ZONE BLITZ",    PERCENT, LT_YELLOW);
	ADD_AUDIT(NEAR_ZONE_PERCENT_AUD,     SMALL_FONT, LT_CYAN,   "NEAR ZONE",     PERCENT, LT_CYAN  );
	ADD_AUDIT(MEDIUM_ZONE_PERCENT_AUD,   SMALL_FONT, LT_YELLOW, "MEDIUM ZONE",   PERCENT, LT_YELLOW);
	ADD_AUDIT(DEEP_ZONE_PERCENT_AUD,     SMALL_FONT, LT_CYAN,   "DEEP ZONE",     PERCENT, LT_CYAN  );
	ADD_AUDIT(GOAL_LINE_PERCENT_AUD,     SMALL_FONT, LT_YELLOW, "GOAL LINE",     PERCENT, LT_YELLOW);
	ADD_AUDIT(DEF_EXTRA_1_PERCENT_AUD,   SMALL_FONT, LT_CYAN,   "EXTRA PLAY 1",  PERCENT, LT_CYAN  );
	ADD_AUDIT(DEF_EXTRA_2_PERCENT_AUD,   SMALL_FONT, LT_YELLOW, "EXTRA PLAY 2",  PERCENT, LT_YELLOW);
	ADD_AUDIT(DEF_EXTRA_3_PERCENT_AUD,   SMALL_FONT, LT_CYAN,   "EXTRA PLAY 3",  PERCENT, LT_CYAN  );
	ADD_AUDIT(DEF_EXTRA_4_PERCENT_AUD,   SMALL_FONT, LT_YELLOW, "EXTRA PLAY 4",  PERCENT, LT_YELLOW);
	ADD_AUDIT(DEF_EXTRA_5_PERCENT_AUD,   SMALL_FONT, LT_CYAN,   "EXTRA PLAY 5",  PERCENT, LT_CYAN  );
	ADD_AUDIT(DEF_EXTRA_6_PERCENT_AUD,   SMALL_FONT, LT_YELLOW, "EXTRA PLAY 6",  PERCENT, LT_YELLOW);
	ADD_AUDIT(DEF_EXTRA_7_PERCENT_AUD,   SMALL_FONT, LT_CYAN,   "EXTRA PLAY 7",  PERCENT, LT_CYAN  );
	ADD_AUDIT(DEF_EXTRA_8_PERCENT_AUD,   SMALL_FONT, LT_YELLOW, "EXTRA PLAY 8",  PERCENT, LT_YELLOW);
	ADD_AUDIT(DEF_EXTRA_9_PERCENT_AUD,   SMALL_FONT, LT_CYAN,   "EXTRA PLAY 9",  PERCENT, LT_CYAN  );
	ADD_AUDIT(BLOCK_FG_PERCENT_AUD,      SMALL_FONT, LT_YELLOW, "BLOCK F.G.",    PERCENT, LT_YELLOW);
	ADD_AUDIT(BLOCK_PUNT_PERCENT_AUD,    SMALL_FONT, LT_CYAN,   "BLOCK PUNT",    PERCENT, LT_CYAN  );
	ADD_AUDIT(PUNT_RETURN_PERCENT_AUD,   SMALL_FONT, LT_YELLOW, "PUNT RETURN",   PERCENT, LT_YELLOW);

#if defined(SEATTLE)

	// Game resets
	ADD_MENU_ITEM("GAME RESETS", "SHOW GAME RESET AUDITS");
	COLUMNS(2);
	ADD_AUDIT(TLB_MOD_AUD,             SMALL_FONT, WHITE,   "TLB MODIFICATIONS",         DECIMAL, WHITE);
	ADD_AUDIT(TLB_LOAD_AUD,            SMALL_FONT, LT_CYAN, "TLB LOAD",                  DECIMAL, LT_CYAN);
	ADD_AUDIT(TLB_STORE_AUD,           SMALL_FONT, WHITE,   "TLB STORE",                 DECIMAL, WHITE);
	ADD_AUDIT(ADDR_LOAD_AUD,           SMALL_FONT, LT_CYAN, "ADDRESS ERROR (LOAD)",      DECIMAL, LT_CYAN);
	ADD_AUDIT(ADDR_STORE_AUD,          SMALL_FONT, WHITE,   "ADDRESS ERROR (STORE)",     DECIMAL, WHITE);
	ADD_AUDIT(BUS_FETCH_AUD,           SMALL_FONT, LT_CYAN, "BUS ERROR (FETCH)",         DECIMAL, LT_CYAN);
	ADD_AUDIT(BUS_DATA_AUD,            SMALL_FONT, WHITE,   "BUS ERROR (DATA)",          DECIMAL, WHITE);
	ADD_AUDIT(SYSCALL_AUD,             SMALL_FONT, LT_CYAN, "SYSTEM CALL",               DECIMAL, LT_CYAN);
	ADD_AUDIT(RSVD_INST_AUD,           SMALL_FONT, WHITE,   "RESERVED INSTRUCTION",      DECIMAL, WHITE);
	ADD_AUDIT(COP_USE_AUD,             SMALL_FONT, LT_CYAN, "COPROCESSOR UNUSABLE",      DECIMAL, LT_CYAN);
	ADD_AUDIT(INT_ARITH_AUD,           SMALL_FONT, WHITE,   "ARITHMETIC OVERFLOW (INT)", DECIMAL, WHITE);
	ADD_AUDIT(TRAP_AUD,                SMALL_FONT, LT_CYAN, "TRAP",                      DECIMAL, LT_CYAN);
	ADD_AUDIT(RSVD_EXC_AUD,            SMALL_FONT, WHITE,   "RESERVED EXCEPTION",        DECIMAL, WHITE);
	ADD_AUDIT(RESTART_AUD,             SMALL_FONT, LT_CYAN, "GAME RESTARTS",             DECIMAL, LT_CYAN);
	ADD_AUDIT(FPU_EXC_AUD,             SMALL_FONT, WHITE,   "FLOATING POINT",            DECIMAL, WHITE);
	ADD_AUDIT(INTEGER_DIV0_AUD,        SMALL_FONT, LT_CYAN, "INTEGER DIVIDE BY ZERO",    DECIMAL, LT_CYAN);
	ADD_AUDIT(INTEGER_OVERFLOW_AUD,    SMALL_FONT, WHITE,   "INTEGER DIVIDE OVERFLOW",   DECIMAL, WHITE);
	ADD_AUDIT(BUFFER_SWAP_TIMEOUT_AUD, SMALL_FONT, LT_CYAN, "BUFFER SWAP TIMEOUT",       DECIMAL, LT_CYAN);
	ADD_AUDIT(DMA_TIMEOUT_AUD,         SMALL_FONT, WHITE,   "DMA TIMEOUT",               DECIMAL, WHITE);
	ADD_AUDIT(DIV0_UNHANDLED_AUD,      SMALL_FONT, LT_CYAN, "UNHANDLED FP DIV0",         DECIMAL, LT_CYAN);
	ADD_AUDIT(VBLANK_FREEZE_AUD,       SMALL_FONT, WHITE,   "PROCESS SYSTEM LOCKED",     DECIMAL, WHITE);
	ADD_AUDIT(TLB_REFILL_AUD,          SMALL_FONT, LT_CYAN, "TLB REFILL",                DECIMAL, LT_CYAN);
	ADD_AUDIT(WATCHDOG_AUDIT_NUM,      SMALL_FONT, LT_RED,  "TOTAL WATCHDOGS",           DECIMAL, LT_RED);

	// Cause and PC
	ADD_MENU_ITEM("C AND PC", "SHOW CAUSE AND PC AUDITS");
	ADD_AUDIT(CAUSE_AUD,  MEDIUM_FONT, WHITE,   "CAUSE", HEX, WHITE);
	ADD_AUDIT(PC_AUD,     MEDIUM_FONT, LT_CYAN, "PC",    HEX, LT_CYAN);
	ADD_AUDIT(CAUSE_AUD2, MEDIUM_FONT, WHITE,   "CAUSE", HEX, WHITE);
	ADD_AUDIT(PC_AUD2,    MEDIUM_FONT, LT_CYAN, "PC",    HEX, LT_CYAN);
	ADD_AUDIT(CAUSE_AUD3, MEDIUM_FONT, WHITE,   "CAUSE", HEX, WHITE);
	ADD_AUDIT(PC_AUD3,    MEDIUM_FONT, LT_CYAN, "PC",    HEX, LT_CYAN);
	ADD_AUDIT(CAUSE_AUD4, MEDIUM_FONT, WHITE,   "CAUSE", HEX, WHITE);
	ADD_AUDIT(PC_AUD4,    MEDIUM_FONT, LT_CYAN, "PC",    HEX, LT_CYAN);
	ADD_AUDIT(CAUSE_AUD5, MEDIUM_FONT, WHITE,   "CAUSE", HEX, WHITE);
	ADD_AUDIT(PC_AUD5,    MEDIUM_FONT, LT_CYAN, "PC",    HEX, LT_CYAN);
	ADD_AUDIT(CAUSE_AUD6, MEDIUM_FONT, WHITE,   "CAUSE", HEX, WHITE);
	ADD_AUDIT(PC_AUD6,    MEDIUM_FONT, LT_CYAN, "PC",    HEX, LT_CYAN);
	
	// REG DUMP
	ADD_MENU_ITEM("REG DUMP", "SHOW ADDITIONAL PC AUDITS");
	COLUMNS(2);
	ADD_AUDIT(V0_AUD, SMALL_FONT, WHITE, "V0", HEX, WHITE);
	ADD_AUDIT(V1_AUD, SMALL_FONT, LT_CYAN, "V1", HEX, LT_CYAN);
	ADD_AUDIT(A0_AUD, SMALL_FONT, WHITE, "A0", HEX, WHITE);
	ADD_AUDIT(A1_AUD, SMALL_FONT, LT_CYAN, "A1", HEX, LT_CYAN);
	ADD_AUDIT(A2_AUD, SMALL_FONT, WHITE, "A2", HEX, WHITE);
	ADD_AUDIT(A3_AUD, SMALL_FONT, LT_CYAN, "A3", HEX, LT_CYAN);
	ADD_AUDIT(T0_AUD, SMALL_FONT, WHITE, "T0", HEX, WHITE);
	ADD_AUDIT(T1_AUD, SMALL_FONT, LT_CYAN, "T1", HEX, LT_CYAN);
	ADD_AUDIT(T2_AUD, SMALL_FONT, WHITE, "T2", HEX, WHITE);
	ADD_AUDIT(T3_AUD, SMALL_FONT, LT_CYAN, "T3", HEX, LT_CYAN);
	ADD_AUDIT(T4_AUD, SMALL_FONT, WHITE, "T4", HEX, WHITE);
	ADD_AUDIT(T5_AUD, SMALL_FONT, LT_CYAN, "T5", HEX, LT_CYAN);
	ADD_AUDIT(T6_AUD, SMALL_FONT, WHITE, "T6", HEX, WHITE);
	ADD_AUDIT(T7_AUD, SMALL_FONT, LT_CYAN, "T7", HEX, LT_CYAN);
	ADD_AUDIT(T8_AUD, SMALL_FONT, WHITE, "T8", HEX, WHITE);
	ADD_AUDIT(T9_AUD, SMALL_FONT, LT_CYAN, "T9", HEX, LT_CYAN);
	ADD_AUDIT(S0_AUD, SMALL_FONT, WHITE, "S0", HEX, WHITE);
	ADD_AUDIT(S1_AUD, SMALL_FONT, LT_CYAN, "S1", HEX, LT_CYAN);
	ADD_AUDIT(S2_AUD, SMALL_FONT, WHITE, "S2", HEX, WHITE);
	ADD_AUDIT(S3_AUD, SMALL_FONT, LT_CYAN, "S3", HEX, LT_CYAN);
	ADD_AUDIT(S4_AUD, SMALL_FONT, WHITE, "S4", HEX, WHITE);
	ADD_AUDIT(S5_AUD, SMALL_FONT, LT_CYAN, "S5", HEX, LT_CYAN);
	ADD_AUDIT(S6_AUD, SMALL_FONT, WHITE, "S6", HEX, WHITE);
	ADD_AUDIT(S7_AUD, SMALL_FONT, LT_CYAN, "S7", HEX, LT_CYAN);
	ADD_AUDIT(S8_AUD, SMALL_FONT, WHITE, "S8", HEX, WHITE);
//	ADD_AUDIT(K0_AUD, SMALL_FONT, LT_CYAN, "K0", HEX, LT_CYAN);
//	ADD_AUDIT(K1_AUD, SMALL_FONT, WHITE, "K1", HEX, WHITE);
//	ADD_AUDIT(GP_AUD, SMALL_FONT, LT_CYAN, "GP", HEX, LT_CYAN);
	ADD_AUDIT(SP_AUD, SMALL_FONT, WHITE, "SP", HEX, WHITE);
	ADD_AUDIT(RA_AUD, SMALL_FONT, LT_CYAN, "RA", HEX, LT_CYAN);
	ADD_AUDIT(CURPROC_AUD, SMALL_FONT, WHITE, "CP", HEX, WHITE);
//	ADD_AUDIT(AT_AUD, SMALL_FONT, WHITE, "AT", HEX, WHITE);

#endif

}


static void clear_aud(int num, int *table)
{
	int	val;

	while(num--)
	{
		set_audit(*table, 0);
		if(get_audit(*table, &val))
		{
#ifdef DEBUG
			fprintf(stderr, "Trouble clear audit number: %d\r\n", *table);
			lockup();
#endif
			return;
		}
		++table;
	}
}
			
void clear_coin_audits(void)
{
	clear_aud(sizeof(coin_audits)/sizeof(int), coin_audits);
}

void clear_credits_audits(void)
{
	clear_aud(sizeof(credits_audits)/sizeof(int), credits_audits);
}

void clear_game_audits(void)
{
	clear_aud(sizeof(game_audits)/sizeof(int), game_audits);
}

void clear_team_audits(void)
{
	clear_aud(sizeof(team_audits)/sizeof(int), team_audits);
}

void clear_reset_audits(void)
{
	clear_aud(sizeof(reset_audits)/sizeof(int), reset_audits);
}

void clear_def_play_audits(void)
{
	clear_aud(sizeof(def_play_audits)/sizeof(int), def_play_audits);
}

void clear_off_play_audits(void)
{
	clear_aud(sizeof(off_play_audits)/sizeof(int), off_play_audits);
}

void clear_cause_pc_audits(void)
{
	clear_aud(sizeof(cause_pc_audits)/sizeof(int), cause_pc_audits);
}

void clear_all_audits(void)
{
	clear_coin_audits();
	clear_credits_audits();
	clear_game_audits();
	clear_team_audits();
	clear_reset_audits();
	clear_def_play_audits();
	clear_off_play_audits();
	clear_cause_pc_audits();
}

static int check_aud(int num, int *table)
{
	int	val;

	while(num--)
	{
		if(get_audit(*table, &val))
		{
			set_audit(*table, 0);
			if(get_audit(*table, &val))
			{
#ifdef DEBUG
				fprintf(stderr, "Trouble with audit number: %d\r\n", *table);
				lockup();
#endif
				return(0);
			}
		}
		++table;
	}
	return(1);
}

int check_coin_audits(void)
{
	return(check_aud(sizeof(coin_audits)/sizeof(int), coin_audits));
}

int check_credits_audits(void)
{
	return(check_aud(sizeof(credits_audits)/sizeof(int), credits_audits));
}

int check_game_audits(void)
{
	return(check_aud(sizeof(game_audits)/sizeof(int), game_audits));
}

int check_team_audits(void)
{
	return(check_aud(sizeof(team_audits)/sizeof(int), team_audits));
}

int check_reset_audits(void)
{
	return(check_aud(sizeof(reset_audits)/sizeof(int), reset_audits));
}

int check_def_play_audits(void)
{
	return(check_aud(sizeof(def_play_audits)/sizeof(int), def_play_audits));
}

int check_off_play_audits(void)
{
	return(check_aud(sizeof(off_play_audits)/sizeof(int), off_play_audits));
}

int check_cause_pc_audits(void)
{
	return(check_aud(sizeof(cause_pc_audits)/sizeof(int), cause_pc_audits));
}

int check_all_audits(void)
{
	int	status = 1;

	if(!check_coin_audits())
	{
		status = 0;
	}

	if(!check_credits_audits())
	{
		status = 0;
	}

	if(!check_game_audits())
	{
		status = 0;
	}

	if(!check_team_audits())
	{
		status = 0;
	}

	if(!check_reset_audits())
	{
		status = 0;
	}

	if(!check_def_play_audits())
	{
		status = 0;
	}

	if(!check_off_play_audits())
	{
		status = 0;
	}

	if(!check_cause_pc_audits())
	{
		status = 0;
	}

	return(status);
}


void do_percent_aud(int naud, int daud, int paud)
{
	int	divis;
	int	nom;

	increment_audit(naud);
	if(!(get_audit(daud, &divis)))
	{
		if(divis > 0)
		{
			get_audit(naud, &nom);
			nom *= 100;
			nom /= divis;
			set_audit(paud, nom);
		}
	}
}

void recalc_percent_aud(int naud, int daud, int paud)
{
	int	divis;
	int	nom;

	if(!(get_audit(daud, &divis)))
	{
		if(divis > 0)
		{
			get_audit(naud, &nom);
			nom *= 100;
			nom /= divis;
			set_audit(paud, nom);
		}
	}
}

static int get_aud_total(int saud, int eaud)
{
	int	i;
	int	divis = 0;
	int	val;

	for(i = saud; i <= eaud; i++)
	{
		if(get_audit(i, &val))
		{
			val = 0;
		}
		divis += val;
	}
	return(divis);
}

static void calc_def_aud_percent(int aud, int divis)
{
	int	i;
	int	nom;
	int	val;

	if(get_audit(aud, &nom))
	{
		nom = 0;
	}

	if(!divis)
	{
		set_audit(aud+(SAFE_COVER_PERCENT_AUD-SAFE_COVER_AUD), 0);
		return;
	}

	nom *= 100;
	nom /= divis;

	set_audit(aud+(SAFE_COVER_PERCENT_AUD-SAFE_COVER_AUD), nom);
}

void do_def_play_percent_aud(int aud)
{
	int	i;
	int	divis;

	increment_audit(aud);
	divis = get_aud_total(SAFE_COVER_AUD, PUNT_RETURN_AUD);
	for(i = SAFE_COVER_AUD; i <= PUNT_RETURN_AUD; i++)
	{
		calc_def_aud_percent(i, divis);
	}
}

static void calc_off_aud_percent(int aud, int divis)
{
	int	i;
	int	nom;
	int	val;

	if(get_audit(aud, &nom))
	{
		nom = 0;
	}

	if(!divis)
	{
//		set_audit(aud+(UPPER_CUT_PERCENT_AUD-UPPER_CUT_AUD), 0);
		set_audit(aud+(TEAMPLAY_1_PERCENT_AUD-TEAMPLAY_1_AUD), 0);
		return;
	}

	nom *= 100;
	nom /= divis;

//	set_audit(aud+(UPPER_CUT_PERCENT_AUD-UPPER_CUT_AUD), nom);
	set_audit(aud+(TEAMPLAY_1_PERCENT_AUD-TEAMPLAY_1_AUD), nom);
}

void do_off_play_percent_aud(int aud)
{
	int	i;
	int	divis;

	increment_audit(aud);
//	divis = get_aud_total(UPPER_CUT_AUD, SUPER_FLY_AUD);
	divis = get_aud_total(TEAMPLAY_1_AUD, EXTRA_POINT_AUD);
	for(i = TEAMPLAY_1_AUD; i <= EXTRA_POINT_AUD; i++)
	{
		calc_off_aud_percent(i, divis);
	}
}
	

static void calc_team_aud_percent(int aud, int divis)
{
	int	i;
	int	nom;
	int	val;

	if(get_audit(aud, &nom))
	{
		nom = 0;
	}

	if(!divis)
	{
		set_audit(aud+(ARIZONA_CARDINALS_PERCENT_AUD-ARIZONA_CARDINALS_AUD), 0);
		return;
	}

	nom *= 100;
	nom /= divis;

	set_audit(aud+(ARIZONA_CARDINALS_PERCENT_AUD-ARIZONA_CARDINALS_AUD), nom);
}


void do_team_percent_aud(int aud)
{
	int	i;
	int	divis;

	increment_audit(aud);
	divis = get_aud_total(FIRST_TEAM_AUD, LAST_TEAM_AUD);
	for(i = FIRST_TEAM_AUD; i <= LAST_TEAM_AUD; i++)
	{
		calc_team_aud_percent(i, divis);
	}
}

/*
** add_to_audit() - This function is used to add to the value
** of an audit.
*/
int add_to_audit(int audit_number, int val)
{
	int	old_val;
	int	status;

	old_val = audit_number;
	status = get_audit(audit_number, &old_val);
	if(status != CMOS_OK)
	{
		return(status);
	}
	old_val += val;
	return(set_audit(audit_number, old_val));
}

/*
** decrement_audit() - This function is used to decrement the value of an
** audit.
*/
int decrement_audit(int audit_number)
{
	int	old_val;
	int	status;

	old_val = audit_number;
	status = get_audit(audit_number, &old_val);
	if(status != CMOS_OK)
	{
		return(status);
	}
	old_val--;
	return(set_audit(audit_number, old_val));
}

/*
** get_audit_val() - Returns the value of an audit. (No error checking)
*/
int get_audit_val( int audit_number )
{
	int		val;

	get_audit( audit_number, &val );

	return val;
}
