#ifndef PLAYSTUF__H
#define PLAYSTUF__H

#include "/Video/Nfl/Include/Ani3d.h"	//	for LIMB

#define PLAYSTUF__AUDIBLES_PER_PLAYER		3
#define PLAYSTUF__NUM_AUDIBLE_PLAYS			PLAYSTUF__AUDIBLES_PER_PLAYER + 1	// +1 for the selected play

#define PLAYSTUF__MAX_PLAY_PAGES			5		//	maximum number of pages per side (offense/defense)
#define	PLAYSTUF__NUM_OFFENSE_PAGES			5
#define	PLAYSTUF__NUM_DEFENSE_PAGES			2
#define PLAYSTUF__PLAYS_PER_PAGE			9		//	maximum number of plays on one play page (now is 3x3 = 9 plays)

#define PLAYSTUF__OFF_PAGE_EXTRA			3		// offense page #: extra plays
#define PLAYSTUF__OFF_PAGE_ID_CUSTOM		4		// offense page #: custom page

#define PLAYSTUF__TWEAK_ID_NORMAL			0
#define PLAYSTUF__TWEAK_ID_FIELD_GOAL		1
#define PLAYSTUF__TWEAK_ID_PUNT				2
#define PLAYSTUF__TWEAK_ID_EXTRA_POINT		3

#define AUDIBLE_DOUBLE_TAP_TIME_LIMIT		10	//	time limit between tap 1 and tap 2 (in ticks) ... to get a double-tap for audibles

// drone script commands
#define DS_WAIT_SNAP		1
#define DS_FACE				2
#define DS_GOTO_LOS_PLUS	3
#define DS_RUN_YDS			4
#define DS_IMPROVISE		5
#define DS_COVER_TGT		6
#define DS_RUSH_QB			7
#define DS_BLOCK			8
#define DS_JUKE				9
#define DS_FAKE_OUT			10
#define DS_TURBO_ON			11
#define DS_TURBO_OFF		12
#define DS_WAVE_ARM 		13
#define DS_SPIN				14
#define DS_MOVE_DEFENDER	15
#define DS_WAIT				16
#define DS_ZONE				17
#define DS_SETFLAG			18
#define DS_COVER_WEAK		19
#define DS_DELAY_BLOCK		20

// zones
#define DZ_LEFT_THIRD		0
#define DZ_MIDDLE_THIRD		1
#define DZ_RIGHT_THIRD		2
#define DZ_LEFT_HALF		3
#define DZ_RIGHT_HALF		4
#define DZ_WIDE				5
#define DZ_EZ				-1

// control word flags
#define LU_RIGHT			0x001		// I am/line up over & cover rightmost receiver
#define LU_LEFT				0x002		// I am/line up over & cover leftmost receiver
#define LU_CENTER			0x004		// I am/line up over & cover center receiver
#define LU_POS_MASK			(LU_RIGHT|LU_CENTER|LU_LEFT)
#define LU_CAPN				0x008		// qb starts as me
#define	LU_PLYR2			0x010		// other off starts as me
#define LU_COVER			0x040		// cover guy, but don't line up over him
#define LU_WEAK				0x080		// flip in Z if most weapons are on left
#define LU_MOTION			0x100		// motion guy

// play flags
#define PLF_BLITZ			0x0001		// play is a blitz
#define PLF_PUNT			0x0002		// play is a punt
#define PLF_FIELD_GOAL		0x0004		// play is a field goal attempt
#define PLF_ONSIDE_KICK		0x0008		// play is an onside kick
#define PLF_FAKE_FG		    0x0010		// play is a fake field goal
#define PLF_FAKE_PUNT   	0x0020		// play is a fake punt
#define PLF_CUSTOM			0x0040		// play is custom-designed
#define PLF_PRI_RCVR1		0x0080		// primary rcvr 1
#define PLF_PRI_RCVR2		0x0100		// primary rcvr 2
#define PLF_PRI_RCVR3		0x0200		// primary rcvr 3
#define PLF_REDZONE			0x0400		// Indicates a goal line defensive play
#define PLF_LINEDIVE		0x0800		// Linemen Divable Quickly
#define PLF_FAKEZONE		0x1000		// Go into zone then man 2 man
#define PLF_CHEAPSHOT		0x2000		// Everyone cheapshot artist
#define PLF_SMART_INT		0x4000		// Smart Player Path Intercept
#define	PLF_TRYINT			0x8000		// Try to intercept more!

#define PLF_PRI_RCVRS		0x0380		// primary rcvrs mask

// formation element
typedef struct _fline
{
	float	x,z;
	char	*seq;
    int     mode;
	int		control;
} fline_t;

// play struct
typedef struct _play
{
	fline_t	*formation;
	int		flags;			// PLF_???, defined below
	int		*routes[7];
} play_t;

typedef struct play_aud_info
{
	play_t	*play;
	int		aud_num;
	int		off;
} play_aud_info_t;

typedef struct _PLAYSTUF__SLOT
{
	char *		name;
	play_t *	play_ptr;
	int			aud_num;
	void *		limb_ptrs[2];	//	[NORMAL/FLIPPED]
} PLAYSTUF__SLOT;

typedef struct _PLAYSTUF__PAGE
{
	PLAYSTUF__SLOT *	slot_ptrs[PLAYSTUF__PLAYS_PER_PAGE];
} PLAYSTUF__PAGE;


//	Globals (external)
extern PLAYSTUF__PAGE play_pages[2][PLAYSTUF__MAX_PLAY_PAGES];


//	Member access functions (external)
PLAYSTUF__SLOT *PlayStuf__GetSlot(int side, int page, int num);
play_t *		PlayStuf__GetAudiblePlay(int pnum, int off_def, int aud_num);
//void			PlayStuf__SetAudiblePlay(int pnum, int off_def, int aud_num, play_t *play_ptr);
int				PlayStuf__GetCurrentPlayPage(int side);
void			PlayStuf__SetCurrentPlayPage(int side, int page);


//	Normal functions (external)
void PlayStuf__ResetAudibles(void);
void PlayStuf__SetupCurrentPlayPages( int do_specials, int do_extra_pt );

#endif
