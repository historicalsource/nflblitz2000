//-------------------------------------------------------------------------------------------------
//                  This files has various info for NFL "2 minute warning"
//-------------------------------------------------------------------------------------------------
#ifndef _GAME__H
#define _GAME__H

#include "/Video/Nfl/Include/PlayStuf.h"						// need definition of fline struct

#ifndef FALSE
#define FALSE	0
#define TRUE	!FALSE
#endif

// #define	NO_GAME_PLATES		//	Developer can toggle transition "slam" plates on/off (define in LOCALSET.MAK)

// #define	FG_PUNT=0			// 0 for normal plays, 1 to always fg, 2 to always punt (define in LOCALSET.MAK)

#define ALL_ABCD_MASK	(P1_ABCD_MASK|P2_ABCD_MASK|P3_ABCD_MASK|P4_ABCD_MASK)

#define FG_MAX_TARGET_Y			(GOAL_TOP)			//	(for field goals) maximum value the ball should be when it reaches the goal
#define FG_MIN_TARGET_Y			(GOAL_CROSSBAR + 3)	//	(for field goals) minimum value to be above the goal post crossbar
#define FG_MAX_TARGET_Z_WIDTH	((int)GOAL_Z + 40)	//	Extra width on each side of goal that the ball can land in
#define FG_POST_BOUNDING_SIZE	(1.0f)				//	(for field goals) just a lame bounding volume around the post so the ball has more chance of hitting the post (cause it's cool when the ball hits the post, so make it happen more often!)

//	These values determine how fast (and how far) the ball will go after being punted
//	The values themselves are the velocity of the ball along each axis for one frame, multiplied by 10 for calculation purposes.
//	The actual velocity of the ball is determined based on the kicker's position on the field.
//	If he is 100 yards away from his goal, the ball velocity will be = (PUNT_MAX_VX / 10).
//	If he is 0 yards away from his goal, the ball velocity will be = (PUNT_MIN_VX / 10).
//	...and the intermediate distances are scaled linearly.
#define PUNT_MIN_VX		20		//	for actual velocity value divide by 10
#define PUNT_MAX_VX		50		//	for actual velocity value divide by 10
#define PUNT_MIN_VY		40		//	for actual velocity value divide by 10
#define PUNT_MAX_VY		60		//	for actual velocity value divide by 10

//
// Play types
//
#define PT_NORMAL			0
#define PT_KICKOFF			1
#define PT_FIELD_GOAL		2
#define PT_PUNT				3
#define PT_EXTRA_POINT		4
#define N_TYPES	    		5

//#define BARX1		(1.0f*HRESf/6.0f)
//#define BARX2		(5.0f*HRESf/6.0f)

//#define BARX1_K		( .9f*HRESf/6.0f)
//#define BARX2_K		(2.3f*HRESf/6.0f)
//#define BARX3_K		(3.8f*HRESf/6.0f)
//#define BARX4_K		(5.2f*HRESf/6.0f)

#define BARX1		128.0f
#define BARX2		384.0f

#define BARX1_K		64.0f
#define BARX2_K		192.0f
#define BARX3_K		320.0f
#define BARX4_K		448.0f

//#define BARY	(192.0f)
#define BARY	(26.0f)
#define BARY_LR	(20.0f)

//
// Game flags -- cleared at the end of every play
//
#define GF_REC_IN_EZ		0x0001		// carrier recovered ball in end zone
#define GF_LEFT_EZ			0x0002		// carrier has moved outside end zone
#define GF_THROUGH_UPRIGHTS	0x0004		// kick passses between goal posts
#define GF_POSS_CHANGE		0x0008		// ball changed hands at least once
#define GF_BOBBLE			0x0010		// ball was bobbled at some point
#define GF_FD_LINE_CROSSED	0x0020		// ball carrier crossed first down
#define GF_FWD_PASS			0x0040		// pass thrown and crossed LOS (not neccesarily caught)
#define GF_PASS_THROWN		0x0080		// pass was thrown (anywhere)
#define GF_BALL_PUNTED		0x0100		// ball was punted
#define GF_BALL_KICKED		0x0200		// ball was kicked off or FG attempt
#define GF_PLAY_ENDING		0x0400		// ball carrier hit and is going down
#define GF_FUMBLE			0x0800		// ball was fumbled at some point
#define GF_QUICK_KICK		0x1000		// play is a special quick kickoff
#define GF_AIR_CATCH		0x2000		// last guy to catch ball was airborne and hasn't touched ground yet
#define GF_BALL_CROSS_LOS	0x4000		// ball has crossed LOS (not set on KICKOFF plays)
#define GF_HIKING_BALL		0x8000		// ball traveling from center to qb
#define GF_ONSIDE_KICK		0x00010000	// onside kick
#define GF_OLS_SPEEDS_SET	0x00020000	// over-line-of-scrimmage speeds set
#define GF_OPENING_KICK		0x00040000	// opening kickoff of either half

//
// Game state constants
//
#define GS_DIAGNOSTICS    -1           // diagnostics mode
#define GS_ATTRACT_MODE    0           // None of the below
#define GS_PRE_GAME        1           // All screens before game play
#define GS_GAME_MODE       2           // Actual game play
#define GS_EOQ_BUY_IN      3           // all screens from end of quarter to start of next
#define GS_GAME_OVER       4           // Everything after game until start of ATTRACT MODE
#define GS_FREE_PRICE      5           // screen at game end...telling winning team stays free
#define GS_HALF_PRICE      6           // screen at game end...telling winning team stays 1/2 price
#define GS_CREATE_PLAY     7           // create-a-play editor mode

//
// Game mode constants
//
#define GM_LINING_UP		0
#define	GM_PRE_SNAP			1
#define GM_IN_PLAY			2
#define	GM_PLAY_OVER		3
#define	GM_GAME_OVER		4
#define GM_PRE_KICKOFF		5			// kicker running toward ball
#define GM_INITIALIZING		6			// set before game starts

//
// How'd the play end?
//
#define	PEC_CAR_OOB				0		// carrier out of bounds
#define PEC_CAR_TACKLED_IB		1		// carrier tackled in bounds
#define PEC_CAR_TACKLED_OWNEZ	2		// carrier tackled in own end zone
#define	PEC_CAR_TOUCHDOWN		3		// carrier reaches enemy end zone
#define PEC_DEAD_BALL_IB		4		// dead ball inbounds
#define PEC_BALL_OOB			5		// ball hit ground out of bounds
#define N_CAUSES		        6

//
// Play results
//
#define PR_DOWN					0
#define PR_TOUCHDOWN			1
#define PR_SAFETY				2
#define PR_PAT_SAFETY			3
#define PR_NEW_POSSESSION		4
#define PR_INCOMPLETE			5
#define PR_FUMBLED_OOB			6
#define PR_FIELD_GOAL			7
#define PR_EXTRA_POINT			8
#define PR_MISSED_KICK			9
#define PR_FAILED_CONVERSION	10
#define PR_CONVERSION			11
#define PR_PUNT_OOB				12
#define PR_DOWNED_PUNT			13
#define PR_KICKOFF_OOB			14
#define PR_TOUCHBACK			15

//
// Play result flags
//
#define PRF_TURNOVER_ON_DOWNS	0x0000
#define PRF_FIRST_DOWN			0x0001
#define PRF_PAT					0x0002
#define PRF_KICKOFF				0x0004

//
// Powerups
//
#define PU_JUMBO_BALL			0x0001

//
// Big messages that appear after plays
//
#define MS_TOUCHDOWN			0
#define MS_TURNOVER			    1
#define MS_SAFETY				2
#define MS_2POINTS   			3
#define MS_4THDOWN       		4
#define MS_ITSGOOD   			5
#define MS_1STDOWN    			6
#define MS_FUMBLE    			7
#define MS_INTERCEPTION		    8
#define MS_NOGOOD			    9
#define MS_SACK       			10
#define MS_DBL_SACK  			11
#define MS_ON_DOWNS  			12
#define MS_TURNOVER2			13
#define MS_BLOCKED  			14
#define MS_PUNTRETURN  			15

#ifdef DEBUG
#define	special_lockup()	__asm__("	teqi	$0,0")
#else
#define	special_lockup()
#endif

#define	SPRITE_HRES	512.0f
#define	SPRITE_VRES	384.0f

#ifdef	LOW_RESOLUTION
#undef	HRES
#undef	VRES
#undef	HRESf
#undef	VRESf

#define	HRES	512
#define	VRES	256
#define	HRESf	512.0f
#define	VRESf	256.0f
#endif

#if defined(VEGAS)
#ifndef	HRESf
#define	HRESf	512.0f
#endif
#ifndef	VRESf
#define	VRESf	384.0f
#endif
#ifndef	HRES
#define	HRES	512
#endif
#ifndef	VRES
#define	VRES	384
#endif
#endif

//
// Number of NFL teams
//
#define	MAX_PLYRS    4						// # of human players supported

#define NUM_QUARTERS    4
#define MAX_QUARTERS    (NUM_QUARTERS+3)	// NUM_QUARTERS + 3 overtimes
#define NUM_TEAMS       31
#define NUM_PLAYERS     14
#define NUM_PERTEAM     (NUM_PLAYERS/2)
#define NUM_PERTEAM_SPD 16					// # per team in <st_plyr_data> table

// TIME in EACH QUARTER
#define QUARTER_TIME			0x00020000	// 2 minutes, 0 seconds, 0 tenths of seconds

// TIME minimum needed to allow buy-in
#define MINIMUM_QUARTER_TIME	0x00011e00	// (1:30:00) min time left to allow buyin

// equats for QUARTERS
#define	EOQ_1		0
#define	EOQ_2		1
#define	EOQ_3		2
#define	EOQ_4		3
#define	EOQ_OT1		4
#define	EOQ_OT2		5
#define	EOQ_OT3		6

#define DEFENSE_FIRECNT     2
#define OFFENSE_FIRECNT     3
#define YARDS_TO_GAIN       10.0f

#define OFFENSE				0
#define DEFENSE				1

// Goal post dimensions
#define GOAL_X			460.0f
#define GOAL_Z			22.0f
#define GOAL_Y			27.0f
#define GOAL_TOP		103.5f
#define GOAL_CROSSBAR	GOAL_Y

// Field Height from mid to backfield
#define FIELDHB	468.0f
//#define FIELDHB	540.0f
//#define FIELDH	360.0f

// Field Height from mid to endzone
#define FIELDHZ	390.0f
//#define FIELDHZ	450.0f
//#define FIELDHZ	300.0f

// Field Width from mid to sideline
#define FIELDW	182.85f
//#define FIELDW	206.7f
//#define FIELDW	239.0f
//#define FIELDW	159.0f

// distance from center of field to hash mark
#define HASHWIDTH	24.05

// Focal length correction factor
#define FOCFAC	(FIELDHZ / 300.0f)


// First kickoff los; Y & Z are going to be assumed 0
#define FIRSTKO_LOS	30

// yards to go for first down (int)
#define FDYARDS		30

//
// Team index constants (in TEAM CITY alphabetical order)
//
#define TEAM_CARDINALS		0
#define TEAM_FALCONS		1
#define TEAM_RAVENS			2
#define TEAM_BILLS			3
#define TEAM_PANTHERS		4
#define TEAM_BEARS			5
#define TEAM_BENGALS		6
#define TEAM_BROWNS			7
#define TEAM_COWBOYS		8
#define TEAM_BRONCOS		9
#define TEAM_LIONS			10
#define TEAM_PACKERS		11
#define TEAM_COLTS			12
#define TEAM_JAGUARS		13
#define TEAM_CHIEFS			14
#define TEAM_DOLPHINS		15
#define TEAM_VIKINGS		16
#define TEAM_PATRIOTS		17
#define TEAM_SAINTS			18
#define TEAM_GIANTS			19
#define TEAM_JETS			20
#define TEAM_RAIDERS		21
#define TEAM_EAGLES			22
#define TEAM_STEELERS		23
#define TEAM_CHARGERS		24
#define TEAM_FORTYNINERS	25
#define TEAM_SEAHAWKS		26
#define TEAM_RAMS			27
#define TEAM_BUCS			28
#define TEAM_TITANS			29
#define TEAM_REDSKINS		30

#define TEAM_BEA		TEAM_BEARS
#define TEAM_BEN		TEAM_BENGALS
#define TEAM_BIL		TEAM_BILLS
#define TEAM_BRO		TEAM_BRONCOS
#define TEAM_BUC		TEAM_BUCS
#define TEAM_CAR		TEAM_CARDINALS
#define TEAM_CHA		TEAM_CHARGERS
#define TEAM_CHI		TEAM_CHIEFS
#define TEAM_COL		TEAM_COLTS
#define TEAM_COW		TEAM_COWBOYS
#define TEAM_DOL		TEAM_DOLPHINS
#define TEAM_EAG		TEAM_EAGLES
#define TEAM_FAL		TEAM_FALCONS
#define TEAM_FOR		TEAM_FORTYNINERS
#define TEAM_GIA		TEAM_GIANTS
#define TEAM_JAG		TEAM_JAGUARS
#define TEAM_JET		TEAM_JETS
#define TEAM_LIO		TEAM_LIONS
#define TEAM_TIT		TEAM_TITANS
#define TEAM_PAC		TEAM_PACKERS
#define TEAM_PAN		TEAM_PANTHERS
#define TEAM_PAT		TEAM_PATRIOTS
#define TEAM_RAI		TEAM_RAIDERS
#define TEAM_RAM		TEAM_RAMS
#define TEAM_RAV		TEAM_RAVENS
#define TEAM_RED		TEAM_REDSKINS
#define TEAM_SAI		TEAM_SAINTS
#define TEAM_SEA		TEAM_SEAHAWKS
#define TEAM_STE		TEAM_STEELERS
#define TEAM_VIK		TEAM_VIKINGS
#define TEAM_BRW		TEAM_BROWNS

//
// misc constants
//
#define HA_HOME	0
#define HA_AWAY	1

#define SIDE_0	0
#define SIDE_1	1

#define NOBALLCARRIER   	-1			// ball_carrier value for no ball owner

#ifndef	MAX_HIKE_TIME
#define MAX_HIKE_TIME   	5*tsec      // auto hike ball after this time
#endif
#define MAX_KOFF_TIME   	3	//90    //  same as MAX_HIKE_TIME but for kickoffs (must be > 1)
#define INITIAL_KOFF_TIME 	90          // First kickoff of game
#define DISALLOW_PASS_TIME	30          // disallow accidental passes after an auto hike for this many ticks

//#define	PLATE_TRANSITION_TICKS	75
#define	PLATE_TRANSITION_TICKS	10

#define	PLATES_COMING_OFF		-1
#define	PLATES_OFF				0
#define	PLATES_COMING_ON		1
#define	PLATES_ON				2

// # of ticks after which a player is IN CONTROL of the ball
// After this time, losing the ball is a fumble, not a bobble,
// and before this time, touchdowns and such won't register.
#define CONTROL_TIME	18

//
// misc macros
//
#define WORLDX(yd) (((FOCFAC*6.0f) * (float)((game_info.off_side) ? (100 - (yd)) : (yd))) - FIELDHZ)
#define WORLDX2(yd,side) (((FOCFAC*6.0f) * (float)((side) ? (100 - (yd)) : (yd))) - FIELDHZ)
#define FIELDX(yd)	((int)((FIELDHZ + ((game_info.off_side) ? (-1.0f * (yd)) : (yd))) / (FOCFAC*6.0f)))
#define FIELDX2(yd,side)	((int)((FIELDHZ + ((side) ? (-1.0f * (yd)) : (yd))) / (FOCFAC*6.0f)))

#define ALLTEAMS_MASK	(((unsigned int)1<<NUM_TEAMS)-1)

#define TEAM1_MASK		(four_plr_ver ? 0x3 : 0x1)
#define TEAM2_MASK		(four_plr_ver ? 0xC : 0x2)
#define TEAM_MASK(x)	((x) ? TEAM2_MASK : TEAM1_MASK)
#define HUMAN_TEAMS		((!!(p_status & TEAM1_MASK))+(!!(p_status & TEAM2_MASK)))
#define ISHUMANTEAM(x)	(p_status & TEAM_MASK((x)->team))
#define ISDRONETEAM(x)	(!(ISHUMANTEAM(x)))
#define	PLYRTEAM(x)		(four_plr_ver ? ((x)/2):(!!(x)))

#define	FRELX(x,z,t)	(z * isin(t) + x * icos(t))
#define	FRELZ(x,z,t)	(z * icos(t) - x * isin(t))

#define LESSER(x,y)		((x)>(y) ? (y) : (x))
#define GREATER(x,y)	((x)>(y) ? (x) : (y))

#define ABS(x)			(((x)<0) ? (-(x)) : (x))

#define YD2UN	(FIELDHZ / 50.0f)				// Yards-to-units factor (units/yard)
#define FT2UN	(YD2UN / 3.0f)					// Feet-to-units factor (units/foot)
#define IN2UN	(FT2UN / 12.0f)					// Inches-to-units factor (units/inch)

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
typedef struct	font_load_info
{
	font_info_t	*font_info;
	int			id;
} font_load_info_t;

//
// Player(1-4) data struture
//
typedef struct pdata
{
	struct sprite_info * si_frame;		// turbo bar frame sprite *
	struct sprite_info * si_turbo;		// turbo bar sprite *
	struct sprite_info * si_tmsg;		// message sprite *

	float	turbo;						// turbo % (0.0 to 100.0) remaining
	float	turbo_last;					// last turbo % remaining
	float	turbo_w;					// turbo bar pixel width

	int		powerups;					// PU_???, defined below

	int     credits_paid;				// paid credits...for FULL game purchased
	int     credits_show;				// info box is showing # of credits
	int     quarters_purchased;			// bought quarters
	int     please_wait;				// player has to wait until quarter end
	struct	sprite_info * si_fframe;	// turbo bar fire frame sprite *
	int		fframe;						// frame nbr. of FIRE effect
	int		fdelay;						// fire frame change delay
} pdata_t;

//
// Team (0-1) data structure...which keeps track of the stats (bookeeping)
//
typedef struct tdata
{
	int			time_of_poss;
	int 		first_downs;
	int 		touchdowns;
	int 		pass_touchdowns;
	int 		safetys;
	int 		fg_attempts;
	int 		field_goals;
	int 		block_fgs;
	int 		extra_points;
	int 		two_pt_convs;
	int 		two_pt_attempts;
	int 		tackles;
	int 		punts;
	int 		punt_yards;
	int 		missed_tackles;
	int 		interceptions;
	int 		fumbles;			// fumbles dropped
	int 		lost_fumbles;		// fumbles recovered by enemy
	int 		rushes;
	int 		rushing_yards;
	int 		passes;
	int 		completions;
	int 		passing_yards;
	int			kick_returns;		// kickoff & punt returns
	int			return_yards;		// kickoffs and punts
	int 		total_yards;		// total pass/rush/return yards
	int 		sacks;
	int 		consecutive_sacks;
	int 		sack_yards;
    int         consecutive_comps;
    int         last_receiver;
    int         bobbles;
    int         deflections;
    int         forthdwn_attempts;
    int         forthdwn_convs;
	int			off_plays;			// # of offensive plays
} tdata_t;

//
// Game info struct
//
typedef struct gameinfo
{
    int		game_state;			// GS_???, defined below
    int		game_quarter;		// 1-MAX_QUARTERS
    int		game_time;			// secs remaining in period   (mins, seconds, and 10ths of seconds) (each 8 bits)
    int		play_time;      	// # of clock seconds ticked off since play started
	int		play_pcount;		// pcount when play started
    int		team[2];			// TEAM_???, defined above
    int		scores[2][MAX_QUARTERS];		// scores by quarter
    int		home_team;			// 0 or 1
	int		first_pos;			// 0 or 1, team that started the game with possession
	int		off_side;			// which side has the ball (0 or 1)
	int		off_init;			// which side started current play with the ball (0 or 1)
	int		last_pos;			// last team to establish clear control of the ball (20+ ticks)
	int		off_turnover;		// Flag = turnover on downs just happened
	int		los;				// line of scrimmage, field_coords (based on off_init)
	int		old_los;			// previous line of scrimmage / point of most recent turnover
	float	line_pos;			// +HASHWIDTH -> -HASHWIDTH
	int		down;				// 1-4
	int		first_down;			// field position for first down, field coords (based on off_init)
	int		show_los;			// displayed los (-1 = off)
	int		show_fd;			// displayed first down line (-1 = off)
	int		game_mode;			// defined below
	play_t	*team_play[2];		// team plays called
	int		off_flip;			// TRUE if offense flipped the play
	int		ball_carrier;		// plyrnum with ball, (-1 is in flight)
	int		last_carrier;		// last plyrnum that had the ball
	int		plyr_control[4];	// which player is the station controlling? (-1 if none)
	int		captain_player[2];	// which player do the team captains control? (-1 if none)
	int		play_num[2];		// which play player chose on play select
	int		play_end_cause;		// how'd the play end?  Defined below.
	float	play_end_pos;		// coords of end of play, world coords
	float	play_end_z;			// coords of end of play, world coords (Z)
	int		play_type;			// PT_???, defined below
	int		last_play_type;		// PT_???, defined below - play just ended
	int		game_flags;			// GF_???, defined below
	int		play_result;		// PR_???, defined below
	int		play_result_flags;	// PRF_???, defined below
	float	fumble_x;			// world coords of point of last fumble
	float	catch_x;			// world coords of point of last catch (punt/pass/kick/fumble rec)
 	int 	ko_catch;			// yardline of point of last kickoff or punt recovery - updated one time
	int		auto_hike_cntr; 	// auto hike ball when cntr reaches 0
	int		disallow_pass_cntr;	// disallow passing after an auto hike
	int 	master_volume;		// for compatibility with bootys sound/coin stuff.
	int 	minimum_volume;
	int 	attract_mode_volume;
	int 	attract_coin_in;
	int		team_fire[2];	    // 1 = team on fire else 0
	int		team_handicap[2];	// 0=wimp, 5=normal, 10=super-tough
    int     spin_cnt[2];        // # times team spun per play
    int     hurdle_cnt[2];      // # times team hurdled per play
	int		just_got_1st;   	// 0 or 1 if last play resulted in player achieving a first down via a gain
	int		fumbles_on_play;	// total fumbles this play, either side
	int		team_head[4];		// special head to use for team (-1 = none)
	int		captain;			// which player on team is captain? (0 or 1)
	int		has_custom[4];		// does player have custom plays loaded?

	int		audible_flags[2];	// flags indicating an audible was called [offense/defense] (0 or 1)
	int		audible_num[2];		// which aud # currently on
	int		audible_plyr[2];	// which player # is calling them
} game_info_t;

extern game_info_t game_info;
extern game_info_t game_info_last;

extern tdata_t tdata_blocks[2];					// bookkeeping structure...for stats during gameplay per team
extern tdata_t tdata_blocks_last[2];				// bookkeeping structure...for stats during gameplay per team

extern pdata_t pdata_blocks[MAX_PLYRS];

extern int	bought_in_first;					// represents plyr on teams wh obought in first
extern int	p_status;							// represents current plyrs in game with bit pos. (0-3)
extern int	p_status2;							// represents previous plyrs in game with bit pos. (0-3)
extern int	winning_team;						// which set of players won last game (0=1&2, 1=3&4)
extern int	game_purchased;           			// set bit (0-4) for plyr which bought full game
extern int qtr_pstatus[MAX_PLYRS];				// human/drone by period.  bit0 = first qtr, bit6 = 3rd OT, 1=human, 0=drone
extern int pcount;								// process loop execution count
extern int	checksum;							// perm. snd bank checksum
extern int	violence;						// 0 = no violence, 1 = violence
extern int	swearing;						// 0 = no swearing, 1 = swearing
extern int	do_playtime;

extern char	*game_version;
extern int four_plr_ver;
extern int do_grand_champ;		// Flg to do grand champ screen
extern int record_me;
extern int clock_active;
extern char spfx_texture[12];

extern int tsec;

extern int	idiot_boxes_shown;

extern int drones_lose;		// set this flag and we'll stack the deck in
extern int drones_winning;	// every way possible to make sure that the drones lose, short of just changing the score.

extern char intro_bnk_str[];
extern char plyr_bnk_str[];
extern char generic_bnk_str[];
extern char eoq_tune_bnk_str[9];
extern char groove_bnk_str[];
extern char ancr_bnk_str[];
extern char ancr_swaped_bnk_str[9];	// swap between TKBANK1.BNK and TKBANK2.BNK
extern char cursor_bnk_str[9];			// swap between CURSORS.BNK and CURSOR2.BNK
extern char team1_bnk_str[9];
extern char team2_bnk_str[9];
extern char taunt_bnk_str[9];			// swap between TAUNT1.BNK and TAUNT2.BNK
extern char game_tune_bnk_str[9];
extern int show_plysel_idiot_box;
extern unsigned int tick_sndn;

extern char * taunt_bnk_strs[];
extern char * announcer_strs[];
extern char * eoq_tunes_strs[];

extern void				chk_game_proc_abort(void);
extern void				wipeout(void);
struct texture_node *	create_flash_texture(void);
extern int				check_scores(void);
extern int				compute_score(int tm_nbr);
extern int				wait_for_any_butn_press(int min_ticks, int wait_ticks);
extern void				wait_for_all_buttons( int min, int max );
extern void				main_load_fonts(void);
extern char *			get_ancr_str(char * dstr);
extern char *			get_taunt_str(char * dstr);
extern void				load_ingame_textures(void);
extern void				flash_obj_proc(int *args);
extern void				flash_text_proc(int *args);
extern void				flash_text_slow_proc(int *args);

#endif