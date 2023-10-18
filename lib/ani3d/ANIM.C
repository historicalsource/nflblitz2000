/****************************************************************************/
/*                                                                          */
/* anim.c - Animation script interpreter                                    */
/*                                                                          */
/* Written by:  Jason Skiles                                                */
/*                                                                          */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <dir.h>
#include <math.h>
#include <goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif
#include <assert.h>

// Put the following line in to make animation data be read from a disk file
#define	USE_ANIM_FILE

#include "/video/tools/wan2seq/lookup.h"
#include "/video/nfl/include/game.h"
#include "/video/nfl/include/player.h"
#include "/video/nfl/include/CamBot.h"
#include "/video/nfl/include/Anim.h"

#ifdef DEBUG
#define ANI_DEBUG	TRUE
#else
#define	ANI_DEBUG	0
#endif

#define GETSHORT(x)		(((int)(pani->pscript[x+0]) & 0xff ) | \
						((((int)(pani->pscript[x+1]))<<8) & 0xff00))

#define GETWORD(x)		(((int)(pani->pscript[x+0]) & 0xff) | \
						((((int)(pani->pscript[x+1]))<<8) & 0xff00) | \
						((((int)(pani->pscript[x+2]))<<16) & 0xff0000) | \
						(((int)(pani->pscript[x+3]))<<24))

#define	FRELX(x,z,t)	(z * isin(t) + x * icos(t))
#define	FRELZ(x,z,t)	(z * icos(t) - x * isin(t))

/* animation block pointers */
char *anim_blocks[ASB_LAST+1];

#ifdef SEATTLE
extern int tick_counter;
#endif
extern char *seq_names[];
extern ball_data	ball_obj;

//////////////////////////////////////////////////////////////////////////////
typedef union intfloat
{
	int		i;
	float	f;
} intfloat_t;

//////////////////////////////////////////////////////////////////////////////
// animation command functions
static int _ani_nop(obj_3d *, anidata * );
static int _ani_end(obj_3d *, anidata * );
static int _ani_newseq(obj_3d *, anidata * );
static int _ani_skipframes(obj_3d *, anidata * );
static int _ani_setmode(obj_3d *, anidata * );
static int _ani_goto(obj_3d *, anidata * );
static int _ani_waittrue(obj_3d *, anidata * );
static int _ani_code(obj_3d *, anidata * );
static int _ani_zerovels(obj_3d *, anidata * );
static int _ani_setplyrmode(obj_3d *, anidata * );
static int _ani_setflag(obj_3d *, anidata * );
static int _ani_clrflag(obj_3d *, anidata * );
static int _ani_inttoseq(obj_3d *, anidata * );
static int _ani_rndwait(obj_3d *, anidata * );
static int _ani_runtoend(obj_3d *, anidata * );
static int _ani_waithitgnd(obj_3d *, anidata * );
static int _ani_waittminus(obj_3d *, anidata * );
static int _ani_intstream(obj_3d *, anidata * );
static int _ani_zeroxzvels(obj_3d *, anidata * );
static int _ani_setvels(obj_3d *, anidata * );
static int _ani_setyvel(obj_3d *, anidata * );
static int _ani_runtoframe(obj_3d *, anidata * );
static int _ani_soundcall(obj_3d *, anidata * );
static int _ani_setfriction(obj_3d *, anidata * );
static int _ani_setcambot(obj_3d *, anidata * );
static int _ani_set_face_rel_vel(obj_3d *, anidata * );
static int _ani_streamtoend(obj_3d *, anidata * );
static int _ani_bz(obj_3d *, anidata * );
static int _ani_bnz(obj_3d *, anidata * );
static int _ani_jifseq(obj_3d *, anidata * );
static int _ani_jifnseq(obj_3d *, anidata * );
static int _ani_jifmode(obj_3d *, anidata * );
static int _ani_prep_xition(obj_3d *, anidata * );
static int _ani_attack_on(obj_3d *, anidata * );
static int _ani_attack_off(obj_3d *, anidata * );
static int _ani_turnhack(obj_3d *, anidata * );
static int _ani_shaker(obj_3d *, anidata * );
static int _ani_leapatt(obj_3d *, anidata * );
static int _ani_code_bnz(obj_3d *, anidata * );
static int _ani_code_bz(obj_3d *, anidata * );
static int _ani_leapat_ball(obj_3d *, anidata * );
static int _ani_leapat_tgtx(obj_3d *, anidata * );
static int _ani_runtoframe16(obj_3d *, anidata * );


static void _next_frame( obj_3d*, anidata *, int );
static void _animate_block( obj_3d *, anidata * );
static void _interpolate_frame( obj_3d *, anidata * );
static void _stream_frame( obj_3d *, anidata * );
static void set_puppet_pos( obj_3d *, obj_3d * );
static int _mid_angle( int, int );

static int interpolate_hack;

typedef int (*PANIF)(obj_3d *, anidata *);
PANIF ani_functions[256] =
{
	_ani_nop,			_ani_end,			_ani_newseq,		_ani_skipframes,
	_ani_setmode,		_ani_goto,			_ani_waittrue,		_ani_code,
	_ani_zerovels,		_ani_setplyrmode,	_ani_setflag,		_ani_clrflag,
	_ani_inttoseq,		_ani_rndwait,		_ani_runtoend,		_ani_waithitgnd,
	_ani_waittminus,	_ani_intstream,		_ani_zeroxzvels,	_ani_setvels,
	_ani_setyvel,		_ani_runtoframe,	_ani_soundcall,		_ani_setfriction,
	_ani_setcambot,		_ani_set_face_rel_vel, _ani_streamtoend,_ani_bz,
	_ani_bnz,			_ani_jifseq,		_ani_jifnseq,		_ani_jifmode,
	_ani_prep_xition,	_ani_attack_on,		_ani_attack_off,	_ani_turnhack,
	_ani_shaker,		_ani_leapatt,		_ani_code_bnz,		_ani_leapat_ball,
	_ani_code_bz, 		_ani_leapat_tgtx, 	_ani_runtoframe16,	 NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL,

	NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL,
};

//////////////////////////////////////////////////////////////////////////////
// anim_sys_init - (Re)initialize the animation system.  Free any currently
// loaded anim blocks.
//////////////////////////////////////////////////////////////////////////////
void anim_sys_init( void )
{
	int		i;

	/* initialize anim_block pointers */
	for( i = 0; i < ASB_LAST+1; i++ )
	{
		if( anim_blocks[i] != NULL )
			JFREE( anim_blocks[i] );

		anim_blocks[i] = NULL;
	}

#if DEBUG
	fprintf(stderr,  "libAni3D: Anim system initialized.\r\n" );
#endif
}

#ifndef USE_ANIM_FILE
extern	int	anim_data[];
#endif

//////////////////////////////////////////////////////////////////////////////
// load_anim_block() - Load animation data from 'file' into anim block index
// 'block.'  Free the block stored at that index if it's already loaded.
// Return 0 is successful, nonzero otherwise.
//////////////////////////////////////////////////////////////////////////////
int load_anim_block( int block, char *file, int version )
{
#ifdef USE_ANIM_FILE
	FILE	*fp;
	int		vstamp;
	struct ffblk ffblk;
	int		stored_crc, generated_crc;
	int		retry = 4;

	/* dump the old block if something's already loaded here */
	if( anim_blocks[block] != NULL )
		free( anim_blocks[block] );

	if(findfirst(file, &ffblk, 0))
	{
		fprintf(stderr, "libAni3D: Error file %s does not exist.\r\n", file);
#ifdef DEBUG
		lockup();
#else
		return(1);
#endif
	}

try_again:
	if( retry < 4 )
	{
		if( anim_blocks[block] )
			free( anim_blocks[block] );
		anim_blocks[block] = NULL;
#ifndef VEGAS
		flush_disk_cache();
		flush_disk_queue();
#endif
	}

	fp = fopen( file, "rb" );
	if( fp == NULL )
	{
		fprintf(stderr,  "libAni3D: Error opening file %s for reading.\r\n", file );
#ifdef DEBUG
		lockup();
#else
		return(1);
#endif
	}

	fread( &stored_crc, 1, sizeof( int ), fp );

	// no storage for CRC
	anim_blocks[block] = (char *)JMALLOC( ffblk.ff_fsize - sizeof( int ));

	if( anim_blocks[block] == NULL )
	{
		fprintf(stderr,  "libAni3D: Error allocating ram for file %s.\r\n", file );
#ifdef DEBUG
		lockup();
#else
		return(1);
#endif
	}

#ifdef DEBUG
	fprintf(stderr,  "libAni3D: Reading anim data into block %d from file %s...",
			block, file );
#endif

	fread( (void *)anim_blocks[block], 1, ffblk.ff_fsize - sizeof(int), fp );
	vstamp = *((int *)anim_blocks[block]);
	fclose( fp );

	generated_crc = crc( anim_blocks[block], ffblk.ff_fsize - sizeof(int));

	if( generated_crc != stored_crc )
	{
		if( retry )
		{
			fprintf( stderr, "Failed CRC check on file %s, retry #%d\n",
				file, 5-retry );
			retry--;
			goto try_again;
		}

		// no retries left.  Very bad.
		fprintf( stderr, "Aborting file load %s\n", file );
		if( anim_blocks[block] )
			free( anim_blocks[block] );
		
		return 1;
	}

	if( vstamp != version )
	{
#ifdef DEBUG
		fprintf( stderr, "failed.\n" );
#endif
		fprintf( stderr, "libAni3D: - Version mismatch on file %s\r\nlibAni"
					"3D: - Expected 0x%X, read 0x%X\r\n", file, version, vstamp );
		free( anim_blocks[block] );
		anim_blocks[block] = NULL;
		return 1;
	}

#ifdef DEBUG
	fprintf(stderr,  "done.\r\n" );
#endif

#else // USE_ANIM_FILE
	int						vstamp;

	// Print a message
#ifdef DEBUG
	fprintf(stderr,  "libAni3D: EMBEDDED ANIM BLOCK INITIALIZATION...");
#endif

	// Get pointer to animimation data
	anim_blocks[block] = (char *)&anim_data[1];

	// Grab the version stamp from the data
	vstamp = anim_data[0];

	// Version match ?
	if(vstamp != version)
	{
		// NOPE
#ifdef DEBUG
		fprintf(stderr,  "failed.\nlibAni3D: - Version mismatch on anim data\r\n"
				"libAni3D: - Expected 0x%X, read 0x%X\r\n"
				"libAni3D: - MAKE SURE YOU'VE REBUILT the ANI3D LIBRARY\r\n", version, vstamp );
#endif
		anim_blocks[block] = NULL;
		return 1;
	}

	// Show status
#ifdef DEBUG
	fprintf(stderr,  "done.\r\n" );
#endif

#endif // USE_ANIM_FILE

	// Retur OK status
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// change_anim() - Change first anim block script - wipes low word bits on other half
//////////////////////////////////////////////////////////////////////////////
void change_anim( obj_3d *pobj, char *pscript )
{
	anidata *pani,*pani2;

	pani = pobj->adata;
	pani2 = pobj->adata + 1;

	// check for script tag
#if DEBUG
	if( pscript[-1] != (char)SCRIPT )
	{
		fprintf(stderr,  "bad change_anim( %x )\r\n", (int)pscript );
		lockup();
	}
#endif

	interpolate_hack = !!(pani->animode & MODE_INTERP);

	pani->pscript = pscript;
	pani->pscrhdr = pscript;
	pani->animode &= 0xFFFF0000;		// clear the low word
	pani2->animode &= 0xFFFF0000;		// clear the low word for torso
	pani->animode &= ~MODE_INTERP;		// clear interpolation bit
	pani->anicnt = 1;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "change_anim to %x\r\n", (int)pscript );
#endif

	// animate to frame break
	_animate_block( pobj, pobj->adata );

	pobj->flags |= PF_ANITICK;
}

//////////////////////////////////////////////////////////////////////////////
// change_anim1() - Change first anim block script (DOESN'T MESS WITH OTHER BLOCK)
//////////////////////////////////////////////////////////////////////////////
void change_anim1( obj_3d *pobj, char *pscript )
{
	anidata *pani = pobj->adata;

	// check for script tag
#if DEBUG
	if( pscript[-1] != (char)SCRIPT )
	{
		fprintf(stderr,  "bad change_anim1( %x )\r\n", (int)pscript );
		lockup();
	}
#endif

	interpolate_hack = !!(pani->animode & MODE_INTERP);

	pani->pscript = pscript;
	pani->pscrhdr = pscript;
	pani->animode &= 0xFFFF0000;		// clear the low word
	pani->animode &= ~MODE_INTERP;		// clear interpolation bit
	pani->anicnt = 1;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "change_anim1 to %x\r\n", (int)pscript );
#endif

	// animate to frame break
	_animate_block( pobj, pobj->adata );

	pobj->flags |= PF_ANITICK;
}

//////////////////////////////////////////////////////////////////////////////
// change_anim2() - Change second anim block script
//////////////////////////////////////////////////////////////////////////////
void change_anim2( obj_3d *pobj, char *pscript )
{
	anidata *pani = pobj->adata + 1;

	// check for script tag
	if( pscript[-1] != (char)SCRIPT )
	{
#if DEBUG
		fprintf(stderr,  "bad change_anim2( %x )\r\n", (int)pscript );
		lockup();
#else
		return;
#endif	
	}

	// make sure we're initialized
	if (!(pani->animode & MODE_TWOPART))
	{
#if DEBUG
		fprintf( stderr, "bad change_anim2, player not in twopart mode\n" );
		lockup();
#else
		return;
#endif
	}

	interpolate_hack = !!(pani->animode & MODE_INTERP);

	pani->pscript = pscript;
	pani->pscrhdr = pscript;
	pani->animode &= 0xFFFF0000;		// clear the low word
	pani->animode &= ~MODE_INTERP;		// clear interpolation bit
	pani->anicnt = 1;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "change_anim2 to %x\r\n", (int)pscript );
#endif

	// animate to frame break
	_animate_block( pobj, pobj->adata+1 );

	pobj->flags |= PF_ANITICK;
}

//////////////////////////////////////////////////////////////////////////////
// animate_model() - Process pobjs script(s) until a frame break.
//////////////////////////////////////////////////////////////////////////////
void animate_model( obj_3d *pobj )
{
#if ANI_DEBUG
	if( pobj->adata[0].animode & MODE_ANIDEBUG )
	{
#ifdef SEATTLE
		fprintf(stderr,  "TICK %d", tick_counter );
#endif
		if( pobj->y > 0.0f )
			fprintf(stderr,  "vy: %9f y: %9f\r\n", pobj->vy, pobj->y );
		else
			fprintf(stderr,  "\r\n" );
		if( pobj->adata[0].animode & MODE_TWOPART )
			fprintf(stderr,  " legs:\r\n" );
	}

#endif
	_animate_block( pobj, pobj->adata );

	if( pobj->adata[1].animode & MODE_TWOPART )
	{
#if ANI_DEBUG
		if( pobj->adata[0].animode & MODE_ANIDEBUG )
			fprintf(stderr,  " torso:\r\n" );
#endif
		_animate_block( pobj, pobj->adata+1 );
	}
}

//////////////////////////////////////////////////////////////////////////////
static void _animate_block( obj_3d *pobj, anidata *pani )
{
	signed char cmd;
	PANIF pfun;

	/* just hold if we're in ANI_END */
	if( pani->animode & MODE_END )
		return;

	if(( pani->anicnt -= 1 ))
		return;

	if( pani->animode & MODE_INTERP )
		_interpolate_frame( pobj, pani );

	if( pani->animode & MODE_INTSTREAM )
		_stream_frame( pobj, pani );

	while( !(pani->anicnt) && !(pani->animode & MODE_END))
	{
		cmd = *(pani->pscript++);

		if( cmd > 0 )
		{
			_next_frame( pobj, pani, cmd );
		}
		else
		{
			pfun = ani_functions[(cmd & 0x7F)];

			if( !pfun )
			{
#ifdef DEBUG
				fprintf(stderr,  "Illegal ANI command: %x\r\n", cmd );
				lockup();			/* Hey! that's not a legal function! */
#else
				while(1) ;
#endif
			}

			pfun( pobj, pani );
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// _next_frame() - go to next frame in sequence and hold for count ticks.
//////////////////////////////////////////////////////////////////////////////
static void _next_frame( obj_3d *pobj, anidata *pani, int count )
{
	unsigned char	*dp;
	int				i;
	float			*fp;
	float			dx,dz;
	fbplyr_data		*ppdata = (fbplyr_data *)pobj;

	pani->iframe += 1;

	if( pani->iframe >= pani->pahdr->nframes )
	{
#ifdef DEBUG
		fprintf(stderr,  "sequence out of frames.\r\n" );
		fflush(stdout);
		sleep(1);
		lockup();					/* Ran out of frames! */
#else
		while(1) ;
#endif
	}

	if( pani->pahdr->flags & AH_COMPRESSED )
	{
		if( pani->iframe )
		{
			dp = pani->pframe + (pani->iframe-1) * 4 * pani->pahdr->nlimbs;
			i = 4 * pani->pahdr->nlimbs;
			fp = pani->adata;
			while( i-- )
				fp[i] += lookup[dp[i]];
		}
	}
	else
	{
		pani->pq = (float *)(pani->pframe) + (pani->iframe-1) * 4 * pani->pahdr->nlimbs;
	}

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  advance to seq:frame %s:%d (%x)\r\n", seq_names[pani->seq_index], pani->iframe, (int)pani->pq );
#endif

	pani->anicnt = count;

	// if we're a puppet, set our position and quit
	if( pani->animode & MODE_PUPPET )
	{
		set_puppet_pos( (obj_3d *)(ppdata->puppet_link), pobj );
		return;
	}

	// If we're a secondary animation, don't bother with any anipt stuff.
	if( pani->animode & MODE_SECONDARY )
		return;

	// adjust world position based on x and z anipt drift, but not
	// if we're in MODE_ANCHORXZ
	if( pani->animode & MODE_ANCHORXZ )
	{
		// mode anchor gets cleared when it first takes effect
		pani->animode &= ~MODE_ANCHORXZ;
	#if ANI_DEBUG
		if( pani->animode & MODE_ANIDEBUG )
			fprintf(stderr,  "  anchor XZ bit applied & cleared\r\n" );
	#endif
	}
	else
	{
	   	dx = pobj->ascale * pani->pxlate[3*pani->iframe] - pobj->ax;
		dz = pobj->ascale * pani->pxlate[3*pani->iframe+2] - pobj->az;
	   	pobj->x += dz * isin( pobj->fwd ) + dx * icos( pobj->fwd );
	   	pobj->z += dz * icos( pobj->fwd ) - dx * isin( pobj->fwd );
	}

	if( pani->animode & MODE_ANCHORY )
	{
		pani->animode &= ~MODE_ANCHORY;
		pobj->y += pobj->ay - pobj->ascale * pani->pxlate[3*pani->iframe+1];

		// don't set y to less than zero
		if( pobj->y < 0.0f )
			pobj->y = 0.0f;

#ifdef DEBUG
		if( pani->animode & MODE_ANIDEBUG )
			fprintf(stderr,  "  anchor Y bit applied & cleared\r\n" );
#endif
	}

	pobj->ax = pobj->ascale * pani->pxlate[3*pani->iframe];
	pobj->ay = pobj->ascale * pani->pxlate[3*pani->iframe+1];
	pobj->az = pobj->ascale * pani->pxlate[3*pani->iframe+2];

	// if we're a puppeteer, set victim position
	if( pani->animode & MODE_PUPPETEER )
		set_puppet_pos( pobj, (obj_3d *)(ppdata->puppet_link) );
}

//////////////////////////////////////////////////////////////////////////////
static void set_puppet_pos( obj_3d *att, obj_3d *vic )
{
	anidata	*aani,*vani;
	float	offx,offz;

	if( !att || !vic )
	{
#ifdef DEBUG
		fprintf( stderr, "Bogus puppet link\n" );
		lockup();
#else
		return;
#endif
	}

	aani = att->adata;
	vani = vic->adata;

	vic->fwd = att->fwd;
	vic->tgt_fwd = att->tgt_fwd;

	vic->ax = vic->ascale * vani->pxlate[3*vani->iframe];
	vic->ay = vic->ascale * vani->pxlate[3*vani->iframe+1];
	vic->az = vic->ascale * vani->pxlate[3*vani->iframe+2];

	offx = vic->ax - att->ax;
	offz = vic->az - att->az;

	vic->x = att->x + FRELX( offx, offz, att->fwd );
	vic->z = att->z + FRELZ( offx, offz, att->fwd );
	vic->y = att->y;
	
	// victim vels match attacker
	// they're never actually applied, but we need them
	// for stuff like hit-ground-time and intercept calculations.
	vic->vx = att->vx;
	vic->vy = att->vy;
	vic->vz = att->vz;
}

//////////////////////////////////////////////////////////////////////////////
static void _interpolate_frame( obj_3d *pobj, anidata *pani )
{
	float	s,f,t;
	anidata *pani2 = pani+2;
	float	iy;
	int		i;

	pani->icnt++;

	pani->anicnt = 1;			// to end the loop in animate_block

	if( pani->icnt >= pani2->icnt )
	{
//		// done interpolating.  begin regular animation at dest frame
//		pani->anicnt = 0;
		// done interpolating.  hold target frame for 1 tick

		if( pani->pahdr->flags & AH_COMPRESSED )
		{
			if( pani2->pahdr->flags & AH_COMPRESSED )
			{
				// from compressed to compressed
				// copy frame data
				for( i = 0; i < 4 * pani->pahdr->nlimbs; i++ )
					pani->pq[i] = pani2->pq[i];
			}
			else
			{
				// from compressed to uncompressed
				pani->pq = pani2->pq;
			}
		}
		else
		{
			if( pani2->pahdr->flags & AH_COMPRESSED )
			{
				// from uncompressed to compressed
				pani->pq = pani->adata;
				// copy frame data
				for( i = 0; i < 4 * pani->pahdr->nlimbs; i++ )
					pani->pq[i] = pani2->pq[i];
			}
			else
			{
				// from uncompressed to uncompressed
				pani->pq = pani2->pq;
			}
		}

		pani->pxlate = pani2->pxlate;
		pani->pframe0 = pani2->pframe0;
		pani->pframe = pani2->pframe;
		pani->pahdr = pani2->pahdr;
		pani->animode &= ~(MODE_INTERP|MODE_ANCHORY);
		pani->iframe = pani2->iframe;
		pani->icnt = 0;
#if ANI_DEBUG
		if( pani->animode & MODE_ANIDEBUG )
			fprintf( stderr,  "  done interpolating\r\n" );
#endif
		return;
	}

	t = (float)(pani->icnt) / (float)(pani2->icnt);

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf( stderr,  "  interpolation frame %d, t=%f, (from %s:%d to %s:%d)\r\n",
			pani->icnt-1, t,
			seq_names[pani->seq_index2], pani->iframe, seq_names[pani2->seq_index],
			pani2->iframe );
#endif

	// interpolate limb data
	for( i = 0; i < pani->pahdr->nlimbs; i++ )
	{
		slerp( pani->idata + 4*i, pani2->pq + 4*i, t, pani->adata + 4*i );
	}

	// interpolate anim points (not if we're the torso, though)
	if( pani->animode & MODE_SECONDARY )
		return;

	if( pani->animode & MODE_ANCHORY )
	{
		pobj->ay = pobj->ascale * pani->pxlate[(3*pani->iframe)+1];
	}
	else
	{
		s = pobj->ascale * pani->pxlate[(3*pani->iframe)+1];
		f = pobj->ascale * pani2->pxlate[(3*pani2->iframe)+1];
		iy = s + ((f-s)*t);
		pobj->ay = iy;
	}

}

//////////////////////////////////////////////////////////////////////////////
static void _stream_frame( obj_3d *pobj, anidata *pani )
{
	float	t,s,f,ix,iy,iz,dx,dz;
	float	*f1, *f2, *fp;
	int		i;
	int		oframe;
	float	dbuf[60];
	unsigned char *dp;
	fbplyr_data		*ppdata = (fbplyr_data *)pobj;

	// see note in ani_intstream for explanation of this weirdness
	if(( pani->iframe != -1 ) || ( pani->sinc >= 1.0f ))
		pani->icnt++;

	oframe = pani->iframe;

	if( pani->icnt > pani->scnt )
	{
		// done streaming
		pani->animode &= ~MODE_INTSTREAM;
		pani->iframe = (int)(pani->fframe + 0.5f);
		if( pani->pahdr->flags & AH_COMPRESSED )
		{
			pani->pq = pani->adata;

			// here decompress next frame if neccesary
			while( pani->iframe > oframe )
			{
				oframe++;

				if( oframe )
				{
					dp = pani->pframe + (oframe-1) * 4 * pani->pahdr->nlimbs;
					i = 4 * pani->pahdr->nlimbs;
					fp = pani->adata;
					while( i-- )
						fp[i] += lookup[dp[i]];
				}
			}
		}
		else
			pani->pq = (float *)(pani->pframe) + (pani->iframe-1) * 4 * pani->pahdr->nlimbs;
		pani->anicnt = 0;
		return;
	}

	// advance frame counter
	if(( pani->iframe == -1 ) && ( pani->sinc < 1.0f ))
		pani->fframe = 0.0f;
	else
	{
		pani->fframe = (float)((int)((pani->fframe + pani->sinc) * 10000.0f) / 10000.0f);

	}

	if( pani->fframe > (float)(pani->pahdr->nframes-1))
	{
#ifdef DEBUG
		fprintf( stderr, "Error: Interpolating to frame %f in %d-frame s"
				"equence: %s.\n", pani->fframe, pani->pahdr->nframes,
				seq_names[pani->seq_index] );
		fflush(stdout);
		sleep(1);
		lockup();
#endif
		pani->fframe = (float)(pani->pahdr->nframes-1);
	}

	pani->iframe = (int)(pani->fframe);
	t = pani->fframe - (float)(pani->iframe);
	pani->anicnt = 1;

	if( pani->pahdr->flags & AH_COMPRESSED )
	{
		// if iframe > oframe, decompress more frames
		while( pani->iframe > oframe )
		{
			oframe++;

			if( oframe )
			{
				dp = pani->pframe + (oframe-1) * 4 * pani->pahdr->nlimbs;
				i = 4 * pani->pahdr->nlimbs;
				fp = pani->adata;
				while( i-- )
					fp[i] += lookup[dp[i]];
			}
		}
	}

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  streaming tick %d of %d, seq:frame %s:%f", pani->icnt,
				pani->scnt, seq_names[pani->seq_index], pani->fframe );
#endif

	// if t is 0, then we're right on a frame and don't need to interpolate
	if( fabs(t) < 0.05f )
	{
#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  " (no interpolation)\r\n" );
#endif
		if( pani->pahdr->flags & AH_COMPRESSED )
			pani->pq = pani->adata;
		else
			pani->pq = (float *)(pani->pframe) + (pani->iframe-1) * 4 * pani->pahdr->nlimbs;
		ix = pobj->ascale * pani->pxlate[3*pani->iframe];
		iy = pobj->ascale * pani->pxlate[3*pani->iframe+1];
		iz = pobj->ascale * pani->pxlate[3*pani->iframe+2];
	}
	else
	{
#if ANI_DEBUG
		if( pani->animode & MODE_ANIDEBUG )
			fprintf(stderr,  "\r\n" );
#endif
		pani->pq = pani->idata;
		if( pani->pahdr->flags & AH_COMPRESSED )
		{
			f1 = pani->adata;
			f2 = dbuf;
			dp = pani->pframe + (pani->iframe-1+1) * 4 * pani->pahdr->nlimbs;
			i = 4 * pani->pahdr->nlimbs;
			while( i-- )
				f2[i] = f1[i] + lookup[dp[i]];

#ifdef DEBUG
			if( pani->animode & MODE_ANIDEBUG )
				fprintf( stderr, "seq: %s fframe: %9f\nstream frames %d and %d:\n",
						seq_names[pani->seq_index], pani->fframe, pani->iframe, pani->iframe+1 );
#endif

		}
		else
		{
			f1 = (float *)(pani->pframe) + (pani->iframe-1) * 4 * pani->pahdr->nlimbs;
			f2 = (float *)(pani->pframe) + (pani->iframe) * 4 * pani->pahdr->nlimbs;
		}

		// interpolate frame data
		for( i = 0; i < pani->pahdr->nlimbs; i++ )
		{
			slerp( f1 + 4*i, f2 + 4*i, t, pani->idata + 4*i );
		}

#if 0
		if( pani->pahdr->flags & AH_COMPRESSED )
		if( pani->animode & MODE_ANIDEBUG )
		{
			float *pq;

			pq = f1;
			fprintf( stderr, "src:\r\n" );
			for( i = 0; i < 15; i++ )
				fprintf( stderr, "%9f %9f %9f %9f\n",
					pq[4*i+0],pq[4*i+1],pq[4*i+2],pq[4*i+3]);
			pq = f2;
			fprintf( stderr, "dest:\r\n" );
			for( i = 0; i < 15; i++ )
				fprintf( stderr, "%9f %9f %9f %9f\n",
					pq[4*i+0],pq[4*i+1],pq[4*i+2],pq[4*i+3]);
			pq = pani->idata;
			fprintf( stderr, "result at t=%f:\r\n", t );
			for( i = 0; i < 15; i++ )
				fprintf( stderr, "%9f %9f %9f %9f\n",
					pq[4*i+0],pq[4*i+1],pq[4*i+2],pq[4*i+3]);
		}
#endif
		// interpolate anim points
		s = pobj->ascale * pani->pxlate[3*pani->iframe];
		f = pobj->ascale * pani->pxlate[3*pani->iframe+3];
		ix = s + ((f-s)*t);

		s = pobj->ascale * pani->pxlate[3*pani->iframe+1];
		f = pobj->ascale * pani->pxlate[3*pani->iframe+4];
		iy = s + ((f-s)*t);

		s = pobj->ascale * pani->pxlate[3*pani->iframe+2];
		f = pobj->ascale * pani->pxlate[3*pani->iframe+5];
		iz = s + ((f-s)*t);
	}

	// If we're a secondary animation, don't bother with any anipt stuff.
	if( pani->animode & MODE_SECONDARY )
		return;

	// if we're a puppet, set our position and quit
	if( pani->animode & MODE_PUPPET )
	{
		set_puppet_pos( (obj_3d *)(ppdata->puppet_link), pobj );
		return;
	}

	// adjust world position based on x and z anipt drift, but not
	// if we're in MODE_ANCHORXZ
	if( pani->animode & MODE_ANCHORXZ )
	{
		// mode anchor gets cleared when it first takes effect
		pani->animode &= ~MODE_ANCHORXZ;
	#if ANI_DEBUG
		if( pani->animode & MODE_ANIDEBUG )
			fprintf(stderr,  "  anchor XZ bit applied & cleared\r\n" );
	#endif
	}
	else
	{
	   	dx = ix - pobj->ax;
		dz = iz - pobj->az;
	   	pobj->x += dz * isin( pobj->fwd ) + dx * icos( pobj->fwd );
	   	pobj->z += dz * icos( pobj->fwd ) - dx * isin( pobj->fwd );
	}

	if( pani->animode & MODE_ANCHORY )
	{
		pani->animode &= ~MODE_ANCHORY;
		pobj->y += pobj->ay - pobj->ascale * pani->pxlate[3*pani->iframe+1];

		// don't set y to less than zero
		if( pobj->y < 0.0f )
			pobj->y = 0.0f;
#ifdef DEBUG
		if( pani->animode & MODE_ANIDEBUG )
			fprintf(stderr,  "  anchor Y bit applied & cleared\r\n" );
#endif
	}

	pobj->ax = ix;
	pobj->ay = iy;
	pobj->az = iz;

	// if we're a puppeteer, set victim position
	if( pani->animode & MODE_PUPPETEER )
		set_puppet_pos( pobj, (obj_3d *)(ppdata->puppet_link) );

}

//////////////////////////////////////////////////////////////////////////////
// _ani_nop() - script function - do nothing.
//////////////////////////////////////////////////////////////////////////////
static int _ani_nop( obj_3d *pobj, anidata *pani )
{
#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_NOP\r\n" );
#endif
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_end() - script function - set animode to MODE_END and halt script
// execution.
//////////////////////////////////////////////////////////////////////////////
static int _ani_end( obj_3d *pobj, anidata *pani )
{
#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_END\r\n" );
#endif

	pani->animode |= MODE_END;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_newseq() - script function - Set pxlate, pframe0, pframe, iframe, and
// pahdr in object data block, then sets model on frame -1 and initializes cq
// array.  Sets anicnt.  Must call _next_frame once before attempting to
// display the player.
//////////////////////////////////////////////////////////////////////////////
static int _ani_newseq( obj_3d *pobj, anidata *pani )
{
	int		block, index, word;
	anihdr	*panihdr;
	int		i;
	float	*to, *from;

	word = (int)GETWORD(0);
	pani->pscript += 4;

	index = word & 0xffff;
	block = (word >> 16) & 0xffff;

	panihdr = (struct anim_header *)(anim_blocks[block]+4);

	pani->seq_index = index | (block<<16);
	pani->pxlate = (float *)((char *)panihdr + panihdr[index].offset);
	pani->pframe0 = pani->pxlate + panihdr[index].nframes * 3;
	pani->pframe = (char *)(pani->pframe0 + panihdr[index].nlimbs * 4);
	pani->iframe = -1;
	pani->pahdr = panihdr + index;
	pani->anicnt = 0;
	interpolate_hack = 0;

	if( panihdr[index].flags & AH_COMPRESSED )
	{
		pani->pq = pani->adata;
		i = 4 * panihdr[index].nlimbs;
		to = pani->adata;
		from = pani->pframe0;
		while( i-- )
			*to++ = *from++;

//		fprintf( stderr, "frame 0 of coompressed sequence %s:\r\n", seq_names[pani->seq_index] );
//		to = pani->adata;
//		for( i = 0; i < 15; i++ )
//			fprintf( stderr, "%9f %9f %9f %9f\r\n",
//				to[(i*4)+0],to[(i*4)+1],to[(i*4)+2],to[(i*4)+3]);
	}
	else
	{
		pani->pq = pani->pframe0;
	}

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_NEWSEQ: %s\r\n", seq_names[pani->seq_index] );
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_skipframes() - script function - Skip over frames in the current
// sequence.
//////////////////////////////////////////////////////////////////////////////
static int _ani_skipframes( obj_3d *pobj, anidata *pani )
{
	int count;
	int	arg, base;

	count = (int)(pani->pscript[0]);
	pani->pscript += 1;

	if( count == AA_RAND )
	{
		arg = (int)(pani->pscript[0]);
		base = (int)(pani->pscript[1]);
		pani->pscript += 2;

		count = base + randrng(arg);
#if ANI_DEBUG
		if( pani->animode & MODE_ANIDEBUG )
			fprintf(stderr,  "  ANI_SKIPFRAMES(%d+randrng(%d) = %d)\r\n",
					base, arg, count );
#endif
	}
	else
	{
#if ANI_DEBUG
		if( pani->animode & MODE_ANIDEBUG )
			fprintf(stderr,  "  ANI_SKIPFRAMES(%d)\r\n", count );
#endif
	}

	if( pani->pahdr->flags & AH_COMPRESSED )
	{
		while( count-- )
			_next_frame(pobj, pani, 0 );

	}
	else
	{
		pani->iframe += (count-1);
		_next_frame(pobj, pani, 0 );
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_setmode() - script function - Set bits in animode field.
//////////////////////////////////////////////////////////////////////////////
static int _ani_setmode( obj_3d *pobj, anidata *pani )
{
	int		word;

	word = ((int)(pani->pscript[0]) & 0xff) |
		   ((((int)(pani->pscript[1]))<<8) & 0xff00) |
		   ((((int)(pani->pscript[2]))<<16) & 0xff0000) |
		   (((int)(pani->pscript[3]))<<24);
	pani->pscript += 4;

	pani->animode = (pani->animode & 0xFFFF0000 ) | word;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_SETMODE +%x = %x\r\n", word, pani->animode );
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_goto() - script function - Jump to address.
//////////////////////////////////////////////////////////////////////////////
static int _ani_goto( obj_3d *pobj, anidata *pani )
{
	int addr;
	addr = ((int)(pani->pscript[0]) & 0xff) |
		   ((((int)(pani->pscript[1]))<<8) & 0xff00) |
		   ((((int)(pani->pscript[2]))<<16) & 0xff0000) |
		   (((int)(pani->pscript[3]))<<24);

	pani->pscript = (char *)addr;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_GOTO(%x)\r\n", addr );
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_waittrue() - script function - Hold until function returns TRUE.
//////////////////////////////////////////////////////////////////////////////
static int _ani_waittrue( obj_3d *pobj, anidata *pani )
{
	int word;
	int (*func)( obj_3d * );

	word = ((int)(pani->pscript[0]) & 0xff) |
		   ((((int)(pani->pscript[1]))<<8) & 0xff00) |
		   ((((int)(pani->pscript[2]))<<16) & 0xff0000) |
		   (((int)(pani->pscript[3]))<<24);

	func = (int(*)(obj_3d *))word;

	if( func( pobj ) == TRUE)
	{
		pani->pscript += 4;
	}
	else
	{
		pani->pscript -= 1;		// back up to ANI_WAITTRUE cmd
		pani->anicnt = 1;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_code() - script function - Call function with 4 word args.  Set
// MODE_ZERO bit if function returns zero, clear it otherwise.
//////////////////////////////////////////////////////////////////////////////
static int _ani_code( obj_3d *pobj, anidata *pani )
{
	int word, ret;
	int arg0, arg1, arg2;
	int (*func)( obj_3d *, int, int, int );

	word = ((int)(pani->pscript[0]) & 0xff) |
		   ((((int)(pani->pscript[1]))<<8) & 0xff00) |
		   ((((int)(pani->pscript[2]))<<16) & 0xff0000) |
		   (((int)(pani->pscript[3]))<<24);

	func = (int(*)(obj_3d *, int, int, int))word;

	arg0 = ((int)(pani->pscript[4]) & 0xff) |
		   ((((int)(pani->pscript[5]))<<8) & 0xff00) |
		   ((((int)(pani->pscript[6]))<<16) & 0xff0000) |
		   (((int)(pani->pscript[7]))<<24);

	arg1 = ((int)(pani->pscript[8]) & 0xff) |
		   ((((int)(pani->pscript[9]))<<8) & 0xff00) |
		   ((((int)(pani->pscript[10]))<<16) & 0xff0000) |
		   (((int)(pani->pscript[11]))<<24);

	arg2 = ((int)(pani->pscript[12]) & 0xff) |
		   ((((int)(pani->pscript[13]))<<8) & 0xff00) |
		   ((((int)(pani->pscript[14]))<<16) & 0xff0000) |
		   (((int)(pani->pscript[15]))<<24);

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_CODE: %x(%d,%d,%d)\r\n",
				word, arg0, arg1, arg2 );
#endif

	pani->pscript += 16;
	ret = func( pobj, arg0, arg1, arg2 );

	if( ret )
		pani->animode &= ~MODE_ZERO;
	else
		pani->animode |= MODE_ZERO;

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_zerovels() - script function - Zero velocities.
//////////////////////////////////////////////////////////////////////////////
static int _ani_zerovels( obj_3d *pobj, anidata *pani )
{
	pobj->vx = 0.0f;
	pobj->vy = 0.0f;
	pobj->vz = 0.0f;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_ZEROVELS\r\n" );
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_setplyrmode() - script function - Set player mode
//////////////////////////////////////////////////////////////////////////////
static int _ani_setplyrmode( obj_3d *pobj, anidata *pani )
{
#if 0
	int mode;

	mode = (int)(pani->pscript[0]);
	pobj->plyrmode = mode;

	pani->pscript += 1;
#endif
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_setflag() - script function - OR word with player flags
//////////////////////////////////////////////////////////////////////////////
static int _ani_setflag( obj_3d *pobj, anidata *pani )
{
	int mask;

	mask = ((int)(pani->pscript[0]) & 0xff) |
		   ((((int)(pani->pscript[1]))<<8) & 0xff00) |
		   ((((int)(pani->pscript[2]))<<16) & 0xff0000) |
		   (((int)(pani->pscript[3]))<<24);

	pobj->flags |= mask;

	pani->pscript += 4;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_clrflag() - script function - ANDN word with player flags
//////////////////////////////////////////////////////////////////////////////
static int _ani_clrflag( obj_3d *pobj, anidata *pani )
{
	int mask;

	mask = ((int)(pani->pscript[0]) & 0xff) |
		   ((((int)(pani->pscript[1]))<<8) & 0xff00) |
		   ((((int)(pani->pscript[2]))<<16) & 0xff0000) |
		   (((int)(pani->pscript[3]))<<24);

	pobj->flags &= ~mask;

	pani->pscript += 4;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_inttoseq() - script function - Interpolate char ticks to
// sequence word, frame char
//////////////////////////////////////////////////////////////////////////////
static int _ani_inttoseq( obj_3d *pobj, anidata *pani )
{
	int		ticks, frame, prev;
	anidata	*pani2 = pani + 2;
	float	t;
	float	*p, *q, *d, *fp;
	float	pdq, npdq;
	int		i, j;
	float	fq[4], pp[4];
	unsigned char	*dp;
//	int		ihack = FALSE;

	prev = pani->seq_index;

	if( interpolate_hack )
	{	// we're interrupting another interpolation
		// if the old interpolation was more than half finished, set cur seq
		// and frame to the old dest.  Otherwise set to old src.
//		ihack = TRUE;

//		fprintf( stderr, "int a: %d %d %d %d %d %d %d\n",
//			pani->icnt, pani2->icnt,
//			pani->seq_index, pani->iframe, pani->seq_index2,
//			pani2->seq_index, pani2->iframe );

		if (pani->icnt > (pani2->icnt/2))
		{	// more than half finished
			pani->iframe = pani2->iframe;
		}
		else
		{	// not more than half finished
			pani->seq_index = pani->seq_index2;
		}

//		fprintf( stderr, "int b: %d %d %d %d %d %d %d\n",
//			pani->icnt, pani2->icnt,
//			pani->seq_index, pani->iframe, pani->seq_index2,
//			pani2->seq_index, pani2->iframe );
	}

	// read & parse script args
	ticks = (int)(pani->pscript[0]);
	pani->pscript += 1;
	pani2->pscript = pani->pscript;
	_ani_newseq( pobj, pani2 );

//	if( ihack )
//	fprintf( stderr, "int c: %d %d %d %d %d %d %d\n",
//		pani->icnt, pani2->icnt,
//		pani->seq_index, pani->iframe, pani->seq_index2,
//		pani2->seq_index, pani2->iframe );

	//  the situation now, just after _ani_newseq( pobj, pani2 ):
	//
	//						before (if not interpolating)			after the below assignments
	//	pani->seq_index		cur seq									dest seq
	//	pani->iframe		cur frame								src frame
	//	pani->seq_index2	-										src seq
	//	pani2->seq_index	dest seq								-
	//	pani2->iframe		dest frame								dest frame
	//	pani2->seq_index2	-
	//       Neither pani->seq_index2 nor pani->iframe change during the course
	//       of the interpolation, nor does either matter a whole lot apart from
	//       debug information.  For the convenience of the instant replay code,
	//       we're going to mess with these a bit.  IF a new inttoseq begins BEFORE
	//       the last inttoseq is MORE THAN HALF finished, leave seq_index2 alone,
	//       so that the replay code will think we're interpolating from the original
	//       source frame.  If a new inttoseq begins AFTER the previous inttoseq is
	//       MORE THAN HALF finished, but before it's entirely complete, set iframe
	//       to pani2->iframe, to make the replay code think we're interpolating from
	//       the original destination frame.  (If neither of these things is done, and
	//       one inttoseq interrupts another, the replay code will use the destination
	//       sequence, but the source frame index.  This is bad in any case, and
	//       probably fatal if the source frame index is higher than the destination
	//       frame count.)  This is the longest comment I've ever written.

	pani->seq_index2 = pani->seq_index;
	pani->seq_index = pani2->seq_index;
	pani2->iframe = 0;

//	if( ihack )
//	fprintf( stderr, "int d: %d %d %d %d %d %d %d\n",
//		pani->icnt, pani2->icnt,
//		pani->seq_index, pani->iframe, pani->seq_index2,
//		pani2->seq_index, pani2->iframe );

	pani->pscript = pani2->pscript;
	frame = (int)(pani->pscript[0]);
	pani->pscript += 1;

	// throw an error if we're in a puppet move.
	if( pani->animode & (MODE_PUPPET|MODE_PUPPETEER))
	{
		fprintf( stderr, "Hey! No using INTTOSEQ while in puppet mode.\n" );
		fprintf( stderr, "Offender is in interpolating from %s to %s\n",
				seq_names[prev], seq_names[pani->seq_index] );
		fprintf( stderr, "team_fire: %d %d\n",
				game_info.team_fire[0], game_info.team_fire[1] );
#ifdef DEBUG
		lockup();
#else
//		while(1) ;
#endif
	}

	if( frame == AA_VAR_FRAME )
		frame = pani->vframe;

	if( ticks == AA_VAR_COUNT )
		ticks = pani->vcount;

	// if frame is AA_PROPFRAME, do some special calculations.
	if( frame == AA_PROPFRAME )
	{
		t = (float)(pani->iframe) / (float)(pani->pahdr->nframes-1);
		t *= (float)(pani2->pahdr->nframes-1);

		i = (int)(t+0.5f) + ticks;
		frame = i % pani2->pahdr->nframes;
	}

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_INTTOSEQ: %d ticks to frame %d\r\n", ticks, frame );
#endif

	// init interpolation counts
	pani->icnt = 0;			// starting on 0
	pani2->icnt = ticks;	// and interpolate ticks frames

	// make sure that we're frame 0 or the sequence is uncompressed
	if( frame )
	{
		if( pani2->pahdr->flags & AH_COMPRESSED )
		{
			// decompress to target frame (shout a warning if this is too long)
#ifdef DEBUG
			if( frame > 25 )
				fprintf( stderr, "Warning: Interpolating to frame %d of sequen"
						"ce %s.  That's a long way.  We should maybe decompres"
						"s that sequence.\n", frame,
						seq_names[pani2->seq_index] );
#endif
			i = frame;
			while( i-- )
			{
				pani2->iframe += 1;
				dp = pani2->pframe + (pani2->iframe-1) * 4 * pani2->pahdr->nlimbs;
				j = 4 * pani2->pahdr->nlimbs;
				fp = pani2->adata;
				while( j-- )
					fp[j] += lookup[dp[j]];
			}
		}
		else
		{
			pani2->iframe += frame;
			pani2->pq = (float *)(pani2->pframe) + (pani2->iframe-1) * 4 * pani2->pahdr->nlimbs;
		}
	}

	// copy src data to interpolation src block.  Can't use original block
	// because it might be initialized script data, and while we CAN modify
	// that, it's bad karma.  And sometimes we need to use -q as a source
	// instead of q.

	p = pani->pq;
	q = pani2->pq;
	d = pani->idata;

	if( pani->animode & MODE_TURNHACK )
	{
		fq[0] = 0.0f;
		fq[1] = isin( pani->hackval/2 );
		fq[2] = 0.0f;
		fq[3] = icos( pani->hackval/2 );

		// flag a MODE_TURNHACK for replay recorder
		pani->hackval = (unsigned short)pani->hackval + RF_TURNHACK;

		pani->animode &= ~MODE_TURNHACK;

#if DEBUG
		if( pani->animode & MODE_ANIDEBUG )
			fprintf(stderr,  "  MODE_TURNHACK bit applied & cleared.\r\n" );
#endif
	}
	else
	{
		fq[0] = 0.0f;
		fq[1] = 0.0f;
		fq[2] = 0.0f;
		fq[3] = 1.0f;

		// be sure replay recorder isn't flagged a MODE_TURNHACK
		pani->hackval = (int)((short)pani->hackval);
	}

	qxq( fq, p, pp );

#if 0
	if( pani->animode & MODE_ANIDEBUG )
	{
		fprintf( stderr, "src quat: %f %f %f %f\n", p[0], p[1], p[2], p[3] );
		fprintf( stderr, "rot quat: %f %f %f %f\n", fq[0], fq[1], fq[2], fq[3] );
		fprintf( stderr, "dst quat: %f %f %f %f\n", pp[0], pp[1], pp[2], pp[3] );
	}
#endif

	pdq = (pp[0]-q[0])*(pp[0]-q[0]) +
			(pp[1]-q[1])*(pp[1]-q[1]) +
			(pp[2]-q[2])*(pp[2]-q[2]) +
			(pp[3]-q[3])*(pp[3]-q[3]);

	npdq = (pp[0]+q[0])*(pp[0]+q[0]) +
			(pp[1]+q[1])*(pp[1]+q[1]) +
			(pp[2]+q[2])*(pp[2]+q[2]) +
			(pp[3]+q[3])*(pp[3]+q[3]);

	if( pdq < npdq )
	{
		d[0] = pp[0]; d[1] = pp[1];
		d[2] = pp[2]; d[3] = pp[3];
	}
	else
	{
		d[0] = -pp[0]; d[1] = -pp[1];
		d[2] = -pp[2]; d[3] = -pp[3];
	}

	p += 4; d += 4; q += 4;

	for( i = 1; i < pani->pahdr->nlimbs; i++ )
	{
		pdq = (p[0]-q[0])*(p[0]-q[0]) +
				(p[1]-q[1])*(p[1]-q[1]) +
				(p[2]-q[2])*(p[2]-q[2]) +
				(p[3]-q[3])*(p[3]-q[3]);

		npdq = (p[0]+q[0])*(p[0]+q[0]) +
				(p[1]+q[1])*(p[1]+q[1]) +
				(p[2]+q[2])*(p[2]+q[2]) +
				(p[3]+q[3])*(p[3]+q[3]);

		if( pdq < npdq )
		{
			d[0] = p[0]; d[1] = p[1];
			d[2] = p[2]; d[3] = p[3];
		}
		else
		{
			d[0] = -p[0]; d[1] = -p[1];
			d[2] = -p[2]; d[3] = -p[3];
		}

		p += 4; d += 4; q += 4;
	}

	// set interpolation mode
	pani->animode |= MODE_INTERP;
	pani->pq = pani->adata;			// read from ani data scratchpad

	// do first frame here
	_interpolate_frame( pobj, pani );

	pani->anicnt = 1;		// to prevent regular frame processing
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_rndwait() - script function - hold in place for rnd(arg1) ticks.
//////////////////////////////////////////////////////////////////////////////
static int _ani_rndwait( obj_3d *pobj, anidata *pani )
{
#if 0
	int		ticks;


	ticks = (int)(pani->pscript[0]);

	pani->pscript += 1;
	pani->anicnt = 1 + randrng(ticks);
#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_RNDWAIT: %d of a possible %d ticks\r\n", pani->anicnt,
				ticks );
#endif
#endif
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_runtoend() - script function - Run through the rest of a sequence,
// holding each frame for 'byte' ticks.
//////////////////////////////////////////////////////////////////////////////
static int _ani_runtoend( obj_3d *pobj, anidata *pani )
{
	int		ticks;

	ticks = (int)(pani->pscript[0]);

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_RUNTOEND: %d ticks (%d frames)\r\n", ticks,
				pani->pahdr->nframes - (pani->iframe+1));
#endif

	if( pani->iframe >= (pani->pahdr->nframes-1))
	{
		pani->pscript += 1;
	}
	else
	{
		_next_frame( pobj, pani, ticks );
		pani->pscript -= 1;		// back up to ANI_RUNTOEND cmd
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_waithitgnd() - script function - Hold until y=0.
//////////////////////////////////////////////////////////////////////////////
static int _ani_waithitgnd( obj_3d *pobj, anidata *pani )
{
#if 0
	if( pobj->y > 0.0f )
	{
		pani->pscript -= 1;		// back up to ANI_RUNTOEND cmd
		pani->anicnt = 1;
	}

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_WAITHITGND\r\n" );
#endif
#endif
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_waittminus() - script function - Hold until BYTE ticks from impact.
//////////////////////////////////////////////////////////////////////////////
static int _ani_waittminus( obj_3d *pobj, anidata *pani )
{
#if 0
	int		val, t;
	float	a,b,c,b2m4ac,root1,root2;

	val = (int)(pani->pscript[0]);

	a = GRAVITY / 2.0f;
	b = pobj->vy;
	c = pobj->y;

	b2m4ac = b * b - 4.0f * a * c;
	root1 = (-1.0f * b + fsqrt( b2m4ac )) / (2.0f * a);
	root2 = (-1.0f * b - fsqrt( b2m4ac )) / (2.0f * a);

	t = 1;

	if( root1 > 0.0f )
		t = (int)root1;

	if( root2 > 0.0f )
		t = (int)root2;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_WAITTMINUS:%d (%d,%f)\r\n", val, t, pobj->y );
#endif
	if( t > val )
	{
		pani->pscript -= 1;
		pani->anicnt = 1;
	}
	else
	{
		pani->pscript += 1;
	}
#endif
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
static int _ani_intstream( obj_3d *pobj, anidata *pani )
{
	int		frames, ticks, *pinc;
	float	a,b,c,b2m4ac,root1,root2,inc;

	frames = (int)(pani->pscript[0]);
	ticks = (int)(pani->pscript[1]);

	pani->pscript += 2;

	if( frames == AA_TOEND )
	{
		pinc = (int *)&inc;
		*pinc = GETWORD(-1);
		pani->pscript += 3;
		frames = pani->pahdr->nframes - (pani->iframe+1);
		ticks = (int)(inc * (float)frames);
	}

	if (ticks == AA_TOGROUND)
	{
		// compute ticks for me to hit the ground;
		a = GRAVITY / 2.0f;
		b = pobj->vy;
		c = pobj->y;

		b2m4ac = b * b - 4.0f * a * c;
		root1 = (-1.0f * b + fsqrt( b2m4ac )) / (2.0f * a);
		root2 = (-1.0f * b - fsqrt( b2m4ac )) / (2.0f * a);

		ticks = 1;

		if (root1 > 1.0f)
			ticks = (int)(root1+0.9f);

		if (root2 > 1.0f)
			ticks = (int)(root2+0.9f);
			
		// if I'm in a puppet move, compute ground time for the
		// guy to whom I'm linked and use the lesser of the two.
		if (pani->animode & (MODE_PUPPET|MODE_PUPPETEER))
		{
			int		pticks;
//			fprintf( stderr, "puppet-linked AA_TOGROUND\n" );
//			fprintf( stderr, "Me: y=%f, vy=%f\n", c, b );
			
			a = GRAVITY / 2.0f;
			b = ((fbplyr_data *)pobj)->puppet_link->odata.vy;
			c = ((fbplyr_data *)pobj)->puppet_link->odata.y;
//			fprintf( stderr, "Him: y=%f, vy=%f\n", c, b );
	
			b2m4ac = b * b - 4.0f * a * c;
			root1 = (-1.0f * b + fsqrt( b2m4ac )) / (2.0f * a);
			root2 = (-1.0f * b - fsqrt( b2m4ac )) / (2.0f * a);
	
			pticks = 1;
	
			if (root1 > 1.0f)
				pticks = (int)(root1+0.9f);
	
			if (root2 > 1.0f)
				pticks = (int)(root2+0.9f);
				
//			fprintf( stderr, "I (%2x) will hit in %d ticks\n",
//				JERSEYNUM(((fbplyr_data *)pobj)->plyrnum), ticks );
//			fprintf( stderr, "Link (%2x) will hit in %d ticks\n",
//				JERSEYNUM(((fbplyr_data *)pobj)->puppet_link->plyrnum), pticks );
			ticks = LESSER(ticks,pticks);
//			fprintf( stderr, "Using time of %d\n", ticks );
		}
	}

	// if iframe is -1, we just started this sequence.  If ticks > frames,
	// we're advancing less than one frame per tick.  This is bad, since
	// we can't have a frame index less than zero at display time.  So in
	// this case, the stream routine will set fframe to 0 for the first
	// tick, ignoring sinc, and it won't decrement the counter.  So compute
	// the sinc using the decremented tick count.
	if(( pani->iframe == -1 ) && ( ticks > frames ))
	{
		ticks--;
		frames--;
	}

	// set stream interpolation mode
	pani->animode |= MODE_INTSTREAM;

	// initialize stream info
	pani->fframe = (float)(pani->iframe);
	pani->icnt = 0;
	pani->scnt = ticks;
	pani->sinc = (float)frames / (float)ticks;

	// read data from interpolation scratch pad
	pani->pq = pani->idata;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_INTSTREAM: %d frames in %d ticks, inc: %9f\r\n",
				frames, ticks, pani->sinc );
#endif

	// process the first frame's worth
	_stream_frame( pobj, pani );

	pani->anicnt = 1;

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
static int _ani_zeroxzvels( obj_3d *pobj, anidata *pani )
{
	pobj->vx = 0.0f;
	pobj->vz = 0.0f;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_ZEROXZVELS\r\n" );
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
#define	FRELX(x,z,t)	(z * isin(t) + x * icos(t))
#define	FRELZ(x,z,t)	(z * icos(t) - x * isin(t))
static int _ani_setvels( obj_3d *pobj, anidata *pani )
{
	int		vx,vy,vz;
	float	*px,*py,*pz;

	px = (float *)&vx;
	py = (float *)&vy;
	pz = (float *)&vz;

	vx = (int)GETWORD(0);
	vy = (int)GETWORD(4);
	vz = (int)GETWORD(8);
	pani->pscript += 12;

	pobj->vx = FRELX(*px,*pz,pobj->fwd );
	pobj->vy = *py;
	pobj->vz = FRELZ(*px,*pz,pobj->fwd );

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_SETVELS(%9f,%9f,%9f)\r\n", *px, *py, *pz );
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
static int _ani_setyvel( obj_3d *pobj, anidata *pani )
{
	int		vy;
	float	*py;

	vy = (int)GETWORD(0);
	py = (float *)&vy;
	pani->pscript += 4;

	pobj->vy = *py;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_SETYVEL(%9f)\r\n", *py );
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
static int _ani_runtoframe( obj_3d *pobj, anidata *pani )
{
	int		frame, ticks;

	frame = 0xff & (int)(pani->pscript[0]);
	ticks = (int)(pani->pscript[1]);

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_RUNTOFRAME: %d\r\n", frame );
#endif

	if( pani->iframe >= frame )
	{
		pani->pscript += 2;
	}
	else
	{
		_next_frame( pobj, pani, ticks );
		pani->pscript -= 1;		// back up to ANI_RUNTOFRAME cmd
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
static int _ani_soundcall( obj_3d *pobj, anidata *pani )
{
#if 0
	int		i1,i2,i3,i4,i5;
	char	*cp;

	i1 = (int)GETWORD(0);
	i2 = (int)GETWORD(4);
	i3 = (int)GETWORD(8);
	i4 = (int)GETWORD(12);
	i5 = (int)GETWORD(16);
	cp = (char *)i1;

	pani->pscript += 20;

	snd_scall_bank(cp,i2,i3,i4,i5);
#endif
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//#define	FRELX(x,z,t)	(z * isin(t) + x * icos(t))
//#define	FRELZ(x,z,t)	(z * icos(t) - x * isin(t))
static int _ani_set_face_rel_vel( obj_3d *pobj, anidata *pani )
{
	int		vel;
	float	*pvel;


	vel = (int)GETWORD(0);
	pvel = (float *)&vel;
	pani->pscript += 4;

	pobj->vx = FRELX(0.0f,*pvel,pobj->pobj_hit->fwd);
	pobj->vz = FRELZ(0.0f,*pvel,pobj->pobj_hit->fwd);

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_SET_FACE_REL_VEL(%9f,%9f)\r\n", pobj->vx,pobj->vz);
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
static int _ani_setfriction( obj_3d *pobj, anidata *pani )
{
	int		f;
	float	*pf;

	f = (int)GETWORD(0);
	pf = (float *)&f;
	pani->pscript += 4;

	pobj->friction = *pf;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_SETFRICTION(%9f)\r\n", *pf );
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
CAMSEQINFO * select_csi_ptr( CAMSEQINFO * pcsi )
{
	// Count # of table entries & select a random entry in that range
	register int count = 0;
	while (!(pcsi[count].mode < 0)) count++;
	pcsi += randrng(count);

	// "NULL" entry if not a valid mode; outta here
	if (pcsi->mode >= CAM_MODECNT)
		return NULL;

	return pcsi;
};

//////////////////////////////////////////////////////////////////////////////
int select_csi_item( CAMSEQINFO * pcsi )
{
	// Get random entry; abort if got a "NULL" entry
	if (!(pcsi = select_csi_ptr( pcsi )))
		return 0;

#ifdef DEBUG
	if (pcsi->mode < 0 || pcsi->mode >= CAM_MODECNT ||
		pcsi->mode_cnt < 0 ||
		pcsi->trav_cnt < 0 ||
		pcsi->theta < -M_TWOPI || pcsi->theta > M_TWOPI ||
		pcsi->phi <= -M_PI_2 || pcsi->phi >= M_PI_2 ||
		pcsi->d < 0 || pcsi->d > 2000)
	{
		fprintf(stderr,"===> Bogus <select_csi_item>\r\n");
		fprintf(stderr,"===> mode: %d  mode_cnt: %d  trav_cnt: %d\r\n"
			"===> theta: %f  phi: %f  d: %f\r\n",
			pcsi->mode,
			pcsi->mode_cnt,
			pcsi->trav_cnt,
			pcsi->theta,
			pcsi->phi,
			pcsi->d);
		lockup();
		return 0;
	}
#endif

	// Copy seq control data to cambot struct
	cambot.csi = *pcsi;

	// Reset elapsed-time to start`er up
	cambot.tick_cnt = 0;

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
static int _ani_setcambot( obj_3d *pobj, anidata *pani )
{
	register CAMSEQINFO * pcsi = (CAMSEQINFO *)(GETWORD(0));
	pani->pscript += 4;


	// if player doesn't have ball...dont zoom in
//	if ( ((fbplyr_data *)pobj)->plyrnum != game_info.ball_carrier)
//		return 0;

	if (!select_csi_item( pcsi ))
	{
#ifdef DEBUG
		fprintf(stderr, "===> <_ani_setcambot> from plyr %d seq %d failed\r\n",
			((fbplyr_data *)pobj)->plyrnum,
			pani->seq_index);
#endif
		return 0;
	}

	// Save obj ptr
	cambot.pmodeobj = (void *)pobj;

//#if ANI_DEBUG
//	{
//	char * dbgtxt[CAM_MODECNT];
//	dbgtxt[CAM_RESUME] = "CAM_RESUME";
//	dbgtxt[CAM_ZOOM] =   "CAM_ZOOM";
//	dbgtxt[CAM_SPLINE] = "CAM_SPLINE";
//	dbgtxt[CAM_PATH]   = "CAM_PATH";
//
//	if( pani->animode & MODE_ANIDEBUG )
//		fprintf(stderr,"  ANI_SETCAMBOT: %s  mode_cnt:%i  trav_cnt:%i\r\n"
//					   "    theta:%f  phi:%f  dist:%f\r\n",
//				dbgtxt[cambot.csi.mode], cambot.csi.mode_cnt, cambot.csi.trav_cnt,
//				cambot.csi.theta, cambot.csi.phi, cambot.csi.distance );
//	}
//#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
static int _ani_streamtoend( obj_3d *pobj, anidata *pani )
{
	int		frames, iticks_per, ticks;
	float	*ticks_per;

	iticks_per = (int)(GETWORD(0));
	frames = pani->pahdr->nframes - 1 - pani->iframe;
	ticks_per = (float *)&iticks_per;

	ticks = (int)(0.5f + *ticks_per * (float)frames);

	pani->pscript += 4;

	if( frames == 0 )
	{
#if ANI_DEBUG
		if( pani->animode & MODE_ANIDEBUG )
			fprintf( stderr, "  ANI_STREAMTOEND: ignored, already at end\n" );
#endif
		return 0;
	}

	// if iframe is -1, then we just started this sequence.  If ticks is
	// greater than frames, we have to cheat to avoid a negative frame index.
	// If this happens, decrement ticks and rely on _stream_frame to start us
	// at frame 0 and not increment the tick counter the first time through.
	if(( pani->iframe == -1 ) && ( ticks > frames ))
		ticks -= 1;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_STREAMTOEND: %9f (%d frames in %d ticks, inc: %9f)\r\n",
				*ticks_per, frames, ticks, (float)frames / (float)ticks );
#endif

	// set stream interpolation mode
	pani->animode |= MODE_INTSTREAM;

	// initialize stream info
	pani->fframe = (float)(pani->iframe);
	pani->icnt = 0;
	pani->scnt = ticks;
	pani->sinc = (float)frames / (float)ticks;

	// read data from decompression scratch pad
	pani->pq = pani->adata;

	// process the first frame's worth
	_stream_frame( pobj, pani );

	pani->anicnt = 1;

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_bz() - script function - Jump to address if MODE_ZERO bit set.
static int _ani_bz( obj_3d *pobj, anidata *pani )
{
	int addr;

	addr = (int)(GETWORD(0));

	pani->pscript += 4;

	if( pani->animode & MODE_ZERO )
		pani->pscript = (char *)addr;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
	{
		fprintf(stderr,  "  ANI_BZ: " );
		if( pani->animode & MODE_ZERO )
			fprintf(stderr,  " branching to %x\f\n", addr );
		else
			fprintf(stderr,  " no branch.\r\n" );
	}
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_bnz() - script function - Jump to address if MODE_ZERO bit not set.
static int _ani_bnz( obj_3d *pobj, anidata *pani )
{
	int addr;

	addr = (int)(GETWORD(0));

	pani->pscript += 4;

	if(!( pani->animode & MODE_ZERO ))
		pani->pscript = (char *)addr;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
	{
		fprintf(stderr,  "  ANI_BNZ: " );
		if(!( pani->animode & MODE_ZERO ))
			fprintf(stderr,  " branching to %x\f\n", addr );
		else
			fprintf(stderr,  " no branch.\r\n" );
	}
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_jifseq() - script function - Jump to address if current seq_index is
// any member of the preceeding list of sequence indices.
static int _ani_jifseq( obj_3d *pobj, anidata *pani )
{
	int jump = FALSE, word;

	while( !((word = (int)(GETWORD(0))) & 0x80000000 ))
	{
		pani->pscript += 4;

		if( word == pani->seq_index )
			jump = TRUE;
	}

	pani->pscript += 4;

	if( jump )
		pani->pscript = (char *)word;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_JIFSEQ: branch %s", jump ? "taken\r\n" : "not taken\n" );
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_jifnseq() - script function - Jump to address if current seq_index is NOT
// any member of the preceeding list of seqequence indices.
static int _ani_jifnseq( obj_3d *pobj, anidata *pani )
{
	int jump = TRUE, word;

	while( !((word = (int)(GETWORD(0))) & 0x80000000 ))
	{
		pani->pscript += 4;

		if( word == pani->seq_index )
			jump = FALSE;
	}

	pani->pscript += 4;

	if( jump )
		pani->pscript = (char *)word;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_JIFNSEQ: branch %s", jump ? "taken\r\n" : "not taken\r\n" );
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// _ani_jifmode() - script function - Jump to address if animode&mask.
static int _ani_jifmode( obj_3d *pobj, anidata *pani )
{
#if 0
	int	mask, addr;

	mask = (int)GETWORD(0);
	addr = (int)GETWORD(4);

	pani->pscript += 8;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_JIFMODE: %x:%x, ", pani->animode, mask );
#endif

	if( pani->animode & mask )
	{
		pani->pscript = (char *)addr;
#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "branch taken\r\n" );
#endif
	}
#if ANI_DEBUG
	else
	{
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "branch not taken\r\n" );
	}
#endif
#endif
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
static int _ani_prep_xition( obj_3d *pobj, anidata *pani )
{
	int	count, frame;

	count = (int)GETWORD(0);
	frame = (int)GETWORD(4);

	pani->pscript += 8;

	pani->animode |= MODE_INTXITION;
	pani->vcount = count;
	pani->vframe = frame;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_PREP_XITION: %d ticks to frame %d\r\n", count, frame );
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
static int _ani_attack_on( obj_3d *pobj, anidata *pani )
{
	int		mode,j1,j2,time;
	intfloat_t ext1,ext2,radius;
	fbplyr_data *ppdata;

	ppdata = (fbplyr_data *)pobj;

	mode = (int)(pani->pscript[0]);
	j1 = (int)(pani->pscript[1]);
	j2 = (int)(pani->pscript[2]);
	time = (int)(pani->pscript[3]);
	ext1.i = (int)GETWORD(4);
	ext2.i = (int)GETWORD(8);
	radius.i = (int)GETWORD(12);
	pani->pscript += 16;

	ppdata->attack_mode = mode;
	ppdata->acj1 = j1;
	ppdata->acj2 = j2;
	ppdata->acx1 = ext1.f;
	ppdata->acx2 = ext2.f;
	ppdata->acr = radius.f;
	ppdata->attack_time = time;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_ATTACK_ON: mode %d, %d to %d, exts %f,%f, radius %f, for %d ticks\r\n",
			mode, j1, j2, ext1.f, ext2.f, radius.f, time );
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
static int _ani_attack_off( obj_3d *pobj, anidata *pani )
{
	fbplyr_data *ppdata;

	ppdata = (fbplyr_data *)pobj;

	ppdata->attack_mode = 0;
	ppdata->attack_time = 0;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_ATTACK_OFF\r\n" );
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
static int _ani_turnhack( obj_3d *pobj, anidata *pani )
{
	int			val;

	val = (int)GETWORD(0);

	pani->pscript += 4;

	pani->animode |= MODE_TURNHACK;
	pani->hackval = val;
	pobj->fwd = (pobj->fwd - val) % 1024;
	pobj->tgt_fwd = pobj->fwd;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_TURNHACK(%d)\r\n", val );
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
extern int shaker_amp;						// Shaker amplification level
extern int shaker_tot;						// Shaker count total
extern int shaker_cnt;						// Shaker count

static int _ani_shaker( obj_3d *pobj, anidata *pani )
{
	fbplyr_data *ppdata = (fbplyr_data *)pobj;
	shaker_amp = (int)(pani->pscript[0]);
	shaker_tot = (int)GETWORD(1);
	pani->pscript += 5;

	if( ppdata->plyrnum != game_info.ball_carrier )
		return 0;

	shaker_cnt = 0;

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_SHAKER: level %d for %d ticks\r\n", shaker_amp, shaker_tot);
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
static int _ani_leapatt( obj_3d *pobj, anidata *pani )
{
	int			victim, vjoint, time, i;
	float		offx,offy,offz, *tm;
	intfloat_t	td;
	float		x,y,z, tx,ty,tz, ax,ay,az, t, vxz;
	float		max_yv, max_xzv, min_xzv;
	obj_3d		*ptgt;

	// get args
	victim = GETWORD(0);
	vjoint = GETWORD(4);
	time = GETWORD(8);
	td.i = GETWORD(12); offx = td.f;
	td.i = GETWORD(16);	offy = td.f;
	td.i = GETWORD(20);	offz = td.f;
	td.i = GETWORD(24); max_yv = td.f;
	td.i = GETWORD(28); max_xzv = td.f;
	td.i = GETWORD(32); min_xzv = td.f;
	pani->pscript += 36;

	// if victim is AA_TGT_CARRIER, target carrier
	if( victim == AA_TGT_CARRIER )
		victim = game_info.ball_carrier;

	if( victim == AA_TGT_TARGET )
		victim = ((fbplyr_data *)pobj)->throw_target;

	if( victim == -1 )
		victim = 3 + 7 * game_info.off_side;

//	fprintf( stderr, "LEAPATT: %d %d %d, %f %f %f\n",
//		victim, vjoint, time, offx, offy, offz );

	// if time is AA_TOGROUND, pick t such that vy remains
	// unchanged.
	if( time == AA_TOGROUND )
	{
		float	a,b,c,b2m4ac,root1,root2;

		a = GRAVITY / 2.0f;
		b = pobj->vy;
		c = pobj->y + offy;

		b2m4ac = b * b - 4.0f * a * c;
		root1 = (-1.0f * b + fsqrt( b2m4ac )) / (2.0f * a);
		root2 = (-1.0f * b - fsqrt( b2m4ac )) / (2.0f * a);

		t = 1.0f;

		if( root1 > t )
			t = root1;

		if( root2 > t )
			t = root2;

		// time must be integer
		time = (int)t;
	}

	t = (float)time;

	// get target x,y,z (in camera coords)
	x = player_blocks[victim].jpos[vjoint][0];
	y = player_blocks[victim].jpos[vjoint][1];
	z = player_blocks[victim].jpos[vjoint][2];

	// transform target x,y,z into world coords
	tm = cambot.xform;
	tx = x * tm[0] + y * tm[4] + z * tm[8] + cambot.x;
	ty = x * tm[1] + y * tm[5] + z * tm[9] + cambot.y;
	tz = x * tm[2] + y * tm[6] + z * tm[10] + cambot.z;

	ptgt = (obj_3d *)(player_blocks + victim);

	// adjust for target velocity
	tx += t * ptgt->vx;
	ty += t * ptgt->vy + 0.5f * GRAVITY * t * t;
	tz += t * ptgt->vz;

	for( i = 0; i < 4; i++ )
	{
		float		newpt[3];
		int			angle, new_fwd;

		// rotate attack offsets into attacker's tgt_fwd f.o.r.
		ax = pobj->x + FRELX(offx,offz,pobj->tgt_fwd);
		ay = pobj->y + offy;
		az = pobj->z + FRELZ(offx,offz,pobj->tgt_fwd);

		pobj->vx = (tx - ax) / t;
		pobj->vy = ((ty - ay) / t) - (0.5f * GRAVITY * t);
		pobj->vz = (tz - az) / t;

		vxz = fsqrt( pobj->vx * pobj->vx + pobj->vz * pobj->vz );

		if( vxz > max_xzv )
		{

//#if DEBUG
//	fprintf(stderr,  "Maximum values used!.\r\n" );
//#endif

			pobj->vx = pobj->vx / vxz * max_xzv;
			pobj->vz = pobj->vz / vxz * max_xzv;
		}

		if( vxz < min_xzv )
		{

		// TODO: Make this minimum vel check direction-sensitive.
//#if DEBUG
//	fprintf(stderr,  "Minimum values used!.\r\n" );
//#endif

			pobj->vx = FRELX(0.0f,min_xzv,pobj->tgt_fwd);
			pobj->vz = FRELZ(0.0f,min_xzv,pobj->tgt_fwd);
			return 0;
		}

		// turn to face velocity
		newpt[0] = pobj->x + pobj->vx;
		newpt[2] = pobj->z + pobj->vz;

		angle = ptangle( &(pobj->x), newpt );

		new_fwd = _mid_angle( angle, pobj->tgt_fwd );

		if( i == 3 )
			new_fwd = angle;

		pobj->tgt_fwd = new_fwd;
	}

	// now compute Y velocity
	ty += t * ptgt->vy + 0.5f * GRAVITY * t * t;
	if( ty < 0.0f )
		ty = 0.0f;
	pobj->vy = ((ty - ay) / t) - (0.5f * GRAVITY * t);

	// cap the y vel
	if( pobj->vy > max_yv )
		pobj->vy = max_yv;

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
static int _ani_code_bnz( obj_3d *pobj, anidata *pani )
{
	_ani_code( pobj, pani );
	_ani_bnz( pobj, pani );

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
static int _ani_code_bz( obj_3d *pobj, anidata *pani )
{
	_ani_code( pobj, pani );
	_ani_bz( pobj, pani );

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
static int _ani_leapat_ball( obj_3d *pobj, anidata *pani )
{
	float		max_xzv,min_xzv;
	intfloat_t	td;
	float		xop,zop,vxp,vzp, xob,zob,vxb,vzb;
	float		n, d, vp, ix, iz, pid, nvp;

	// get args
	td.i = GETWORD(0); max_xzv = td.f;
	td.i = GETWORD(4); min_xzv = td.f;
	pani->pscript += 8;

	xop = pobj->x;
	zop = pobj->z;
	vxp = pobj->vx;
	vzp = pobj->vz;

	xob = ball_obj.odata.x;
	zob = ball_obj.odata.z;
	vxb = ball_obj.odata.vx;
	vzb = ball_obj.odata.vz;

	d = vxb*vzp - vzb*vxp;

	if( fabs( d ) < 0.01 )
		return 0;			// parallel.  can't do much

	n = (vxp * (zob-zop) - vzp * (xob-xop)) / d;

	if( n < 1.0f )
		n = 1.0f;

	// n is ticks for ball to hit intercept point
	// find intercept point
	ix = xob + n * vxb;
	iz = zob + n * vzb;

	// find distance from player to int pt
	pid = fsqrt((ix-xop)*(ix-xop) + (iz-zop)*(iz-zop));

	// find player velocity
	vp = fsqrt(vxp*vxp + vzp*vzp);
	
	if( vp < 0.001f )
		return 0;

	// find the velocity he'd need to connect
	nvp = pid / n;

	// cap it
	if( nvp > 1.5f * max_xzv )
		return 0;		// not even close.  go home

	if( nvp > max_xzv )
	{
#ifdef DEBUG
		fprintf( stderr, "leapat max\n" );
#endif
		nvp = max_xzv;
	}
	if( nvp < min_xzv )
	{
#ifdef DEBUG
		fprintf( stderr, "leapat min\n" );
#endif
		nvp = min_xzv;
	}
	
	// apply new vel
	pobj->vx = pobj->vx / vp * nvp;
	pobj->vz = pobj->vz / vp * nvp;

#if 0
	fprintf( stderr, "ticks to intercept: %f\n", n );
	fprintf( stderr, "pid: %f  point of int: %f %f\n", pid, ix, iz );
	fprintf( stderr, "ball pos: %f %f  ball vel: %f %f\n", xob, zob, vxb, vzb );
	fprintf( stderr, "player vel: %f  needed vel: %f\n", vp, nvp );
	fprintf( stderr, "player pos %f %f\n", xop, zop );
	fprintf( stderr, "old v: %f %f\n", vxp, vzp );
	fprintf( stderr, "new v: %f %f\n", pobj->vx, pobj->vz );
#endif

	return 0;
}


//////////////////////////////////////////////////////////////////////////////
static int _ani_leapat_tgtx( obj_3d *pobj, anidata *pani )
{
	float		max_xzv,min_xzv;
	intfloat_t	td;
	float		xop,zop, dx, dz;
	float		n, ix, iz, pid, nvp;

	// get args
	td.i = GETWORD(0); max_xzv = td.f;
	td.i = GETWORD(4); min_xzv = td.f;
	pani->pscript += 8;

	xop = pobj->x;
	zop = pobj->z;

	ix = ball_obj.tx;
	iz = ball_obj.tz;
	n = 25.0f;			// just a guess
	
	dx = ix - xop;
	dz = iz - zop;

	// find distance from player to int pt
	pid = fsqrt(dx*dx + dz*dz);

	// find the velocity he'd need to connect
	nvp = pid / n;

	// cap it
	if( nvp > 1.5f * max_xzv )
		return 0;		// not even close.  go home

	if( nvp > max_xzv )
	{
#ifdef DEBUG
		fprintf( stderr, "leapat max\n" );
#endif
		nvp = max_xzv;
	}
	if( nvp < min_xzv )
	{
#ifdef DEBUG
		fprintf( stderr, "leapat min\n" );
#endif
		nvp = min_xzv;
	}
	
	// apply new vel
	pobj->vx = dx / pid * nvp;
	pobj->vz = dz / pid * nvp;

#if 0
	fprintf( stderr, "ticks to intercept: %f\n", n );
	fprintf( stderr, "pid: %f  point of int: %f %f\n", pid, ix, iz );
	fprintf( stderr, "ball pos: %f %f  ball vel: %f %f\n", xob, zob, vxb, vzb );
	fprintf( stderr, "player vel: %f  needed vel: %f\n", vp, nvp );
	fprintf( stderr, "player pos %f %f\n", xop, zop );
	fprintf( stderr, "old v: %f %f\n", vxp, vzp );
	fprintf( stderr, "new v: %f %f\n", pobj->vx, pobj->vz );
#endif

	return 0;
}


//////////////////////////////////////////////////////////////////////////////
static int _ani_runtoframe16( obj_3d *pobj, anidata *pani )
{
#if 0
	int		frame, ticks;

	frame = (int)(GETWORD(0));
	ticks = (int)(GETWORD(8));

#if ANI_DEBUG
	if( pani->animode & MODE_ANIDEBUG )
		fprintf(stderr,  "  ANI_RUNTOFRAME16: %d\r\n", frame );
#endif

	if( pani->iframe >= frame )
	{
		pani->pscript += 8;
	}
	else
	{
		_next_frame( pobj, pani, ticks );
		pani->pscript -= 1;		// back up to ANI_RUNTOFRAME cmd
	}
#endif
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// return angle between (p1,p1+z) and (p2-p1)
int ptangle( float *p1, float *p2 )
{
	float	dz,dx,mag,cosine;
	int		angle;

	dz = p2[2] - p1[2];
	dx = p2[0] - p1[0];

	mag = fsqrt( dx*dx + dz*dz );

	if( mag < 0.01f )
		return 0;

	cosine = dz / mag;

	angle = iacos( cosine );

	if( dx < 0.0f )
		angle *= -1;

	angle = angle & 0x3FF;

	return angle;

}

//////////////////////////////////////////////////////////////////////////////
// return angle that's halfway from a to b
static int _mid_angle( int a, int b )
{
	int diff;

	diff = abs( a - b );

	if( diff <= 512 )
		return(( a + b ) / 2 );

	if( a < b )
		a += 1024;
	else
		b += 1024;

	return((( a + b ) % 1024 ) / 2 );
}

//////////////////////////////////////////////////////////////////////////////
