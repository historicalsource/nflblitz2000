#ifndef CAP__H
#define CAP__H

#include "/Video/Nfl/Include/PlayStuf.h"

// cap_point_t flags
#define CPPT_JUKE		0x1
#define CPPT_SPIN		0x2
#define CPPT_TURBO		0x4
#define CPPT_LAST		0x8
#define CPPT_JST_MASK	(CPPT_JUKE|CPPT_SPIN|CPPT_TURBO)

// how many of each allowed in a play
#define CAP_JUKE_MAX	3
#define CAP_SPIN_MAX	3
#define CAP_TURBO_MAX	3

// max control points (counting base node)
#define CP_COUNT		9
#define CP_NAMELEN		9

// cap_play_t flags
#define CPLT_EXISTS		0x01		// play exists
#define CPLT_QB_FAR		0x02		// shotgun (near by default)
#define CPLT_PROTECT	0x04		// can't edit play
#define CPLT_PRI_RCVR1	0x08		// primary rcvr 1
#define CPLT_PRI_RCVR2	0x10		// primary rcvr 2
#define CPLT_PRI_RCVR3	0x20		// primary rcvr 3
#define CPLT_UNUSED		0x40		// play available but unused

#define CPLT_PRI_RCVRS	0x38		// primary rcvrs mask
#define CPLT_VALID		0X7f		// valid flags mask

// how many blocks on the drive for a record
#define HD_RECORD_BLOCKS	2

/////////////////////////////////////////////////////////////////////////////
// route flags
#define RF_DIM			0x1			/* dimmed in display */
#define RF_ACTIVE		0x2			/* being edited */
#define RF_PRIMARY		0x4			/* primary receiver being selected */

/////////////////////////////////////////////////////////////////////////////
// define all Zs here

#define CAP_BACK_Z			500.0f

#define LCB_BUTTON_Z		10.0f
#define LCB_TEXT_Z			3.4f
#define LCB_Y_TOP			321.0f	//307.0f
#define LCB_Y_INC			27.0f	//29.0f	//26.0f
#define LCB_X				105.0f
#define TIMER_X				110.0f
#define TIMER_Y				57.0f
#define TIMER_Z				2.0f

#define HELP_TEXT_X			362.0f
#define HELP_TEXT_Y1		362.0f
#define HELP_TEXT_Y15		356.0f
#define HELP_TEXT_Y2		350.0f
#define HELP_TEXT_Z			5.0f

#define PLAY_NAME_X			362.0f
#define PLAY_NAME_Y			328.0f

#define	PLAY_NAME_BOX_X		362.0f
#define	PLAY_NAME_BOX_Y		315.0f
#define	PLAY_NAME_BOX_Z		5.0f

#define	FORM_BOX_X			362.0f
#define	FORM_BOX_Y			210.0f
#define	FORM_BOX_Z			5.0f

#define	FORM_SEL_BOX_X		362.0f
#define	FORM_SEL_BOX_Y		140.0f
#define	FORM_SEL_BOX_Z		5.0f
#define	FORM_SEL_TXT_Z		3.0f

#define	FORM_CUR_Z			4.0f
#define	FORM_CUR_Y			FORM_SEL_BOX_Y+30.0f
#define	FORM_CUR_YINC		15.0f


#define	QB_INFO_BOX_X		362.0f
#define	QB_INFO_BOX_Y		190.0f
#define	QB_INFO_BOX_Z		5.0f

#define	QB_BOX_X			362.0f
#define	QB_BOX_Y			140.0f
#define	QB_BOX_Z			5.0f
#define	QB_TXT_Z			3.0f

#define	QB_CUR_Z			4.0f
#define	QB_CUR_Y			QB_BOX_Y+8.0f
#define	QB_CUR_YINC			15.0f


#define NO_ACTIVE_X			364.0f
#define NO_ACTIVE_Y			162.0f
#define NO_ACTIVE_Z			5.0f
#define NO_ACTIVE_Z2		4.9f
#define NO_ACTIVE_Z3		4.8f
#define PD_LINE_Z			6.0f

#define YESNO_X				364.0f //256.0f
#define YESNO_Y				192.0f
#define YESNO_Z				1.6f
#define YESNO_Z2			1.5f
#define YESNO_LINE1_Y		230.0f
#define YESNO_LINE15_Y		214.0f
#define YESNO_LINE2_Y		208.0f
#define YESNO_LINE25_Y		202.0f
#define YESNO_LINE3_Y		196.0f

#define NAME_Z				4.5f
#define NAME_CURSOR_Z		4.4f
#define NAME_X				364.0f
#define NAME_Y				182.0f
#define NAME_TEXT_X			364.0f
#define NAME_TEXT_Z			2.0f
#define NAME_TEXT_Y1		38.0f
#define NAME_TEXT_Y2		20.0f
#define BCKPLTE_Y			NAME_Y-20.0f

#define PIN_X				364.0f
#define PIN_Y				142.0f
#define PIN_Z				4.5f

#define ER_OVERLAY_X		0.0f
#define ER_OVERLAY_Y		384.0f
#define ER_OVERLAY_Z		(CAP_BACK_Z-10.0f)

#define ER_LINEMAN_Z		11.0f
#define ER_GRAYQB_Z			10.5
#define ER_QB_Z				9.4

#define ER_GRAYLINE_Z		10.0f
#define ER_GRAYARROW_Z		12.0f
#define ER_GRAYNODE_Z		9.8f
#define ER_GRAYROOT_Z		9.7f

#define ER_LINE_Z		9.6f
#define ER_ARROW_Z		12.0f
#define ER_NODE_Z		9.5f
#define ER_ROOT_Z		9.3f

#define ER_MENU_BAK_Z		4.3f
#define ER_MENU_HILITE_Z	4.2f
#define ER_MENU_TEXT_Z		4.1f
#define ER_MENU_OVERLAY_Z	4.0f
#define ER_MENU_DIGIT_Z		3.9f

#define ER_HELP_BOX_Z		2.5f

#define ER_HELP_BOX1_X		365.0f
#define ER_HELP_BOX1_Y		135.0f

#define ER_HELP_BOX2_X		325.0f
#define ER_HELP_BOX2_Y		155.0f

#define ER_HELP_BOX3_Y		3.0f
#define ER_HELP_BOX3_Y		3.0f

#define AUD_BOX_XR			365
#define AUD_BOX_YR			189

#define AUD_BOX_XL			114
#define AUD_BOX_YL			227

/////////////////////////////////////////////////////////////////////////////
// grid-to-screen conversion macros
//#define GY2W(x)				(8.0f+7.0f*(x))
#define GX2W(x)				(224.0f+7.0f*(x))
#define GY2W(x)				(is_low_res ? (4.0f + 5.0f*(x)) : (8.0f + 7.0f*(x)))
#define GY2WNS(x)			(8.0f + (is_low_res ? 7.4f*(x) : 7.0f*(x)))

// grid-to-formation conversion macros
// ten grid squares is 11 yards
// one yard is 7.8 units
// so one grid square is 7.09 units
#define GY2FX(y)			(((float)((y)-10)) * 7.09f)
#define GX2FZ(x)			(((float)((x)-20)) * 7.09f)

/////////////////////////////////////////////////////////////////////////////
// grid motion limits
#define GRID_MIN_X			1
#define GRID_MAX_X			39
#define GRID_MIN_Y			1
#define GRID_MAX_Y			43
#define GRID_LOS_Y			8
#define REC_START_MIN_X		8
#define REC_START_MAX_X		32

/////////////////////////////////////////////////////////////////////////////
// main button bar configuration
//#define LCB_COUNT		8		/* how many left-side buttons */
//#define LCB_GRAY_TOP	2		/* index of first button gray when no play */
//#define LCB_GRAY_BOT	6		/*          last                           */

#define LCB_COUNT		9		/* how many left-side buttons */
#define LCB_GRAY_TOP	1		/* index of first button gray when no play */
#define LCB_GRAY_BOT	6		/*          last                           */

#define LCB_REPEAT_TIME	28
#define LCB_REPEAT_INC	7

/////////////////////////////////////////////////////////////////////////////
// miscellaneous IDs
#define CAP_TIMER_TID	CAP_TID+1
#define CAP_BACK_TID	CAP_TID+2

#define HELP_TEXT_SID	0x1000
#define TIMER_SID		0x1010
#define NO_ACTIVE_SID	0x1020
#define YESNO_TEXT_SID	0x1030
#define NAME_TEXT_SID	0x1040
#define CHOOSE_PLAY_SID	0x1050
#define	INFO_SID		0x1060

#define OID_CAP_PDISPLAY			(OID_CAP+1)
#define OID_CAP_YESNO				(OID_CAP+2)
#define OID_ER_MENU					(OID_CAP+3)
#define OID_ER_OVERLAY				(OID_CAP+4)
#define OID_CAP_NAME				(OID_CAP+5)
#define OID_CAP_CREATE				(OID_CAP+6)
#define OID_CAP_BOX					(OID_CAP+7)
#define OID_CAP_BACK				(OID_CAP+8)
#define OID_CAP_AUDIBLE_CURSOR		(OID_CAP+9)

#define CAP_TIMER_PID		(CREATE_PLAY_PID+1)
#define DISPLAY_BLINKER_PID	(CREATE_PLAY_PID+2)

/////////////////////////////////////////////////////////////////////////////
// control point menu results
#define ER_ACTION_ABORT		-1
#define ER_ACTION_NONE		0
#define ER_ACTION_JUKE		1
#define ER_ACTION_SPIN		2
#define ER_ACTION_TURBO		3
#define ER_ACTION_DELETE	4
#define ER_ACTION_DONE		5

// control point menu misc
#define ER_MENU_LENGTH		6
#define ER_MENU_ITEM_HEIGHT		15.0f
#define ER_MENU_ITEM_HEIGHT_LR	12.0f
#define ER_STICK_REPEAT_TIME	1//2
//#define ER_STICK_REPEAT_TIME	15
#define ER_STICK_REPEAT_RATE	2//1

#define NAME_REPEAT_TIME	15
#define NAME_REPEAT_RATE	4

/////////////////////////////////////////////////////////////////////////////
// partition stuff

typedef struct partition_info
{
	int	starting_block;				// Logical starting block number
	int	num_blocks;						// Number of blocks in partition
	int	partition_type;				// Type of partition
} partition_info_t;

typedef struct partition_table
{
	int					magic_number; 		// The magic number 'PART'
	int					num_partitions;	// Number of partitions
	partition_info_t	partition[64];		// Partition info
} partition_table_t;

// x is always non-zero for valid points
// So that's how we tell the difference
// between a point that's being set for
// the first time and an old point that's
// being edited.
typedef struct _cap_point_t
{
	unsigned char	x;
	unsigned char	y;
	unsigned char	flags;
} cap_point_t;

typedef struct _cap_line_t
{
	int				x1,y1;
	int				x2,y2;
	float			z;
	int				color;
} cap_line_t;

typedef struct _cap_play_t
{
	char			name1[CP_NAMELEN];
	char			name2[CP_NAMELEN];
	unsigned char	motion;
	unsigned char	flags;
	cap_point_t		wpn[3][CP_COUNT];
} cap_play_t;

typedef struct _audible_ids_t
{
	signed short	ids[2][PLAYSTUF__AUDIBLES_PER_PLAYER];
} audible_ids_t;

typedef struct _cap_block_t
{
	unsigned long	crc;				// __attribute__((packed));
	cap_play_t		custom_plays[9];	// __attribute__((packed));
	audible_ids_t	audible_ids;		// __attribute__((packed));
} cap_block_t;

// This is mainly so Diagnostics can compile in debug mode
#ifndef MAX_PLYRS			
#define	MAX_PLYRS    4						// # of human players supported
#endif

extern play_t			custom_play;
extern cap_play_t		custom_plays[MAX_PLYRS][9];
extern cap_play_t *		convert_play_last;
extern audible_ids_t	audible_ids[MAX_PLYRS];

extern void convert_play(cap_play_t *pplay);
extern void create_play_proc(int *);

#endif