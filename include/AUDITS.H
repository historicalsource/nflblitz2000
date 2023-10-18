#ifndef	__AUDITS_H__
#define	__AUDITS_H__

#include <stdio.h>

// Audit numbers
enum
{
// Coin Audits
LEFT_COIN_AUDIT_NUM,			// Left coin slot coins
RIGHT_COIN_AUDIT_NUM,			// Right coin slot coins
BILLS_COIN_AUDIT_NUM,			// Dollar bill validator bills
CENTER_COIN_AUDIT_NUM,			// Center coin slot coins
EXTRA_COIN_AUDIT_NUM,			// Extra coin slot coins
GAME_START_AUD,					// Attract mode game starts (unless play editor)
MID_GAME_START_AUD,				// Mid-game starts
CONTINUE_AUD,					// Game continues
FREE_QUARTERS_AUD,				// Free quarter awarded
FREE_GAME_AUD,					// Free games awarded
SERVICE_CREDITS_AUD,			// Total service credits
TOTAL_PLAYS_AUD,				// Total number of plays
TOTAL_PAID_CREDITS_AUD,			// Total number of credits paid for
TOTAL_MONEY_AUDIT_NUM,			// Total amount of money taken in

// credits audits
CREDITS_AUD,					// Available credits

// misc game audits
UPTIME_AUDIT_NUM,				// Total game uptime
PLAYTIME_AUD,					// Total play time
PLAYTIME_PERCENT_AUD,			// Playtime vs uptime
FULL_GAME_PURCHASE_AUD,			// Up-front full game purchases
FULL_GAME_PURCHASE_PCT_AUD,		// full games purchased / starts

INITIALS_ENTERED_AUD,			// Number of times initials entered
INITIALS_ENTERED_PERCENT_AUD,	// Initials enterd percentage

CREATE_PLAY_AUD,				// play editor starts
FINISHED_GAMES_AUD,				// games finished (including ties in OT3)
QUARTERS_PLAYED_AUD,			// ?p quarters played

ONE_PLAYER_AUD,					// 1p quarters played
TWO_PLAYER_AUD,					// 2p quarters played
THREE_PLAYER_AUD,				// 3p quarters played
FOUR_PLAYER_AUD,				// 4p quarters played
TWOVCPU_PLAYER_AUD,				// 2p quarters played as 2 vs CPU

ONE_PLAYER_PERCENT_AUD,			// 1p aud / quarters played
TWO_PLAYER_PERCENT_AUD,			// 2p aud / quarters played
THREE_PLAYER_PERCENT_AUD,		// 3p aud / quarters played
FOUR_PLAYER_PERCENT_AUD,		// 4p aud / quarters played
TWOVCPU_PLAYER_PERCENT_AUD,		// 2vcpu / 2p aud

ONE_QUARTER_AUD,				// first quarters played
TWO_QUARTER_AUD,				// second quarters played
THREE_QUARTER_AUD,				// third quarters played
FOUR_QUARTER_AUD,				// fourth quarters played
ONE_OVERTIME_AUD,				// first overtimes played
TWO_OVERTIME_AUD,				// second overtimes played
THREE_OVERTIME_AUD,				// third overtimes played

ONE_QUARTER_PERCENT_AUD, 		// One quarter play percentage
TWO_QUARTER_PERCENT_AUD, 		// Two quarter play percentage
THREE_QUARTER_PERCENT_AUD,		// Three quarter play percentage
FOUR_QUARTER_PERCENT_AUD, 		// Four quarter play percentage
ONE_OVERTIME_PERCENT_AUD, 		// One overtime quarter percentage
TWO_OVERTIME_PERCENT_AUD, 		// Two overtime quarter percentage
THREE_OVERTIME_PERCENT_AUD,		// Three overtime quarter percentage

ONE_PLAYER_FINISH_AUD,			// games ending in 1p mode
TWO_PLAYER_FINISH_AUD,			// games ending in 2p mode
THREE_PLAYER_FINISH_AUD,		// games ending in 3p mode
FOUR_PLAYER_FINISH_AUD,			// games ending in 4p mode
TWOVCPU_PLAYER_FINISH_AUD,		// games ending in 2p mode with 2 vs cpu

ONE_PLAYER_FINISH_PERCENT_AUD,	// games ending in 1p mode / finished
TWO_PLAYER_FINISH_PERCENT_AUD,	// games ending in 2p mode / finished
THREE_PLAYER_FINISH_PERCENT_AUD,// games ending in 3p mode / finished
FOUR_PLAYER_FINISH_PERCENT_AUD,	// games ending in 4p mode / finished
TWOVCPU_PLAYER_FINISH_PERCENT_AUD,		// games ending in 2p mode with 2 vs cpu / finished

HVH_FINISH_AUD,					// games ending with humans on both teams
HVC_FINISH_AUD,					// games ending with humans on only one team (no percent)
HVH_FINISH_PERCENT_AUD,			// games ending with humans on both teams / finished

TOTAL_HVSH_POINTS_AUD,			// Total points scored in h vs h games
TOTAL_HVSC_POINTS_AUD,			// Total points scored in h vs cpu games
AVERAGE_HVSH_POINTS_AUD,		// Average h vs h points
AVERAGE_HVSC_POINTS_AUD,		// Average h vs cpu points
TOTAL_WINNING_POINTS_AUD,		// Total winning points
TOTAL_LOSING_POINTS_AUD,		// Total losing points
AVERAGE_WINNING_SCORE_AUD,		// Average winning score
AVERAGE_LOSING_SCORE_AUD,		// Average losing score
TOTAL_CPU_VICTORIES_AUD,		// Total number of CPU victories
CPU_VICTORY_PERCENT_AUD,		// Percentage number of CPU victories
LARGEST_CPU_VICTORY_AUD,		// Largest cpu victory margin
LARGEST_CPU_LOSS_AUD,			// Largest cpu loss margin

LEFT_SIDE_WINS_AUD,				// Left side win count (hvh)
LEFT_SIDE_WINS_PERCENT_AUD,		// Left side win percentage

// Team Audits
ARIZONA_CARDINALS_AUD,
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
WASHINGTON_REDSKINS_AUD,
// Team Audit Percentages
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

// Failure audits
TEX_LOAD_FAIL_AUD,				// Texture load failures
SND_LOAD_FAIL_AUD,				// Sound Bank load failures

// Game reset audits
WATCHDOG_AUDIT_NUM,				// Total watchdogs
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

// Defensive Play Audits
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
// Defensive Play Audit Percentages
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

// Offensive Play Audits
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
// Offensive Play Audit Percentages
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

// Audits for tracking dogs
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

// N64 Memory Card audits; keep these in the PC_AUDn to NEXT_AUD
//  expansion space so audits won't be cleared by an update;
//  don't add more audits than space allows; (20-n*2+1) = 9

//N64CARD_AUDITED_AUD,
//N64CARD_PLUGINS_AUD,
//N64CARD_BLITZED_AUD,
//N64CARD_PLAY_OK_AUD,
//N64CARD_PLAY_BD_AUD,

// Last audit
NEXT_AUD = PC_AUD+20 
};

// Definitions of font sizes
#define	LARGE_FONT	0
#define	MEDIUM_FONT	1
#define	SMALL_FONT	2

// Definitions of prompt and audit horizontal justification modes
#define	RIGHT_JUST	1
#define	LEFT_JUST	2
#define	CENTER_JUST	3

// Definitions of formatting parameters
#define	DECIMAL		"decimal"
#define	HEX			"hex"
#define	TIMESTAMP	"timestamp"
#define	DURATION	"duration"
#define	PERCENT		"percent"
#define	MONEY		"money"
#define	FLOAT		"float"
#define	STRING		"string"


// Macro definitions
#define	ADD_MENU_ITEM(a, b) 	fprintf(fp, "\r\nitem \"%s\" itemhelp \"%s\" itemdone\r\n", (a), (b))

#define	ADD_AUDIT(a, b, c, d, e, f)	fprintf(fp, "startaudit %d font %d pcolor %u prompt \"%s\" %s acolor %u auditdone\r\n", (a), (b), (c), (d), (e), (f))

#define	ADD_AUDIT_XY(a, b, c, d, e, f, g, h, i, j)	fprintf(fp, "startaudit %d font %d pcolor %u prompt \"%s\" %s acolor %u ax %u ay %u px %u py %u auditdone\r\n", (a), (b), (c), (d), (e), (f), (g), (h), (i), (j))

#define	MESSAGE(a, b, c, d, e)	fprintf(fp, "startaudit 0 font %d pcolor %u \"%s\" px %u py %u auditdone\r\n", (a), (b), (c), (d), (e))

#define	PAGE_BREAK	fprintf(fp, "pagebreak\r\n")

#define	COLUMNS(a)	fprintf(fp, "columns %d\r\n", (a))

#define	LINESPACE(a)	fprintf(fp, "linespace %d\r\n", (a))

#define	PROMPT_JUSTIFY	fprintf(fp, "padjust %d\r\n", (a))

#define	AUDIT_JUSTIFY	fprintf(fp, "aadjust %d\r\n", (a))


// Prototypes
void write_audit_info(FILE *fp);
int decrement_audit(int);
int add_to_audit( int, int );
void do_percent_aud(int, int, int);
void recalc_percent_aud(int, int, int);
void clear_all_audits(void);
void update_gamestart_percents( void );
void update_quarters_percents( void );
int get_audit_val( int );
void dump_raw_audits( void );

#endif