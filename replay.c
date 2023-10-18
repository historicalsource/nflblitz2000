/*
 *		$Archive: /video/nfl/replay.c $
 *		$Revision: 34 $
 *		$Date: 7/22/99 5:56a $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifdef INCLUDE_SSID
char *ss_replay_c = "$Workfile: replay.c $ $Revision: 34 $";
#endif

/*
 *		SYSTEM INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif
#include <goose/goose.h>
#include <assert.h>

/*
 *		USER INCLUDES
 */

#include "/Video/Nfl/Include/game.h"
#include "/Video/Nfl/Include/player.h"
#include "/Video/Nfl/Include/id.h"
#include "/Video/Nfl/Include/fontid.h"
#include "/Video/Nfl/Include/Pup.h"
#include "/Video/Nfl/Include/PInfo.h"

/*
 *		DEFINES
 */

/*
 *		STRUCTURES/TYPEDEFS
 */

/*
 *		PROTOTYPES
 */

static void inc_and_write( void );
static void record_cur_frame( void );
static void reset_replay( void );

static float slomo_fraction(void);
static void prep_frame_quats( int );
static int get_frame( int, int, float *, float * );
static void replay_draw_prep( void );
static int zcmp( const void *, const void * );
static void init_replay_blocks( void );

static void draw_replay_shadows( void * );
static void draw_replay( void * );

static void dump_frame( FILE *, int );
static void dump_player( FILE *, int );

/*
 *		GLOBAL REFERNCES
 */

extern LIMB			**models[];
extern LIMB			*jerseys[];
extern LIMB			limb_ballshadow_obj;
extern LIMB			limb_medball_obj_big;
extern SNODE		fbplyr_skel[];
extern SNODE		tophalf_skel[];
extern SNODE		bothalf_skel[];
extern char			*anim_blocks[];
extern float		lookup[];
extern int			hide_status_box;
extern char			*seq_names[];
#ifdef SEATTLE
extern GrMipMapId_t	ball_decal;
extern GrMipMapId_t	ball_shadow_decal;
#else
extern Texture_node_t *ball_decal;
extern Texture_node_t *ball_shadow_decal;
#endif
extern int			ball_ratio;
extern int			ball_shadow_ratio;

/*
 *		GLOBAL VARIABLES
 */

replay_obj_t replay_blocks[NUM_ROBJS];

int	recorder = FALSE;							// recorder on? set with RR_??? <set_recorder_mode>
int	playback = FALSE;							// playback on? set with RP_??? <set_playback_mode>

#ifdef DEBUG
int dbg_playback = FALSE;
//!!!!!!
int showit = FALSE;
//!!!!!!
#endif //DEBUG

/*
 *		STATIC VARIABLES
 */

static replay_t replay_data[REPLAY_FRAMES][NUM_ROBJS];

static int ball_phi = 0;
static int record_head;							// index of most recently written frame, -1 == none
static int record_tail;							// index of oldest valid frame, -1 == none
static int tape_counter;						// non-wrapping frame count

static int play_head;							// index of play head
static int play_tail;							// index of last frame to play
static int tick_dly;							// tick delay between frames in playback
static int tick_cnt;							// tick delay counter

static int replay_dump = 0;

//////////////////////////////////////////////////////////////////////////////
void replay_proc( int *args )
{
#ifdef DEBUG
#define PLAYBACK_BTN	P_B
#define RECORDER_BTN	P_C
#define HOLD			30
#define REPEAT			4
#endif //DEBUG

	// initialize
	reset_replay();

	create_object( "replay", OID_REPLAY, OF_HIDDEN, DRAWORDER_PLAYER, NULL, draw_replay );
	create_object( "rshad", OID_REPLAY, OF_HIDDEN, DRAWORDER_SHADOW, NULL, draw_replay_shadows );

	while( 1 )
	{
		sleep( 1 );

#ifdef DEBUG
		{
			static int dbsw_old, dbsw_cur, dbsw_new, cnt;

			dbsw_old = dbsw_cur;
			dbsw_cur = get_p5_sw_current();
			dbsw_new = get_p5_sw_close();

			if( dbsw_new & PLAYBACK_BTN )
			{
				if( !playback && !halt )
				{
					if( dbg_playback )
					{
						fprintf( stderr, "Stop playback.\n" );
						resume_players();
						dbg_playback = FALSE;
					}
					else
					{
						if( record_head == -1 )
						{
							fprintf( stderr, "Can't playback--no frames.\n" );
						}
						else
						{
							play_head = record_head;
							dbsw_old = 0;
							fprintf( stderr, "Start playback at frame %d.\n", play_head );
							if( recorder )
							{
								fprintf( stderr, "Stop recorder.\n" );
								recorder = FALSE;
							}
							suspend_players();
							dbg_playback = TRUE;
						}
					}
				}
			}
			else if( dbsw_new & RECORDER_BTN )
			{
				if( !dbg_playback && !playback )
				{
					// Start/Stop recorder
					recorder = !recorder;
					fprintf( stderr, "%s recorder at frame %d.\n", recorder ? "Start" : "Stop", record_head );
				}
				else
				{
					// dump to disk
					write_replay_data();
				}
			}
			else if( dbg_playback && !(dbsw_cur & P_ABCD_MASK) )
			{
				if( dbsw_cur & P_RIGHT )
				{
					if (!(dbsw_old & P_RIGHT)) cnt = HOLD;
					else
					if (!(--cnt)) cnt = REPEAT;

					if ( cnt == HOLD || cnt == REPEAT)
					{
						// frame advance
						if( play_head == record_head )
						{
							fprintf( stderr, "Can't advance--at end of replay.\n" );
						}
						else
						{
							ball_phi = (ball_phi + 40) % 1024;
							play_head = ++play_head % REPLAY_FRAMES;
							fprintf( stderr, "Advancing to replay frame %d\n", play_head );
							prep_frame_quats( play_head );
							dump_frame( stderr, play_head );
						}
					}
				}
				else if( dbsw_cur & P_LEFT )
				{
					if (!(dbsw_old & P_LEFT)) cnt = HOLD;
					else
					if (!(--cnt)) cnt = REPEAT;

					if ( cnt == HOLD || cnt == REPEAT)
					{
						// frame backup
						if( play_head == record_tail )
						{
							fprintf( stderr, "Can't back up--already at start of replay.\n" );
						}
						else
						{
							ball_phi = (ball_phi - 40 + 1024) % 1024;
							play_head = (--play_head + REPLAY_FRAMES) % REPLAY_FRAMES;
							fprintf( stderr, "Backing up to frame %d.\n", play_head );
							prep_frame_quats( play_head );
							dump_frame( stderr, play_head );
						}
					}
				}
			}
		}

		delete_multiple_strings(0xcccc, 0xffffffff);
		if( dbg_playback || playback )
		{
			set_text_font(FONTID_BAST10);
			set_text_transparency_mode(TRANSPARENCY_ENABLE);
			set_string_id(0xcccc);
			set_text_position(256, 20, 1.2f);
			oprintf("%dj%dcREPLAY", (HJUST_CENTER|VJUST_CENTER), LT_CYAN);

			if( !dbg_playback )
			{
#endif //DEBUG

		if( playback )
		{
#if 0
//!!!!!!
			if ((play_head == play_tail && !tick_cnt) || showit)
			{
				showit = TRUE;
				while (TRUE)
				{
					int dbsw_new;
					while(!(dbsw_new = get_p5_sw_close()))
						sleep(1);
					if(dbsw_new & P_C)
					{
						showit = FALSE;
						break;
					}
					if(dbsw_new & P_LEFT)
					{
						++tick_cnt;
						if(!(++tick_cnt > tick_dly && play_head == record_tail))
						{
							if(++tick_cnt > tick_dly)
							{
								play_head--;
								tick_cnt++;
							}
						}
						break;
					}
					if(dbsw_new & P_RIGHT)
					{
						play_head++;
						tick_cnt++;
						break;
					}
				}
			}
//!!!!!!
#endif //0
			if (--tick_cnt < 0)
			{
				if (play_head == play_tail)
				{
					resume_players();
					playback = FALSE;
				}
				else
				{
					play_head = ++play_head % REPLAY_FRAMES;
					ball_phi = (ball_phi - 40 + 1024) % 1024;
					tick_cnt = tick_dly;
				}
			}
		}
		if( playback )
			prep_frame_quats( play_head );

#ifdef DEBUG
			}
			else
				prep_frame_quats( play_head );
		}
#endif //DEBUG

		if( recorder )
			inc_and_write();

		if( replay_dump )
			if(!(--replay_dump ))
				write_replay_data();
	}
}

//////////////////////////////////////////////////////////////////////////////
static void inc_and_write( void )
{
	// update head and tail
	tape_counter += 1;

	record_head = tape_counter % REPLAY_FRAMES;

	if( record_tail == record_head )
		record_tail = (tape_counter + 1) % REPLAY_FRAMES;

	if( record_tail == -1 )
		record_tail = 0;

	// record current frame
	record_cur_frame();
}

//////////////////////////////////////////////////////////////////////////////
static void record_cur_frame( void )
{
	fbplyr_data	*ppdata  = player_blocks;
	replay_t	*pframe  = &replay_data[record_head][0];
	replay_t	*pframe2 = &replay_data[record_head][TWOPART_IDX+TWOPART_CNT];
	int			index, block;
	int			i, twopart_count = TWOPART_CNT;

	while( twopart_count )
	{
		twopart_count--;
		pframe2--;
		pframe2->flags   = R_UNUSED;
		pframe2->hackval = 0;
	}

	for( i = 0; i < NUM_PLAYERS; ppdata++, pframe++, i++ )
	{
		anihdr  *panihdr;
		anidata *pani  = ppdata->odata.adata;
		anidata *pani2 = ppdata->odata.adata + 2;

		pframe->x       = ppdata->odata.x;
		pframe->y       = ppdata->odata.y + ppdata->odata.ay;
		pframe->z       = ppdata->odata.z;
		pframe->fwd     = ppdata->odata.fwd;
		pframe->hackval = 0;

		if( pani->animode & MODE_INTSTREAM )
		{
			pframe->seq1   = pani->seq_index;
			pframe->frame1 = pani->iframe;
			pframe->t      = pani->fframe - (float)pani->iframe;

			block = pframe->seq1 >> 16;
			index = pframe->seq1 & 0xffff;

			panihdr = (struct anim_header *)(anim_blocks[block]+4);

			// Chk if at the end of the INTSTREAM, e.g. ANI_STREAMTOEND;
			// don't flag it as INTSTREAM if so
			if( pframe->frame1 >= panihdr[index].nframes - 1 )
			{
#ifdef DEBUG
				if( pframe->frame1 >= panihdr[index].nframes )
				{
					fprintf( stderr, "ERROR: record_cur_frame INTSTREAM "
						"out of range, seq %s frame %d (limit %d)\n",
						seq_names[index],
						pframe->frame1,
						panihdr[index].nframes );
				}
				if(pframe->t > 0.000001 )
				{
					fprintf( stderr, "ERROR: record_cur_frame INTSTREAM "
						"interpolate overflow, seq %s fframe %f (limit %d)\n",
						seq_names[index],
						pani->fframe,
						panihdr[index].nframes );
				}
#endif //DEBUG
				pframe->flags = 0;
			}
			else
			{
				pframe->flags = R_INTSTREAM;
			}
		}
		else if( pani->animode & MODE_INTERP )
		{
			pframe->seq1   = pani->seq_index2;
			pframe->seq2   = pani->seq_index;
			pframe->frame1 = pani->iframe;
			pframe->frame2 = pani2->iframe;
			pframe->t      = (float)(pani->icnt) / (float)(pani2->icnt);
			pframe->flags  = R_INTTOSEQ;

			// Chk if there was a MODE_TURNHACK
			if ((pani->hackval & 0xffff0000) == RF_TURNHACK)
			{
				// Clr MODE_TURNHACK condition & record <adata.hackval>
				pani->hackval = (int)((short)pani->hackval);
				pframe->hackval = pani->hackval;
			}
		}
		else
		{
			pframe->seq1   = pani->seq_index;
			pframe->frame1 = pani->iframe;
			pframe->flags  = 0;
		}

		if( ppdata->plyrnum == game_info.ball_carrier )
		{
			if((ppdata->odata.plyrmode == MODE_QB) && (ppdata->odata.flags & PF_FLIP))
				pframe->flags |= R_LBALL;
			else
				pframe->flags |= R_RBALL;
		}

		if( pani->animode & MODE_TWOPART )
		{
			pani  = ppdata->odata.adata + 1;
			pani2 = ppdata->odata.adata + 3;

			twopart_count++;
			assert( twopart_count <= 2 );

			pframe->flags |= twopart_count << TWOPART_BIT;
			pframe2->fwd   = ppdata->odata.fwd2;

			if( pani->animode & MODE_INTSTREAM )
			{
				pframe2->seq1    = pani->seq_index;
				pframe2->frame1  = pani->iframe;
				pframe2->t       = pani->fframe - (float)pani->iframe;

				block = pframe2->seq1 >> 16;
				index = pframe2->seq1 & 0xffff;

				panihdr = (struct anim_header *)(anim_blocks[block]+4);

				// Chk if at the end of the INTSTREAM, e.g. ANI_STREAMTOEND;
				// don't flag it as INTSTREAM if so
				if( pframe2->frame1 >= panihdr[index].nframes - 1 )
				{
	#ifdef DEBUG
					if( pframe2->frame1 >= panihdr[index].nframes )
					{
						fprintf( stderr, "ERROR: record_cur_frame INTSTREAM "
							"out of range, seq %s frame %d (limit %d)\n",
							seq_names[index],
							pframe2->frame1,
							panihdr[index].nframes );
					}
					if(pframe2->t > 0.000001 )
					{
						fprintf( stderr, "ERROR: record_cur_frame INTSTREAM "
							"interpolate overflow, seq %s fframe %f (limit %d)\n",
							seq_names[index],
							pani->fframe,
							panihdr[index].nframes );
					}
	#endif //DEBUG
					pframe2->flags = 0;
				}
				else
				{
					pframe2->flags = R_INTSTREAM;
				}
			}
			else if( pani->animode & MODE_INTERP )
			{
				pframe2->seq1   = pani->seq_index2;
				pframe2->seq2   = pani->seq_index;
				pframe2->frame1 = pani->iframe;
				pframe2->frame2 = pani2->iframe;
				pframe2->t      = (float)(pani->icnt) / (float)(pani2->icnt);
				pframe2->flags  = R_INTTOSEQ;

				// Chk if there was a MODE_TURNHACK
				if ((pani->hackval & 0xffff0000) == RF_TURNHACK)
				{
					// Clr MODE_TURNHACK condition & record <adata.hackval>
					pani->hackval = (int)((short)pani->hackval);
					pframe2->hackval = pani->hackval;
//fprintf(stderr, "===> 2PRT+INTRP:  J#:%02X,  %s -> %s,  hackval:%d,  fwd2:%d\r\n",
//	replay_blocks[i].static_data->number,
//	seq_names[pframe2->seq1 & 0xffff],
//	seq_names[pframe2->seq2 & 0xffff],
//	pframe2->hackval,
//	pframe2->fwd);
				}
			}
			else
			{
				pframe2->seq1   = pani->seq_index;
				pframe2->frame1 = pani->iframe;
				pframe2->flags  = 0;
			}
			pframe2++;
		}
	}

	// will be just a no-op if BALL_IDX follows the players
	pframe += BALL_IDX - NUM_PLAYERS;

	// store ball position
	if( game_info.ball_carrier == -1 )
	{
		((replay_ball_t *)pframe)->x    = ball_obj.odata.x;
		((replay_ball_t *)pframe)->y    = ball_obj.odata.y;
		((replay_ball_t *)pframe)->z    = ball_obj.odata.z;
		((replay_ball_t *)pframe)->vx   = ball_obj.odata.vx;
		((replay_ball_t *)pframe)->vy   = ball_obj.odata.vy;
		((replay_ball_t *)pframe)->vz   = ball_obj.odata.vz;
		((replay_ball_t *)pframe)->fwd  = ball_obj.odata.fwd;
		((replay_ball_t *)pframe)->fwd2 = ball_obj.odata.fwd2;

		((replay_ball_t *)pframe)->flags |= R_DRAWBALL;
	}
	else
	{
		((replay_ball_t *)pframe)->x = player_blocks[game_info.ball_carrier].odata.x;
		((replay_ball_t *)pframe)->y = (5.0f*FT2UN);
		((replay_ball_t *)pframe)->z = player_blocks[game_info.ball_carrier].odata.z;

		((replay_ball_t *)pframe)->flags &= ~R_DRAWBALL;
	}
}

//////////////////////////////////////////////////////////////////////////////
static void reset_replay( void )
{
	record_head = -1;
	record_tail = -1;
	tape_counter = -1;
	recorder = FALSE;
	playback = FALSE;
	init_replay_blocks();
}

#ifdef DEBUG
//////////////////////////////////////////////////////////////////////////////
static float qmag( float *pq );
static float qmag( float *pq )
{
	float	mag;

	mag = fsqrt( pq[0] * pq[0] + pq[1] * pq[1] + pq[2] * pq[2] + pq[3] * pq[3] );

	return mag;
}

static int quat_check( float *pq );
static int quat_check( float *pq )
{
	float	mag;

	mag = qmag( pq );

	if (fabs(mag-1.0f) > 0.1f)
	{
		return 1;
	}
	else
		return 0;
}

int dbg_check_quats(replay_obj_t * probj, int pnum);
int dbg_check_quats(replay_obj_t * probj, int pnum)
{
	// verify that all quats are normalized
	int error = FALSE;
	int j = 0;
	do
	{
		float *fp = probj->pq + j*4;
		float   n = fsqrt( fp[0]*fp[0] + fp[1]*fp[1] + fp[2]*fp[2] + fp[3]*fp[3] );

		if( fabs( n - 1.0f ) > 0.15 )
		{
			fprintf( stderr, "Non-normalized quat on replay player %d limb %d\n", pnum, j );
			fprintf( stderr, "%f %f %f %f - mag %f\n", fp[0], fp[1], fp[2], fp[3], n );
			fprintf( stderr, "seqs %d & %d, frames %d & %d, t %f, flags %x\n",
				replay_data[play_head][pnum].seq1,
				replay_data[play_head][pnum].seq2,
				replay_data[play_head][pnum].frame1,
				replay_data[play_head][pnum].frame2,
				replay_data[play_head][pnum].t,
				replay_data[play_head][pnum].flags
				);
			error = TRUE;
			lockup();
		}
	} while (++j < 15);
	return error;
}

int dbg_check_matrices(replay_obj_t * probj, int pnum);
int dbg_check_matrices(replay_obj_t * probj, int pnum)
{
	// make sure all the matrices are normalized
	int error = FALSE;
	int j = 0;
	do
	{
		float *fp = (float *)(probj->matrices) + j*4;
		float   n = fsqrt(fp[0]*fp[0]+fp[1]*fp[1]+fp[2]*fp[2]);
		int  limb = j / 3;

		if( fabs(1.0f - n) > 0.25 )
		{
			fprintf( stderr, "Error in replay data\n" );
			fprintf( stderr, "non-normalized matrix line %f %f %f, mag %f\r\n",
					fp[0], fp[1], fp[2], n );
			fprintf( stderr, "seqs %d & %d, frames %d & %d, t %f, flags %x\n",
				replay_data[play_head][pnum].seq1,
				replay_data[play_head][pnum].seq2,
				replay_data[play_head][pnum].frame1,
				replay_data[play_head][pnum].frame2,
				replay_data[play_head][pnum].t,
				replay_data[play_head][pnum].flags
				);
			fprintf( stderr, "limb: %d\n", limb );
			fprintf( stderr, "src quat: %9f %9f %9f %9f - mag %9f\n",
				probj->pq[limb*4+0],
				probj->pq[limb*4+1],
				probj->pq[limb*4+2],
				probj->pq[limb*4+3],
				fsqrt(probj->pq[limb*4+0]*probj->pq[limb*4+0]+
					  probj->pq[limb*4+1]*probj->pq[limb*4+1]+
					  probj->pq[limb*4+2]*probj->pq[limb*4+2]+
					  probj->pq[limb*4+3]*probj->pq[limb*4+3])
				);

			write_replay_data();
			error = TRUE;
			lockup();
		}
	} while (++j < 45);
	return error;
}
#endif //DEBUG

//////////////////////////////////////////////////////////////////////////////
// figure slow motion fraction per <tick_dly>; returns:
//   a) 0 to use current <play_head> data;
//   b) fractional amount to use of deltas between current & next frame
//      of <play_head> data
static float slomo_fraction(void)
{
	int cnt = tick_dly - tick_cnt;
	if (
#ifdef DEBUG
		dbg_playback ||
#endif //DEBUG
		cnt <= 0)
	{
		// Use current frame if tick_cnt >= tick_dly
		return 0.0f;
	}
	return (float)cnt / (float)(tick_dly + 1);
}

//////////////////////////////////////////////////////////////////////////////
// set replay_block quats to match recorded frame N
static void prep_frame_quats( int frame )
{
	replay_obj_t	*probj;
	replay_t		*pframe, *pframe1;
	float			iblock1[60], iblock2[60];
	float			fq[4], pp[4];
	float			frac = slomo_fraction();
	float			t;
	int				i,j;

	// barf if frame isn't valid
	if( frame < 0 || frame >= REPLAY_FRAMES ||
			( record_tail == 0 && record_head < frame ))
	{	 
#ifdef DEBUG
		fprintf( stderr, "Error: Replay of invalid frame.\n" );
		lockup();
#else
		while(1) ;
#endif //DEBUG
	}

	probj   = replay_blocks;
	pframe  = replay_data[frame];
	pframe1 = pframe;
	if (
#ifdef DEBUG
		(dbg_playback && play_head != record_head) ||
#endif //DEBUG
		frame != play_tail)
	{
		pframe1 = replay_data[(frame + 1) % REPLAY_FRAMES];
	}

	// Generate quats; TWOPART_IDX records must follow player records!
	for( i = 0; i < (NUM_PLAYERS+TWOPART_CNT); probj++, pframe++, pframe1++, i++ )
	{
		if( pframe->flags & R_UNUSED )
			continue;

		if( pframe->flags & R_INTSTREAM )
		{
			j = get_frame( pframe->seq1, pframe->frame1, iblock1, iblock2 );
#ifdef DEBUG
			if (j)
			{
				if (i < NUM_PLAYERS)
					fprintf(stderr,"iframe:%d  fframe:%f  i:%d  P#:%d  J#:%02X\r\n",pframe->frame1,pframe->t,i,probj->plyrnum,probj->static_data->number);
				else
					fprintf(stderr,"iframe:%d  fframe:%f  i:%d\r\n",pframe->frame1,pframe->t,i);
			}
#endif //DEBUG
			for( j = 0; j < 15; j++ )
				slerp( iblock1 + 4*j, iblock2 + 4*j, pframe->t, probj->pq + 4*j );
		}
		else if( pframe->flags & R_INTTOSEQ )
		{
			if (probj->last_seq1   != pframe->seq1   ||
				probj->last_frame1 != pframe->frame1 ||
				probj->last_seq2   != pframe->seq2   ||
				probj->last_frame2 != pframe->frame2)
			{
				probj->hackval = pframe->hackval;
			}

			get_frame( pframe->seq1, pframe->frame1, iblock1, NULL );
			get_frame( pframe->seq2, pframe->frame2, iblock2, NULL );

			t = pframe->t;

			// Interpolate fraction with portion of now-to-next delta
			t += frac * (pframe1->t - t);

			if (probj->hackval)
			{
				fq[0] = 0.0f;
				fq[1] = isin( probj->hackval/2 );
				fq[2] = 0.0f;
				fq[3] = icos( probj->hackval/2 );
//if (i < TWOPART_IDX)
//	fprintf(stderr,"replay hackval:%d  J#:%02X  fwd:%d  nxtfwd:%d\r\n",
//		probj->hackval,probj->static_data->number,pframe->fwd,pframe1->fwd);
//else
//	fprintf(stderr,"replay hackval:%d  fwd:%d  nxtfwd:%d\r\n",
//		probj->hackval,pframe->fwd,pframe1->fwd);
//sleep(20);
			}
			else
			{
				fq[0] = 0.0f;
				fq[1] = 0.0f;
				fq[2] = 0.0f;
				fq[3] = 1.0f;
			}

			qxq( fq, iblock1, pp );

			iblock1[0] = pp[0];
			iblock1[1] = pp[1];
			iblock1[2] = pp[2];
			iblock1[3] = pp[3];

			for( j = 0; j < 15; j++ )
				adjust_quat( iblock1 + 4*j, iblock2 + 4*j );

			for( j = 0; j < 15; j++ )
				slerp( iblock1 + 4*j, iblock2 + 4*j, t, probj->pq + 4*j );

#ifdef DEBUG
			for( j = 0; j < 15; j++ )
			{
				if (quat_check( probj->pq + 4*j ))
				{
					fprintf( stderr, "playback frame %d, player %d, limb %d\n", frame, i, j );
					fprintf( stderr, "bogus quat:\nsrc:  %f %f %f %f - %f\ndest: %f %f %f %f - %f\nt: %f\nint:  %f %f %f %f - %f\n",
						iblock1[4*j+0],
						iblock1[4*j+1],
						iblock1[4*j+2],
						iblock1[4*j+3],
						qmag( iblock1 + 4*j ),
						iblock2[4*j+0],
						iblock2[4*j+1],
						iblock2[4*j+2],
						iblock2[4*j+3],
						qmag( iblock2 + 4*j ),
						pframe->t,
						probj->pq[4*j+0],
						probj->pq[4*j+1],
						probj->pq[4*j+2],
						probj->pq[4*j+3],
						qmag( probj->pq + 4*j ));
					fprintf( stderr, "get_frame( %s, %d, iblock1, NULL );\n", seq_names[pframe->seq1], pframe->frame1 );
					fprintf( stderr, "get_frame( %s, %d, iblock2, NULL );\n", seq_names[pframe->seq2], pframe->frame2 );
					dump_player( stderr, i );
					lockup();
				}
			}
#endif //DEBUG
		}
		else
		{
			get_frame( pframe->seq1, pframe->frame1, probj->pq, NULL );
		}

		probj->last_seq1   = pframe->seq1;
		probj->last_frame1 = pframe->frame1;
		probj->last_seq2   = pframe->seq2;
		probj->last_frame2 = pframe->frame2;

		probj->x   = pframe->x;
		probj->y   = pframe->y;
		probj->z   = pframe->z;
		probj->fwd = pframe->fwd;

		if (frac && !(pframe1->flags & R_UNUSED))
		{
			// Interpolate position with portion of now-to-next deltas
			probj->x   += frac * (pframe1->x - probj->x);
			probj->y   += frac * (pframe1->y - probj->y);
			probj->z   += frac * (pframe1->z - probj->z);

			j = pframe1->fwd - probj->fwd;
			// Remove any <hackval> from the yet-to-be-active <fwd>
			if (pframe != pframe1) j += pframe1->hackval;
			while (j >= 512) j -= 1024;
			while (j < -512) j += 1024;
			probj->fwd += (int)(frac * (float)j);
		}
	}

	// will be just a no-op if BALL_IDX follows the two-parts
	probj   += BALL_IDX - (NUM_PLAYERS+TWOPART_CNT);
	pframe  += BALL_IDX - (NUM_PLAYERS+TWOPART_CNT);
	pframe1 += BALL_IDX - (NUM_PLAYERS+TWOPART_CNT);

	// set ball stuff
	probj->x          = ((replay_ball_t *)pframe)->x;
	probj->y          = ((replay_ball_t *)pframe)->y;
	probj->z          = ((replay_ball_t *)pframe)->z;
	probj->jpos[0][0] = ((replay_ball_t *)pframe)->vx;
	probj->jpos[0][1] = ((replay_ball_t *)pframe)->vy;
	probj->jpos[0][2] = ((replay_ball_t *)pframe)->vz;
	probj->fwd        = ((replay_ball_t *)pframe)->fwd;
	probj->plyrnum    = ((replay_ball_t *)pframe)->fwd2;

	if (frac)
	{
		if (((replay_ball_t *)pframe1)->flags & R_DRAWBALL)
		{
			// Interpolate position with portion of now-to-next deltas
			probj->x          += frac * (((replay_ball_t *)pframe1)->x - probj->x);
			probj->y          += frac * (((replay_ball_t *)pframe1)->y - probj->y);
			probj->z          += frac * (((replay_ball_t *)pframe1)->z - probj->z);
			probj->jpos[0][0] += frac * (((replay_ball_t *)pframe1)->vx - probj->jpos[0][0]);
			probj->jpos[0][1] += frac * (((replay_ball_t *)pframe1)->vy - probj->jpos[0][1]);
			probj->jpos[0][2] += frac * (((replay_ball_t *)pframe1)->vz - probj->jpos[0][2]);

			j = ((replay_ball_t *)pframe1)->fwd - probj->fwd;
			while (j >= 512) j -= 1024;
			while (j < -512) j += 1024;
			probj->fwd += (int)(frac * (float)j);

			j = ((replay_ball_t *)pframe1)->fwd2 - probj->plyrnum;
			while (j >= 512) j -= 1024;
			while (j < -512) j += 1024;
			probj->plyrnum += (int)(frac * (float)j);
		}
		else
		{
			// Interpolate position with portion of last velocities
			probj->x += frac * probj->jpos[0][0];
			probj->y += frac * probj->jpos[0][1];
			probj->z += frac * probj->jpos[0][2];
		}
	}

	// set ball draw flag
	if( ((replay_ball_t *)pframe)->flags & R_DRAWBALL )
	{
		probj->flags = R_DRAWBALL;
	}
	else
	{
		probj->flags = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
// Get the quats corresponding to an arbitrary frame.  If dest2 != NULL, write
// next frame to it.
// Returns:
//  FALSE = no problems
//  bit 0 = frame # out of range
//  bit 1 = next frame # out of range
static int get_frame( int seq_index, int frame, float *dest1, float *dest2 )
{
	anidata			pani;
	anihdr			*panihdr;
	float			*pq;
	unsigned char	*dp;
	int				i,j;
	int				block = seq_index >> 16;
	int				index = seq_index & 0xffff;
	int				rtn = FALSE;

	panihdr = (struct anim_header *)(anim_blocks[block]+4);

	// range checking
	if (frame >= panihdr[index].nframes)
	{
#ifdef DEBUG
		fprintf( stderr, "ERROR: get_frame seq %s frame %d, out of range (limit %d)\n",
			seq_names[index], frame, panihdr[index].nframes );
#endif //DEBUG
		// Backup to last valid frame
		while(--frame >= panihdr[index].nframes);
		rtn |= 1;
	}
	if( dest2 && (frame + 1) == panihdr[index].nframes)
	{
#ifdef DEBUG
		fprintf( stderr, "ERROR: get_frame seq %s, Next Frame request is out of range (limit %d)\n",
			seq_names[index], panihdr[index].nframes );
#endif //DEBUG
		// Backup one more frame
		frame--;
		rtn |= 2;
	}

	pani.pahdr = panihdr + index;
	pani.pxlate = (float *)((char *)panihdr + panihdr[index].offset);
	pani.pframe0 = pani.pxlate + panihdr[index].nframes * 3;
	pani.pframe = (char *)(pani.pframe0 + panihdr[index].nlimbs * 4);

	if( panihdr[index].flags & AH_COMPRESSED )
	{
		// start at frame 0
		for( i = 0; i < 4 * panihdr[index].nlimbs; i++ )
			dest1[i] = pani.pframe0[i];

		// decompress as much as neccesary
		for( i = 0; i < frame; i++ )
		{
			dp = pani.pframe + i * 4 * pani.pahdr->nlimbs;
			j = 4 * pani.pahdr->nlimbs;
			while( j-- )
				dest1[j] += lookup[dp[j]];
		}

		if( dest2 )
		{
			dp = pani.pframe + i * 4 * pani.pahdr->nlimbs;
			j = 4 * pani.pahdr->nlimbs;
			while( j-- )
				dest2[j] = dest1[j] + lookup[dp[j]];
		}
	}
	else
	{
		pq = pani.pframe0 + panihdr[index].nlimbs * 4 * frame;
		for( i = 0; i < 4 * panihdr[index].nlimbs; i++ )
			dest1[i] = pq[i];

		if( dest2 )
		{
			pq = pani.pframe0 + panihdr[index].nlimbs * 4 * (frame+1);
			for( i = 0; i < 4 * panihdr[index].nlimbs; i++ )
				dest2[i] = pq[i];
		}
	}
	return rtn;
}

//////////////////////////////////////////////////////////////////////////////
// final pre-draw prep.  This data is invalidated by any call to
// prep_frame_quats() or any change to the cambot matrix.
static void replay_draw_prep( void )
{
	replay_obj_t *	probj = replay_blocks;
	float *			cam = cambot.xform;
	float			rx,ry,rz,x,y,z,xf,yf;
	float			mat[12], cm[12],cm2[12];
	int				i, j;

//	// When X players are on screen, high_counts[X] of them use the high-detail
//	// model.  -1 means all use it.
	int				onscreen;
	int				high_counts[] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,7,6,5,4,3,2 };
	static int		zsort[NUM_PLAYERS];

	// flag onscreen players
	onscreen = 0;
	for( i = 0; i < NUM_PLAYERS; probj++, i++ )
	{
		rx = probj->x - cambot.x;
		ry = probj->y - cambot.y;
		rz = probj->z - cambot.z;

		// compute screen coords of player center
		x = cam[0]*rx+cam[1]*ry+cam[2]*rz+cam[3];
		y = cam[4]*rx+cam[5]*ry+cam[6]*rz+cam[7];
		z = cam[8]*rx+cam[9]*ry+cam[10]*rz+cam[11];

		xf = z * (50.0f/hres + 0.5f);
		yf = xf * (vres/hres);

		// set or clear onscreen bit
		if( z < 10.0f || fabs(x) > xf || fabs(y) > yf )
			probj->flags &= ~R_ONSCREEN;
		else
		{
			probj->flags |= R_ONSCREEN;
			onscreen++;
		}

#ifdef DEBUG
		dbg_check_quats(probj, i);
#endif //DEBUG

		// onscreen or not, we need the matrices for collision checking
		mat[3]  = rx;
		mat[7]  = ry;
		mat[11] = rz;

		roty( probj->fwd, mat );
		mxm( cam, mat, cm );
		cm[0] *= probj->ascale; cm[1] *= probj->ascale; cm[2]  *= probj->ascale;
		cm[4] *= probj->ascale; cm[5] *= probj->ascale; cm[6]  *= probj->ascale;
		cm[8] *= probj->ascale; cm[9] *= probj->ascale; cm[10] *= probj->ascale;

		if( replay_data[play_head][i].flags & R_TWOPART)
		{
			int tp_index = NUM_PLAYERS + (((replay_data[play_head][i].flags & R_TWOPART) >> TWOPART_BIT) - 1);

			// do top half first so bottom overwrites pelvis matrix (top and bottom
			// each have a pelvis, but only the bottom's pelvis is displayed.)
			roty( replay_blocks[tp_index].fwd, mat );
			mxm( cam, mat, cm2 );
			cm2[0] *= probj->ascale; cm2[1] *= probj->ascale; cm2[2]  *= probj->ascale;
			cm2[4] *= probj->ascale; cm2[5] *= probj->ascale; cm2[6]  *= probj->ascale;
			cm2[8] *= probj->ascale; cm2[9] *= probj->ascale; cm2[10] *= probj->ascale;

			generate_matrices( tophalf_skel, cm2, replay_blocks[tp_index].pq, probj->matrices[0], probj->jpos[0] );
			generate_matrices( bothalf_skel, cm, probj->pq, probj->matrices[0], probj->jpos[0] );
		}
		else
		{
			generate_matrices( fbplyr_skel, cm, probj->pq, probj->matrices[0], probj->jpos[0] );
		}

#ifdef 0//DEBUG
		if (probj->x != replay_data[play_head][i].x ||
			probj->y != replay_data[play_head][i].y ||
			probj->z != replay_data[play_head][i].z ||
			probj->fwd != replay_data[play_head][i].fwd)
		{
			fprintf( stderr, "==>> Replay data mismatch!  head=%d plyr=%d\n",play_head,i);
			fprintf( stderr, "==>> %f:%f %f:%f %f:%f   %d:%d\n",
				probj->x,replay_data[play_head][i].x,
				probj->y,replay_data[play_head][i].y,
				probj->z,replay_data[play_head][i].z,
				probj->fwd,replay_data[play_head][i].fwd);
			lockup();
		}
#endif //DEBUG
	}

	// assign model pointers based on the number onscreen
	if( high_counts[onscreen] == -1 )
	{
		for( i = 0; i < NUM_PLAYERS; i++ )
			player_blocks[i].odata.flags |= PF_HICOUNT;
	}
	else
	{
		// sort players into ascending Z order
		// nearest onscreen players get higher model
		qsort( (void *)zsort, NUM_PLAYERS, sizeof( int ), zcmp );

		j = high_counts[onscreen];

		for( i = 0; i < NUM_PLAYERS; i++ )
		{
			if( j && ( replay_blocks[zsort[i]].flags & R_ONSCREEN ))
			{
				j--;
				replay_blocks[zsort[i]].flags |= PF_HICOUNT;
			}
			else
				replay_blocks[zsort[i]].flags &= ~PF_HICOUNT;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// zcmp() - Z-test function for qsort().
// TODO: Make this work for replay guys.
static int zcmp( const void *p1, const void *p2 )
{
	int		i1,i2;

	return 0;

	i1 = *((int *)p1);
	i2 = *((int *)p2);

	if( replay_blocks[i2].jpos[0][2] > player_blocks[i1].jpos[0][2] )
		return -1;

	if( replay_blocks[i2].jpos[0][2] < player_blocks[i1].jpos[0][2] )
		return 1;

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// initialize replay_blocks to correspond to the 14 currently active players.
static void init_replay_blocks( void )
{
	replay_obj_t * probj = replay_blocks;
	fbplyr_data * ppdata = player_blocks;
	int i;

	for( i = 0; i < NUM_PLAYERS; probj++, ppdata++, i++ )
	{
		probj->ascale      = ppdata->odata.ascale;
		probj->plyrnum     = ppdata->plyrnum;
		probj->flags       = ppdata->odata.flags;
		probj->pdecal      = ppdata->odata.pdecal;
		probj->static_data = ppdata->static_data;
	}
}

//////////////////////////////////////////////////////////////////////////////
// Set recorder mode
// Returns:
//    TRUE if successful, FALSE otherwise
//
int set_recorder_mode( int mode )
{
	switch( mode )
	{
		case RR_RESTART:
			reset_replay();
		case RR_START:
			recorder = TRUE;
			break;

		case RR_REWIND:
			reset_replay();
		case RR_STOP:
			recorder = FALSE;
			break;

		default:
#ifdef DEBUG
			fprintf( stderr, "Error: bad replay recorder mode: %d\n", mode );
			lockup();
#else
//fix!!!	while(1) ;
#endif //DEBUG
			return FALSE;
	}
	// Valid mode - stop playback
	playback = FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// Set playback mode - wants to be called before <player_procs> execute (?)
// Returns:
//    playback tick count if successful, 0 otherwise
//
int set_playback_mode( int mode, int sframe, int eframe, int ticks )
{
	int i;

	if ((mode != RP_STOP && ticks < 0) ||
		(mode == RP_START &&
		(sframe < 0 || eframe < 0 || sframe >= REPLAY_FRAMES || sframe < eframe)))
	{
#ifdef DEBUG
		fprintf( stderr, "Error: bad replay playback parameter(s): "
			"mode=%d  sframe=%d  eframe=%d  ticks=%d\n",
			mode, sframe, eframe, ticks );
#endif //DEBUG
		return 0;
	}
	if (mode != RP_STOP && (record_head < 0 || record_tail < 0))
	{
		i = recorder;
#ifdef DEBUG
		fprintf( stderr, "Error: bad or no tape for replay playback - reseting\n");
#endif //DEBUG
		// reset recorder but keep current mode
		reset_replay();
		recorder = i;

		return 0;
	}

	switch( mode )
	{
		case RP_STARTALL:
			sframe = REPLAY_FRAMES - 1;
			eframe = 0;
		case RP_START:
			i = (record_head - record_tail + REPLAY_FRAMES) % REPLAY_FRAMES;

			if (sframe > i) sframe = i;
			if (eframe > i) eframe = i;

			play_head = (record_head - sframe + REPLAY_FRAMES) % REPLAY_FRAMES;
			play_tail = (record_head - eframe + REPLAY_FRAMES) % REPLAY_FRAMES;
			tick_dly = ticks;
			tick_cnt = ticks + 1;	// +1 to show first frame

			suspend_players();

			playback = TRUE;
			recorder = FALSE;

			// Prep first frame so replay_blocks will be initialized for caller
			prep_frame_quats( play_head );

			return (sframe - eframe + 1) * (ticks + 1);

		case RP_STOP:
			if (playback)
				resume_players();

			playback = FALSE;

			break;

		default:
#ifdef DEBUG
			fprintf( stderr, "Error: bad replay playback mode: %d\n", mode );
			lockup();
#else
//fix!!!	while(1) ;
#endif //DEBUG
			break;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
static void draw_replay_shadows( void *oi )
{
	replay_obj_t	*probj = replay_blocks;
	fbplyr_data		temp_player;
	int				i,j;
	float			mat[12],cm[12];

	temp_player.odata.flags = PF_ONSCREEN;

	replay_draw_prep();

	// player shadows
	for( i = 0; i < NUM_PLAYERS; probj++, i++ )
	{
		if(!( probj->flags & R_ONSCREEN ))
			continue;

		for( j = 0; j < 45; j++ )
			temp_player.jpos[j/3][j%3] = probj->jpos[j/3][j%3];

		draw_player_shadow( &temp_player );
	}

	// ball shadow
	if( replay_blocks[BALL_IDX].flags & R_DRAWBALL )
	{
		mat[3] = replay_blocks[BALL_IDX].x - cambot.x;
		mat[7] = -cambot.y;
		mat[11] = replay_blocks[BALL_IDX].z - cambot.z;

		roty( replay_blocks[BALL_IDX].fwd, mat );
		mxm( cambot.xform, mat, cm );

		cm[0] *= 3.0f; cm[1] *= 3.0f; cm[2] *= 3.0f;
		cm[4] *= 3.0f; cm[5] *= 3.0f; cm[6] *= 3.0f;
		cm[8] *= 3.0f; cm[9] *= 3.0f; cm[10] *= 3.0f;

		render_limb( &limb_ballshadow_obj, cm, &ball_shadow_decal, &ball_shadow_ratio );
	}
}

//////////////////////////////////////////////////////////////////////////////
#define JNUM(x) ((x) ? ((x)-1) : 9 )
static void draw_replay( void *oi )
{
	replay_obj_t	*probj  = replay_blocks;
	replay_t		*pframe = replay_data[play_head];
	int				mod;
	int				i, j1,j2;

	// init glide states for players
	guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	grTexClampMode(0,GR_TEXTURECLAMP_WRAP,GR_TEXTURECLAMP_WRAP);

	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
	grDepthBiasLevel( 0 );
	grDepthMask( FXTRUE );

	grChromakeyMode( GR_CHROMAKEY_DISABLE );

	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	grAlphaBlendFunction( GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA,
			GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA );

	grDepthBufferFunction( GR_CMP_LESS );

	for( i = 0; i < NUM_PLAYERS; probj++, pframe++, i++ )
	{
		// on screen?
		if(!(probj->flags & R_ONSCREEN ))
			continue;

		// select model
		mod = (probj->flags & PF_HICOUNT ) ? MOD_498 : MOD_266;
	
		if( probj->static_data->size == PS_FAT )
			mod |= MOD_FAT;
	
		if( pframe->flags & R_RBALL )
				mod |= MOD_RBALL;
	
		if( pframe->flags & R_LBALL )
				mod |= MOD_LBALL;
	
		grDepthBiasLevel( 0 );
		render_node_mpc( fbplyr_skel, models[mod], probj->matrices[0], probj->pdecal, probj->jpos[0] );
	
		grDepthBiasLevel( -8 );
		guTexSource( probj->pdecal[5] );
		j1 = probj->static_data->number >> 4;
		j2 = probj->static_data->number & 0x0F;
	
		mod &= MOD_FAT|MOD_498;
		if( j1 )
			mod |= MOD_2DIGITS;
	
		render_limb_jers( jerseys[mod], probj->matrices[1], JNUM(j1), JNUM(j2) );
	}

	// draw ball
	if( replay_blocks[BALL_IDX].flags & R_DRAWBALL )
	{
		float mat1[12], mat2[12], mat3[12];
		float bsf = ball_obj.scale;

		mat1[3] = 0.0f;	mat1[7] = 0.0f;	mat1[11] = 0.0f;
		mat3[3] = 0.0f;	mat3[7] = 0.0f;	mat3[11] = 0.0f;

		rotx(-replay_blocks[BALL_IDX].plyrnum, mat3 );
		roty( replay_blocks[BALL_IDX].fwd, mat1 );
		mxm( mat1, mat3, mat2 );
		rotz( ball_phi, mat1 );
		mxm( mat2, mat1, mat3 );

		mat3[3]  = replay_blocks[BALL_IDX].x - cambot.x;
		mat3[7]  = replay_blocks[BALL_IDX].y - cambot.y;
		mat3[11] = replay_blocks[BALL_IDX].z - cambot.z;

		if (pup_bigball)
			bsf *= 1.5f;

		mat3[0] *= bsf;	mat3[1] *= bsf;	mat3[2]  *= bsf;
		mat3[4] *= bsf;	mat3[5] *= bsf;	mat3[6]  *= bsf;
		mat3[8] *= bsf;	mat3[9] *= bsf;	mat3[10] *= bsf;

		mxm( cambot.xform, mat3, mat1 );

		render_limb( &limb_medball_obj_big, mat1, &ball_decal, &ball_ratio );
	}
}

//////////////////////////////////////////////////////////////////////////////
void suspend_players( void )
{
	suspend_multiple_processes( PLAYER_PID, -1 );
	suspend_multiple_processes( PRE_PLAYER_PID, -1 );
	suspend_multiple_processes( BALL_PID, -1 );
	suspend_multiple_processes( GAME_CLOCK_PID, -1 );
	suspend_multiple_processes( COLLIS_PID, -1 );

	hide_multiple_objects( OID_SMKPUFF, -1 );
	hide_multiple_objects( OID_PLAYER, -1 );
	hide_multiple_objects( OID_CURSOR, -1 );
	hide_multiple_objects( OID_BALL, -1 );
	hide_multiple_objects( OID_SHADOW, -1 );

	arrowinfo[0].ai_flags |= AF_INACTIVE;
	arrowinfo[1].ai_flags |= AF_INACTIVE;
	arrowinfo[2].ai_flags |= AF_INACTIVE;
	arrowinfo[3].ai_flags |= AF_INACTIVE;

	unhide_multiple_objects( OID_REPLAY, -1 );

	showhide_pinfo(0);
	showhide_status_box(0);

	hide_status_box = TRUE;
}

//////////////////////////////////////////////////////////////////////////////
void resume_players( void )
{
	resume_multiple_processes( PLAYER_PID, -1 );
	resume_multiple_processes( PRE_PLAYER_PID, -1 );
	resume_multiple_processes( BALL_PID, -1 );
	resume_multiple_processes( GAME_CLOCK_PID, -1 );
	resume_multiple_processes( COLLIS_PID, -1 );

	unhide_multiple_objects( OID_SMKPUFF, -1 );
	unhide_multiple_objects( OID_PLAYER, -1 );
	unhide_multiple_objects( OID_CURSOR, -1 );
	unhide_multiple_objects( OID_BALL, -1 );
	unhide_multiple_objects( OID_SHADOW, -1 );

	arrowinfo[0].ai_flags &= ~AF_INACTIVE;
	arrowinfo[1].ai_flags &= ~AF_INACTIVE;
	arrowinfo[2].ai_flags &= ~AF_INACTIVE;
	arrowinfo[3].ai_flags &= ~AF_INACTIVE;

	hide_multiple_objects( OID_REPLAY, -1 );

	showhide_pinfo(1);
	showhide_status_box(1);

	hide_status_box = FALSE;
}

//////////////////////////////////////////////////////////////////////////////
void write_replay_data( void )
{
	char	filename[13];
	FILE	*fp = NULL;
	int		i=0,num_frames,cur_frame;

	while( fp == NULL )
	{
		sprintf( filename, "RDATA.%03d", i );
		fp = fopen( filename, "rb" );
		if( fp == NULL )
		{
			if((fp = fopen( filename, "wb" )) == NULL )
			{
				fprintf( stderr, "Error opening file %s for writing.\n", filename );
				return;
			}
		}
		else
		{
			fclose( fp );
			fp = NULL;
			i++;
		}
	}

	fprintf( stderr, "Dumping replay data to file %s\n", filename );

	if(( record_tail == 0 ) && ( record_head != REPLAY_FRAMES ))
		num_frames = record_head + 1;
	else
		num_frames = REPLAY_FRAMES;

	if( num_frames > 50 )
		num_frames = 50;

	fprintf( fp, "Replay file %d, head %d, tail %d, num %d\n", i,
			record_head, record_tail, num_frames );

	for( i = 0; i < num_frames; i++ )
	{
		cur_frame = (record_head + REPLAY_FRAMES - i) % REPLAY_FRAMES;
		fprintf( fp, "\nFrame %d\n", num_frames-i );
		dump_frame( fp, cur_frame );
	}

	for( i = 0; i < NUM_PLAYERS; i++ )
	{
		fprintf( fp, "\nPlayer %d jersey %x\n", i, replay_blocks[i].static_data->number );
		dump_player( fp, i );
	}

	for( i = 0; i < num_frames; i++ )
	{
		cur_frame = (record_head + REPLAY_FRAMES - i) % REPLAY_FRAMES;
		fprintf( fp, "\nFrame %d\n", num_frames-i );
		dump_frame( fp, cur_frame );
	}

	for( i = 0; i < NUM_PLAYERS; i++ )
	{
		fprintf( fp, "\nPlayer %d jersey %x\n", i, replay_blocks[i].static_data->number );
		dump_player( fp, i );
	}

	fflush( fp );

	fclose( fp );
}

//////////////////////////////////////////////////////////////////////////////
static void dump_frame( FILE *fp, int frame )
{
	replay_t	*pframe;
	int			i;

	pframe = replay_data[frame];

	for( i = 0; i < NUM_PLAYERS; i++ )
	{
		fprintf( fp, "%2x: ", replay_blocks[i].static_data->number );

		if( pframe[i].flags & R_INTSTREAM )
		{
			fprintf( fp, "%s:%f\n", seq_names[pframe[i].seq1],
					(float)pframe[i].frame1 + pframe[i].t );
		}
		else if( pframe[i].flags & R_INTTOSEQ )
		{
			fprintf( fp, "%s:%d->%s:%d (%f)\n",
					seq_names[pframe[i].seq1],
					pframe[i].frame1,
					seq_names[pframe[i].seq2],
					pframe[i].frame2,
					pframe[i].t );
		}
		else
		{
			fprintf( fp, "%s:%d\n", seq_names[pframe[i].seq1],
					pframe[i].frame1 );
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
static void dump_player( FILE *fp, int player )
{
	replay_t	*pframe;
	int			i,num_frames,cur_frame;
	int			c;

	if(( record_tail == 0 ) && ( record_head != REPLAY_FRAMES ))
		num_frames = record_head + 1;
	else
		num_frames = REPLAY_FRAMES;

	if( num_frames > 300 )
		num_frames = 300;


	for( i = 0; i < num_frames; i++ )
	{
		cur_frame = (record_head + REPLAY_FRAMES - i) % REPLAY_FRAMES;
		pframe = replay_data[cur_frame];

		fprintf( fp, "%2d: ", i );

		if( pframe[player].flags & R_INTSTREAM )
		{
			c = fprintf( fp, "%s:%f\n", seq_names[pframe[player].seq1],
					(float)pframe[player].frame1 + pframe[player].t );
		}
		else if( pframe[player].flags & R_INTTOSEQ )
		{
			c = fprintf( fp, "%s:%d->%s:%d (%f)\n",
					seq_names[pframe[player].seq1],
					pframe[player].frame1,
					seq_names[pframe[player].seq2],
					pframe[player].frame2,
					pframe[player].t );
		}
		else
		{
			c = fprintf( fp, "%s:%d\n", seq_names[pframe[player].seq1],
					pframe[player].frame1 );
		}

		if( c < 1 )
			fprintf( stderr, "Error %i writing to debug file.\n", c );
	}
}

//////////////////////////////////////////////////////////////////////////////
