#include "/Video/Include/Stdlib.h"
#include "/Video/Include/String.h"
#include "/Video/Nfl/Include/Game.h"
#include "/Video/Nfl/Include/Id.h"
#include "/Video/Nfl/Include/SndCalls.h"
#include "/Video/Nfl/Include/FontId.h"
#include "/Video/Nfl/Include/Audits.h"
#include "/Video/Nfl/Include/Player.h"
#include "/Video/Nfl/Include/select.h"

#include "/Video/Nfl/Include/plyrinfo.h"

extern font_info_t	bast7t_font;
extern font_info_t	bast8t_font;
extern font_info_t	bast10_font;
extern font_info_t	bast13_font;
extern font_info_t	bast18_font;
extern font_info_t	bast23_font;
extern font_info_t	bast25_font;
extern font_info_t	clock12_font;
extern font_node_t * font_list;
extern char msg_tdn_tex_name[];
extern char msg_4dn_tex_name[];
extern char msg_1dn_tex_name[];
extern char msg_trn_tex_name[];
extern char msg_int_tex_name[];
extern char flash_tex_name[];


#if DEBUG
//char	*game_version = "BETA 5 DEBUG ONLY";
//char	*game_version = "BETA DUAL BOOT DEBUG ONLY";
//char	*game_version = "DUAL BOOT DEBUG ONLY";
char	*game_version = "B2K DEBUG TEST 0.99 ";
#else
// release version beta string exact same length as final release string!
//char	*game_version = "BLITZ 99 1.17F";
//char	*game_version = "BLITZ 99 1.30 ";
//char	*game_version = "BLITZ 99 1.31 ";
//char	*game_version = "BLITZ 99 1.32 ";
//char	*game_version = "BLITZ 99 1.41 ";		/* the NBA On NBC/Blitz 99 dual boot version */
//char	*game_version = "B2K TEST 0.99 ";
//char	*game_version = "B2K   1.5 TEST";		/* the NBA On NBC/Blitz 99 dual boot version */
char	*game_version = "BLITZ 2000 1.5";		/* the NBA On NBC/Blitz 99 dual boot version */
#endif

// create the INFO structures for the game & last game played
game_info_t game_info;
game_info_t game_info_last;

tdata_t tdata_blocks[2];					// bookkeeping structure...for stats during gameplay per team
tdata_t tdata_blocks_last[2];				// bookkeeping structure...for stats during gameplay per team

pdata_t pdata_blocks[MAX_PLYRS];

int	bought_in_first;					// represents plyr on teams wh obought in first
int	p_status;							// represents current plyrs in game with bit pos. (0-3)
int	p_status2;							// represents previous plyrs in game with bit pos. (0-3)
int	winning_team;						// which set of players won last game (0=1&2, 1=3&4)
int	game_purchased;           			// set bit (0-4) for plyr which bought full game
int qtr_pstatus[MAX_PLYRS];				// human/drone by period.  bit0 = first qtr, bit6 = 3rd OT, 1=human, 0=drone
int pcount;								// process loop execution count
//int	checksum;							// perm. snd bank checksum
int	violence = 0;						// 0 = no violence, 1 = violence
//int	swearing = 0;						// 0 = no swearing, 1 = swearing
//int	do_playtime = 0;
int show_plysel_idiot_box = 0;
unsigned int tick_sndn;

// Number of ticks per second
int	tsec = 100;

//  0= none shown
// -1= showing instructions box
//  1= boxes were shown
int	idiot_boxes_shown = 0;

int drones_lose;		// set this flag and we'll stack the deck in
int drones_winning;		// every way possible to make sure that the
						// drones lose, short of just changing the score.

char intro_bnk_str[] = "intro";
char plyr_bnk_str[] = "players";
char generic_bnk_str[] = "generic";
char eoq_tune_bnk_str[9];
//char groove_bnk_str[] = "halftime";
char ancr_bnk_str[] = "tkperm";
char ancr_swaped_bnk_str[9];	// swap between TKBANK1.BNK and TKBANK2.BNK
char cursor_bnk_str[9];			// swap between CURSORS.BNK and CURSOR2.BNK
char team1_bnk_str[9];
char team2_bnk_str[9];
char taunt_bnk_str[9];			// swap between TAUNT1.BNK and TAUNT2.BNK
char game_tune_bnk_str[9];

char * taunt_bnk_strs[] = {"taunts3","taunts4"};
char * announcer_strs[] = {"tkbank3","tkbank4"};
char * eoq_tunes_strs[] = {"buyin", "halftime", "buyin", "buyin", "buyin", "buyin", "buyin"};

int four_plr_ver = 1;		// 0 = 2 plyrs, 1 = 4 plyrs  (0 = low res, 1 = med. res)
int record_me;									// Flag bits of players to record to cmos
int do_grand_champ;								// Flg to do grand champ screen
int clock_active;
char spfx_texture[12];					// name of texture.

// Every wipeout calls main_load_fonts
// Used for attract mode
static font_load_info_t	mid_res_fonts[] = {
{&bast7t_font,  FONTID_BAST7T},
{&bast8t_font,  FONTID_BAST8T},
{&bast10_font,  FONTID_BAST10},
{&clock12_font, FONTID_CLOCK12},
{&bast18_font,  FONTID_BAST18},
{&bast23_font,  FONTID_BAST23},
{&bast25_font,  FONTID_BAST25},
{NULL, 0}
};

// Used for attract mode
static font_load_info_t	low_res_fonts[] = {
{&bast7t_font,  FONTID_BAST7T},
{&bast8t_font,  FONTID_BAST8T},
{&bast8t_font,  FONTID_BAST10},
{&bast10_font,  FONTID_BAST10LOW},
{&clock12_font, FONTID_CLOCK12},
{&bast13_font,  FONTID_BAST18},
{&bast18_font,  FONTID_BAST23},
{&bast25_font,  FONTID_BAST23LOW},
{&bast18_font,  FONTID_BAST25},
{NULL, 0}
};

// Used for in game stuff
static font_load_info_t	mid_res_fonts2[] = {
{&bast7t_font,  FONTID_BAST7T},
{&bast8t_font,  FONTID_BAST8T},
{&bast10_font,  FONTID_BAST10},
{&clock12_font, FONTID_CLOCK12},
{&bast18_font,  FONTID_BAST18},
{&bast25_font,  FONTID_BAST25},
{NULL, 0}
};

// Used for in game stuff
static font_load_info_t	low_res_fonts2[] = {
{&bast7t_font,  FONTID_BAST7T},
{&bast8t_font,  FONTID_BAST8T},
{&bast8t_font,  FONTID_BAST10},
{&bast10_font,  FONTID_BAST10LOW},
{&clock12_font, FONTID_CLOCK12},
{&bast13_font,  FONTID_BAST18},
{&bast18_font,  FONTID_BAST23},
{&bast25_font,  FONTID_BAST23LOW},
{&bast18_font,  FONTID_BAST25},
{NULL, 0}
};

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
char * get_ancr_str(char * dstr)
{
	return(strcpy(dstr, announcer_strs[randrng(sizeof(announcer_strs)/sizeof(char *))]));
}

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
char * get_taunt_str(char * dstr)
{
	return(strcpy(dstr, taunt_bnk_strs[randrng(sizeof(taunt_bnk_strs)/sizeof(char *))]));
}

/****************************************************************************/
void chk_game_proc_abort(void)
{
	if (game_info.attract_coin_in)
	{
		game_info.attract_coin_in = 0;
#if defined(SEATTLE)
		install_disk_info((void **)NULL, 0, 0);
#else
		dio_mode(DIO_MODE_BLOCK);
#endif
		die(0);
	}
}

void wipeout(void)
{
	delete_all_sprites();
	delete_all_strings();
	delete_all_background_sprites();
	clear_object_list();
	clear_texture_list(TEXTURES_ALL);
	delete_fonts();
	killall( 0, 0 );						// kill all processes except INDESTRUCTABLES!!!!
	guTexMemReset();
	main_load_fonts();
}

//---------------------------------------------------------------------------------------------------------------------------
// Load flash texture
// 128 x 128 - used on messages, etc.
//---------------------------------------------------------------------------------------------------------------------------
struct texture_node *create_flash_texture(void)
{
	return create_texture(flash_tex_name, SCR_PLATE_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
}

//---------------------------------------------------------------------------------------------------------------------------
//					This routine compares the players scores
//
// RETURNS: FALSE if equal, else TRUE
//---------------------------------------------------------------------------------------------------------------------------
int check_scores(void)
{
	return !!(compute_score(0) - compute_score(1));
}

//---------------------------------------------------------------------------------------------------------------------------
//					This routine compares the players scores
//
// RETURNS: score
//---------------------------------------------------------------------------------------------------------------------------
int compute_score(int tm_nbr)
{
int scr, i;

	scr = 0;
	for (i=0; i < MAX_QUARTERS; i++)
		scr += game_info.scores[tm_nbr][i];
	return(scr);
}

//-------------------------------------------------------------------------------------------------
// Wait for a minimum # of ticks, then read buttons & joystick for x ticks then return
//  rtns: 0 if timed-out, !0 if buttoned-out
//-------------------------------------------------------------------------------------------------
int wait_for_any_butn_press(int min_ticks, int wait_ticks)
{
	int switches = 0;
	
	while (min_ticks--) sleep (1);

	while (wait_ticks--)
	{
		switches = get_player_sw_current();
		if (switches & ALL_ABCD_MASK)
		{
			if (!show_plysel_idiot_box)
				snd_scall_direct(tick_sndn,VOLUME1,127,1);
				//snd_scall_bank(cursor_bnk_str,TICK_SND,VOLUME1,127,1);
			break;
		}
		// Clr 'cuz no valid buttons were hit
		switches = 0;
		// Sleep here so we don't follow the <min_tick> sleep
		sleep (1);
	}
	return switches;
}

//---------------------------------------------------------------------------------------------------------------------------
// Look for all 3 butns to be pressed for 5 ticks before breaking out
//---------------------------------------------------------------------------------------------------------------------------
void wait_for_all_buttons( int min, int max )
{
	int	i;
	int ticks = 0;
	int	times[4];
	int time1 = 0;
	int time2 = 0;
	int time3 = 0;
	int time4 = 0;
	int	done = FALSE;
	int switches;

	do
	{
		sleep(1);
		ticks++;

		if (ticks < min)
			continue;

		for( i = 0; i < 4; i++ )
		{
			if (p_status & (1<<i))
			{
				switches = (get_player_sw_current() >> (8*i)) & P_ABCD_MASK;
				if (switches == (P_A|P_B|P_C))
				{
					if (++times[i] >= 8)
					{
						snd_scall_bank(plyr_bnk_str,RICOCHET_SND,VOLUME2,127,PRIORITY_LVL5);
						done = TRUE;
					}
				}
				else
					times[i] = 0;
			}
		}

	} while( (done == FALSE) && (ticks < max));
}

void main_load_fonts(void)
{
	register font_load_info_t * pfli;
	register font_node_t * pfnode;
	register font_node_t * pfn;
	register unsigned int failed;

	// Load attract or game fonts?
	if (game_info.game_state == GS_ATTRACT_MODE ||
			game_info.game_state == GS_DIAGNOSTICS)
		pfli = is_low_res ? low_res_fonts : mid_res_fonts;
	else
		pfli = is_low_res ? low_res_fonts2 : mid_res_fonts2;

	// Walk the font load list and load all fonts
	pfnode = NULL;
	failed = 0;
	while (pfli->font_info)
	{
		// Create the font
		if (!(pfn = create_font(pfli->font_info, pfli->id)))
		{
#ifdef DEBUG
			fprintf(stderr,"main_load_fonts(): create_font() failed ID# %d\r\n", pfli->id);
			lockup();
#endif
			failed++;
		}
		else
		{
			// Save ptr to the 1st one created (smallest per table order)
			if (!pfnode) pfnode = pfn;
			//fprintf(stderr,"made ID# %d\r\n", pfli->id);
		}
		failed <<= 1;
		pfli++;
	}
	// Fail completely if couldn't create any of them
	if (!pfnode)
	{
#ifdef DEBUG
		fprintf(stderr,"main_load_fonts(): all create_font() calls failed -- aborting\r\n");
		lockup();
#endif
		while (1);
	}

	// Try fix it if any one of them failed
	while (failed)
	{
		// Go to the next one that failed
		while (!(failed & 1))
		{
			failed >>= 1;
			pfli--;
		}
#ifdef DEBUG
		fprintf(stderr,"main_load_fonts(): failed %X\r\n", failed);
#endif
		// Allocate memory for the font node; abort if fails
		if (!(pfn = (font_node_t *)malloc(sizeof(font_node_t))))
		{
#ifdef DEBUG
			fprintf(stderr,"main_load_fonts(): Can not allocate memory for font node -- aborting\r\n");
			lockup();
#endif
			while(1);
		}
		// Got a node; default it to the 1st one created but with the new ID
		*pfn = *pfnode;
		pfn->id = pfli->id;

		// Link the new node to the font list
		pfn->prev = NULL;
		pfn->next = font_list;
		if (font_list) font_list->prev = pfn;
		font_list = pfn;

		// Clr current failure bit
		failed &= ~1;
	}
}

//---------------------------------------------------------------------------------------------------------------------------
// Load general textures needed during gameplay
//---------------------------------------------------------------------------------------------------------------------------
void load_ingame_textures(void)
{
	struct texture_node	* tn0;
	struct texture_node	* tn1;
	struct texture_node	* tn2;
	struct texture_node	* tn3;
	struct texture_node	* tn4;
	struct texture_node	* tn5;
	struct texture_node * tn6;

	char pnames[] = "???nms.wms";

	////////////
	// 256 x 256
	if (!(create_texture(firebr_tex_name, SCR_PLATE_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR)))
	{
#ifdef DEBUG
		fprintf( stderr,"Error loading %s\r\n",firebr_tex_name);
#endif
		increment_audit(TEX_LOAD_FAIL_AUD);
	}
#ifdef DEBUG
	else
		fprintf( stderr,"Loaded %s\r\n",firebr_tex_name);
#endif
	chk_game_proc_abort();

	////////////
	// 256 x 128
	if (!(create_texture(msg_tdn_tex_name, MESSAGE_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR)))
	{
#ifdef DEBUG
		fprintf( stderr,"Error loading %s\r\n",msg_tdn_tex_name);
#endif
		increment_audit(TEX_LOAD_FAIL_AUD);
	}
#ifdef DEBUG
	else
		fprintf( stderr,"Loaded %s\r\n",msg_tdn_tex_name);
#endif
	chk_game_proc_abort();

	////////////
	// 256 x 128
	if (!(create_texture(msg_trn_tex_name, MESSAGE_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR)))
	{
#ifdef DEBUG
		fprintf( stderr,"Error loading %s\r\n",msg_trn_tex_name);
#endif
		increment_audit(TEX_LOAD_FAIL_AUD);
	}
#ifdef DEBUG
	else
		fprintf( stderr,"Loaded %s\r\n",msg_trn_tex_name);
#endif
	chk_game_proc_abort();

	////////////
	// 256 x 128
	if (!(create_texture(msg_1dn_tex_name, MESSAGE_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR)))
	{
#ifdef DEBUG
		fprintf( stderr,"Error loading %s\r\n",msg_1dn_tex_name);
#endif
		increment_audit(TEX_LOAD_FAIL_AUD);
	}
#ifdef DEBUG
	else
		fprintf( stderr,"Loaded %s\r\n",msg_1dn_tex_name);
#endif
	chk_game_proc_abort();

	////////////
	// 256 x 128
	if (!(create_texture(msg_int_tex_name, MESSAGE_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR)))
	{
#ifdef DEBUG
		fprintf( stderr,"Error loading %s\r\n",msg_int_tex_name);
#endif
		increment_audit(TEX_LOAD_FAIL_AUD);
	}
#ifdef DEBUG
	else
		fprintf( stderr,"Loaded %s\r\n",msg_int_tex_name);
#endif
	chk_game_proc_abort();

	////////////
	// This one texture is loaded after the VS screen at start of game,
	// but for each new quarter, it needs to get reloaded;
	// also need to load it if in attract since VS didn't even happen
	if (game_info.game_quarter || game_info.game_state == GS_ATTRACT_MODE)
	{
		// 256 x 128
		if (!(create_texture(msg_4dn_tex_name, MESSAGE_TID, 0, CREATE_NORMAL_TEXTURE,
				GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP,
				GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR)))
		{
#ifdef DEBUG
			fprintf( stderr,"Error loading %s\r\n",msg_4dn_tex_name);
#endif
			increment_audit(TEX_LOAD_FAIL_AUD);
		}
#ifdef DEBUG
		else
			fprintf( stderr,"Loaded %s\r\n",msg_4dn_tex_name);
#endif
	}

	////////////
	// 256 x 256
	if (!(tn0 = create_texture(ingame00_tex_name, SCR_PLATE_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR)))
	{
#ifdef DEBUG
		fprintf( stderr,"Error loading %s\r\n",tn0->texture_name);
#endif
		increment_audit(TEX_LOAD_FAIL_AUD);
	}
#ifdef DEBUG
	else
		fprintf( stderr,"Loaded %s\r\n",tn0->texture_name);
#endif
	chk_game_proc_abort();

	////////////
	// 256 x 256
	if (!(tn1 = create_texture(ingame01_tex_name, SCR_PLATE_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR)))
	{
#ifdef DEBUG
		fprintf( stderr,"Error loading %s\r\n",tn1->texture_name);
#endif
		increment_audit(TEX_LOAD_FAIL_AUD);
	}
#ifdef DEBUG
	else
		fprintf( stderr,"Loaded %s\r\n",tn1->texture_name);
#endif
	chk_game_proc_abort();

	////////////
	// 256 x 256
	if (!(tn2 = create_texture(ingame02_tex_name, SCR_PLATE_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP,
			GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR)))
	{
#ifdef DEBUG
		fprintf( stderr,"Error loading %s\r\n",tn2->texture_name);
#endif
		increment_audit(TEX_LOAD_FAIL_AUD);
	}
#ifdef DEBUG
	else
		fprintf( stderr,"Loaded %s\r\n",tn2->texture_name);
#endif
	chk_game_proc_abort();

	////////////
	// These are already loaded & locked on the VS page; redo here
	//  for the between qtr flush & reload
	// 128 x 128 - used on messages, etc.
	if (!(tn3 = create_flash_texture()))
	{
#ifdef DEBUG
		fprintf( stderr,"Error loading %s\r\n",tn3->texture_name);
#endif
		increment_audit(TEX_LOAD_FAIL_AUD);
	}
#ifdef DEBUG
	else
		fprintf( stderr,"Loaded %s\r\n",tn3->texture_name);
#endif
	chk_game_proc_abort();

	////////////
	// 64 x 64 - head/offscrn arrows
	if (!(tn4 = create_texture(arrows_tex_name, SCR_PLATE_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP,
			GR_TEXTUREFILTER_POINT_SAMPLED, GR_TEXTUREFILTER_POINT_SAMPLED)))
	{
#ifdef DEBUG
		fprintf( stderr,"Error loading %s\r\n",tn4->texture_name);
#endif
		increment_audit(TEX_LOAD_FAIL_AUD);
	}
#ifdef DEBUG
	else
		fprintf( stderr,"Loaded %s\r\n",tn4->texture_name);
#endif
	chk_game_proc_abort();

	////////////
	// 256 x 32 - player names
	pnames[0] = teaminfo[game_info.team[0]].prefix[0];
	pnames[1] = teaminfo[game_info.team[0]].prefix[1];
	pnames[2] = teaminfo[game_info.team[0]].prefix[2];
	if (!(tn5 = create_texture(pnames, SCR_PLATE_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP,
			GR_TEXTUREFILTER_POINT_SAMPLED, GR_TEXTUREFILTER_POINT_SAMPLED)))
	{
#ifdef DEBUG
		fprintf( stderr,"Error loading %s\r\n",tn5->texture_name);
#endif
		increment_audit(TEX_LOAD_FAIL_AUD);
	}
#ifdef DEBUG
	else
		fprintf( stderr,"Loaded %s\r\n",tn5->texture_name);
#endif
	chk_game_proc_abort();

	////////////
	// 256 x 32 - player names
	pnames[0] = teaminfo[game_info.team[1]].prefix[0];
	pnames[1] = teaminfo[game_info.team[1]].prefix[1];
	pnames[2] = teaminfo[game_info.team[1]].prefix[2];
	if (!(tn6 = create_texture(pnames, SCR_PLATE_TID, 0, CREATE_NORMAL_TEXTURE,
			GR_TEXTURECLAMP_CLAMP, GR_TEXTURECLAMP_CLAMP,
			GR_TEXTUREFILTER_POINT_SAMPLED, GR_TEXTUREFILTER_POINT_SAMPLED)))
	{
#ifdef DEBUG
		fprintf( stderr,"Error loading %s\r\n",tn6->texture_name);
#endif
		increment_audit(TEX_LOAD_FAIL_AUD);
	}
#ifdef DEBUG
	else
		fprintf( stderr,"Loaded %s\r\n",tn6->texture_name);
#endif
	chk_game_proc_abort();


	////////////
	lock_multiple_textures(SCR_PLATE_TID,0xffffffff);
	lock_multiple_textures(MESSAGE_TID,0xffffffff);

#if 0
#ifdef DEBUG
	fprintf( stderr,
		"%.12s= %d  %.12s= %d  %.12s= %d\r\n"
		"%.12s= %d  %.12s= %d  %.12s= %d\r\n"
		"%.12s= %d\r\n",
		tn0->texture_name,tn0->texture_flags,
		tn1->texture_name,tn1->texture_flags,
		tn2->texture_name,tn2->texture_flags,
		tn3->texture_name,tn3->texture_flags,
		tn4->texture_name,tn4->texture_flags,
		tn5->texture_name,tn5->texture_flags,
		tn6->texture_name,tn6->texture_flags
		);
#endif
#endif
}

//-------------------------------------------------------------------------------------------------
//	This process flashes the object white
//
// 	INPUT:	Pointer to flash, cnt, delay
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void flash_obj_proc(int *args)
{ 
sprite_info_t *sp;
int cnt, delay;

	sp = (sprite_info_t *)args[0];
	cnt = args[1];
	delay = args[2];
	
	flash_obj_white(sp,cnt,delay);
}

//-------------------------------------------------------------------------------------------------
//	This process flashes 1-4 lines of text white then original color then white, etc.
//
// 	INPUT:	Strings to flash
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void flash_text_proc(int *args)
{ 
ostring_t	*pmsg[4];
int pmsg_org_color[4];
int i;

//sprite_info_t *sp;
//	sp = (sprite_info_t *)args[0];

	pmsg[0] = (ostring_t *)*args;
	// Other strings could be blank
	pmsg[1] = (ostring_t *)*(args+1);
	pmsg[2] = (ostring_t *)*(args+2);
	pmsg[3] = (ostring_t *)*(args+3);

	pmsg_org_color[0] = pmsg[0]->state.color;

	if (pmsg[1])
		pmsg_org_color[1] = pmsg[1]->state.color;
	if (pmsg[2])
		pmsg_org_color[2] = pmsg[2]->state.color;
	if (pmsg[3])
		pmsg_org_color[3] = pmsg[3]->state.color;
		
//	for (i=0; i < 100; i++)
	while(1)
	{
		sleep(7);
		pmsg[0]->state.color = WHITE;
//		change_string_state(pmsg[0]);
		change_string_cc( pmsg[0], WHITE );

		// Check for other strings
		if (pmsg[1])
		{
			pmsg[1]->state.color = WHITE;
//			change_string_state(pmsg[1]);
			change_string_cc( pmsg[1], WHITE );
		}
		if (pmsg[2])
		{
			pmsg[2]->state.color = WHITE;
//			change_string_state(pmsg[2]);
 			change_string_cc( pmsg[2], WHITE );
		}
		if (pmsg[3])
		{
			pmsg[3]->state.color = WHITE;
//			change_string_state(pmsg[3]);
			change_string_cc( pmsg[3], WHITE );
		}
		sleep(7);

		pmsg[0]->state.color = pmsg_org_color[0];
//		change_string_state(pmsg[0]);
		change_string_cc( pmsg[0], pmsg_org_color[0] );

		// Check for other strings
		if (pmsg[1])
		{
			pmsg[1]->state.color = pmsg_org_color[1];
//			change_string_state(pmsg[1]);
			change_string_cc( pmsg[1], pmsg_org_color[1] );
		}
		if (pmsg[2])
		{
			pmsg[2]->state.color = pmsg_org_color[2];
//			change_string_state(pmsg[2]);
			change_string_cc( pmsg[2], pmsg_org_color[2] );
		}
		if (pmsg[3])
		{
			pmsg[3]->state.color = pmsg_org_color[3];
//			change_string_state(pmsg[3]);
			change_string_cc( pmsg[3], pmsg_org_color[3] );
		}
	}
}

//-------------------------------------------------------------------------------------------------
//	This process flashes 1-4 lines of text white then original color then white, etc.
//  More slowly
//
// 	INPUT:	Strings to flash
//	OUTPUT:	Nothing
//-------------------------------------------------------------------------------------------------
void flash_text_slow_proc(int *args)
{ 
ostring_t	*pmsg[4];
int pmsg_org_color[4];
int i;

//sprite_info_t *sp;
//	sp = (sprite_info_t *)args[0];

	pmsg[0] = (ostring_t *)*args;
	// Other strings could be blank
	pmsg[1] = (ostring_t *)*(args+1);
	pmsg[2] = (ostring_t *)*(args+2);
	pmsg[3] = (ostring_t *)*(args+3);

	pmsg_org_color[0] = pmsg[0]->state.color;

	if (pmsg[1])
		pmsg_org_color[1] = pmsg[1]->state.color;
	if (pmsg[2])
		pmsg_org_color[2] = pmsg[2]->state.color;
	if (pmsg[3])
		pmsg_org_color[3] = pmsg[3]->state.color;
		
//	for (i=0; i < 100; i++)
	while(1)
	{
		sleep(12);
		pmsg[0]->state.color = WHITE;
//		change_string_state(pmsg[0]);
		change_string_cc( pmsg[0], WHITE );

		// Check for other strings
		if (pmsg[1])
		{
			pmsg[1]->state.color = WHITE;
//			change_string_state(pmsg[1]);
			change_string_cc( pmsg[1], WHITE );
		}
		if (pmsg[2])
		{
			pmsg[2]->state.color = WHITE;
//			change_string_state(pmsg[2]);
			change_string_cc( pmsg[2], WHITE );
		}
		if (pmsg[3])
		{
			pmsg[3]->state.color = WHITE;
//			change_string_state(pmsg[3]);
			change_string_cc( pmsg[3], WHITE );
		}
		sleep(12);

		pmsg[0]->state.color = pmsg_org_color[0];
//		change_string_state(pmsg[0]);
		change_string_cc( pmsg[0], pmsg_org_color[0] );

		// Check for other strings
		if (pmsg[1])
		{
			pmsg[1]->state.color = pmsg_org_color[1];
//			change_string_state(pmsg[1]);
			change_string_cc( pmsg[1], pmsg_org_color[1] );
		}
		if (pmsg[2])
		{
			pmsg[2]->state.color = pmsg_org_color[2];
//			change_string_state(pmsg[2]);
			change_string_cc( pmsg[2], pmsg_org_color[2] );
		}
		if (pmsg[3])
		{
			pmsg[3]->state.color = pmsg_org_color[3];
//			change_string_state(pmsg[3]);
			change_string_cc( pmsg[3], pmsg_org_color[3] );
		}
	}
}
