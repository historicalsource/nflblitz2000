#ifndef _PUP__H
#define _PUP__H

#define PUP_CODE_CNT	(sizeof(pupcodes)/sizeof(struct tagPUPCODE))

#define PUP_ICON_CNT	3
#define PUP_ICON_IMGCNT	(sizeof(pii_pup_icon)/sizeof(void *))

// control bits for powerups
#define PC_TRNY		0x1			// not allowed in tournament mode
#define PC_1P		0x2			// only works if one team is all-drone
#define PC_2P		0x4			// only works if humans are on both teams
#define PC_AGREE	0x8			// both teams must set
#define PC_TCANCEL	0x10		// cleared if two teammates both set
#define PC_4PONLY	0x20		// not available in 2-player kit

struct tagPUPCODE {
	int    code;
	int    stick;
	int	   control;
	int *  flag;
	char * text1;
	char * text2;
};

extern int	pup_violence;					// Set for violent moves

// Stick combo when exitting team select
extern int pup_field;						// 0:newgrass,1:oldgrass,2:turf,3:dirt,4:snow
extern int pup_stadium;						// 0:day,1:night,2:snow,3:NO stadium
extern int pup_showaudibles;				// Set to show audible names
extern int pup_playbook;					// 0:extra plays,1:default,2:no custom

// RIGHT stick
extern int pup_pullback;					//R1 Set for camera to pull back further
extern int pup_bighead;						//R2 Set for player to have a big head
extern int pup_bigheadteam;					//R3 Set for team to have big heads
extern int pup_bigplyrs;					//R4 Set for team to have big players
extern int pup_babyplyrs;					//R5 Set for team to have little players
extern int pup_bigball;						//R6 Set for oversize football
extern int pup_rcvrname;					//R7 Set to show receiver name on catch
extern int pup_rain;						//R8 Set to have rain
extern int pup_altstadium;					//R9 Set for alternate stadium
extern int pup_always_receiver;				//R10 Player is never captain on offense

// DOWN stick (all rev 1.0 is here)
extern int pup_fog;							//D1 Set for fog game
extern int pup_noassist;					//D2 Set for no computer assistance
extern int pup_nofumbles;					//D3 Set to eliminate fumbles (except onside kicks and spin abuse)
extern int pup_superdrones;					//D4 Set drone team to be super-unbeatable
extern int pup_showfgper;					//D5 Set to show fg percentage
extern int pup_tourney;						//D6 Set for tournament mode
extern int pup_thickfog;					//D7 Set for thick fog
extern int pup_groundfog;					//D8 Set for ground fog
extern int pup_secret_page;					//D9 Set for secret page of offensive plays (old plays from blitz)
extern int pup_notarget;					//D10 Set for no hiliting target receiver

// UP stick
extern int pup_offense;						//U1 Set for powerup offense
extern int pup_defense;						//U2 Set for powerup defense
extern int pup_drones;						//U3 Set for powerup drones
extern int pup_blitzers;					//U4 Set for super blitzers
extern int pup_infturbo;					//U5 Set for infinite turbo
extern int pup_nopunt;						//U6 Set for no punts
extern int pup_nointercept;					//U7 Set for no extern interceptions
extern int pup_no1stdown;					//U8 Set for no 1st downs
extern int pup_hugehead;					//U9 Set for player to have a huge head
extern int pup_noalways;					//U10 Set to cancel any all-time qb/wr on team
#ifdef Q_GAME
extern int pup_qgame;						//U11 Set for quick game
#endif

// LEFT stick
extern int pup_nostadium;					//L1 Set for no stadium
extern int pup_noplaysel;					//L2 Set for no play select screen
extern int pup_fastpass;					//L3 Set to always have fast passes
extern int pup_fastturborun;				//L4 Set to have turbo run go faster
extern int pup_speed;						//L5 Set for powerup speed (players run faster)
extern int pup_block;						//L6 Set for powerup blocking
extern int pup_noshowqb;					//L7 Set to not show QB in MODE_QB
extern int pup_runob;						//L8 Set to have player run as well as jump OB
extern int pup_superfg;						//L9 Set for no range limit field goals
extern int pup_always_qb;					//L10 Player is always captain on offense
extern int pup_perfect_fg;					//L11 Set for perfect field goals every time

extern void clear_tourney_pups(void);

#endif