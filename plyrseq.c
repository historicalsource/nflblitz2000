/****************************************************************************/ 
/*                                                                          */
/* plyrseq.c - player animation sequences                                   */
/*                                                                          */
/* Written by:  JBJ / DJT / Jason Skiles / S. Claus                         */
/* Version:     1.00                                                        */
/*                                                                          */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/****************************************************************************/
#include <math.h>
#include <goose/pmath.h>
#include "include/ani3d.h"
#include "include/camdefs.h"
#include "include/camdata.h"

#ifdef GETUPAGAIN
asm("
			.equ	GETUPAGAIN,0
");
#else
asm("
			.equ	GETUPAGAIN,1 
");
#endif

#ifdef NOSETCAM
asm("
			.equ	DOSETCAM,0
");
#else
asm("
			.equ	DOSETCAM,1 
");
#endif

asm("
			.include include/ani3d.equ
			.include anim/genseq.equ
			.include include/player.equ
			.include include/sndcalls.equ

			.globl	set_trgt_fwd
			.globl	rotate_anim
			.globl	turn_player
			.globl	plyr_bnk_str
			.globl	play_rand_taunt
			.globl	general_run
			.globl	release_puppet,celebrate
			.globl	face_carrier
			.globl	face_attacker
			.globl	maybe_dive
			.globl	maybe_qb_mad
			.globl	slower_spin
			.globl	lower_player
			.globl	set_unint_lowerhalf
			.globl	maybe_pop_helmet
#			.globl	maybe_drop_kick
#			.globl	is_plr_on_grnd
#			.globl	maybe_dizzy
			.globl	maybe_get_up_b_hurt
			.globl	maybe_get_up_c_hurt
			.globl	maybe_spear_head
			.globl	rotate_torso
#			.globl	change_to_attacker_fwd
			.globl	set_plyr_y_pos


#;;;;;;;;;
#;;;;;;;;; QUARTERBACK SEQUENCES
#;;;;;;;;;

#;;;;;;;;;;;; QB waiting for ball on knees for kicker

			.globl	qbhut_knee_anim
			.byte	SCRIPT
qbhut_knee_anim:
			.byte	ANI_SETMODE
			.word	0
slowkhut:
			.byte	ANI_NEWSEQ
			.word	SEQ_M_KHHUT
			.byte	ANI_RUNTOFRAME,31,2
			.byte	ANI_CODE
			.word	hut_snd,P_LONG_HUT_SP,0,0

			.byte	ANI_RUNTOEND,3
			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,slowkhut
fastkhut:
			.byte	ANI_NEWSEQ
			.word	SEQ_M_KHHUT
			.byte	ANI_INTSTREAM,31,20
			.byte	ANI_CODE
			.word	hut_snd,-1,0,0
			.byte	ANI_INTSTREAM,12,5

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,fastkhut
			.byte	ANI_NEWSEQ
			.word	SEQ_M_KHHUT
			.byte	ANI_INTSTREAM,31,10
			.byte	ANI_CODE
			.word	hut_snd,-1,0,0
			.byte	ANI_INTSTREAM,12,5
			.byte	ANI_GOTO
			.word	slowkhut

#;;;;;;;;;;;; QB holding ball for kicker
			
			.globl	qbhut_place_ball_anim
			.byte	SCRIPT
qbhut_place_ball_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			
			.byte	ANI_CLRFLAG
			.word	PF_FLIP
			
			.byte	ANI_NEWSEQ
			.word	SEQ_M_KHHOLD
			
			.byte	ANI_INTSTREAM,44,15
			
			.byte	ANI_RUNTOEND,3
			
		#this should never happen
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END
			
			
#;;;;;;;;;;;; QB un-holds ball for kicker
			
			.globl	qbhut_unhold_ball_anim
			.byte	SCRIPT
qbhut_unhold_ball_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			
			.byte	ANI_NEWSEQ
			.word	SEQ_M_KHRELE
			
			.byte	ANI_RUNTOEND,1
			
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END
			
			
#;;;;;;;;;;;; QB waiting for ball in shotgun formation

			.globl	shotgun_anim
			.byte	SCRIPT
shotgun_anim:
			.byte	ANI_SETMODE
			.word	0
slowgun:
			.byte	ANI_NEWSEQ
			.word	SEQ_SGUN_2
			.byte	ANI_RUNTOFRAME,28,2
			.byte	ANI_CODE
			.word	hut_snd,P_LONG_HUT_SP,0,0
			.byte	ANI_RUNTOEND,3
			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,slowgun
fastgun:
			.byte	ANI_NEWSEQ
			.word	SEQ_SGUN_2
			.byte	ANI_INTSTREAM,28,20
			.byte	ANI_CODE
			.word	hut_snd,-1,0,0
			.byte	ANI_INTSTREAM,39,45

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,fastgun
			.byte	ANI_NEWSEQ
			.word	SEQ_SGUN_2
			.byte	ANI_INTSTREAM,28,10
			.byte	ANI_CODE
			.word	hut_snd,-1,0,0
			.byte	ANI_INTSTREAM,39,35
			.byte	ANI_GOTO
			.word	slowgun
			.byte	ANI_END
			
#;;;;;;;;;;;; QB waiting for ball to snap
			
			.globl	qbset_anim
			.byte	SCRIPT
qbset_anim:
			.byte	ANI_SETMODE
			.word	0
slowhut:

# 80% of time...do normal hut
			.byte	ANI_CODE_BNZ
			.word	rand_func,85,0,0,dohut

# 50% of non-80% time... do point left
			.byte	ANI_CODE_BNZ
			.word	rand_func,35,0,0,ptl

			.byte	ANI_GOTO
			.word	qbptr_anim
ptl:
			.byte	ANI_GOTO
			.word	qbptl_anim
dohut:	
			.byte	ANI_NEWSEQ
			.word	SEQ_HUT
start_hut:	
			.byte	ANI_RUNTOFRAME,26,2
			.byte	ANI_CODE
			.word	hut_snd,P_LONG_HUT_SP,0,0

			.byte	ANI_RUNTOEND,3
			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,slowhut
fasthut:
			.byte	ANI_NEWSEQ
			.word	SEQ_HUT
			.byte	ANI_INTSTREAM,26,20
			.byte	ANI_CODE
			.word	hut_snd,-1,0,0
			.byte	ANI_INTSTREAM,26,45

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,fasthut
			.byte	ANI_NEWSEQ
			.word	SEQ_HUT
			.byte	ANI_INTSTREAM,26,10
			.byte	ANI_CODE
			.word	hut_snd,-1,0,0
			.byte	ANI_INTSTREAM,26,35
			.byte	ANI_GOTO
			.word	slowhut


#;;;;;;;;;;;; QB -- points left

			.globl	qbptl_anim
			.byte	SCRIPT
qbptl_anim:
			.byte	ANI_SETMODE
			.word	0
#			.word	MODE_ANCHORXZ
			.byte	ANI_NEWSEQ
			.word	SEQ_POINT_L

	.if DOSETCAM
# Cool camera for attract mode
#			.byte	ANI_SETCAMBOT
#			.word	csi_cd_suplex
##			.word	csi_rs_rsx1

#			.byte	ANI_SETCAMBOT
#			.word	csi_sn_spx1
#			.byte	1
#			.byte	ANI_SETCAMBOT
#			.word	csi_fs_spx1
	.endif
			.byte	ANI_CODE
			.word	hut_snd,-2,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_HUT
			.byte	0
			.byte	ANI_GOTO
			.word	start_hut
#			.word	qbset_anim
			.byte	ANI_END


#;;;;;;;;;;;; QB -- points right

			.globl	qbptr_anim
			.byte	SCRIPT
qbptr_anim:
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_NEWSEQ
			.word	SEQ_POINT_R

			.byte	ANI_CODE
			.word	hut_snd,-2,0,0
			
			.byte	ANI_RUNTOEND,1

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_HUT
			.byte	0

			.byte	ANI_GOTO
			.word	start_hut
			.byte	ANI_END

;;;;;;;;;;
			.globl	qb_run_anim
			.globl	qb_t_run_anim
			.byte	SCRIPT
qb_run_anim:
			.byte	ANI_SETMODE
			.word	MODE_TWOPART|MODE_ANCHORXZ
#			.byte	ANI_JIFNSEQ
#			.word	SEQ_QBRUNF_T,qb_run_loop
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_QBRUNF
			.byte	AA_VAR_FRAME
			.byte	ANI_RUNTOEND,1
qb_run_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_QBRUNF
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	qb_run_loop

			.byte	SCRIPT
qb_t_run_anim:
			.byte	ANI_SETMODE
			.word	MODE_TWOPART|MODE_ANCHORXZ
#			.byte	ANI_JIFNSEQ
#			.word	SEQ_QBRUNF,qb_t_run_loop
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_QBRUNF_T
			.byte	AA_VAR_FRAME
			.byte	ANI_STREAMTOEND
#			.float	0.75
#			.float	0.65
			.float	0.81
qb_t_run_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_QBRUNF_T
			.byte	ANI_STREAMTOEND
#			.float	0.75
#			.float	0.65
			.float	0.81
			.byte	ANI_GOTO
			.word	qb_t_run_loop

;;;;;;;;;;
			.globl	qb_runb_anim
			.globl	qb_t_runb_anim
			.byte	SCRIPT
qb_runb_anim:
			.byte	ANI_SETMODE
			.word	MODE_TWOPART|MODE_ANCHORXZ
#			.byte	ANI_JIFNSEQ
#			.word	SEQ_QBRUNB_T,qb_runb_loop
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_QBRUNB
			.byte	AA_VAR_FRAME
			.byte	ANI_RUNTOEND,1
qb_runb_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_QBRUNB
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	qb_runb_loop

			.byte	SCRIPT
qb_t_runb_anim:
			.byte	ANI_SETMODE
			.word	MODE_TWOPART|MODE_ANCHORXZ
#			.byte	ANI_JIFNSEQ
#			.word	SEQ_QBRUNB,qb_t_runb_loop
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_QBRUNB_T
			.byte	AA_VAR_FRAME
			.byte	ANI_STREAMTOEND
			.float	0.81
qb_t_runb_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_QBRUNB_T
			.byte	ANI_STREAMTOEND
			.float	0.81
			.byte	ANI_GOTO
			.word	qb_t_runb_loop

;;;;;;;;;;
			.globl	qb_runl_anim
			.globl	qb_t_runl_anim
			.byte	SCRIPT
qb_runl_anim:
			.byte	ANI_SETMODE
			.word	MODE_TWOPART|MODE_ANCHORXZ
#			.byte	ANI_JIFNSEQ
#			.word	SEQ_QBRUNL_T,qb_runl_loop
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_QBRUNL
			.byte	AA_VAR_FRAME
			.byte	ANI_RUNTOEND,1
qb_runl_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_QBRUNL
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	qb_runl_loop

			.byte	SCRIPT
qb_t_runl_anim:
			.byte	ANI_SETMODE
			.word	MODE_TWOPART|MODE_ANCHORXZ
#			.byte	ANI_JIFNSEQ
#			.word	SEQ_QBRUNL,qb_t_runl_loop
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_QBRUNL_T
			.byte	AA_VAR_FRAME
			.byte	ANI_STREAMTOEND
			.float	0.81
qb_t_runl_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_QBRUNL_T
			.byte	ANI_STREAMTOEND
			.float	0.81
			.byte	ANI_GOTO
			.word	qb_t_runl_loop

;;;;;;;;;;
			.globl	qb_runr_anim
			.globl	qb_t_runr_anim
			.byte	SCRIPT
qb_runr_anim:
			.byte	ANI_SETMODE
			.word	MODE_TWOPART|MODE_ANCHORXZ
#			.byte	ANI_JIFNSEQ
#			.word	SEQ_QBRUNR_T,qb_runr_loop
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_QBRUNR
			.byte	AA_VAR_FRAME
			.byte	ANI_RUNTOEND,1
qb_runr_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_QBRUNR
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	qb_runr_loop

			.byte	SCRIPT
qb_t_runr_anim:
			.byte	ANI_SETMODE
			.word	MODE_TWOPART|MODE_ANCHORXZ
#			.byte	ANI_JIFNSEQ
#			.word	SEQ_QBRUNR,qb_t_runr_loop
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_QBRUNR_T
			.byte	AA_VAR_FRAME
			.byte	ANI_STREAMTOEND
			.float	0.81
qb_t_runr_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_QBRUNR_T
			.byte	ANI_STREAMTOEND
			.float	0.81
			.byte	ANI_GOTO
			.word	qb_t_runr_loop

;;;;;;;;;;
			.globl	qb_runl45_anim
			.globl	qb_t_runl45_anim
			.byte	SCRIPT
qb_runl45_anim:
			.byte	ANI_SETMODE
			.word	MODE_TWOPART|MODE_ANCHORXZ
#			.byte	ANI_JIFNSEQ
#			.word	SEQ_QBRL45_T,qb_runl45_loop
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_QBRUNL45
			.byte	AA_VAR_FRAME
			.byte	ANI_RUNTOEND,1
qb_runl45_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_QBRUNL45
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	qb_runl45_loop

			.byte	SCRIPT
qb_t_runl45_anim:
			.byte	ANI_SETMODE
			.word	MODE_TWOPART|MODE_ANCHORXZ
#			.byte	ANI_JIFNSEQ
#			.word	SEQ_QBRUNL45,qb_t_runl45_loop
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_QBRL45_T
			.byte	AA_VAR_FRAME
			.byte	ANI_STREAMTOEND
			.float	0.81
qb_t_runl45_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_QBRL45_T
			.byte	ANI_STREAMTOEND
			.float	0.81
			.byte	ANI_GOTO
			.word	qb_t_runl45_loop

;;;;;;;;;;
			.globl	qb_runr45_anim
			.globl	qb_t_runr45_anim
			.byte	SCRIPT
qb_runr45_anim:
			.byte	ANI_SETMODE
			.word	MODE_TWOPART|MODE_ANCHORXZ
#			.byte	ANI_JIFSEQ
#			.word	SEQ_QBRR45_T,qb_runr45_loop
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_QBRUNR45
			.byte	AA_VAR_FRAME
			.byte	ANI_RUNTOEND,1
qb_runr45_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_QBRUNR45
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	qb_runr45_loop

			.byte	SCRIPT
qb_t_runr45_anim:
			.byte	ANI_SETMODE
			.word	MODE_TWOPART|MODE_ANCHORXZ
#			.byte	ANI_JIFNSEQ
#			.word	SEQ_QBRR45_T,qb_t_runr45_loop
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_QBRR45_T
			.byte	AA_VAR_FRAME
			.byte	ANI_STREAMTOEND
			.float	0.81
qb_t_runr45_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_QBRR45_T
			.byte	ANI_STREAMTOEND
			.float	0.81
			.byte	ANI_GOTO
			.word	qb_t_runr45_loop

#;;;;;;;;;; QB throws ball (RUNNING) short and fast (non-flipped)

			.globl	qb_rthrow_sr_anim 
			.byte	SCRIPT
qb_rthrow_sr_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_THROW_SR
			.byte	6

			.byte	ANI_RUNTOFRAME,9,1

			.byte	ANI_CODE
			.word	qb_release_ball,0,0,0

			.byte	ANI_RUNTOFRAME,23,1

			.byte	ANI_INTSTREAM,26,13

		# turn off two_part
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; QB throws ball (RUNNING) short and fast (flipped)

			.globl	qb_rthrow_sl_anim 
			.byte	SCRIPT
qb_rthrow_sl_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_THROW_SL
			.byte	6

			.byte	ANI_RUNTOFRAME,9,1

			.byte	ANI_SETFLAG
			.word	PF_FLIP

			.byte	ANI_CODE
			.word	qb_release_ball,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FLIP

			.byte	ANI_RUNTOFRAME,23,1

			.byte	ANI_INTSTREAM,26,13

		# turn off two_part
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; QB shuttles ball (PLANTED) short and fast (non-flipped)

#			.globl	new_lateral_r_anim
#			.byte	SCRIPT
#new_lateral_r_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORXZ
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_QB_SHTLR
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_INTSTREAM,5,7
#
#			.byte	ANI_CODE
#			.word	lateral_ball,0,0,0
#
#			.byte	ANI_INTSTREAM,10,13
#
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END


#;;;;;;;;;; new lateral (PLANTED) short and fast (flipped)

#			.globl	new_lateral_l_anim
#			.byte	SCRIPT
#new_lateral_l_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORXZ
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_QB_SHTLL
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_INTSTREAM,5,7
#
#			.byte	ANI_CODE
#			.word	lateral_ball,0,0,0
#
#			.byte	ANI_INTSTREAM,10,13
#
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END


#;;;;;;;;;; QB shuttles ball

#			.globl	qb_shuttle_pass_anim
#			.byte	SCRIPT
#qb_shuttle_pass_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORXZ
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_QB_SPASS
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_RUNTOFRAME,9,1
#
#			.byte	ANI_CODE
#			.word	qb_release_ball,0,0,0
#
#			.byte	ANI_INTSTREAM,23,40
##			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END


#;;;;;;;;;; QB throws ball (PLANTED) short and fast (non-flipped)

			.globl	qb_throw_sr_anim 
			.byte	SCRIPT
qb_throw_sr_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_NEWSEQ
			.word	SEQ_THROW_SR

			.byte	ANI_ZEROVELS

			.byte	ANI_RUNTOFRAME,9,1

			.byte	ANI_CODE
			.word	qb_release_ball,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; QB throws ball (PLANTED) short and fast (flipped)

			.globl	qb_throw_sl_anim 
			.byte	SCRIPT
qb_throw_sl_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_NEWSEQ
			.word	SEQ_THROW_SL

			.byte	ANI_ZEROVELS

			.byte	ANI_RUNTOFRAME,9,1

			.byte	ANI_SETFLAG
			.word	PF_FLIP

			.byte	ANI_CODE
			.word	qb_release_ball,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FLIP

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; QB throws ball deep and sorta fast (non-flipped)

			.globl	qb_throw_lr_anim
			.byte	SCRIPT
qb_throw_lr_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_THROW_LR

			.byte	ANI_INTSTREAM,72,14

			.byte	ANI_CLRFLAG
			.word	PF_FLIP

			.byte	ANI_CODE
			.word	qb_release_ball,0,0,0

			.byte	ANI_INTSTREAM,114,39

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; QB throws ball deep and sorta fast (flipped)

			.globl	qb_throw_ll_anim
			.byte	SCRIPT
qb_throw_ll_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_THROW_LL

			.byte	ANI_INTSTREAM,72,14

			.byte	ANI_SETFLAG
			.word	PF_FLIP

			.byte	ANI_CODE
			.word	qb_release_ball,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FLIP

			.byte	ANI_INTSTREAM,114,39
													  
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END



#;;;;;;;;;; QB slides on ground (instead of r_diveoh_anim)

			.globl	qb_slide_anim 
			.byte	SCRIPT
qb_slide_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_QB_SLIDE
			.byte	4

			.byte	ANI_ATTACK_ON,ATTMODE_PUSH,JOINT_RANKLE,JOINT_LANKLE,50
			.float	1.0,1.0,4.0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_SETYVEL
			.float	.50
			.byte	1

			.byte	ANI_INTSTREAM,34,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float .035

			.byte	ANI_CODE
			.word	grnd_hit_snd,P_GRND_HIT_SOFT_SND,0,0

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_INTSTREAM,50,25

			.byte	ANI_ATTACK_OFF

			.byte	ANI_RUNTOEND,1

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_GET_UP_B
			.byte	0
			.byte	ANI_GOTO
			.word	get_up_b_anim
			.byte	ANI_END


#;;;;;;;;;;  player attempts to stiff arm opponent

			.globl	stiff_arm2_anim
			.byte	SCRIPT
stiff_arm2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_TWOPART|MODE_STIFF_ARM

			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_QB_STIFF

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_CODE
			.word	rotate_torso,0,0,0

			.byte	ANI_ATTACK_ON,ATTMODE_PUSH,JOINT_LELBOW,JOINT_LWRIST,10
			.float	1.0,1.0,4.0

#			.byte	ANI_RUNTOFRAME,7,1
			.byte	ANI_INTSTREAM,7,11

			.byte	ANI_ATTACK_OFF

			.byte	ANI_RUNTOEND,1

		# turn off two_part
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_CODE
			.word	general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;  player attempts to stiff arm opponent

			.globl	stiff_arm2_f_anim
			.byte	SCRIPT
stiff_arm2_f_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_TWOPART|MODE_STIFF_ARM

			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_QBF_STIF

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_CODE
			.word	rotate_torso,0,0,0

			.byte	ANI_ATTACK_ON,ATTMODE_PUSH,JOINT_LELBOW,JOINT_LWRIST,10
			.float	1.0,1.0,4.0

#			.byte	ANI_RUNTOFRAME,7,1
			.byte	ANI_INTSTREAM,7,11

			.byte	ANI_ATTACK_OFF

			.byte	ANI_RUNTOEND,1

		# turn off two_part
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_CODE
			.word	general_run,0,0,0

			.byte	ANI_END


#;;;;;;;;;; player laterals ball (RUNNING) short and fast (non-flipped)

			.globl	m_lateralr_anim
			.byte	SCRIPT
m_lateralr_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ
#			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_M_LATERR
			.byte	0

			.byte	ANI_INTSTREAM,7,4
#			.byte	ANI_RUNTOFRAME,7,1

			.byte	ANI_CODE
			.word	lateral_ball,0,0,0

			.byte	ANI_RUNTOEND,1

		# turn off two_part
#			.byte	ANI_SETMODE
#			.word	0

#			.byte	ANI_CODE
#			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; player laterals ball (RUNNING) short and fast (flipped)

			.globl	m_laterall_anim
			.byte	SCRIPT
m_laterall_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ
#			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART

			.byte	ANI_ZEROVELS

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_M_LATERL
			.byte	0

			.byte	ANI_INTSTREAM,7,4
#			.byte	ANI_RUNTOFRAME,7,1

			.byte	ANI_SETFLAG
			.word	PF_FLIP

			.byte	ANI_CODE
			.word	lateral_ball,0,0,0

			.byte	ANI_CLRFLAG
			.word	PF_FLIP

			.byte	ANI_RUNTOEND,1

		# turn off two_part
#			.byte	ANI_SETMODE
#			.word	0

#			.byte	ANI_CODE
#			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END



#;;;;;;;;;; QB - stances

			.globl	qb_stance_anim
#			.globl	qb_stance_anim_top
#			.globl	qb_stance_anim_bot
			.globl	qb_ready_anim
			.globl	qb_ready_f_anim

#;;;;;;;; used on team selection screen

			.byte	SCRIPT
qb_ready_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_ZEROVELS
qb_ready_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_QB_RDY
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	qb_ready_loop

#;;;;;;;; used on team selection screen

			.byte	SCRIPT
qb_ready_f_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_ZEROVELS
qb_readyf_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_QB_RDYF
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	qb_readyf_loop



#			.byte	SCRIPT
#qb_stance_anim_bot:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORXZ|MODE_TWOPART
#			.byte	ANI_ZEROVELS
#			.byte	ANI_NEWSEQ
#			.word	SEQ_QBSTANCE
#			.byte	ANI_RUNTOEND,1
#			.byte	ANI_SETMODE
#			.word	0
#			.byte	ANI_GOTO
#			.word	qb_stance_loop

#			.byte	SCRIPT
#qb_stance_anim_top:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORXZ|MODE_TWOPART
#			.byte	ANI_INTTOSEQ,5
#			.word	SEQ_QBSTANCE
#			.byte	5
#			.byte	ANI_RUNTOEND,1
#			.byte	ANI_END

			.byte	SCRIPT
qb_stance_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_ZEROVELS
qb_stance_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_QBSTANCE
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	qb_stance_loop

;;;;;;;;;;
			.globl	qb_flip_stance_anim
#			.globl	qb_flip_stance_anim_top
#			.globl	qb_flip_stance_anim_bot

#			.byte	SCRIPT
#qb_flip_stance_anim_bot:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORXZ|MODE_TWOPART
#			.byte	ANI_ZEROVELS
#			.byte	ANI_NEWSEQ
#			.word	SEQ_QBFSTANC
#			.byte	ANI_RUNTOEND,1
#			.byte	ANI_SETMODE
#			.word	0
#			.byte	ANI_GOTO
#			.word	qb_flip_stance_loop
#
#			.byte	SCRIPT
#qb_flip_stance_anim_top:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORXZ|MODE_TWOPART
#			.byte	ANI_INTTOSEQ,5
#			.word	SEQ_QBFSTANC
#			.byte	5
#			.byte	ANI_RUNTOEND,1
#
#qb_flip_top_stance_loop:
#			.byte	ANI_NEWSEQ
#			.word	SEQ_QBFSTANC
#			.byte	ANI_RUNTOEND,1
#			.byte	ANI_GOTO
#			.word	qb_flip_top_stance_loop
#			.byte	ANI_END

			.byte	SCRIPT
qb_flip_stance_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_ZEROVELS
qb_flip_stance_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_QBFSTANC
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	qb_flip_stance_loop

;;;;;;;;;;
			.globl	duck_anim
			.globl	duck_flip_anim
			.byte	SCRIPT
duck_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT
			.byte	ANI_ZEROVELS
			.byte	ANI_NEWSEQ
			.word	SEQ_DUCK
			.byte	ANI_RUNTOFRAME,4,1
			.byte	ANI_INTSTREAM,6,12
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	qb_stance_anim


			.byte	SCRIPT
duck_flip_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT
			.byte	ANI_ZEROVELS
			.byte	ANI_NEWSEQ
			.word	SEQ_DUCKF
			.byte	ANI_RUNTOFRAME,4,1
			.byte	ANI_INTSTREAM,6,12
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	qb_flip_stance_anim

#;;;;;;;;;
#;;;;;;;;; MISC SEQUENCES
#;;;;;;;;;

#;;;;;;;;;;  player pushing

			.globl	t_push_anim
			.byte	SCRIPT
t_push_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CODE_BNZ
			.word	choose_target,0,0,0,tp_cont
			.byte	ANI_CODE
			.word	face_throw_target,128,0,0

tp_cont:
			.byte	ANI_ZEROVELS
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_T_PUSH
			.byte	0

			.byte	ANI_RUNTOFRAME,8,1

			.byte	ANI_ATTACK_ON,ATTMODE_PUSH,JOINT_RWRIST,JOINT_LWRIST,17
			.float	1.0,1.0,4.0

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	stance_anim_int
			.byte	ANI_END

#;;;;;;;;;;  player pushing

			.globl	t_running_push_anim
			.byte	SCRIPT
t_running_push_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_TWOPART|MODE_RUNNING_PUSH

#			.byte	ANI_INTTOSEQ,3
#			.word	SEQ_T_PUSH
#			.byte	0

			.byte	ANI_NEWSEQ
			.word	SEQ_T_RPUSH

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

#			.byte	ANI_CODE
#			.word	rotate_torso,0,0,0

			.byte	ANI_RUNTOFRAME,8,1

			.byte	ANI_ATTACK_ON,ATTMODE_PUSH,JOINT_RWRIST,JOINT_LWRIST,17
			.float	1.0,1.0,4.0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_ATTACK_OFF

		# turn off two_part
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  player jumps and kicks opponent and doesnt break stride

			.globl	t_runkic_anim
			.byte	SCRIPT
t_runkic_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_CODE_BNZ
			.word	choose_target,0,0,0,rk_cont
			.byte	ANI_CODE
			.word	face_throw_target,128,0,0
rk_cont:
			.byte	ANI_SETVELS
			.float	0.0,0.0,0.60

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_T_RUNKIC
			.byte	0

			.byte	ANI_INTSTREAM,17,8

			.byte	ANI_ATTACK_ON,ATTMODE_PUSH,JOINT_RKNEE,JOINT_RANKLE,10
			.float	1.2,1.2,2.5

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_RUNTOFRAME,25,1

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETFRICTION
			.float .085

			.byte	ANI_RUNTOFRAME,44,1

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END




#;;;;;;;;;;  player jumps up to block a kick or somethimg

#			.globl	blockpas_anim
#			.byte	SCRIPT
#blockpas_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_BLOCKPAS
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_INTSTREAM,18,7
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
## this will want to be a ani_code someday....to put rules on height of jump
#			.byte	ANI_SETVELS
#			.float	0.0,1.2,0.2
#			.byte 1
#
#			.byte	ANI_INTSTREAM,36,AA_TOGROUND
#
##			.byte	ANI_SETFRICTION
##			.float .0856
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_GOTO
#			.word	stance_anim_int
#			.byte	ANI_END

#;;;;;;;;;;  player punts the ball
			
			
			.globl	punt_ball_anim
			.byte	SCRIPT
punt_ball_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_M_KICK

			.byte	ANI_RUNTOFRAME,4,1
#			.byte	ANI_INTSTREAM,12,7

		#punt ball
			.byte	ANI_CODE
			.word	attack_snd,P_KICK_BALL_SND,0,0

			.byte	ANI_CODE
			.word	punt_ball,0,0,0
			
			.byte	ANI_SETVELS
			.float	0.0,0.0,0.25
			
			.byte	ANI_RUNTOFRAME,41,1

			.byte	ANI_SETFRICTION
			.float .055

			.byte	ANI_RUNTOEND,1

			.byte	ANI_PREP_XITION
			.word	3,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END
			
#;;;;;;;;;;  player attempts to kick ball

			.globl	kickball_anim
			.byte	SCRIPT
kickball_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_M_KICK

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.25

			.byte	ANI_SKIPFRAMES,4

		#kick ball sound
			.byte	ANI_CODE
			.word	attack_snd,P_KICK_BALL_SND,0,0

			.byte	ANI_INTSTREAM,38,30

			.byte	ANI_SETFRICTION
			.float .055

			.byte	ANI_INTSTREAM,32,25

			.byte	ANI_PREP_XITION
			.word	3,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  player attempts a field goal

			.globl	kickball2_anim
			.byte	SCRIPT
kickball2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_M_KICK




			.byte	ANI_SETVELS
#			.float	0.0,0.0,0.25
			.float	0.0,0.0,0.20



# Delay field goal kick a bit
#			.byte	ANI_SKIPFRAMES,4
			.byte	ANI_RUNTOFRAME,4,10





#kick ball sound
			.byte	ANI_CODE
			.word	attack_snd,P_KICK_BALL_SND,0,0

			.byte	ANI_INTSTREAM,38,30

			.byte	ANI_SETFRICTION
			.float .055

			.byte	ANI_INTSTREAM,32,25

			.byte	ANI_PREP_XITION
			.word	3,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;  player attempts to plow thru people with his head

			.globl	running_plow_anim
			.byte	SCRIPT
running_plow_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_TWOPART

			.byte	ANI_NEWSEQ
			.word	SEQ_M_PLOWHD

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_ATTACK_ON,ATTMODE_HEAD,JOINT_LSHOULDER,JOINT_RSHOULDER,24
			.float	1.0,1.0,3.0

			.byte	ANI_INTSTREAM,8,3

			.byte	ANI_INTSTREAM,11,7

		# hold head down for a little while
			.byte	ANI_INTSTREAM,4,18

			.byte	ANI_INTSTREAM,21,11

			.byte	ANI_ATTACK_OFF

		# turn off two_part
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

#			.byte	ANI_PREP_XITION
#			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  receiver waves at qb

			.globl	m_wavqb_anim
			.byte	SCRIPT
m_wavqb_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_TWOPART

			.byte	ANI_NEWSEQ
			.word	SEQ_M_WAVQB

			.byte	ANI_CODE
			.word	plyr_open_sp,-1,0,0
			
			.byte	ANI_INTSTREAM,84,65

		# turn off two_part
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_CODE
			.word	general_run,0,0,0

			.byte	ANI_END

#;;;;;;;;;;  receiver waves at qb (flipped)

			.globl	m_wavqbf_anim
			.byte	SCRIPT
m_wavqbf_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_TWOPART

			.byte	ANI_NEWSEQ
			.word	SEQ_M_WAVQBF

#			.byte	ANI_CODE
#			.word	plyr_open_sp,-1,0,0

			.byte	ANI_INTSTREAM,84,65

		# turn off two_part
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_CODE
			.word	general_run,0,0,0

			.byte	ANI_END




#;;;;;;;;;;  player attempts to stiff arm opponent

			.globl	stiff_arm_anim
			.byte	SCRIPT
stiff_arm_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_TWOPART|MODE_STIFF_ARM

			.byte	ANI_NEWSEQ
			.word	SEQ_STIFARM

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

#			.byte	ANI_CODE
#			.word	rotate_torso,0,1,0

# if gonna stiff someone in front of me... head plow
			.byte	ANI_CODE_BNZ
			.word	rotate_torso,0,1,0,running_plow_anim

			.byte	ANI_INTSTREAM,5,2

			.byte	ANI_ATTACK_ON,ATTMODE_PUSH,JOINT_LELBOW,JOINT_LWRIST,10
			.float	1.0,1.0,3.0

			.byte	ANI_INTSTREAM,9,4

		# hold arm out for a little while
			.byte	ANI_INTSTREAM,2,6

			.byte	ANI_ATTACK_OFF

			.byte	ANI_INTSTREAM,15,10

		# turn off two_part
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  player attempts to stiff arm opponent

			.globl	stiff_arm_f_anim
			.byte	SCRIPT
stiff_arm_f_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_TWOPART|MODE_STIFF_ARM

			.byte	ANI_NEWSEQ
			.word	SEQ_STIFARMF

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

#			.byte	ANI_CODE
#			.word	rotate_torso,0,1,0

# if gonna stiff someone in front of me... head plow
			.byte	ANI_CODE_BNZ
			.word	rotate_torso,0,1,0,running_plow_anim

			.byte	ANI_INTSTREAM,5,2

			.byte	ANI_ATTACK_ON,ATTMODE_PUSH,JOINT_RELBOW,JOINT_RWRIST,10
			.float	1.0,1.0,3.0

			.byte	ANI_INTSTREAM,9,4

		# hold arm out for a little while
			.byte	ANI_INTSTREAM,2,6

			.byte	ANI_ATTACK_OFF

			.byte	ANI_INTSTREAM,15,10

		# turn off two_part
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END



#;;;;;;;;;;  player attempts to stiff arm opponent with NOT ENOUGH turbo remaining

			.globl	stiff_arm_nt_anim
			.byte	SCRIPT
stiff_arm_nt_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_TWOPART|MODE_STIFF_ARM
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_TWOPART

			.byte	ANI_NEWSEQ
			.word	SEQ_STIFARM

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_INTSTREAM,5,2

			.byte	ANI_ATTACK_ON,ATTMODE_PUSH,JOINT_LELBOW,JOINT_LWRIST,4
			.float	1.0,1.0,3.0

			.byte	ANI_INTSTREAM,9,4

			.byte	ANI_ATTACK_OFF
		# hold arm out for a little while
			.byte	ANI_INTSTREAM,2,6

#			.byte	ANI_ATTACK_OFF

			.byte	ANI_INTSTREAM,15,6

		# turn off two_part
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  player jumps up and swats at ball

			.globl	swat_anim
			.byte	SCRIPT
swat_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_SWAT

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.15

			.byte	ANI_SKIPFRAMES,15

			.byte	ANI_CODE
			.word	face_carrier,256,0,0

			.byte	ANI_INTSTREAM,5,3

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

# this will want to be a ani_code someday....to put rules on height of jump
			.byte	ANI_SETYVEL
			.float	1.3
			.byte	1

# this int...needs a AA_TOBALL or something (thats all that needs to change)
			.byte	ANI_INTSTREAM,15,10
#ready to swat

			.byte	ANI_INTSTREAM,12,10
#swatted

			.byte	ANI_INTSTREAM,8,AA_TOGROUND
#hold till ground

			.byte	ANI_SETMODE
			.word	MODE_ROTONLY

			.byte	ANI_INTSTREAM,16,6
#follow thru

			.byte	ANI_SETMODE
			.word	MODE_UNINT

			.byte	ANI_PREP_XITION
			.word	5,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END



#;;;;;;;;;;  player jumps up and swats at ball (2)

#			.globl	swat_2_anim
#			.byte	SCRIPT
#swat_2_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_SWAT_2
#
#			.byte	ANI_SETVELS
#			.float	0.0,0.0,0.2
#
#			.byte	ANI_INTSTREAM,18,9
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
## this will want to be a ani_code someday....to put rules on height of jump
#			.byte	ANI_SETVELS
#			.float	0.0,1.2,0.2
#			.byte 1
#
#			.byte	ANI_RUNTOFRAME,29,1
#
#			.byte	ANI_INTSTREAM,27,AA_TOGROUND
#
#			.byte	ANI_INTSTREAM,21,10
#
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END

#;;;;;;;;;;  player dives with ball in hand (reaching for extra yards)


			.globl	r_diveoh_anim
			.byte	SCRIPT
r_diveoh_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_ATTACK_ON,ATTMODE_PUSH,JOINT_LELBOW,JOINT_RELBOW,50
			.float	2.0,2.0,4.0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_R_DIVEOH
			.byte	11

			.byte	ANI_CODE
			.word	attack_snd,P_DIVESTRT_SND,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_SETYVEL
			.float	1.0

	.if DOSETCAM
#			.byte	ANI_SETCAMBOT
#			.word	csi_zm_divetack
# If victim has ball, do some camera shit - otherwise, skip it
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,cont_dive15
			.byte	ANI_SETCAMBOT
			.word	csi_zm_divetack
	.endif
cont_dive15:

			.byte	1
			.byte	ANI_INTSTREAM,41,AA_TOGROUND

			.byte	ANI_ATTACK_OFF

			.byte	ANI_CODE
			.word	grnd_hit_snd,P_GRND_HIT1_SND,0,0
			.byte	ANI_CODE
			.word	dust_cloud,0,0,0
			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float .067

			.byte	ANI_RUNTOFRAME,58,1

# hold ball out on ground
			.byte	ANI_INTSTREAM,2,15

			.byte	ANI_INTSTREAM,95,30

			.byte	ANI_PREP_XITION
			.word	3,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  player jumps up and swats at ball

			.globl	air_swat_anim
			.byte	SCRIPT
air_swat_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_SWAT
			.byte	39

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0
			
			.byte	ANI_INTSTREAM,6,AA_TOGROUND
#hold till ground

#			.byte	ANI_SETMODE
#			.word	MODE_ROTONLY

			.byte	ANI_INTSTREAM,31,15
#follow thru

#			.byte	ANI_SETMODE
#			.word	MODE_UNINT

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END
			
#;;;;;;;;;; player get up from chest

			.globl	get_up_c_anim
			.byte	SCRIPT
get_up_c_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_LATEHITABLE

			.byte	ANI_NEWSEQ
			.word	SEQ_M_GU_C2
#			.word	SEQ_GET_UP_C

			.byte	ANI_ZEROVELS

#			.byte	ANI_INTSTREAM,43,20
			.byte	ANI_INTSTREAM,59,35

			.byte	ANI_CODE
			.word	maybe_qb_mad,0,0,0
			
			.byte	ANI_CODE
			.word	maybe_receiver_mad,0,0,0

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0
			
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; player get up from butt

			.globl	get_up_b_anim
			.byte	SCRIPT
get_up_b_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_LATEHITABLE

			.byte	ANI_INTTOSEQ,5
			.word	SEQ_GET_UP_B
			.byte	8

			.byte	ANI_ZEROVELS

			.byte	ANI_INTSTREAM,56,30

			.byte	ANI_CODE
			.word	maybe_qb_mad,0,0,0
			
			.byte	ANI_CODE
			.word	maybe_receiver_mad,0,0,0

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END



#;;;;;;;;;; player get up from chest and then be in pain

#			.globl	get_up_c1_hurt_anim
#			.byte	SCRIPT
#get_up_c1_hurt_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_CLRFLAG
#			.word	PF_LATEHITABLE
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_CODE
#			.word	drop_ball,0,0,0
#
#			.byte	ANI_INTTOSEQ,8
#			.word	SEQ_GUC_HURT
#			.byte	2
#
#			.byte	ANI_CODE
#			.word	taunt_speech,29,0,0
#
#			.byte	ANI_INTSTREAM,38,50
#
#			.byte	ANI_INTSTREAM,54,112
#			.byte	ANI_INTSTREAM,54,112
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_TURNHACK
#			.word	256
#
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END


#;;;;;;;;;; player get up from chest and then be in pain

			.globl	get_up_c2_hurt_anim
			.byte	SCRIPT
get_up_c2_hurt_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_LATEHITABLE

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	drop_ball,0,0,0

			.byte	ANI_INTTOSEQ,8
			.word	SEQ_GUC_HRT2
			.byte	2

			.byte	ANI_CODE
			.word	taunt_speech,29,0,0

			.byte	ANI_INTSTREAM,68,125
			.byte	ANI_INTSTREAM,68,125
			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	-256

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;; player get up from chest and then be in pain

			.globl	get_up_c3_hurt_anim
			.byte	SCRIPT
get_up_c3_hurt_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_LATEHITABLE

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	drop_ball,0,0,0

			.byte	ANI_INTTOSEQ,8
			.word	SEQ_GUC_HRT3
			.byte	2

			.byte	ANI_CODE
			.word	taunt_speech,28,0,0

			.byte	ANI_INTSTREAM,68,125
			.byte	ANI_INTSTREAM,68,125

			.byte	ANI_CODE
			.word	taunt_speech,30,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;; player get up from chest and then be in pain

#			.globl	get_up_c4_hurt_anim
#			.byte	SCRIPT
#get_up_c4_hurt_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_CLRFLAG
#			.word	PF_LATEHITABLE
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_CODE
#			.word	drop_ball,0,0,0
#
#			.byte	ANI_INTTOSEQ,8
#			.word	SEQ_GUC_HRT4
#			.byte	2
#
#			.byte	ANI_CODE
#			.word	taunt_speech,29,0,0
#
#			.byte	ANI_INTSTREAM,36,70
#			.byte	ANI_INTSTREAM,36,70
#			.byte	ANI_INTSTREAM,36,70
#			.byte	ANI_INTSTREAM,36,70
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END


#;;;;;;;;;; player get up from laying on back then be in pain

			.globl	get_up_b1_hurt_anim
			.byte	SCRIPT
get_up_b1_hurt_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_LATEHITABLE

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	drop_ball,0,0,0

			.byte	ANI_INTTOSEQ,8
			.word	SEQ_GUB_HURT
			.byte	2

			.byte	ANI_CODE
			.word	taunt_speech,28,0,0

			.byte	ANI_INTSTREAM,36,70
			.byte	ANI_INTSTREAM,36,85
			.byte	ANI_INTSTREAM,36,85
			.byte	ANI_INTSTREAM,36,85
			.byte	ANI_RUNTOEND,1

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; player get up from laying on back then be in pain

			.globl	get_up_b2_hurt_anim
			.byte	SCRIPT
get_up_b2_hurt_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_CLRFLAG
			.word	PF_LATEHITABLE

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	drop_ball,0,0,0

			.byte	ANI_INTTOSEQ,8
			.word	SEQ_GUB_HRT2
			.byte	2

			.byte	ANI_CODE
			.word	taunt_speech,29,0,0

			.byte	ANI_INTSTREAM,36,70
			.byte	ANI_INTSTREAM,36,70
			.byte	ANI_INTSTREAM,36,70
			.byte	ANI_INTSTREAM,36,70
#			.byte	ANI_RUNTOEND,1

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; player get up from laying on back then be in pain

#			.globl	get_up_b3_hurt_anim
#			.byte	SCRIPT
#get_up_b3_hurt_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_CLRFLAG
#			.word	PF_LATEHITABLE
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_CODE
#			.word	drop_ball,0,0,0
#
#			.byte	ANI_INTTOSEQ,8
#			.word	SEQ_GUB_HRT3
#			.byte	2
#
#			.byte	ANI_RUNTOFRAME,126,1
#
#			.byte	ANI_CODE
#			.word	taunt_speech,31,0,0
#
#			.byte	ANI_RUNTOFRAME,194,1
#
#			.byte	ANI_CODE
#			.word	taunt_speech,31,0,0
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END


#;;;;;;;;;;  player dives for an opponent (spaz)

			.globl	t_crzdiv_anim
			.byte	SCRIPT
t_crzdiv_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_T_CRZDIV

# Select the target victim, if no one in front of me, or is more than
# 15 yards away, then skip the leapatt

			.byte	ANI_CODE_BNZ
			.word	my_choose_target,0,0,0,no_target

# If victim target is within x angle of my facing dir, then turn to
# face him

			.byte	ANI_CODE_BNZ
			.word	maybe_spear_head,0,0,0,t_sprhed_anim

#			.byte	ANI_CODE_BNZ
#			.word	maybe_drop_kick,0,0,0,t_drpkic_anim

	# do dive
#			.byte	ANI_SKIPFRAMES,14
			
#new
			.byte	ANI_SETVELS
			.float	0.0,0.65,1.2
#new

			.byte	ANI_INTSTREAM,15,3

			.byte	ANI_CODE
			.word	attack_snd,P_DIVESTRT_SND,0,0

#			.byte	1
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_NOCATCH

			.byte	ANI_ATTACK_ON,ATTMODE_DIVE,JOINT_NECK,JOINT_PELVIS,100
			.float	.85,1.5,7.0
#			.float	.85,1.5,8.0

#new
#			.byte	1,1,1
#new

			.byte	ANI_LEAPATT
			.word	AA_TGT_TARGET,JOINT_TORSO,22
		# joint offsets
			.float	0.0,-4.4,0.0
		# yvel_cap, xzvel_cap, xzvel_min
			.float	.70,2.2,1.65

			.byte	1
#			.byte	ANI_INTSTREAM,55,AA_TOGROUND
			.byte	ANI_INTSTREAM,58,AA_TOGROUND
			
	# landed
	# let diver grab opponent...even if on ground
	
			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_SETFRICTION
			.float .060
			
			.byte	1,1,1,1

			.byte	ANI_SETYVEL
			.float	.40

			.byte	ANI_INTSTREAM,7,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,P_GRND_HIT_SOFT_SND,0,0

#			.byte	ANI_CODE
#			.word	dust_cloud,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_M_GU_C2
#			.word	SEQ_GET_UP_C
			.byte	0
			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END

no_target:
#			.byte	ANI_NEWSEQ
#			.word	SEQ_T_CRZDIV

			.byte	ANI_SKIPFRAMES,14

			.byte	ANI_CODE
			.word	attack_snd,P_DIVESTRT_SND,0,0

			.byte	1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_NOCATCH

			.byte	ANI_SETVELS
			.float	0,.85,1.60

			.byte	ANI_ATTACK_ON,ATTMODE_DIVE,JOINT_NECK,JOINT_PELVIS,100
			.float	2.2,2.2,2.0

			.byte	1
			.byte	ANI_INTSTREAM,58,AA_TOGROUND

	# landed
	# let diver grab opponent...even if on ground
	
			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_SETFRICTION
			.float .060
			
			.byte	1,1,1,1

			.byte	ANI_SETYVEL
			.float	.40

			.byte	ANI_INTSTREAM,7,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,P_GRND_HIT_SOFT_SND,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_M_GU_C2
#			.word	SEQ_GET_UP_C
			.byte	0
			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END

			
#;;;;;;;;;;  player drop kicks an opponent

#			.globl	t_drpkic_anim
#			.byte	SCRIPT
#t_drpkic_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_T_DRPKIC
#
#			.byte	ANI_SKIPFRAMES,7
#
#			.byte	ANI_CODE
#			.word	attack_snd,P_DIVESTRT_SND,0,0
#
#			.byte	ANI_RUNTOFRAME,10,1
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_NOCATCH
#
#			.byte	1
#
#			.byte	ANI_LEAPATT
#
#
#
##			.word	AA_TGT_TARGET,JOINT_TORSO,20
#			.word	AA_TGT_TARGET,JOINT_TORSO,22
#
#
#
#
#		# joint offsets
#			.float	0,-8.0,8.0
#		# yvel_cap, xzvel_cap, xzvel_min
#			.float	1.2,2.2,1.1
#
#			.byte	ANI_ATTACK_ON,ATTMODE_KICK,JOINT_LANKLE,JOINT_RANKLE,100
#			.float	1.0,2.0,2.0
#
#			.byte	ANI_INTSTREAM,43,AA_TOGROUND
#
#			.byte	ANI_ATTACK_OFF
#
#			.byte	ANI_CODE
#			.word	dust_cloud,0,0,0
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,0
#
#			.byte	ANI_SETFRICTION
#			.float .084
#
#			.byte	ANI_RUNTOFRAME,64,1
#
#			.byte	ANI_INTSTREAM,95,65
#
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END



#;;;;;;;;;;  player rams opponent with turbo shoulder to tackle him

			.globl	t_dshoul_anim
			.byte	SCRIPT
t_dshoul_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_T_DSHOUL

			.byte	ANI_SKIPFRAMES,14

			.byte	ANI_SETVELS
			.float	0.0,0.0,.65

			.byte	ANI_CODE
			.word	get_hit_snd,P_DIVEHIT_SND,0,0

			.byte	ANI_RUNTOFRAME,17,1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_NOCATCH

			.byte	ANI_SETVELS
			.float	0.0,.12,.93
			.byte	1
			.byte	ANI_INTSTREAM,36,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

#			.byte	ANI_ZEROVELS
			.byte	ANI_SETFRICTION
			.float .054

			.byte	ANI_RUNTOFRAME,90,1

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_M_GU_C2
#			.word	SEQ_GET_UP_C
			.byte	0
			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END


#;;;;;;;;;;  player uppercuts opponent

#			.globl	t_uprcut_anim
#			.byte	SCRIPT
#t_uprcut_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_T_UPRCUT
#
#			.byte	ANI_CODE
#			.word	attack_snd,-1,0,0
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_RUNTOFRAME,3,1
#
#			.byte	ANI_CODE
#			.word	attack_snd,P_HARD_HIT1_SND,0,0
#
#			.byte	ANI_RUNTOFRAME,12,1
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_CANBLOCK
#
#			.byte	ANI_INTSTREAM,66,25
#
#			.byte	ANI_GOTO
#			.word	stance_anim_int
#			.byte	ANI_END


#;;;;;;;;;;  player rams opponent without turbo shoulder to tackle him

			.globl	t_stshld_anim
			.byte	SCRIPT
t_stshld_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_T_STSHLD

			.byte	ANI_ZEROVELS

			.byte	ANI_SKIPFRAMES,4

			.byte	ANI_CODE
			.word	attack_snd,P_KNEE_DUDE_SP,0,0

			.byte	ANI_INTSTREAM,13,26

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0
			
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;  player spears (with head) opponent in the damn head

			.globl	t_sprhed_anim
			.byte	SCRIPT
t_sprhed_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_T_SPRHED

			.byte	ANI_ATTACK_ON,ATTMODE_SPEAR_HEAD,JOINT_RSHOULDER,JOINT_LSHOULDER,100
#			.float	1.2,1.2,2.1
			.float	2.5,2.5,5.5
			

#new
			.byte	ANI_SETVELS
			.float	0.0,0.65,1.2
#new

			.byte	ANI_INTSTREAM,11,3

			.byte	ANI_CODE
			.word	attack_snd,P_DIVESTRT_SND,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_NOCATCH

#new
#			.byte	1,1,1
#new

			.byte	ANI_LEAPATT
			.word	AA_TGT_TARGET,JOINT_TORSO,22
		# joint offsets
			.float	0.0,-3.1,0.0
		# yvel_cap, xzvel_cap, xzvel_min
			.float	.70,2.2,1.65
#			.float	.75,2.2,1.1

			.byte	1
#			.byte	ANI_INTSTREAM,29,AA_TOGROUND
			.byte	ANI_INTSTREAM,32,AA_TOGROUND

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_NOCATCH

			.byte	ANI_RUNTOEND,1

			.byte	ANI_SETFRICTION
			.float	.080

			.byte	ANI_ATTACK_OFF

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_M_GU_C2
#			.word	SEQ_GET_UP_C
			.byte	0
			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END



#;;;;;;;;;; tackler does a leaping handstand

#			.globl	handstand_anim
#			.byte	SCRIPT
#handstand_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_NOCYL
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_CODE
#			.word	attack_snd,P_KNEE_DUDE_SP,0,0
#
#			.byte	ANI_INTTOSEQ,3
#			.word	SEQ_P_FROL_A
#			.byte	1
#			.byte	1,1,1,1
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY|MODE_NOCYL
#
#			# hit ball carrier in neck in 55 ticks
#			.byte	ANI_LEAPATT
#			.word	AA_TGT_CARRIER,JOINT_PELVIS,35
#		# joint offsets (the Y offset...must take into account: ( (dirt frame-1 yani)-(dirt_frame yani) * .12898
#			.float	0.0,-2.0,5.0
#		# yvel_cap, xzvel_cap, xzvel_min
#			.float	1.4,1.0,0.0
#
#			.byte	ANI_ATTACK_ON,ATTMODE_LATE_HIT,JOINT_NECK,JOINT_RELBOW,40
#			.float	1.5,1.5,2.0
#
#			.byte	1
#			.byte	ANI_INTSTREAM,9,AA_TOGROUND
#
#			.byte	ANI_CODE
#			.word	dust_cloud,0,0,0
#
#			.byte	ANI_ATTACK_OFF
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,-1
#			
#			.byte	ANI_SHAKER,3
#			.word	10
#
#			.byte	ANI_RUNTOFRAME,30,1
#			
#			.byte	ANI_SETVELS
#			.float	0.0,0.80,0.0
#			.byte	1
#			.byte	ANI_INTSTREAM,23,AA_TOGROUND
#
#			
#			.byte	ANI_RUNTOEND,1
#
##			.byte	ANI_TURNHACK
##			.word	512
#
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
#			
#
#;;;;;;;;;; tackler does a leaping elbow drop on opponent

			.globl	elbow_drp_anim
			.byte	SCRIPT
elbow_drp_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	attack_snd,P_KNEE_DUDE_SP,0,0

			.byte	ANI_INTTOSEQ,5
			.word	SEQ_M_ELBDRP
			.byte	14

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_NOCYL

			# hit ball carrier in neck in 55 ticks
			.byte	ANI_LEAPATT
			.word	AA_TGT_CARRIER,JOINT_PELVIS,29
		# joint offsets (the Y offset...must take into account: ( (dirt frame-1 yani)-(dirt_frame yani) * .12898
			.float	-2.0,-3.21,0
		# yvel_cap, xzvel_cap, xzvel_min
			.float	1.4,1.0,0.0

			.byte	ANI_ATTACK_ON,ATTMODE_LATE_HIT,JOINT_NECK,JOINT_RELBOW,45
			.float	1.5,1.5,2.0

			.byte	1
			.byte	ANI_INTSTREAM,39,AA_TOGROUND

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1
			
			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_INTSTREAM,27,15

			.byte	ANI_INTSTREAM,123,61

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_PREP_XITION
			.word	6,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; tackler does a leaping stomp on dudes stomach

#			.globl	gut_stomp_anim
#			.byte	SCRIPT
#gut_stomp_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_NOCYL
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_M_STOMPG
#			
#			.byte	ANI_INTSTREAM,31,10
#			
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY|MODE_NOCYL
#
#			# hit ball carrier in gut in 29 ticks
#			.byte	ANI_LEAPATT
#			.word	AA_TGT_CARRIER,JOINT_PELVIS,29
#		# joint offsets (the Y offset...must take into account: ( (dirt frame-1 yani)-(dirt_frame yani) * .12898
#			.float	0.0,0.0,0
#		# yvel_cap, xzvel_cap, xzvel_min
#			.float	1.4,1.0,0.0
#
#			.byte	ANI_INTSTREAM,15,10
#			
#			.byte	ANI_ATTACK_ON,ATTMODE_LATE_HIT,JOINT_LANKLE,JOINT_RANKLE,100
#			.float	1.5,1.5,2.0
#			
#			.byte	ANI_CODE
#			.word	attack_snd,P_KNEE_DUDE_SP,0,0
#			
#			.byte	ANI_INTSTREAM,9,AA_TOGROUND
#		#hit em
#		
#			.byte	ANI_ATTACK_OFF
#			.byte	ANI_ZEROVELS
#			
#			.byte	ANI_INTSTREAM,20,15
#			.byte	ANI_CODE
#			.word	attack_snd,P_KNEE_DUDE_SP,0,0
#			.byte	ANI_ATTACK_ON,ATTMODE_LATE_HIT,JOINT_LANKLE,JOINT_RANKLE,11
#			.float	1.5,1.5,2.0
#			.byte	ANI_INTSTREAM,12,9
#			.byte	ANI_SHAKER,3
#			.word	10
#		#hit em again
#			.byte	ANI_ATTACK_OFF
#			
#			.byte	ANI_INTSTREAM,15,12
#			.byte	ANI_CODE
#			.word	attack_snd,P_KNEE_DUDE_SP,0,0
#			.byte	ANI_ATTACK_ON,ATTMODE_LATE_HIT,JOINT_LANKLE,JOINT_RANKLE,11
#			.float	1.5,1.5,2.0
#			.byte	ANI_INTSTREAM,12,9
#			.byte	ANI_SHAKER,3
#			.word	10
#		#hit em again
#			.byte	ANI_ATTACK_OFF
#			
#			.byte	ANI_INTSTREAM,15,12
#			.byte	ANI_CODE
#			.word	attack_snd,P_KNEE_DUDE_SP,0,0
#			.byte	ANI_ATTACK_ON,ATTMODE_LATE_HIT,JOINT_LANKLE,JOINT_RANKLE,11
#			.float	1.5,1.5,2.0
#			.byte	ANI_INTSTREAM,10,8
#			.byte	ANI_SHAKER,3
#			.word	10
#		#hit em again
#			.byte	ANI_ATTACK_OFF
#			
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_TURNHACK
#			.word	512
#
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END

			
#;;;;;;;;;; tackler does a leaping butt drop on opponent

			.globl	butt_drp_anim
			.byte	SCRIPT
butt_drp_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_NOCYL

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	attack_snd,P_KNEE_DUDE_SP,0,0

			.byte	ANI_INTTOSEQ,5
			.word	SEQ_M_BUTDRP
			.byte	14

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_NOCYL

			# hit ball carrier in neck in 55 ticks
			.byte	ANI_LEAPATT
			.word	AA_TGT_CARRIER,JOINT_PELVIS,30
		# joint offsets (the Y offset...must take into account: ( (dirt frame-1 yani)-(dirt_frame yani) * .12898
			.float	0.0,-3.0,0.0
		# yvel_cap, xzvel_cap, xzvel_min
			.float	1.4,1.0,0.0

			.byte	ANI_ATTACK_ON,ATTMODE_LATE_HIT,JOINT_NECK,JOINT_PELVIS,45
			.float	1.5,2.5,2.0

			.byte	1
			.byte	ANI_INTSTREAM,29,AA_TOGROUND

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETVELS
			.float	0.0,0.75,0.0

			.byte	ANI_SETMODE
			.word	MODE_UNINT

			.byte	ANI_RUNTOFRAME,74,1

			.byte	ANI_INTSTREAM,100,65


			.byte	ANI_PREP_XITION
			.word	5,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END



#;;;;;;;;;; tackler does a belly flop on opponent

			.globl	belly_flop_anim
			.byte	SCRIPT
belly_flop_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_NOCYL

			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_M_BELLYF

			.byte	ANI_CODE
			.word	attack_snd,P_KNEE_DUDE_SP,0,0
			
			.byte	ANI_RUNTOFRAME,3,1
			
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_NOCYL

			# hit ball carrier in neck in 55 ticks
			.byte	ANI_LEAPATT
			.word	AA_TGT_CARRIER,JOINT_PELVIS,40
		# joint offsets (the Y offset...must take into account: ( (dirt frame-1 yani)-(dirt_frame yani) * .12898
			.float	0.0,0.0,0.0
		# yvel_cap, xzvel_cap, xzvel_min
			.float	1.4,1.0,0.0

			.byte	ANI_ATTACK_ON,ATTMODE_LATE_HIT,JOINT_NECK,JOINT_LKNEE,45
			.float	1.0,1.0,2.2

			.byte	1
			.byte	ANI_INTSTREAM,38,AA_TOGROUND

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_ATTACK_OFF

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_SHAKER,5
			.word	10

			.byte	ANI_SETVELS
			.float	0.0,0.45,0.0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_NOCYL

			.byte	ANI_INTSTREAM,17,25

			.byte	ANI_INTSTREAM,114,75
#			.byte	ANI_RUNTOEND,1
			
			.byte	ANI_SETMODE
			.word	MODE_UNINT
						
			.byte	ANI_PREP_XITION
			.word	5,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END


#;;;;;;;;;; tackler does a leaping butt drop on opponent

#			.globl	knee_drp_anim
#			.byte	SCRIPT
#knee_drp_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_CODE
#			.word	attack_snd,P_KNEE_DUDE_SP,0,0
#
#			.byte	ANI_INTTOSEQ,5
#			.word	SEQ_M_KNEDRP
#			.byte	15
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY|MODE_NOCYL
#
#			# hit ball carrier in neck in 55 ticks
#			.byte	ANI_LEAPATT
#			.word	AA_TGT_CARRIER,JOINT_PELVIS,30
#		# joint offsets (the Y offset...must take into account: ( (dirt frame-1 yani)-(dirt_frame yani) * .12898
#			.float	0.0,-3.00,0.0
#		# yvel_cap, xzvel_cap, xzvel_min
#			.float	1.4,1.0,0.0
#
#			.byte	5
#
#			.byte	ANI_ATTACK_ON,ATTMODE_LATE_HIT,JOINT_RKNEE,JOINT_LKNEE,45
#			.float	1.5,1.5,2.0
#
#			.byte	1
#			.byte	ANI_INTSTREAM,34-5,AA_TOGROUND
#
#			.byte	ANI_CODE
#			.word	dust_cloud,0,0,0
#
#			.byte	ANI_ATTACK_OFF
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,0
#
#			.byte	ANI_SHAKER,3
#			.word	10
#
#			.byte	ANI_INTSTREAM,80,65
#
#			.byte	ANI_PREP_XITION
#			.word	5,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
#
#
##			.byte	ANI_CODE
##			.word	general_run,0,0,0
##			.byte	ANI_END


#;;;;;;;;;; tackler kicks player (on ground) in head

#			.globl	foot_stomp_anim
#			.byte	SCRIPT
#foot_stomp_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_NOCYL
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_CODE
#			.word	face_carrier,512,0,0
#
#			.byte	ANI_INTTOSEQ,4
#			.word	SEQ_M_STOMPH
#			.byte	0
#
#			.byte	ANI_RUNTOFRAME,17,1
#
#			.byte	ANI_CODE
#			.word	attack_snd,-1,0,0
#
#			.byte	ANI_ATTACK_ON,ATTMODE_LATE_HIT,JOINT_RKNEE,JOINT_RANKLE,16
#			.float	2.5,4.0,3.0
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END


#;;;;;;;;;; tackler does a leaping elbow drop on opponent

#			.globl	punch_dwn_anim
#			.byte	SCRIPT
#punch_dwn_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_CODE
#			.word	face_carrier,0,0,0
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_M_PNCHDN
#
#			.byte	ANI_CODE
#			.word	attack_snd,P_KNEE_DUDE_SP,0,0
#
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_NOCYL
#
#			.byte	ANI_ATTACK_ON,ATTMODE_LATE_HIT,JOINT_RELBOW,JOINT_RWRIST,30
#			.float	1.5,4.5,2.0
#
#			.byte	ANI_INTSTREAM,28,20
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,0
#
#			.byte	ANI_SHAKER,3
#			.word	10
#			
#			.byte	ANI_INTSTREAM,30,10
#
#			.byte	ANI_ATTACK_OFF
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_M_PNCHDN
#
#			.byte	ANI_CODE
#			.word	attack_snd,P_KNEE_DUDE_SP,0,0
#
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_NOCYL
#
#			.byte	ANI_INTSTREAM,15,9
#
#			.byte	ANI_ATTACK_ON,ATTMODE_LATE_HIT,JOINT_RELBOW,JOINT_RWRIST,13
#			.float	1.5,4.5,2.0
#
#			.byte	ANI_INTSTREAM,13,7
#
#			.byte	ANI_INTSTREAM,30,21
#
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END


#;;;;;;;;;; tackler taunts (1) the player on the ground

			.globl	taunt_1_anim
			.byte	SCRIPT
taunt_1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS
			
			.byte	ANI_CODE
			.word	face_carrier,256,0,0
#			.word	face_carrier,512,0,0

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_TAUNT_1
			.byte	7

#			.byte	ANI_SKIPFRAMES,14

			.byte	ANI_CODE
			.word	taunt_speech,-2,0,0

			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	stance_anim_int
			.byte	ANI_END

#;;;;;;;;;; tackler taunts (2) the player on the ground (points at dude)

			.globl	taunt_2_anim
			.byte	SCRIPT
taunt_2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS
			
			.byte	ANI_CODE
			.word	face_carrier,256,0,0

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_TAUNT_2
			.byte	0

			.byte	ANI_CODE
			.word	taunt_speech,-2,0,0
			
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	stance_anim_int
			.byte	ANI_END

#;;;;;;;;;; tackler taunts (3) the player on the ground (waves hands at dude)

			.globl	taunt_3_anim
			.byte	SCRIPT
taunt_3_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS
			
			.byte	ANI_CODE
			.word	face_carrier,256,0,0

			.byte	ANI_INTTOSEQ,14
			.word	SEQ_TAUNT_3
			.byte	14

			.byte	ANI_CODE
			.word	taunt_speech,-2,0,0
			
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	stance_anim_int
			.byte	ANI_END

#;;;;;;;;;; tackler taunts (4) the player on the ground (put hands in air)

			.globl	taunt_4_anim
			.byte	SCRIPT
taunt_4_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	face_carrier,512,0,0

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_TAUNT_4
			.byte	14

			.byte	ANI_CODE
			.word	taunt_speech,-1,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_GOTO
			.word	stance_anim_int
#			.word	breath_anim
			.byte	ANI_END

#;;;;;;;;;; tackler taunts the player on the ground (spits on him)

#			.globl	taunt_5_anim
#			.byte	SCRIPT
#taunt_5_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_ZEROVELS

#			.byte	ANI_CODE
			.word	face_carrier,512,0,0
#
#			.byte	ANI_INTTOSEQ,10
#			.word	SEQ_TAUNT_5
#			.byte	14
#
#			.byte	ANI_RUNTOEND,1
#			.byte	ANI_GOTO
#			.word	stance_anim_int
#			.byte	ANI_END

#;;;;;;;;;; tackler taunts the player on the ground (kicks him)

#			.globl	taunt_6_anim
#			.byte	SCRIPT
#taunt_6_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_NOCYL
#
#			.byte	ANI_ZEROVELS
#			
#			.byte	ANI_CODE
#			.word	face_carrier,512,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_TAUNT_6
#
#			.byte	ANI_INTSTREAM,56,36
#
#			.byte	ANI_ATTACK_ON,ATTMODE_LATE_HIT,JOINT_RKNEE,JOINT_RANKLE,56
#			.float	2.5,4.0,3.0
#
#			.byte	ANI_CODE
#			.word	taunt_speech,-1,0,0
#			
#			.byte	ANI_INTSTREAM,10,6
#
#			.byte	ANI_INTSTREAM,58,45
#
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
##			.byte	ANI_GOTO
##			.word	stance_anim_int
##			.byte	ANI_END


#;;;;;;;;;; tackler laughs at player #1

			.globl	m_laugh1_anim
			.byte	SCRIPT
m_laugh1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS
			
			.byte	ANI_CODE
			.word	face_carrier,256,0,0

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_M_LAUGH1
			.byte	0
			
			.byte	ANI_CODE
			.word	taunt_speech,-3,0,0
			
			.byte	ANI_INTSTREAM,37,80
			.byte	ANI_INTSTREAM,37,80
			.byte	ANI_INTSTREAM,37,80
			.byte	ANI_INTSTREAM,37,80
			.byte	ANI_RUNTOEND,1
			
			.byte	ANI_GOTO
			.word	stance_anim_int
			.byte	ANI_END

#;;;;;;;;;; tackler laughs at player #2

#			.globl	m_laugh2_anim
#			.byte	SCRIPT
#m_laugh2_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_ZEROVELS
#			
#			.byte	ANI_CODE
#			.word	face_carrier,512,0,0
#
#			.byte	ANI_INTTOSEQ,10
#			.word	SEQ_M_LAUGH2
#			.byte	0
#			
##			.byte	ANI_CODE
##			.word	taunt_speech,-3,0,0
#
#			.byte	ANI_RUNTOEND,1
#			
#			.byte	ANI_GOTO
#			.word	stance_anim_int
#			.byte	ANI_END
			

#;;;;;;;;;; player is juiced up! (player on fire stance)

			.globl	m_dblood_anim
			.byte	SCRIPT
m_dblood_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR
dblood_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_M_DBLOOD

			.byte	ANI_RUNTOEND,1

			.byte	ANI_GOTO
			.word	dblood_loop
			.byte	ANI_END


			.globl	m_dblood2_anim
			.byte	SCRIPT
m_dblood2_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR
db2_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_M_DBLOOD

			.byte	ANI_INTSTREAM,76,100

			.byte	ANI_GOTO
			.word	db2_loop
			.byte	ANI_END



			.globl	m_dblood3_anim
			.byte	SCRIPT
m_dblood3_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR
db3_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_M_DBLOOD

			.byte	ANI_INTSTREAM,76,50

			.byte	ANI_GOTO
			.word	db3_loop
			.byte	ANI_END


#;;;;;;;;;; player claps hands (change player anim)

			.globl	m_hopclp_anim
			.byte	SCRIPT
m_hopclp_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

	# change player sound
			.byte	ANI_CODE
			.word	attack_snd,127,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_M_HOPCLP

			.byte	ANI_SETYVEL
			.float	.54

			.byte	ANI_INTSTREAM,19,13

	# clap sound
#			.byte	ANI_CODE
#			.word	attack_snd,45,116,0
#			.word	attack_snd,116,0,0

			.byte	ANI_INTSTREAM,16,11

			.byte	ANI_GOTO
			.word	stance_anim_int
			.byte	ANI_END

#;;;;;;;;;; player claps hands (change player anim)

			.globl	m_hophop_anim
			.byte	SCRIPT
m_hophop_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS

	# change player sound
			.byte	ANI_CODE
			.word	attack_snd,127,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_M_HOPHOP

			.byte	ANI_SETYVEL
			.float	.54

			.byte	ANI_RUNTOFRAME,27,1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_BLK_RDY
			.word	0
			.byte	ANI_GOTO
			.word	block_ready_anim
			.byte	ANI_END


#;;;;;;;;;; player claps hands (change player anim)

			.globl	m_swipe_anim
			.byte	SCRIPT
m_swipe_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_M_SWIPE
			
			.byte	ANI_SETFRICTION
			.float .065
			
			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_INTSTREAM,15,6

#			.byte	ANI_ATTACK_ON,ATTMODE_SWIPE,JOINT_RELBOW,JOINT_RWRIST,5
#			.float	1.0,1.0,4.0

#			.byte	ANI_INTSTREAM,5,3

#			.byte	ANI_SETFRICTION
#			.float	.054

#			.byte	ANI_ATTACK_OFF

#			.byte	ANI_INTSTREAM,42,21

			.byte	ANI_RUNTOEND,1	
	
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			
#;;;;;;;;;; player gets up from back and becomes dizzy
	
#			.byte	SCRIPT
#get_up_b_dizzy_anim:		
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_CLRFLAG
#			.word	PF_LATEHITABLE
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_GET_UP_B
#
#			.byte	ANI_INTSTREAM,56,44
#
#			.byte	ANI_INTTOSEQ,6
#			.word	SEQ_M_DIZZY2
#			.byte	0
#			.byte	ANI_GOTO
#			.word	dizzy_anim
#			.byte	ANI_END


#;;;;;;;;;; player gets up from chest and becomes dizzy
			
#			.byte	SCRIPT
#get_up_c_dizzy_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_CLRFLAG
#			.word	PF_LATEHITABLE
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_GET_UP_C
#
#			.byte	ANI_INTSTREAM,43,34
#
#			.byte	ANI_INTTOSEQ,6
#			.word	SEQ_M_DIZZY2
#			.byte	0
#			.byte	ANI_GOTO
#			.word	dizzy_anim
#			.byte	ANI_END
			
			
#;;;;;;;;;; player lays on back-- waiting to get hit :)

			.byte	SCRIPT
m_wob_fu_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_M_WOB_FU
			.byte	0

			.byte	ANI_SETFLAG
			.word	PF_LATEHITABLE

			.byte	ANI_INTSTREAM,61,125
			.byte	ANI_INTSTREAM,61,125
			.byte	ANI_INTSTREAM,61,125
			.byte	ANI_INTSTREAM,62,125
#			.byte	ANI_RUNTOEND,1

wob_fu_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_M_WOB_FU
			
			.byte	ANI_INTSTREAM,61,125
			.byte	ANI_INTSTREAM,61,125
			.byte	ANI_INTSTREAM,61,125
			.byte	ANI_INTSTREAM,62,125
#			.byte	ANI_RUNTOEND,1
			
			.byte	ANI_GOTO
			.word	wob_fu_loop
			.byte	ANI_END
			
			
#;;;;;;;;;; player lays on chest-- waiting to get hit :)

			.byte	SCRIPT
m_wob_fd_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

#			.byte	ANI_CODE
#			.word	drop_ball,0,0,0

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_M_WOB_FD
			.byte	0

			.byte	ANI_SETFLAG
			.word	PF_LATEHITABLE

			.byte	ANI_RUNTOEND,1
#			.byte	ANI_INTSTREAM,28,100

wob_fd_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_M_WOB_FD
			
			.byte	ANI_RUNTOEND,1
#			.byte	ANI_INTSTREAM,28,100
	
			.byte	ANI_GOTO
			.word	wob_fd_loop

			
#;;;;;;;;;; player gets kicked on ground 

#			.globl	m_getkik_anim
#			.byte	SCRIPT
#m_getkik_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#		.byte	ANI_INTTOSEQ,3
#		.word	SEQ_M_GETKIK
#			.byte	8
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#
#			.byte	ANI_SET_FACE_REL_VEL
#			.float	.75
#			.byte	ANI_SETYVEL
#			.float	.90
#
#			.byte	ANI_INTSTREAM,24,AA_TOGROUND
#
#			.byte	ANI_SETFRICTION
#			.float .070
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_END
#

#;;;;;;;;;; player lying on chest and gets hit on back

			.globl	m_cnv_fd2_anim
			.byte	SCRIPT
m_cnv_fd2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_M_CNV_FD2

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0
			
			.byte	ANI_SETYVEL
			.float	.80
			.byte	1
			.byte	ANI_INTSTREAM,33,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_M_CNV_FD2

			.byte	ANI_SETYVEL
			.float	.35
			.byte	1
			.byte	ANI_INTSTREAM,3,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_M_WOB_FD
			.byte	0
			.byte	ANI_GOTO
			.word	m_wob_fd_anim
			.byte	ANI_END

#;;;;;;;;;; player lying on back...gets hit in stomach

			.globl	m_cnv_fu1_anim
			.byte	SCRIPT
m_cnv_fu1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_NEWSEQ
			.word	SEQ_M_CNV_FU1

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0
			
			.byte	ANI_SETYVEL
			.float	.80
			.byte	1
			.byte	ANI_INTSTREAM,24,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_TURNHACK
			.word	-512

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_M_WOB_FU
			.byte	0
			.byte	ANI_GOTO
			.word	m_wob_fu_anim
			.byte	ANI_END

#;;;;;;;;;;  Stiff arm reactions

			.globl	h_stiff_arm_react_anim
			.globl	h_stiff_arm_react2_anim
			.globl	h_stiff_arm_react3_anim
			.byte	SCRIPT
h_stiff_arm_react_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_ZEROVELS
			
			.byte	ANI_CODE
			.word	get_hit_snd,P_HARD_HIT3_SND,0,0
			.byte	ANI_CODE
			.word	taunt_speech,33,0,0

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_STIF_R1
			.byte	0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
h_stiff_arm_react2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	face_attacker,0,0,0
	
			.byte	ANI_CODE
			.word	get_hit_snd,P_HARD_HIT3_SND,0,0
			.byte	ANI_CODE
			.word	taunt_speech,29,0,0

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_STIF_R2
			.byte	0

			.byte	ANI_INTSTREAM,10,14

			.byte	ANI_INTSTREAM,37,29
			
			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_GOTO
			.word	get_up_b_anim
			.byte	ANI_END


			.byte	SCRIPT
h_stiff_arm_react3_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	face_attacker,0,0,0
	
			.byte	ANI_CODE
			.word	get_hit_snd,P_HARD_HIT3_SND,0,0
			.byte	ANI_CODE
			.word	taunt_speech,33,0,0

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_STIF_R3
			.byte	0

			.byte	ANI_INTSTREAM,61,55
			.byte	ANI_INTSTREAM,61,55

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	-320

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_REACTING

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_ZEROVELS

			.byte	ANI_INTSTREAM,59,35

#			.byte	ANI_TURNHACK
#			.word	-32

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  player flails (medium)

			.globl	h_flailm_anim
			.byte	SCRIPT
h_flailm_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_SET_FACE_REL_VEL
			.float	.85

			.byte	ANI_SETYVEL
			.float	1.00

			.byte	ANI_CODE
			.word	get_hit_snd,-1,P_FLAILED_SND,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_H_FLAILM
			.byte	0

			.byte	ANI_INTSTREAM,27,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float .080

			.byte	ANI_RUNTOEND,1

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;  ball carrier player flails (medium)

			.globl	h_flailm_carrier_anim
			.byte	SCRIPT
h_flailm_carrier_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_CODE
			.word	face_attacker,0,0,0
			
			.byte	ANI_SET_FACE_REL_VEL
			.float	1.0

			.byte	ANI_SETYVEL
			.float	0.65

			.byte	ANI_CODE
			.word	dust_cloud,1,0,0
			
			.byte	ANI_CODE
			.word	get_hit_snd,-1,P_FLAILED_SND,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_H_FLAILM
			.byte	0

#			.byte	ANI_INTSTREAM,27,AA_TOGROUND
			.byte	ANI_INTSTREAM,27,18

			.byte	ANI_SETFRICTION
			.float .080

#			.byte	ANI_RUNTOEND,1
			.byte	ANI_INTSTREAM,17,9

			.byte	ANI_PREP_XITION
#			.word	4,0
			.word	3,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  player flails (short)

#			.globl	h_flails_anim
#			.byte	SCRIPT
#h_flails_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_H_FLAILS
#
#			.byte	ANI_SET_FACE_REL_VEL
#			.float	.65
#			.byte	ANI_SETFRICTION
#			.float	.027
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#
#			.byte	ANI_INTSTREAM,34,20
#
#			.byte	ANI_PREP_XITION
#			.word	3,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END


#;;;;;;;;;;  player gets hit on front

			.globl	h_bsidef_anim
			.byte	SCRIPT
h_bsidef_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_NEWSEQ
			.word	SEQ_H_BSIDEF

			.byte	ANI_SET_FACE_REL_VEL
			.float	.95

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	ANI_INTSTREAM,3,2

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_SETYVEL
			.float	.80
			.byte	1
			.byte	ANI_INTSTREAM,37,AA_TOGROUND

			.byte	ANI_CODE
			.word	tackle_spch,HS_HARD,-1,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float .043

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	384

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_ZEROVELS

			.byte	ANI_INTSTREAM,59,35

			.byte	ANI_TURNHACK
			.word	64

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;  player gets hit on side (flipped)

			.globl	h_bsider_anim
			.byte	SCRIPT
h_bsider_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_NEWSEQ
			.word	SEQ_H_BSIDER

			.byte	ANI_SET_FACE_REL_VEL
			.float	.95

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	ANI_INTSTREAM,3,2

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_SETYVEL
			.float	.80
			.byte	1
			.byte	ANI_INTSTREAM,37,AA_TOGROUND

			.byte	ANI_CODE
			.word	tackle_spch,HS_HARD,-1,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float .043

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	-384

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_ZEROVELS

			.byte	ANI_INTSTREAM,59,35

			.byte	ANI_TURNHACK
			.word	-64

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  player gets hit on side

			.globl	h_bsidel_anim
			.byte	SCRIPT
h_bsidel_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_NEWSEQ
			.word	SEQ_H_BSIDEL

			.byte	ANI_SET_FACE_REL_VEL
			.float	.95

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	ANI_INTSTREAM,3,2

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_SETYVEL
			.float	.80
			.byte	1
			.byte	ANI_INTSTREAM,37,AA_TOGROUND

			.byte	ANI_CODE
			.word	tackle_spch,HS_HARD,-1,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float .043

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	384

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_ZEROVELS

			.byte	ANI_INTSTREAM,59,35

			.byte	ANI_TURNHACK
			.word	64

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  player gets hit on side (flipped)

			.globl	h_blindr_anim
			.byte	SCRIPT
h_blindr_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_NEWSEQ
			.word	SEQ_H_BLINDR

			.byte	ANI_SET_FACE_REL_VEL
			.float	.95

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	ANI_INTSTREAM,3,2

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_SETYVEL
			.float	.80
			.byte	1
			.byte	ANI_INTSTREAM,27,AA_TOGROUND

			.byte	ANI_CODE
			.word	tackle_spch,HS_WEAK,-1,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float .054

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	lower_player,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	384

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_ZEROVELS

			.byte	ANI_INTSTREAM,59,35

			.byte	ANI_TURNHACK
			.word	64

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  player gets hit on side

			.globl	h_blindl_anim
			.byte	SCRIPT
h_blindl_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_NEWSEQ
			.word	SEQ_H_BLINDL

			.byte	ANI_SET_FACE_REL_VEL
			.float	.95

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	ANI_INTSTREAM,3,2

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_SETYVEL
			.float	.80
			.byte	1
			.byte	ANI_INTSTREAM,26,AA_TOGROUND

			.byte	ANI_CODE
			.word	tackle_spch,HS_WEAK,-1,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float .054

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	lower_player,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	-384

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_ZEROVELS

			.byte	ANI_INTSTREAM,59,35

			.byte	ANI_TURNHACK
			.word	-64

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  player gets hit from front and flips over opponent

			.globl	h_frflip_anim
			.byte	SCRIPT
h_frflip_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.75

			.byte	ANI_NEWSEQ
			.word	SEQ_H_FRFLIP

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	ANI_INTSTREAM,5,2

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_SETYVEL
			.float	.85
			.byte	1
			.byte	ANI_INTSTREAM,68,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float .037

			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING
			.byte	ANI_NEWSEQ
			.word	SEQ_M_CNV_FU1

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fu_anim

			.byte	ANI_CODE
			.word	maybe_get_up_b_hurt,0,0,0
			
			.byte	ANI_GOTO
			.word	get_up_b_anim
			.byte	ANI_END


#;;;;;;;;;;  player gets hit from front and lands on neck then rolls over to stomach

			.globl	h_falnk1_anim
			.byte	SCRIPT
h_falnk1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING

			.byte	ANI_CODE
			.word	rotate_anim,512,0,0

			.byte	ANI_SET_FACE_REL_VEL
			.float	.75

#			.byte	ANI_CODE
#			.word	maybe_pop_helmet,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_H_FALNK1

#			.byte	ANI_CODE
#			.word	blood_spray,1,1,1

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	ANI_RUNTOFRAME,4,1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_SETYVEL
			.float	.85
			.byte	1
			.byte	ANI_INTSTREAM,43,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,-1,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float .037

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	tackle_spch,HS_HARD,-1,0

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

#			.byte	ANI_CODE_BNZ
#			.word	maybe_dizzy,0,0,0,get_up_c_dizzy_anim
			.byte	ANI_CODE
			.word	maybe_get_up_c_hurt,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_GU_C2
#			.word	SEQ_GET_UP_C
			.byte	0
			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END



#;;;;;;;;;;  player gets hit from front and spins like helicopter blade and lands on stomach

			.globl	h_heli1_anim
			.byte	SCRIPT
h_heli1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING

			.byte	ANI_CODE
			.word	rotate_anim,512,0,0

	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam10
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex
psv_nocam10:
	.endif

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	ANI_SHAKER,2
			.word	10

			.byte	ANI_SET_FACE_REL_VEL
			.float	1.1

			.byte	ANI_NEWSEQ
			.word	SEQ_H_HELI1

			.byte	1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH
			.byte	ANI_SETYVEL
			.float	1.6
			.byte	1
			.byte	ANI_INTSTREAM,57,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,-1,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float .035
			
			.byte	ANI_INTSTREAM,19,10

			.byte	ANI_CODE
			.word	tackle_spch,HS_HARD,-1,0

			.byte	ANI_SETFRICTION
			.float .00

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH
			.byte	ANI_SETVELS
			.float	0.0,.8,-0.50
			.byte 1
			.byte	ANI_INTSTREAM,15,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SETFRICTION
			.float .055

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

#			.byte	ANI_CODE_BNZ
#			.word	maybe_dizzy,0,0,0,get_up_c_dizzy_anim
			.byte	ANI_CODE
			.word	maybe_get_up_c_hurt,0,0,0
		
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_GU_C2
			.byte	13
#			.word	SEQ_GET_UP_C
#			.byte	10

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_RUNTOEND,1

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  player gets hit from 45 angle, spins and lands

			.globl	h_spin_anim
			.byte	SCRIPT
h_spin_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_SET_FACE_REL_VEL
			.float	.95

			.byte	ANI_NEWSEQ
			.word	SEQ_H_SPIN2

			.byte	ANI_SKIPFRAMES,4

			.byte	ANI_CODE
			.word	get_hit_snd,-1,P_GET_THRWN1_SP,0

			.byte	1,1,1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_SETYVEL
			.float	.65
			.byte	1
			.byte	ANI_INTSTREAM,41,AA_TOGROUND

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float	.032

			.byte	1

			.byte	ANI_SETYVEL
			.float	.43
			.byte	ANI_INTSTREAM,20,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,2
			.word	10

			.byte	ANI_CODE
			.word	tackle_spch,HS_HARD,-1,0

			.byte	ANI_TURNHACK
			.word	-64

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fu_anim

			.byte	ANI_CODE
			.word	maybe_get_up_b_hurt,0,0,0
			
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_GET_UP_B
			.byte	8

			.byte	ANI_ZEROVELS

			.byte	ANI_INTSTREAM,56,30

			.byte	ANI_TURNHACK
			.word	959

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  player gets hit from 45 angle (flipped), spins and lands

			.globl	h_spinf_anim
			.byte	SCRIPT
h_spinf_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_SET_FACE_REL_VEL
			.float	.95

			.byte	ANI_NEWSEQ
			.word	SEQ_H_SPIN2F

			.byte	ANI_SKIPFRAMES,4

			.byte	ANI_CODE
			.word	get_hit_snd,-1,P_GET_THRWN1_SP,0

			.byte	1,1,1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_SETYVEL
			.float	.65
			.byte	1
			.byte	ANI_INTSTREAM,41,AA_TOGROUND

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float	.032

			.byte	1

			.byte	ANI_SETYVEL
			.float	.43
			.byte	ANI_INTSTREAM,20,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,2
			.word	10

			.byte	ANI_CODE
			.word	tackle_spch,HS_HARD,-1,0

			.byte	ANI_TURNHACK
			.word	64

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fu_anim

			.byte	ANI_CODE
			.word	maybe_get_up_b_hurt,0,0,0
			
#			.byte	ANI_RUNTOEND,1

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_GET_UP_B
			.byte	8

			.byte	ANI_ZEROVELS

			.byte	ANI_INTSTREAM,56,30

			.byte	ANI_TURNHACK
			.word	-959

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  player gets hit and flips twice in air then lands on neck

#			.globl	h_front2_anim
#			.byte	SCRIPT
#h_front2_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_H_FRONT2
#
#			.byte	ANI_SKIPFRAMES,4
#
##			.byte	ANI_CODE
##			.word	blood_spray,1,1,1
#
#			.byte	ANI_SET_FACE_REL_VEL
#			.float	.90
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,-1,0
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH
#			.byte	ANI_SETYVEL
#			.float	1.2
#			.byte	1
#			.byte	ANI_INTSTREAM,68,AA_TOGROUND
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,-1,-1
#
#			.byte	ANI_CODE
#			.word	dust_cloud,0,0,0
#
#			.byte	ANI_SHAKER,3
#			.word	10
#
#			.byte	ANI_SETFRICTION
#			.float .054
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH
#			.byte	ANI_SETYVEL
#			.float	.65
#			.byte	ANI_INTSTREAM,10,AA_TOGROUND
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,0
#
#			.byte	ANI_CODE
#			.word	dust_cloud,0,0,0
#
#			.byte	ANI_SHAKER,2
#			.word	10
#
#			.byte	ANI_INTSTREAM,30,12
#
#			.byte	ANI_CODE
#			.word	tackle_spch,HS_HARD,-1,0
#
#			.byte	ANI_CODE
#			.word	player_down,0,0,0
#
#			.byte	ANI_CODE_BNZ
#			.word	plyr_have_ball,0,0,0,m_wob_fu_anim
#
##			.byte	ANI_CODE_BNZ
##			.word	maybe_dizzy,0,0,0,get_up_b_dizzy_anim
#			.byte	ANI_CODE
#			.word	maybe_get_up_b_hurt,0,0,0
#			
#			.byte	ANI_GOTO
#			.word	get_up_b_anim
#			.byte	ANI_END


#;;;;;;;;;;  player gets hit in front, flies back lands on neck flips to stomach

			.globl	h_front4_anim
			.byte	SCRIPT
h_front4_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_CODE
			.word	rotate_anim,512,0,0

	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam6
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex
psv_nocam6:
	.endif

			.byte	ANI_SET_FACE_REL_VEL
			.float	.95

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_NEWSEQ
			.word	SEQ_H_FRONT4

#			.byte	ANI_CODE
#			.word	blood_spray,1,1,1

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_SETYVEL
			.float	.75
			.byte	1
			.byte	ANI_INTSTREAM,27,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float	.031

			.byte	ANI_INTSTREAM,18,9

			.byte	ANI_SHAKER,1
			.word	5

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	tackle_spch,HS_HARD,-1,0

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

#			.byte	ANI_CODE_BNZ
#			.word	maybe_dizzy,0,0,0,get_up_c_dizzy_anim
			.byte	ANI_CODE
			.word	maybe_get_up_c_hurt,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_M_GU_C2
#			.word	SEQ_GET_UP_C
			.byte	0

			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END



#;;;;;;;;;;  player reacts to uppercut

#			.globl	h_uprcut_anim
#			.byte	SCRIPT
#h_uprcut_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_H_UPRCUT
#
#		.byte	ANI_ZEROVELS
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,-1,0
#
#			.byte	ANI_SKIPFRAMES,5
#
#			.byte	ANI_SET_FACE_REL_VEL
#			.float	.65
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH
#			.byte	ANI_SETYVEL
#			.float	1.5
#			.byte	1
#			.byte	ANI_INTSTREAM,34,AA_TOGROUND
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,-1
#
#			.byte	ANI_CODE
#			.word	dust_cloud,0,0,0
#
#			.byte	ANI_SHAKER,3
#			.word	20
#
#			.byte	ANI_SETFRICTION
#			.float .065
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CODE
#			.word	tackle_spch,HS_HARD,-1,0
#
#			.byte	ANI_CODE
#			.word	player_down,0,0,0
#
#			.byte	ANI_CODE_BNZ
#			.word	plyr_have_ball,0,0,0,m_wob_fu_anim
#
##			.byte	ANI_CODE_BNZ
##			.word	maybe_dizzy,0,0,0,get_up_b_dizzy_anim
#			.byte	ANI_CODE
#			.word	maybe_get_up_b_hurt,0,0,0
#			
#			.byte	ANI_INTTOSEQ,4
#			.word	SEQ_GET_UP_B
#			.byte	0
#			.byte	ANI_GOTO
#			.word	get_up_b_anim
#			.byte	ANI_END


#;;;;;;;;;;  player gets hit in front and rotates landing on chest

			.globl	h_front5_anim
			.byte	SCRIPT
h_front5_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_NEWSEQ
			.word	SEQ_H_FRONT5

			.byte	ANI_SET_FACE_REL_VEL
			.float	.95

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

#			.byte	ANI_CODE
#			.word	blood_spray,1,1,1

			.byte	ANI_SETYVEL
			.float	1.0
			.byte	1
			.byte	ANI_INTSTREAM,22,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETYVEL
			.float	.45

			.byte	ANI_INTSTREAM,2,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float	.070

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_SHAKER,2
			.word	10

			.byte	ANI_CODE
			.word	tackle_spch,HS_WEAK,-1,0

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

#			.byte	ANI_CODE_BNZ
#			.word	maybe_dizzy,0,0,0,get_up_c_dizzy_anim
			.byte	ANI_CODE
			.word	maybe_get_up_c_hurt,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_M_GU_C2
#			.word	SEQ_GET_UP_C
			.byte	0
			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END


#;;;;;;;;;;  player gets hit in back and flies landing on stomach

			.globl	h_back_anim
			.byte	SCRIPT
h_back_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING|MODE_NOCATCH

	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocamb
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex
psv_nocamb:
	.endif

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	ANI_NEWSEQ
			.word	SEQ_HBACK

#			.byte	ANI_INTTOSEQ,6
#			.word	SEQ_HBACK
#			.byte	0

			.byte	ANI_SET_FACE_REL_VEL
			.float	.95

			.byte	ANI_RUNTOFRAME,22,1

			.byte	ANI_SETFRICTION
			.float	.045

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_SHAKER,2
			.word	10

			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	256

			.byte	ANI_GOTO
			.word	stance_anim_int
			.byte	ANI_END


#;;;;;;;;;;  player gets hit in back and flies landing on stomach

			.globl	h_back2_anim
			.byte	SCRIPT
h_back2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_SET_FACE_REL_VEL
			.float	.75



	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam7
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex
psv_nocam7:
	.endif



			.byte	ANI_INTTOSEQ,3
			.word	SEQ_H_BACK2
			.byte	0

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	ANI_SETFRICTION
			.float	.055

			.byte	ANI_RUNTOFRAME,39,1

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_SHAKER,2
			.word	10

			.byte	ANI_RUNTOFRAME,64,1

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

#			.byte	ANI_CODE_BNZ
#			.word	maybe_dizzy,0,0,0,get_up_c_dizzy_anim
			.byte	ANI_CODE
			.word	maybe_get_up_c_hurt,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_M_GU_C2
#			.word	SEQ_GET_UP_C
			.byte	0
			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END


#;;;;;;;;;;  player gets hit in back and flies landing on stomach

			.globl	h_back4_anim
			.byte	SCRIPT
h_back4_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_CODE
			.word	rotate_anim,0,0,0

			.byte	ANI_SET_FACE_REL_VEL
			.float	1.1

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_H_BACK4
			.byte	0

			.byte	ANI_INTSTREAM,34,19

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float	.050

			.byte	ANI_INTSTREAM,17,11

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

#			.byte	ANI_CODE_BNZ
#			.word	maybe_dizzy,0,0,0,get_up_c_dizzy_anim
			.byte	ANI_CODE
			.word	maybe_get_up_c_hurt,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_M_GU_C2
#			.word	SEQ_GET_UP_C
			.byte	0

			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END


#;;;;;;;;;;  player gets hit in back (45 degress) and flies landing on stomach

#			.globl	h_bck45_anim
#			.byte	SCRIPT
#h_bck45_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_H_BCK45
#
#			.byte	ANI_SET_FACE_REL_VEL
#			.float	1.00
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,-1,0
#
#			.byte	1
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING|MODE_NOCATCH
#
#			.byte	ANI_SETYVEL
#			.float	1.30
#			.byte 	1
#			.byte	ANI_INTSTREAM,39,AA_TOGROUND
#
#			.byte	ANI_CODE
#			.word	player_down,0,0,0
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,-1
#
#			.byte	ANI_CODE
#			.word	dust_cloud,0,0,0
#
#			.byte	ANI_SHAKER,3
#			.word	10
#
#			.byte	ANI_SETFRICTION
#			.float	.030
#
#			.byte	ANI_RUNTOFRAME,59,1
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,0
#
#			.byte	ANI_SHAKER,2
#			.word	10
#
#			.byte	ANI_CODE_BNZ
#			.word	plyr_have_ball,0,0,0,m_wob_fd_anim
#
##			.byte	ANI_CODE_BNZ
##			.word	maybe_dizzy,0,0,0,get_up_c_dizzy_anim
#			.byte	ANI_CODE
#			.word	maybe_get_up_c_hurt,0,0,0
#			
#			.byte	ANI_RUNTOFRAME,63,1
#
#			.byte	ANI_TURNHACK
#			.word	896
#
#			.byte	ANI_GOTO
#			.word	get_up_c_anim
#			.byte	ANI_END


#;;;;;;;;;;  player gets hit in back (45 degress) and flies landing on stomach

#			.globl	h_bck45f_anim
#			.byte	SCRIPT
#h_bck45f_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_H_BCK45F
#
#			.byte	ANI_SET_FACE_REL_VEL
#			.float	1.00
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,-1,0
#
#			.byte	1
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING|MODE_NOCATCH
#
#			.byte	ANI_SETYVEL
#			.float	1.30
#			.byte 	1
#			.byte	ANI_INTSTREAM,39,AA_TOGROUND
#
#			.byte	ANI_CODE
#			.word	player_down,0,0,0
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,-1
#
#			.byte	ANI_CODE
#			.word	dust_cloud,0,0,0
#
#			.byte	ANI_SHAKER,3
#			.word	10
#
#			.byte	ANI_SETFRICTION
#			.float	.030
#
#			.byte	ANI_RUNTOFRAME,59,1
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,0
#
#			.byte	ANI_SHAKER,2
#			.word	10
#
#			.byte	ANI_RUNTOFRAME,63,1
#
#			.byte	ANI_TURNHACK
#			.word	896
#
#			.byte	ANI_CODE_BNZ
#			.word	plyr_have_ball,0,0,0,m_wob_fd_anim
#
##			.byte	ANI_CODE_BNZ
##			.word	maybe_dizzy,0,0,0,get_up_c_dizzy_anim
#			.byte	ANI_CODE
#			.word	maybe_get_up_c_hurt,0,0,0
#			
#			.byte	ANI_GOTO
#			.word	get_up_c_anim
#			.byte	ANI_END

#;;;;;;;;;;  player gets hit in back and flies landing on stomach

			.globl	h_back5_anim
			.byte	SCRIPT
h_back5_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	ANI_NEWSEQ
			.word	SEQ_CRZYFALL

			.byte	ANI_SETYVEL
			.float	.95
			.byte	ANI_SET_FACE_REL_VEL
			.float	.95

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING|MODE_NOCATCH
			.byte	1
			.byte	ANI_INTSTREAM,32,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float	.045

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_SHAKER,2
			.word	10

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim


			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR


			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END


#;;;;;;;;;;  player gets hit in back and flies landing on stomach

			.globl	h_back6_anim
			.byte	SCRIPT
h_back6_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	ANI_SETYVEL
			.float	1.1

			.byte	ANI_NEWSEQ
#			.byte	ANI_INTTOSEQ,4
			.word	SEQ_CRZYFLIP
#			.byte	0

			.byte	ANI_SET_FACE_REL_VEL
			.float	1.20

			.byte	1
			.byte	ANI_INTSTREAM,34,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,0,-1,0

			.byte	ANI_SETFRICTION
			.float	.045

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_RUNTOFRAME,43,1

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,2
			.word	10

			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_CLRFLAG
			.word	PF_LATEHITABLE

			.byte	ANI_NEWSEQ
			.word	SEQ_M_GU_C2

			.byte	ANI_ZEROVELS

			.byte	ANI_INTSTREAM,59,35

			.byte	ANI_TURNHACK
			.word	1023

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  player gets pushed from chest

			.globl	h_front1_anim
			.byte	SCRIPT
h_front1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_CODE
			.word	rotate_anim,512,0,0

			.byte	ANI_SET_FACE_REL_VEL
			.float	1.2

			.byte	ANI_NEWSEQ
			.word	SEQ_H_FRONT1

			.byte	ANI_SKIPFRAMES,4

#			.byte	ANI_CODE
#			.word	blood_spray,1,1,1

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_SETYVEL
			.float	0.75
			.byte 	1
			.byte	ANI_INTSTREAM,27,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10
			
			.byte	ANI_SETFRICTION
			.float	.055

			.byte	ANI_RUNTOFRAME,36,1
#			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	tackle_spch,HS_WEAK,-1,0

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fu_anim

#			.byte	ANI_CODE_BNZ
#			.word	maybe_dizzy,0,0,0,get_up_b_dizzy_anim
			.byte	ANI_CODE
			.word	maybe_get_up_b_hurt,0,0,0
			
			.byte	ANI_GOTO
			.word	get_up_b_anim
			.byte	ANI_END


#;;;;;;;;;;  player gets pushed from side

			.globl	h_side1_anim
			.byte	SCRIPT
h_side1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_NEWSEQ
			.word	SEQ_H_SIDE1

			.byte	ANI_CODE
			.word	rotate_anim,512,0,0

			.byte	ANI_SET_FACE_REL_VEL
			.float	1.4

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_SETYVEL
			.float	1.2
			.byte 	1
			.byte	ANI_INTSTREAM,31,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_SETFRICTION
			.float .080

			.byte	1

			.byte	ANI_SETYVEL
			.float	.50

			.byte	ANI_INTSTREAM,2,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_SHAKER,2
			.word	10

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fu_anim

#			.byte	ANI_CODE_BNZ
#			.word	maybe_dizzy,0,0,0,get_up_b_dizzy_anim
			.byte	ANI_CODE
			.word	maybe_get_up_b_hurt,0,0,0
			
	  # do get up here
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_GET_UP_B
			.byte	18

			.byte	ANI_INTSTREAM,46,34

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  player gets pushed from side flipped

			.globl	h_side1f_anim
			.byte	SCRIPT
h_side1f_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_NEWSEQ
			.word	SEQ_H_SIDE1F

			.byte	ANI_CODE
			.word	rotate_anim,512,0,0

			.byte	ANI_SET_FACE_REL_VEL
			.float	1.4

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_SETYVEL
			.float	1.2
			.byte 	1
			.byte	ANI_INTSTREAM,31,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float	.080

			.byte	1
			.byte	ANI_SETYVEL
			.float	.50

			.byte	ANI_INTSTREAM,2,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,2
			.word	10

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fu_anim

#			.byte	ANI_CODE_BNZ
#			.word	maybe_dizzy,0,0,0,get_up_b_dizzy_anim
			.byte	ANI_CODE
			.word	maybe_get_up_b_hurt,0,0,0
			
	# do get-up here
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_GET_UP_B
			.byte	18

			.byte	ANI_INTSTREAM,46,31

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END



#;;;;;;;;;;  player gets pushed from behind

			.globl	h_back1_anim
			.byte	SCRIPT
h_back1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_CODE
			.word	rotate_anim,0,0,0

			.byte	ANI_SET_FACE_REL_VEL
			.float	.85

			.byte	ANI_NEWSEQ
			.word	SEQ_H_BACK1


			.byte	ANI_SKIPFRAMES,4

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_SETYVEL
			.float	0.80
			.byte	1
			.byte	ANI_INTSTREAM,27,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	20

			.byte	ANI_SETFRICTION
			.float	.052

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

			.byte	ANI_CODE
			.word	maybe_get_up_c_hurt,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_GU_C2
			.byte	0
						
			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END


#;;;;;;;;;;  player gets pushed from behind

#			.globl	h_anktkl_anim
#			.byte	SCRIPT
#h_anktkl_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_H_ANKTKL
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,-1,0
#
#			.byte	ANI_SETFRICTION
#			.float .023
#
#			.byte	ANI_INTSTREAM,51,40
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,-1
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CODE
#			.word	player_down,0,0,0
#
#			.byte	ANI_CODE_BNZ
#			.word	plyr_have_ball,0,0,0,m_wob_fd_anim
#
#			.byte	ANI_CODE
#			.word	maybe_get_up_c_hurt,0,0,0
#			
#			.byte	ANI_GOTO
#			.word	get_up_c_anim
#			.byte	ANI_END
#

#;;;;;;;;;;;; STANCES

			.globl	into_stance_anim
			.byte	SCRIPT
into_stance_anim:
			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_ZEROVELS
			.byte	ANI_NEWSEQ
			.word	SEQ_RUSH_I
			.byte	ANI_RUNTOEND,1
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR
			.byte	ANI_GOTO
			.word	stance_anim

;;;;;;;;;;
			.globl	stance_anim_int
			.globl	stance_anim
			.byte	SCRIPT
stance_anim_int:

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,16
			.word	SEQ_RUSH_C
			.byte	0

			.byte	ANI_ZEROVELS

			.byte	ANI_RUNTOEND,1

#			.byte	SCRIPT
stance_anim:
			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_ZEROVELS
			.byte	ANI_NEWSEQ
			.word	SEQ_RUSH_C
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	stance_anim

#;;;;;;;;;;	player just stands there being dizzy

#			.globl	dizzy_anim
#			.byte	SCRIPT
#dizzy_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORXZ
#
#			.byte	ANI_SETFLAG
#			.word	PF_DIZZY
#			.byte	ANI_CLRFLAG
#			.word	PF_LATEHITABLE
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_M_DIZZY2
#
#			.byte	ANI_RUNTOEND,1
#dizzy_lp:
#			.byte	ANI_NEWSEQ
#			.word	SEQ_M_DIZZY2
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_GOTO
#			.word	dizzy_lp
#			.byte	ANI_END


#;;;;;;;;;;	player just stands there and takes a breather

			.globl	breath_anim
			.byte	SCRIPT
breath_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_ZEROVELS
			.byte	ANI_INTTOSEQ,16
			.word	SEQ_BREATH
			.byte	0
			.byte	ANI_RUNTOEND,1
breath_lp:
			.byte	ANI_NEWSEQ
			.word	SEQ_BREATH
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	breath_lp
			.BYTE	ANI_END


#;;;;;;;;;;	player just stands there and takes a breather

			.globl	breath2_anim
			.byte	SCRIPT
breath2_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_ZEROVELS
			.byte	ANI_INTTOSEQ,10
			.word	SEQ_M_SLOBP2
			.byte	0
			.byte	ANI_RUNTOEND,1
breath2_lp:
			.byte	ANI_NEWSEQ
			.word	SEQ_M_SLOBP2
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	breath2_lp
			.BYTE	ANI_END


#;;;;;;;;;;	player just stands there and takes a breather

			.globl	breath3_anim
			.byte	SCRIPT
breath3_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_ZEROVELS
			.byte	ANI_INTTOSEQ,16
			.word	SEQ_M_SLOBP3
			.byte	0
			.byte	ANI_RUNTOEND,1
breath3_lp:
			.byte	ANI_NEWSEQ
			.word	SEQ_M_SLOBP3
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	breath3_lp
			.BYTE	ANI_END


#;;;;;;;;;;	player just stands there and takes a breather

#			.globl	breath4_anim
#			.byte	SCRIPT
#breath4_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORXZ
#			.byte	ANI_ZEROVELS
#			.byte	ANI_INTTOSEQ,10
#			.word	SEQ_AP_2C
#			.byte	0
#			.byte	ANI_RUNTOEND,1
#breath4_lp:
#			.byte	ANI_NEWSEQ
#			.word	SEQ_AP_2C
#			.byte	ANI_RUNTOEND,1
#			.byte	ANI_GOTO
#			.word	breath4_lp
#			.BYTE	ANI_END


#;;;;;;;;;;	player just stands there and takes a breather

#			.globl	breath5_anim
#			.byte	SCRIPT
#breath5_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORXZ
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_INTTOSEQ,4
#			.word	SEQ_M_SLOBP3
#			.byte	0
#
#			.byte	ANI_INTSTREAM,83,72
#breath5_lp:
#			.byte	ANI_NEWSEQ
#			.word	SEQ_M_SLOBP3
#
#			.byte	ANI_INTSTREAM,83,72
#
#			.byte	ANI_GOTO
#			.word	breath5_lp
#			.BYTE	ANI_END


#;;;;;;;;;; player stumbles while running

			.globl	r_stmbl_anim
			.byte	SCRIPT
r_stmbl_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ|MODE_UNINT

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_R_STMBL
			.byte	0

			.byte	ANI_CODE
			.word	taunt_speech,10,0,0
			
			.byte	ANI_RUNTOEND,1

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

			
#;;;;;;;;;;  lineman spazes out...toward qb

			.globl	r_crzbl2_anim
			.byte	SCRIPT
r_crzbl2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_TWOPART

			.byte	ANI_NEWSEQ
			.word	SEQ_R_CRZBL2

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_RUNTOEND,1
			
		# turn off two_part
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0

			.byte	ANI_CODE
			.word	general_run,0,0,0

			.byte	ANI_END
			

#;;;;;;;;;;  lineman spazes out...toward qb

#			.globl	r_crzbl4_anim
#			.byte	SCRIPT
#r_crzbl4_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_TWOPART
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_R_CRZBL4
#
#			.byte	ANI_CODE
#			.word	attack_snd,-1,0,0
#
#			.byte	ANI_RUNTOEND,1
#			
#		# turn off two_part
#			.byte	ANI_SETMODE
#			.word	0
#
#			.byte	ANI_CODE
#			.word	set_unint_lowerhalf,0,0,0
#
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#
#			.byte	ANI_END
			
			
#;;;;;;;;;;  player grabbing for other player

#			.globl	r_crzcha_anim
#			.byte	SCRIPT
#r_crzcha_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_TWOPART
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_R_CRZCHA
#
#			.byte	ANI_RUNTOEND,1
#			
#		# turn off two_part
#			.byte	ANI_SETMODE
#			.word	0
#
#			.byte	ANI_CODE
#			.word	set_unint_lowerhalf,0,0,0
#
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#
#			.byte	ANI_END
			
			
#;;;;;;;;;;
			.globl	run_anim
			.globl	run_anim_int
			.byte	SCRIPT
run_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_JIFNSEQ
			.word	SEQ_R_TRB,run_cyc
			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_INTTOSEQ,2
			.word	SEQ_RUN
			.byte	AA_PROPFRAME
			.byte	ANI_RUNTOEND,1

run_cyc:
			.byte	ANI_NEWSEQ
			.word	SEQ_RUN
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	run_cyc

			.byte	SCRIPT
run_anim_int:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_RUN
			.byte	AA_VAR_FRAME
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	run_cyc

#;;;;;;;;;;
			.globl	run_t_anim
			.globl	run_t_anim_int
			.byte	SCRIPT
run_t_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_JIFNSEQ
			.word	SEQ_RUN,run_t_cyc
			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_INTTOSEQ,2
			.word	SEQ_R_TRB
			.byte	AA_PROPFRAME
			.byte	ANI_STREAMTOEND
			.float	0.733

run_t_cyc:
			.byte	ANI_NEWSEQ
			.word	SEQ_R_TRB
			.byte	ANI_INTSTREAM,30,22
			.byte	ANI_GOTO
			.word	run_t_cyc

			.byte	SCRIPT
run_t_anim_int:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_R_TRB
			.byte	AA_VAR_FRAME
			.byte	ANI_STREAMTOEND
			.float	0.733
			.byte	ANI_GOTO
			.word	run_t_cyc

#;;;;;;;;;;
			.globl	run_wb_anim
			.globl	run_wb_anim_int
			.byte	SCRIPT
run_wb_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_JIFNSEQ
			.word	SEQ_R_TRB_WB,run_wb_cyc
			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_RUN_WB
			.byte	AA_PROPFRAME
			.byte	ANI_RUNTOEND,1
run_wb_cyc:
			.byte	ANI_NEWSEQ
			.word	SEQ_RUN_WB
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	run_wb_cyc

			.byte	SCRIPT
run_wb_anim_int:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_RUN_WB
			.byte	AA_VAR_FRAME
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	run_wb_cyc

;;;;;;;;;;
			.globl	run_t_wb_anim
			.globl	run_t_wb_anim_int
			.byte	SCRIPT
run_t_wb_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_JIFNSEQ
			.word	SEQ_RUN_WB,run_t_wb_cyc
			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_R_TRB_WB
			.byte	AA_PROPFRAME
			.byte	ANI_RUNTOEND,1

run_t_wb_cyc:
			.byte	ANI_NEWSEQ
			.word	SEQ_R_TRB_WB
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	run_t_wb_cyc

			.byte	SCRIPT
run_t_wb_anim_int:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_R_TRB_WB
			.byte	AA_VAR_FRAME
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	run_t_wb_cyc


#;;;;;;;;;;	  huff-and-puff run


#			.globl	run_nb1_anim
#			.globl	run_nb2_anim
#			.globl	run_nb3_anim
#			.byte	SCRIPT
#run_nb1_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORXZ
#nb1_cyc:
#			.byte	ANI_NEWSEQ
#			.word	SEQ_RUN_NB1
#			.byte	ANI_RUNTOEND,1
#			.byte	ANI_GOTO
#			.word	nb1_cyc
#			.byte	ANI_END
#
#			.byte	SCRIPT
#run_nb2_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORXZ
#nb2_cyc:
#			.byte	ANI_NEWSEQ
#			.word	SEQ_RUN_NB2
#			.byte	ANI_RUNTOEND,1
#			.byte	ANI_GOTO
#			.word	nb1_cyc
#			.byte	ANI_END
#
#
#			.byte	SCRIPT
#run_nb3_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORXZ
#nb3_cyc:
#			.byte	ANI_NEWSEQ
#			.word	SEQ_RUN_NB3
#			.byte	ANI_RUNTOEND,1
#			.byte	ANI_GOTO
#			.word	nb1_cyc
#			.byte	ANI_END

#;;;;;;;;;;	  huff-and-puff run


			.globl	r_huff_anim
			.byte	SCRIPT
r_huff_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_JIFNSEQ
			.word	SEQ_R_TRB,r_huff_cyc
			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_HUFFRUN2
			.byte	AA_PROPFRAME
			.byte	ANI_RUNTOEND,1
r_huff_cyc:
			.byte	ANI_NEWSEQ
			.word	SEQ_HUFFRUN2
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	r_huff_cyc


#;;;;;;;;;;	  huff-and-puff run (turbo)

			.globl	r_t_huff_anim
			.byte	SCRIPT
r_t_huff_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
#			.byte	ANI_JIFNSEQ
#			.word	SEQ_RUN,r_t_huff_cyc
#			.byte	ANI_SETMODE
#			.word	0
#			.byte	AA_PROPFRAME
#			.byte	ANI_RUNTOEND,1
r_t_huff_cyc:
			.byte	ANI_NEWSEQ
			.word	SEQ_HUFFRUN
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	r_t_huff_cyc

#;;;;;;;;;;
			.globl	r_histep_anim
			.byte	SCRIPT
r_histep_anim:
			.byte	ANI_CODE
			.word	taunt_speech,-4,0,0

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_JIFNSEQ
			.word	SEQ_R_TRB_WB,SEQ_RUN_WB,r_histep_cyc
			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_R_HISTEP
			.byte	AA_PROPFRAME
			.byte	ANI_RUNTOEND,1
r_histep_cyc:
#			.byte	ANI_CODE
#			.word	taunt_speech,30,20,0
r_histep_cyc2:
			.byte	ANI_NEWSEQ
			.word	SEQ_R_HISTEP
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	r_histep_cyc2

#;;;;;;;;;;

			.globl	r_skip_anim
			.byte	SCRIPT
r_skip_anim:
			.byte	ANI_CODE
			.word	taunt_speech,-4,0,0

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_JIFNSEQ
			.word	SEQ_R_TRB_WB,SEQ_RUN_WB,r_skip_cyc
			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_R_SKIP
			.byte	AA_PROPFRAME
#			.byte	ANI_INTSTREAM,68,60
			.byte	ANI_RUNTOEND,1
r_skip_cyc:
			.byte	ANI_NEWSEQ
			.word	SEQ_R_SKIP
#			.byte	ANI_INTSTREAM,68,60
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	r_skip_cyc


#;;;;;;;;;;
			.globl	r_hihand_anim
			.byte	SCRIPT
r_hihand_anim:
			.byte	ANI_CODE
			.word	taunt_speech,-4,0,0

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_JIFNSEQ
			.word	SEQ_R_TRB_WB,SEQ_RUN_WB,r_hihand_cyc
			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_R_HIHAND
			.byte	AA_PROPFRAME
#			.byte	ANI_INTSTREAM,59,48
			.byte	ANI_RUNTOEND,1
r_hihand_cyc:
#			.byte	ANI_CODE
#			.word	taunt_speech,30,20,0
r_hihand_cyc2:
			.byte	ANI_NEWSEQ
			.word	SEQ_R_HIHAND
			.byte	ANI_INTSTREAM,59,70
#			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	r_hihand_cyc2


#;;;;;;;;;;	 running backward taunt

#			.globl	r_bktau1_anim
#			.byte	SCRIPT
#r_bktau1_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHOR
#			.byte	ANI_NEWSEQ
#			.word	SEQ_R_INBTA1
#			.byte	ANI_INTSTREAM,35,20
#			.byte	ANI_SETMODE
#			.word	0
#r_bktau1_cyc:
#			.byte	ANI_NEWSEQ
#			.word	SEQ_R_BKTAU1
#			.byte	ANI_INTSTREAM,29,22
#			.byte	ANI_GOTO
#			.word	r_bktau1_cyc
#			.byte	ANI_END


#;;;;;;;;;;	 running sideways taunt

			.globl	r_bktau2_anim
			.byte	SCRIPT
r_bktau2_anim:
			.byte	ANI_CODE
			.word	taunt_speech,-4,0,0

			.byte	ANI_SETMODE
			.word	MODE_ANCHOR
			.byte	ANI_JIFNSEQ
			.word	SEQ_R_TRB_WB,SEQ_RUN_WB,r_bktau2_cyc
			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_NEWSEQ
			.word	SEQ_R_INBTA2
			.byte	ANI_SKIPFRAMES,4
			.byte	ANI_INTSTREAM,56,31
#			.byte	ANI_SETMODE
#			.word	0
r_bktau2_cyc:
			.byte	ANI_NEWSEQ
			.word	SEQ_R_BKTAU2
			.byte	ANI_INTSTREAM,30,22
			.byte	ANI_GOTO
			.word	r_bktau2_cyc
			.byte	ANI_END
			
			
#;;;;;;;;;;
			.globl	r_ezsprint_anim
			.byte	SCRIPT
r_ezsprint_anim:
			.byte	ANI_CODE
			.word	taunt_speech,-5,0,0

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_JIFNSEQ
			.word	SEQ_R_TRB_WB,SEQ_RUN_WB,r_ezsprint_cyc
			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_R_EZSPRI
			.byte	AA_PROPFRAME
			.byte	ANI_RUNTOEND,1
r_ezsprint_cyc:
#			.byte	ANI_CODE
#			.word	taunt_speech,10,20,0
			
r_ezsprint_cyc2:
			.byte	ANI_NEWSEQ
			.word	SEQ_R_EZSPRI
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	r_ezsprint_cyc2


#;;;;;;;;;;  backpedals
			.globl	run_back_anim
			.globl	run_back_anim_int
			.globl	run_t_back_anim
			.globl	run_t_back_anim_int
			
			.byte	SCRIPT
run_back_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
run_back_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_RUN_B
			.byte	ANI_STREAMTOEND
			.float	.70
			.byte	ANI_GOTO
			.word	run_back_loop
			
			.byte	SCRIPT
run_back_anim_int:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_RUN_B
			.byte	0
			.byte	ANI_STREAMTOEND
			.float	.70
			.byte	ANI_GOTO
			.word	run_back_loop

			.byte	SCRIPT
run_t_back_anim:
run_t_back_anim_int:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
run_t_back_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_R_TRB_B
			.byte	ANI_STREAMTOEND
			.float	.60
			.byte	ANI_GOTO
			.word	run_back_loop


#;;;;;;;;;;  player comes to a running stop #1

#			.globl	run_slowdwn1_anim
#			.byte	SCRIPT
#run_slowdwn1_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_R_SLO_P1
#
#			.byte	ANI_SETFRICTION
#			.float .030
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_GOTO
#			.word	breath_anim
#			.byte	ANI_END


#;;;;;;;;;;  player comes to a running stop #1

#			.globl	run_slowdwn2_anim
#			.byte	SCRIPT
#run_slowdwn2_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_R_SLO_P2
#
#			.byte	ANI_SETFRICTION
#			.float .050
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_GOTO
#			.word	breath2_anim
#			.byte	ANI_END


#;;;;;;;;;;  player comes to a running stop #3

#			.globl	run_slowdwn3_anim
#			.byte	SCRIPT
#run_slowdwn3_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_R_SLO_P3
#
#			.byte	ANI_SETFRICTION
#			.float .050
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_GOTO
#			.word	breath3_anim
#			.byte	ANI_END



#;;;;;;;;;;  player comes to a running stop #4

#			.globl	run_slowdwn4_anim
#			.byte	SCRIPT
#run_slowdwn4_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_INTTOSEQ,4
#			.word	SEQ_R_SLO_P2
#			.byte	0
#
#			.byte	ANI_SETFRICTION
#			.float .065
#
#			.byte	ANI_INTSTREAM,125,90
##			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_GOTO
#			.word	breath4_anim
#			.byte	ANI_END


#;;;;;;;;;;  player comes to a running stop #5

#			.globl	run_slowdwn5_anim
#			.byte	SCRIPT
#run_slowdwn5_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_INTTOSEQ,4
#			.word	SEQ_R_SLO_P3
#			.byte	0
#
#			.byte	ANI_SETFRICTION
#			.float .065
#
#			.byte	ANI_INTSTREAM,100,72
##			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_GOTO
#			.word	breath5_anim
#			.byte	ANI_END


#;;;;;;;;;;  player backpedals

#			.globl	back_pedal_anim
#			.byte	SCRIPT
#back_pedal_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHOR
#back_pedal_lp:
#			.byte	ANI_NEWSEQ
#			.word	SEQ_BK_PEDAL
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_GOTO
#			.word	back_pedal_lp
#			.byte	ANI_END


#;;;;;;;;;;  RUN into something and flail

			.globl	r_flaild_anim
			.byte	SCRIPT
r_flaild_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_SET_FACE_REL_VEL
			.float	.90

			.byte	ANI_SETYVEL
			.float	1.00

			.byte	ANI_CODE
			.word	get_hit_snd,-1,P_FLAILED_SND,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_R_FLAILD
			.byte	0

#			.byte	ANI_INTSTREAM,10,5

			.byte	ANI_INTSTREAM,29,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float .075

			.byte	ANI_INTSTREAM,42,30

			.byte	ANI_PREP_XITION
			.word	5,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

			.globl	r_flaild_carrier_anim
			.byte	SCRIPT
r_flaild_carrier_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_SET_FACE_REL_VEL
			.float	.90

			.byte	ANI_SETYVEL
			.float	1.00

			.byte	ANI_CODE
			.word	get_hit_snd,-1,P_FLAILED_SND,0

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_R_FLAILD
			.byte	0

#			.byte	ANI_INTSTREAM,10,5

#			.byte	ANI_INTSTREAM,29,AA_TOGROUND
			.byte	ANI_INTSTREAM,29,20

			.byte	ANI_SETFRICTION
			.float .080

			.byte	ANI_INTSTREAM,42,25

			.byte	ANI_PREP_XITION
			.word	3,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END
			
			
#;;;;;;;;;;  RUN into something and flail and continue running

			.globl	r_flailnd_anim
			.byte	SCRIPT
r_flailnd_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_NEWSEQ
			.word	SEQ_R_FLAIND

			.byte	ANI_CODE
			.word	taunt_speech,10,0,0

			.byte	ANI_INTSTREAM,65,35
#			.byte	ANI_RUNTOEND,1

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


;;;;;;;;;;
			.globl	stance_wb_anim
			.globl	stance_wb_anim_int
			.byte	SCRIPT
stance_wb_anim_int:
			.byte	ANI_ZEROVELS
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_INTTOSEQ,12
			.word	SEQ_RDY_WB
			.byte	0
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	stance_wb_loop

			.byte	SCRIPT
stance_wb_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_ZEROVELS
stance_wb_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_RDY_WB
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	stance_wb_loop


#;;;;;;;;;; player tries to juke out defender

			.globl	juke_c_anim
			.byte	SCRIPT
juke_c_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_JUKE_C

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.28

##			.byte	ANI_RUNTOEND,1
			.byte	ANI_STREAMTOEND
			.float	0.5

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; TURBO hurdle

			.globl	turbo_hurdle_anim
			.byte	SCRIPT
turbo_hurdle_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_HURDLE

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0

# Go airborne, retain my current x/z vel

			.byte	ANI_CODE
#			.word	attack_snd,P_HURDLE_SND,0,0
			.word	attack_snd,-1,0,0

			.byte	ANI_SETYVEL
			.float	1.20
			
			.byte	ANI_INTSTREAM,5,3

			.byte	ANI_ATTACK_ON,ATTMODE_HURDLE,JOINT_RANKLE,JOINT_LANKLE,40
			.float	1.0,1.0,4.0
			
			.byte	ANI_INTSTREAM,10,17

			.byte	ANI_INTSTREAM,10,10
# landed
			.byte	ANI_ATTACK_OFF

			.byte	ANI_INTSTREAM,12,9

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; NON-TURBO hurdle 

			.globl	hurdle_anim
			.byte	SCRIPT
hurdle_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_HURDLE

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0

			.byte	ANI_CODE
			.word	attack_snd,64,0,0

# Go airborne, retain my current x/z vel

			.byte	ANI_SETYVEL
			.float	.75
			.byte	ANI_INTSTREAM,15,9

			.byte	ANI_INTSTREAM,10,9
# landed
#			.byte	ANI_SETMODE
#			.word 	MODE_ROTONLY

			.byte	ANI_INTSTREAM,12,9

#			.byte	ANI_SETMODE
#			.word	MODE_UNINT

# Smooth interpolate back into run seq
			.byte	ANI_PREP_XITION
			.word	3,3
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

			
#;;;;;;;;;; TURBO spin hurdle

			.globl	turbo_spin_hurdle_anim
			.byte	SCRIPT
turbo_spin_hurdle_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_R_HURDSP

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			
			.byte	ANI_INTSTREAM,15,3
			
			.byte	ANI_CODE
#			.word	attack_snd,P_HURDLE_SND,0,0
			.word	attack_snd,-1,0,0

			.byte	ANI_SETYVEL
			.float	1.25

		#he'll be in air for 31.25 ticks with 1.25 Y vel
			
			.byte	ANI_ATTACK_ON,ATTMODE_HURDLE,JOINT_RANKLE,JOINT_LANKLE,40
			.float	1.0,1.0,4.0
			
			.byte	ANI_INTSTREAM,28,31
	# landed

			.byte	ANI_ATTACK_OFF
			
			.byte	ANI_INTSTREAM,27,10

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END
			
			
#;;;;;;;;;; NON-TURBO spin hurdle

			.globl	spin_hurdle_anim
			.byte	SCRIPT
spin_hurdle_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_R_HURDSP
			
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0

			.byte	ANI_INTSTREAM,15,3

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0
#			.word	attack_snd,P_HURDLE_SND,0,0
			
			.byte	ANI_SETYVEL
			.float	.85
			
		#he'll be in air for 21.25 ticks with .85 Y vel

			.byte	ANI_ATTACK_ON,ATTMODE_HURDLE,JOINT_RANKLE,JOINT_LANKLE,10
			.float	1.0,1.0,4.0
		
			.byte	ANI_INTSTREAM,28,21
	# landed

			.byte	ANI_ATTACK_OFF

			.byte	ANI_INTSTREAM,27,10
			
			.byte	ANI_PREP_XITION
			.word	3,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END
			
			
#;;;;;;;;;; TURBO two feet up in air hurdle
			
			.globl	turbo_tf_hurdle_anim
			.byte	SCRIPT
turbo_tf_hurdle_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_HURDLE1

			.byte	ANI_SKIPFRAMES,3

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			
			.byte	ANI_CODE
			.word	attack_snd,P_HURDLE_SND,0,0

			.byte	ANI_SETYVEL
			.float	1.25

		#he'll be in air for 31.25 ticks with 1.25 Y vel
			
#			.byte	ANI_ATTACK_ON,ATTMODE_HURDLE,JOINT_RANKLE,JOINT_LANKLE,40
#			.float	1.0,1.0,4.0
			
			.byte	ANI_INTSTREAM,25,31
	# landed

			.byte	ANI_ATTACK_OFF
			
			.byte	ANI_INTSTREAM,14,5

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END
		

#;;;;;;;;;; NON-TURBO two feet up in air hurdle

			.globl	tf_hurdle_anim
			.byte	SCRIPT
tf_hurdle_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_HURDLE1
			
			.byte	ANI_SKIPFRAMES,3

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			
			.byte	ANI_CODE
			.word	attack_snd,P_HURDLE_SND,0,0
			
			.byte	ANI_SETYVEL
			.float	.85
			
		#he'll be in air for 21.25 ticks with .85 Y vel

#			.byte	ANI_ATTACK_ON,ATTMODE_HURDLE,JOINT_RANKLE,JOINT_LANKLE,10
#			.float	1.0,1.0,4.0
		
			.byte	ANI_INTSTREAM,25,21
	# landed

			.byte	ANI_ATTACK_OFF

			.byte	ANI_INTSTREAM,14,5
			
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END
			

#;;;;;;;;;; TURBO just another hurdle
			
			.globl	turbo_hurdle2_anim
			.byte	SCRIPT
turbo_hurdle2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_HURDLE2

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			
			.byte	ANI_CODE
			.word	attack_snd,P_HURDLE_SND,0,0

			.byte	ANI_SETYVEL
			.float	1.25

		#he'll be in air for 31.25 ticks with 1.25 Y vel
			
#			.byte	ANI_ATTACK_ON,ATTMODE_HURDLE,JOINT_RANKLE,JOINT_LANKLE,40
#			.float	1.0,1.0,4.0
			
			.byte	ANI_INTSTREAM,23,31
	# landed

			.byte	ANI_ATTACK_OFF
			
			.byte	ANI_INTSTREAM,28,6

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END
		

#;;;;;;;;;; NON-TURBO just another hurdle

			.globl	hurdle2_anim
			.byte	SCRIPT
hurdle2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_HURDLE2
			
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			
			.byte	ANI_CODE
			.word	attack_snd,P_HURDLE_SND,0,0
			
			.byte	ANI_SETYVEL
			.float	.85
			
		#he'll be in air for 21.25 ticks with .85 Y vel

#			.byte	ANI_ATTACK_ON,ATTMODE_HURDLE,JOINT_RANKLE,JOINT_LANKLE,10
#			.float	1.0,1.0,4.0
		
			.byte	ANI_INTSTREAM,23,21
	# landed

			.byte	ANI_ATTACK_OFF

			.byte	ANI_INTSTREAM,28,6
			
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; player spins

			.globl	spin_anim
			.byte	SCRIPT
spin_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_CODE_BNZ
			.word	slower_spin,0,0,0,slower_spin_anim
			
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_SPIN
			.byte	0

			.byte	ANI_CODE
			.word	attack_snd,SPIN_SND,0,0
			
			.byte	ANI_SKIPFRAMES,4
			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

			
#;;;;;;;;;; player spins slower

			.byte	SCRIPT
slower_spin_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_SPIN
			.byte	0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.90
			
			.byte	ANI_SKIPFRAMES,4
			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END
			

#;;;;;;;;;;  player gets pissed off....

			.globl	disapointed1_anim
			.byte	SCRIPT
disapointed1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_SETFRICTION
			.float .095

			.byte	ANI_NEWSEQ
			.word	SEQ_R_SLO_D1

			.byte	ANI_RUNTOEND,1
#			.byte	ANI_INTSTREAM,185,100

			.byte	ANI_TURNHACK
			.word	-256
			
			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END


#;;;;;;;;;;  player gets pissed off....

			.globl	disapointed2_anim
			.byte	SCRIPT
disapointed2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_SETFRICTION
			.float .095

			.byte	ANI_NEWSEQ
			.word	SEQ_R_SLO_D2

			.byte	ANI_RUNTOEND,1
#			.byte	ANI_INTSTREAM,218,117

			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END

			
#;;;;;;;;;;  player gets mad cause he missed the pass #1

			.globl	m_mscat1_anim
			.byte	SCRIPT
m_mscat1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_M_MSCAT1

			.byte	ANI_CODE
			.word	taunt_speech,47,0,0

			.byte	ANI_INTSTREAM,34,65
			.byte	ANI_INTSTREAM,35,65
			.byte	ANI_INTSTREAM,34,65
			.byte	ANI_INTSTREAM,35,65

			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END
			
#;;;;;;;;;;  player gets mad cause he missed the pass #2
			
			
			.globl	m_mscat2_anim
			.byte	SCRIPT
m_mscat2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_M_MSCAT2

			.byte	ANI_CODE
			.word	taunt_speech,47,0,0

			.byte	ANI_INTSTREAM,34,65
			.byte	ANI_INTSTREAM,35,65
			.byte	ANI_INTSTREAM,34,65
			.byte	ANI_INTSTREAM,34,65

			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END
			
			
#;;;;;;;;;;  QB gets mad cause he got sacked #1

#			.globl	m_qbjoep_anim
#			.byte	SCRIPT
#m_qbjoep_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_M_QBJOEP
#
#			.byte	ANI_CODE
#			.word	taunt_speech,18,0,0
#			
#			.byte	ANI_INTSTREAM,35,67
#			.byte	ANI_INTSTREAM,35,67
#			.byte	ANI_INTSTREAM,35,67
#			.byte	ANI_INTSTREAM,35,67
##			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_GOTO
#			.word	breath_anim
#			.byte	ANI_END
			
#;;;;;;;;;;  QB gets mad cause he got sacked #2

			.globl	m_qbfite_anim
			.byte	SCRIPT
m_qbfite_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_ZEROVELS

#			.byte	ANI_NEWSEQ
#			.word	SEQ_M_QBFITE

			.byte	ANI_INTTOSEQ,8
			.word	SEQ_M_QBFITE
			.byte	2

			.byte	ANI_CODE
			.word	get_hit_snd,70,0,0
			
			.byte	ANI_INTSTREAM,41,80
			.byte	ANI_INTSTREAM,41,80
			.byte	ANI_INTSTREAM,41,80
			.byte	ANI_INTSTREAM,41,80

#			.byte	ANI_RUNTOEND,1

			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END
			
#;;;;;;;;;;  QB gets mad cause he got sacked #3

			.globl	m_qbwhat_anim
			.byte	SCRIPT
m_qbwhat_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_ZEROVELS

#			.byte	ANI_NEWSEQ
#			.word	SEQ_M_QBWHAT

			.byte	ANI_INTTOSEQ,8
			.word	SEQ_M_QBWHAT
			.byte	2

			.byte	ANI_CODE
			.word	taunt_speech,16,0,0

			.byte	ANI_INTSTREAM,43,80
			.byte	ANI_INTSTREAM,43,80
			.byte	ANI_INTSTREAM,43,80
			.byte	ANI_INTSTREAM,43,80
			
#			.byte	ANI_RUNTOEND,1

			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END


#;;;;;;;;;;  player does a JOHN DANCE as team is selected

#			.globl	tmsel_dance1_anim
#			.byte	SCRIPT
#tmsel_dance1_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORXZ
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_SETFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_TS_JON1
#
#			.byte	ANI_RUNTOEND,1
#			
#			.byte	ANI_CLRFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_INTTOSEQ,8
#			.word	SEQ_QB_BUYIN
#			.byte	0
#			.byte	ANI_GOTO
#			.word	tmsel_waiting_anim
#			.byte	ANI_END

#;;;;;;;;;;  player does a JOHN DANCE as team is selected

#			.globl	tmsel_dance1f_anim
#			.byte	SCRIPT
#tmsel_dance1f_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORXZ
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_SETFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_TS_JON1
#
#			.byte	ANI_RUNTOEND,1
#			
#			.byte	ANI_CLRFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_INTTOSEQ,8
#			.word	SEQ_QB_BUYIN
#			.byte	0
#			.byte	ANI_GOTO
#			.word	tmsel_waiting_anim
#			.byte	ANI_END


#;;;;;;;;;;  player does a JOHN DANCE

#			.globl	tmsel_dance2_anim
#			.byte	SCRIPT
#tmsel_dance2_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_SETFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_TS_JON2
#			
##			.byte	ANI_RUNTOFRAME,28,1
#
##			.byte	ANI_CODE
##			.word	spike_ball,0,0,0
#
##			.byte	ANI_CODE
##			.word	drop_ball,0,0,0
#
#			.byte	ANI_RUNTOEND,1
#			
#			.byte	ANI_CLRFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_INTTOSEQ,8
#			.word	SEQ_QB_BUYIN
#			.byte	0
#			.byte	ANI_GOTO
#			.word	tmsel_waiting_anim
#			.byte	ANI_END


#;;;;;;;;;;  player does a JOHN DANCE

#			.globl	tmsel_dance2f_anim
#			.byte	SCRIPT
#tmsel_dance2f_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_SETFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_TS_JON2
#			
##			.byte	ANI_RUNTOFRAME,28,1
#
#			.byte	ANI_RUNTOEND,1
#			
#			.byte	ANI_CLRFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_INTTOSEQ,8
#			.word	SEQ_QB_BUYIN
#			.byte	0
#			.byte	ANI_GOTO
#			.word	tmsel_waiting_anim
#			.byte	ANI_END

#;;;;;;;;;;  player does a JOHN DANCE

#			.globl	tmsel_dance3_anim
#			.byte	SCRIPT
#tmsel_dance3_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_SETFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_TAUNT_4
#			
##			.byte	ANI_CODE
##			.word	celebrate_speech,P_WOO_WOO_SP,0,0
#
#			.byte	ANI_INTSTREAM,44,84
#			.byte	ANI_RUNTOFRAME,44,1
#
#			.byte	ANI_CLRFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_INTTOSEQ,8
#			.word	SEQ_QB_BUYIN
#			.byte	0
#			.byte	ANI_GOTO
#			.word	tmsel_waiting_anim
#			.byte	ANI_END
#
###;;;;;;;;;;  player does a JOHN DANCE
##
##			.globl	tmsel_dance3f_anim
##			.byte	SCRIPT
##tmsel_dance3f_anim:
##			.byte	ANI_SETMODE
##			.word	MODE_UNINT|MODE_ANCHORXZ
##			.byte	ANI_ZEROVELS
##
##			.byte	ANI_SETFLAG
##			.word	PF_CELEBRATE
##
##			.byte	ANI_NEWSEQ
##			.word	SEQ_TS_JON3F
##			
##			.byte	ANI_RUNTOEND,1
##			
##			.byte	ANI_CLRFLAG
##			.word	PF_CELEBRATE
##
##			.byte	ANI_INTTOSEQ,8
##			.word	SEQ_QB_BUYIN
##			.byte	0
##			.byte	ANI_GOTO
##			.word	tmsel_waiting_anim
##			.byte	ANI_END


#;;;;;;;;;;  player waiting

#			.globl	tmsel_waiting_anim
#			.byte	SCRIPT
#tmsel_waiting_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORXZ
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_QB_BUYIN
#			
#			.byte	ANI_RUNTOEND,1
#			
#			.byte	ANI_GOTO
#			.word	tmsel_waiting_anim
#			.byte	ANI_END


#;;;;;;;;;;  player does a JOHN DANCE

#			.globl	end_zone20_anim
#			.byte	SCRIPT
#end_zone20_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_SETFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_INTTOSEQ,6
#			.word	SEQ_M_EZJON3
#			.byte	0
#			
#	.if DOSETCAM
#			.byte	ANI_CODE_BZ
#			.word	plyr_have_ball,0,0,0,psv_nocam10_a
#			
#			.byte	ANI_SETCAMBOT
#			.word	csi_rs_suplex_nf
#psv_nocam10_a:
#	.endif
#
#			.byte	ANI_CODE
#			.word	drop_ball,0,0,0
#
#			.byte	ANI_CODE
#			.word	taunt_speech,0,0,0
#			
#			.byte	ANI_INTSTREAM,37,85
#			.byte	ANI_INTSTREAM,37,85
#			.byte	ANI_INTSTREAM,37,85
#			.byte	ANI_INTSTREAM,37,85
#			
#			.byte	ANI_CLRFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_GOTO
#			.word	breath_anim
#			.byte	ANI_END

			
#;;;;;;;;;;  player does an end-zone (touchdown) dance

			.globl	end_zone1_anim
			.byte	SCRIPT
end_zone1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE

#			.byte	ANI_NEWSEQ
#			.word	SEQ_INZONE1

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_INZONE1
			.byte	0
			

	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam10_b
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex_nf
psv_nocam10_b:
	.endif
			.byte	ANI_CODE
			.word	taunt_speech,0,0,0
			
			.byte	ANI_INTSTREAM,100,80
			
			.byte	ANI_INTSTREAM,100,80
			
			.byte	ANI_CODE
			.word	taunt_speech,2,0,0
			
			.byte	ANI_INTSTREAM,23,19
			
			.byte	ANI_CODE
			.word	spike_ball,0,0,0
			
			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	-128
						
			.byte	ANI_CLRFLAG
			.word	PF_CELEBRATE

			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END

#;;;;;;;;;;  player pounds chest (touchdown) dance

			.globl	end_zone5_anim
			.byte	SCRIPT
end_zone5_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_ZEROVELS
			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE


			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_TAUNBC
			.byte	0

			.byte	ANI_CODE
			.word	taunt_speech,18,0,0
			
			.byte	ANI_STREAMTOEND
			.float	1.90

			
			.byte	ANI_CLRFLAG
			.word	PF_CELEBRATE

			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END
			
#;;;;;;;;;;  player flexs to crowd

#			.globl	end_zone12_anim
#			.byte	SCRIPT
#end_zone12_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#			.byte	ANI_ZEROVELS
#			.byte	ANI_SETFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_INTTOSEQ,6
#			.word	SEQ_M_EZFLEX
#			.byte	0
#
#	.if DOSETCAM
#			.byte	ANI_CODE_BZ
#			.word	plyr_have_ball,0,0,0,psv_nocam10_g
#			
#			.byte	ANI_SETCAMBOT
#			.word	csi_rs_suplex_nf
#psv_nocam10_g:
#	.endif
#
#			.byte	ANI_INTSTREAM,13,30
#			
#			.byte	ANI_CODE
#			.word	spike_ball,1,0,0
#
#			.byte	ANI_CODE
#			.word	taunt_speech,4,0,0
#
#			.byte	ANI_INTSTREAM,33,70
#			.byte	ANI_INTSTREAM,33,70
#			.byte	ANI_INTSTREAM,33,70
#			.byte	ANI_INTSTREAM,34,70
#			
#			.byte	ANI_RUNTOEND,1
#			
#			.byte	ANI_CLRFLAG
#			.word	PF_CELEBRATE
#			.byte	ANI_GOTO
#			.word	breath_anim
#			.byte	ANI_END
#


			
#;;;;;;;;;;  player waves to crowd

			.globl	end_zone6_anim
			.byte	SCRIPT
end_zone6_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_ZEROVELS
			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE

#			.byte	ANI_NEWSEQ
#			.word	SEQ_M_EZCTHR

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_EZCTHR
			.byte	0


	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam10_c
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex
psv_nocam10_c:
	.endif

			.byte	ANI_RUNTOFRAME,172,1
			
			.byte	ANI_CODE
			.word	taunt_speech,20,0,0
			
			.byte	ANI_CODE
			.word	spike_ball,1,0,0
			
			.byte	ANI_INTSTREAM,47,105
			
			.byte	ANI_TURNHACK
			.word	512
			
			.byte	ANI_CLRFLAG
			.word	PF_CELEBRATE
			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END
			
			
#;;;;;;;;;;  player fires guns

			.globl	end_zone9_anim
			.byte	SCRIPT
end_zone9_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_ZEROVELS
			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_EZCGUN
			.byte	0


	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam10_z
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex_nf
psv_nocam10_z:
	.endif

			.byte	ANI_INTSTREAM,100,45
			
			.byte	ANI_CODE
			.word	attack_snd,147,0,0
			
			.byte	ANI_INTSTREAM,16,11
			
			.byte	ANI_CODE
			.word	attack_snd,147,0,0
			
			.byte	ANI_INTSTREAM,12,8
			
			.byte	ANI_CODE
			.word	attack_snd,147,0,0
			
			.byte	ANI_INTSTREAM,12,8
			
			.byte	ANI_CODE
			.word	attack_snd,147,0,0
			
			.byte	ANI_INTSTREAM,10,8
			
			.byte	ANI_CODE
			.word	attack_snd,147,0,0
			
			.byte	ANI_INTSTREAM,10,8
			
			.byte	ANI_CODE
			.word	attack_snd,182,0,0
			
			.byte	ANI_INTSTREAM,11,8
			
			.byte	ANI_CODE
			.word	attack_snd,182,0,0
			
			.byte	ANI_RUNTOFRAME,252,1

			.byte	ANI_CODE
			.word	attack_snd,181,0,0
			
			.byte	ANI_INTSTREAM,9,10
			
			.byte	ANI_CODE
			.word	attack_snd,183,0,0
			
			.byte	ANI_RUNTOEND,1
			
			.byte	ANI_CLRFLAG
			.word	PF_CELEBRATE
			
			.byte	ANI_TURNHACK
			.word	-256
			
			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END
			
#;;;;;;;;;;  player spins the ball on the ground

#			.globl	end_zone16_anim
#			.byte	SCRIPT
#end_zone16_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#			.byte	ANI_ZEROVELS
#			.byte	ANI_SETFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_INTTOSEQ,6
#			.word	SEQ_M_EZBSPN
#			.byte	0
#						
#			.byte	ANI_RUNTOFRAME,60,1
#
#			.byte	ANI_CODE
#			.word	spin_ball,0,0,0
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CLRFLAG
#			.word	PF_CELEBRATE
		
#			.byte	ANI_GOTO
#			.word	breath_anim
#			.byte	ANI_END

#;;;;;;;;;;  player dunks the ball between goal post

#			.globl	end_zone17_anim
#			.byte	SCRIPT
#end_zone17_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_SETFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_INTTOSEQ,6
#			.word	SEQ_M_EZDUNK
#			.byte	0
#			
##			.byte	ANI_RUNTOFRAME,60,1
#
#			.byte	ANI_RUNTOFRAME,28,1
#
#
#
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORY
#
#			.byte	ANI_SETYVEL
#			.float	0.80
#
#			.byte	ANI_RUNTOFRAME,50,1
#
##			.byte	1
#
##			.byte	ANI_INTSTREAM,8,AA_TOGROUND
#
##			.byte	ANI_CODE
##			.word	spin_ball,0,0,0
#
###			.word	256
 #
 #			.byte	ANI_CODE
#			.word	spike_ball,0,0,0
# #
#
#			.byte	ANI_INTSTREAM,16,AA_TOGROUND
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CLRFLAG
#			.word	PF_CELEBRATE
#			
#			.byte	ANI_TURNHACK
#			.word	256
#			
#			.byte	ANI_GOTO
#			.word	breath_anim
#			.byte	ANI_END
#

#;;;;;;;;;; End-zone puppet moves

#			.globl	ez_pin_a_anim
#			.globl	ez_pin_v_anim
#			.byte	SCRIPT
#ez_pin_a_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_EZ_PIN_A
#
##			.byte	ANI_SKIPFRAMES,1
#
#			.byte	ANI_CODE
#			.word	celebrate_speech,P_WOO_WOO_SP,0,0
#			.byte	ANI_RUNTOFRAME,125,1
#
#			.word	celebrate_speech,P_YEAH_BABY_SP,0,0
#
#			.byte	ANI_RUNTOFRAME32
#			.word	277,1
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_PREP_XITION
#			.word	6,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
			

#			.byte	SCRIPT
#ez_pin_v_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHOR|MODE_UNINT|MODE_REACTING|MODE_PUPPET
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_EZ_PIN_V
#
##			.byte	ANI_SKIPFRAMES,1
#
#			.byte	ANI_RUNTOFRAME,125,1
#
#			.byte	ANI_RUNTOFRAME32
#			.word	277,1
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_PREP_XITION
#			.word	6,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END

			
#;;;;;;;;;
#;;;;;;;;; LINEMAN SEQUENCES
#;;;;;;;;;
			.globl	stance3pt_anim
			.byte	SCRIPT
stance3pt_anim:
			.byte	ANI_SETMODE
			.word	0
#			.byte	ANI_INTTOSEQ,6
#			.word	SEQ_3P_C
#			.byte	0
			.byte	ANI_NEWSEQ
			.word	SEQ_3P_C
			.byte	ANI_SKIPFRAMES,AA_RAND,60,0
			.byte	ANI_RUNTOEND,1

stance3pt_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_3P_C
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	stance3pt_loop

;;;;;;;;;;
			.globl	stance3pt_jumpback_anim
			.byte	SCRIPT
stance3pt_jumpback_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR
			.byte	ANI_NEWSEQ
			.word	SEQ_3P_JB
			.byte	ANI_RUNTOEND,1
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR
			.byte	ANI_GOTO
			.word	block_ready_anim_rnd

;;;;;;;;;;
			.globl	block_ready_anim
			.globl	block_ready_anim_int
			.byte	SCRIPT
block_ready_anim_rnd:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
		.byte	ANI_NEWSEQ
		.word	SEQ_BLK_RDY

#			.byte	ANI_INTTOSEQ
#			.word	SEQ_BLK_RDY
#			.byte	0

			.byte	ANI_ZEROVELS

			.byte	ANI_SKIPFRAMES,50,0
			.byte	ANI_GOTO
			.word	block_ready_loop


			.byte	SCRIPT
block_ready_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_NEWSEQ
			.word	SEQ_BLK_RDY
			.byte	ANI_ZEROVELS

block_ready_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_BLK_RDY
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	block_ready_loop

			.byte	SCRIPT
block_ready_anim_int:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_ZEROVELS
			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
			.word	SEQ_BLK_RDY
			.byte	AA_VAR_FRAME
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	block_ready_loop

#;;;;;;;;; lineman moves #1

			.globl	pushbl1_anim
			.byte	SCRIPT
pushbl1_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT
			.byte	ANI_ZEROVELS
			.byte	ANI_NEWSEQ
			.word	SEQ_PUSH_BL1

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_INTSTREAM,32,18

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_CANBLOCK

			.byte	ANI_INTSTREAM,9,4

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;; lineman moves #2

			.globl	pushbl2_anim
			.byte	SCRIPT
pushbl2_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT
			.byte	ANI_ZEROVELS
			.byte	ANI_NEWSEQ
			.word	SEQ_PUSH_BL2

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_INTSTREAM,31,17

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_CANBLOCK

			.byte	ANI_INTSTREAM,8,4

			.byte	ANI_PREP_XITION
			.word	6,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;; lineman moves #3

			.globl	pushbl3_anim
			.byte	SCRIPT
pushbl3_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT
			.byte	ANI_ZEROVELS
			.byte	ANI_NEWSEQ
			.word	SEQ_PUSH_BL3

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_INTSTREAM,23,10

			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_CANBLOCK

			.byte	ANI_INTSTREAM,11,6

			.byte	ANI_PREP_XITION
			.word	6,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;; lineman hitting other lineman

			.globl	charge_anim
			.byte	SCRIPT
charge_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ
			.byte	ANI_ZEROVELS
			.byte	ANI_INTTOSEQ,8
			.word	SEQ_CHARGE3P
			.byte	0
			.byte	ANI_RUNTOEND,1
			.byte	ANI_END

#;;;;;;;;; QB 'PRE-SNAP' SEQUENCES

			.globl	qb_random_anim
			.globl	qb_random_sg_anim
			.globl	qb_pumps_crowd_set_anim
			.globl	qb_claps_set_anim
			.globl	qb_rubs_knee_set_anim

			.byte	SCRIPT
qb_random_sg_anim:
			.byte	ANI_CODE
			.word	pick_random_shotgun_qb_anim,0,0,0
			.byte	ANI_END
			

			.byte	SCRIPT
qb_random_anim:
			.byte	ANI_CODE
			.word	pick_random_qb_anim,0,0,0
			.byte	ANI_END

#;;;;;;;;; qb pumps crowd

			.byte	SCRIPT
qb_pumps_crowd_set_anim:
			.byte	ANI_NEWSEQ
			.word	SEQ_PSQB1

			.byte	ANI_INTSTREAM,37,48
#			.byte	ANI_CODE
#			.word	crowd_cheer,-1,0,0
			.byte	ANI_INTSTREAM,28,36
#			.byte	ANI_CODE
#			.word	crowd_cheer,-1,0,0
			.byte	ANI_INTSTREAM,39,50
			.byte	ANI_CODE
			.word	hut_snd,-1,0,0
			.byte	ANI_INTSTREAM,41,53
			.byte	ANI_CODE
			.word	hut_snd,-1,0,0
			.byte	ANI_RUNTOEND,1

			.byte	ANI_INTTOSEQ,8
			.word	SEQ_HUT
			.byte	0
			.byte	ANI_GOTO
			.word	qbset_anim
			.byte	ANI_END


#;;;;;;;;; qb claps then huts

			.byte	SCRIPT
qb_claps_set_anim:
			.byte	ANI_NEWSEQ
			.word	SEQ_PSQB2

			.byte	ANI_INTSTREAM,77,100
			.byte	ANI_CODE
			.word	hut_snd,-1,0,0
			.byte	ANI_INTSTREAM,45,58
			.byte	ANI_CODE
			.word	hut_snd,-1,0,0

			.byte	ANI_INTSTREAM,27,36
			.byte	ANI_RUNTOEND,1

			.byte	ANI_INTTOSEQ,8
			.word	SEQ_HUT
			.byte	0
			.byte	ANI_GOTO
			.word	qbset_anim
			.byte	ANI_END


#;;;;;;;;; qb pumps crowd

			.byte	SCRIPT
qb_rubs_knee_set_anim:
			.byte	ANI_NEWSEQ
			.word	SEQ_PSQB3

			.byte	ANI_INTSTREAM,53,75
			.byte	ANI_INTSTREAM,53,75
			.byte	ANI_CODE
			.word	hut_snd,-1,0,0
			.byte	ANI_INTSTREAM,39,50
			.byte	ANI_CODE
			.word	hut_snd,-1,0,0
			.byte	ANI_RUNTOEND,1

			.byte	ANI_INTTOSEQ,8
			.word	SEQ_HUT
			.byte	0
			.byte	ANI_GOTO
			.word	qbset_anim
			.byte	ANI_END

#;;;;;;;;; DB 'PRE-SNAP' SEQUENCES

			.globl	db_random_anim
			.byte	SCRIPT
db_random_anim:
			.byte	ANI_CODE
			.word	pick_random_db_anim,0,0,0
			.byte	ANI_END


#;;;;;;;;; LINEMEN 'PRE-SNAP' SEQUENCES

			.globl	lm_random_anim
			.globl	lm_set1_anim
			.globl	lm_set2_anim
			.globl	lm_set3_anim
			.globl	lm_set4_anim
			.byte	SCRIPT
lm_random_anim:
			.byte	ANI_CODE
			.word	pick_random_linemen_anim,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
lm_set1_anim:
			.byte	ANI_NEWSEQ
			.word	SEQ_PSDLINE1
			.byte	ANI_INTSTREAM,50,70
			.byte	ANI_INTSTREAM,50,70
			.byte	ANI_INTSTREAM,49,70
			.byte	ANI_INTTOSEQ,10
			.word	SEQ_3P_C
			.byte	0
			.byte	ANI_GOTO
			.word	stance3pt_anim
			.byte	ANI_END

			.byte	SCRIPT
lm_set2_anim:
			.byte	ANI_NEWSEQ
			.word	SEQ_PSDLINE2
			.byte	ANI_INTSTREAM,50,70
			.byte	ANI_INTSTREAM,50,70
			.byte	ANI_INTSTREAM,49,70
			.byte	ANI_INTTOSEQ,10
			.word	SEQ_3P_C
			.byte	0
			.byte	ANI_GOTO
			.word	stance3pt_anim
			.byte	ANI_END


			.byte	SCRIPT
lm_set3_anim:
			.byte	ANI_NEWSEQ
			.word	SEQ_PSODLIN1
			.byte	ANI_INTSTREAM,50,70
			.byte	ANI_INTSTREAM,50,70
			.byte	ANI_INTSTREAM,54,73
			.byte	ANI_INTTOSEQ,10
			.word	SEQ_3P_C
			.byte	0
			.byte	ANI_GOTO
			.word	stance3pt_anim
			.byte	ANI_END


			.byte	SCRIPT
lm_set4_anim:
			.byte	ANI_NEWSEQ
			.word	SEQ_PSODLIN2
			.byte	ANI_INTSTREAM,50,70
			.byte	ANI_INTSTREAM,50,70
			.byte	ANI_INTSTREAM,42,65
			.byte	ANI_INTTOSEQ,10
			.word	SEQ_3P_C
			.byte	0
			.byte	ANI_GOTO
			.word	stance3pt_anim
			.byte	ANI_END


#;;;;;;;;; WIDE RECEIVER 'PRE-SNAP' SEQUENCES

			.globl wr_random_anim
			.globl wr_sprint1_anim
			.globl wr_sprint2_anim
			.globl wr_sprint3_anim
			.globl wr_sprint4_anim
			.byte	SCRIPT
wr_random_anim:
			.byte	ANI_CODE
			.word	pick_random_wr_anim,0,0,0
			.byte	ANI_END
			

			.byte	SCRIPT
wr_sprint1_anim:
			.byte	ANI_NEWSEQ
			.word	SEQ_PSSPRNT1
			.byte	ANI_INTSTREAM,64,50
			.byte	ANI_INTSTREAM,64,50
			.byte	ANI_INTSTREAM,64,50
			.byte	ANI_INTSTREAM,93,75
			.byte	ANI_INTTOSEQ,8
			.word	SEQ_SPT_C
			.byte	0
			.byte	ANI_GOTO
			.word	wr_set_anim
			.byte	ANI_END
			

			.byte	SCRIPT
wr_sprint2_anim:
			.byte	ANI_NEWSEQ
			.word	SEQ_PSSPRNT2
			.byte	ANI_INTSTREAM,64,50
			.byte	ANI_INTSTREAM,64,50
			.byte	ANI_INTSTREAM,64,50
			.byte	ANI_INTSTREAM,93,75
			.byte	ANI_INTTOSEQ,8
			.word	SEQ_SPT_C
			.byte	0
			.byte	ANI_GOTO
			.word	wr_set_anim
			.byte	ANI_END


			.byte	SCRIPT
wr_sprint3_anim:
			.byte	ANI_NEWSEQ
			.word	SEQ_PSSPRNT3
			.byte	ANI_INTSTREAM,64,50
			.byte	ANI_INTSTREAM,64,50
			.byte	ANI_INTSTREAM,64,50
			.byte	ANI_INTSTREAM,93,75
			.byte	ANI_INTTOSEQ,8
			.word	SEQ_SPT_C
			.byte	0
			.byte	ANI_GOTO
			.word	wr_set_anim
			.byte	ANI_END


			.byte	SCRIPT
wr_sprint4_anim:
			.byte	ANI_NEWSEQ
			.word	SEQ_PSSPRNT4
			.byte	ANI_INTSTREAM,64,50
			.byte	ANI_INTSTREAM,64,50
			.byte	ANI_INTSTREAM,64,50
			.byte	ANI_INTSTREAM,93,75
			.byte	ANI_INTTOSEQ,8
			.word	SEQ_SPT_C
			.byte	0
			.byte	ANI_GOTO
			.word	wr_set_anim
			.byte	ANI_END



			.globl	wr_set_anim
			.globl	wr_set_anim_int

			.byte	SCRIPT
wr_set_anim_int:
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_SPT_C
			.byte	1
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	wr_set_loop

			.byte	SCRIPT
wr_set_anim:
			.byte	ANI_NEWSEQ
			.word	SEQ_SPT_C
			.byte	ANI_SKIPFRAMES,AA_RAND,60,0
			.byte	ANI_RUNTOEND,1
wr_set_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_SPT_C
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	wr_set_loop

			.globl	wr_set2_anim
			.byte	SCRIPT
wr_set2_anim:
			.byte	ANI_NEWSEQ
			.word	SEQ_SPT_CF
			.byte	ANI_SKIPFRAMES,AA_RAND,59,0
			.byte	ANI_RUNTOEND,1
wr_set2_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_SPT_CF
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	wr_set2_loop

#;;;;;;;;;
#;;;;;;;;; CENTER SEQUENCES
#;;;;;;;;;
			.globl	center_set_anim
			.byte	SCRIPT
center_set_anim:
			.byte	ANI_NEWSEQ
			.word	SEQ_CENTER_C
			.byte	ANI_INTSTREAM,58,116
#			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	center_set_anim

#;;;;;;;;;; center snaps the ball

			.globl	center_snap_anim
			.byte	SCRIPT
center_snap_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR
			.byte	ANI_NEWSEQ
			.word	SEQ_C_SNAP
			.byte	ANI_INTSTREAM,21,4

			.byte	ANI_CODE
			.word	hike_ball,0,0,0

			.byte	ANI_INTSTREAM,23,8

			.byte	ANI_SETMODE
			.word	MODE_ANCHOR
			.byte	ANI_GOTO
			.word	stance3pt_jumpback_anim

#;;;;;;;;;
#;;;;;;;;; DEFENSIVE BACK SEQUENCES
#;;;;;;;;;
			.globl	stance2pt_anim_rnd
			.globl	stance2pt_anim_int
			.globl	safty2pt_anim_rnd

			.globl	stance2pt2_anim_rnd
			.globl	stance2pt2_anim_int

#			.byte	SCRIPT
#stance2pt2_anim_int:
#			.byte	ANI_ZEROVELS
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHOR
#			.byte	ANI_INTTOSEQ,4
#			.word	SEQ_2P_C2
#			.byte	0
#			.byte	ANI_RUNTOEND,2
#			.byte	ANI_GOTO
#			.word	stance2pt2_loop
#			.byte	ANI_END
#
			.byte	SCRIPT
stance2pt2_anim_rnd:
			.byte	ANI_ZEROVELS
			.byte	ANI_SETMODE
#			.word	0
			.word	MODE_ANCHORXZ
			.byte	ANI_NEWSEQ
			.word	SEQ_2P_C2
			.byte	ANI_SKIPFRAMES,AA_RAND,79,0
			.byte	ANI_RUNTOEND,2
stance2pt2_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_2P_C2
			
			.byte	ANI_RUNTOEND,2
			.byte	ANI_GOTO
			.word	stance2pt2_loop


			.byte	SCRIPT
stance2pt_anim_int:
			.byte	ANI_ZEROVELS
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_2P_C
			.byte	0
			.byte	ANI_RUNTOEND,2
			.byte	ANI_GOTO
			.word	stance2pt_loop
			.byte	ANI_END

			.byte	SCRIPT
stance2pt_anim_rnd:
			.byte	ANI_ZEROVELS
			.byte	ANI_SETMODE
			.word	0
			.byte	ANI_NEWSEQ
			.word	SEQ_2P_C
			.byte	ANI_SKIPFRAMES,AA_RAND,79,0
			.byte	ANI_RUNTOEND,2
stance2pt_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_2P_C

			.byte	ANI_RUNTOEND,2
			.byte	ANI_GOTO
			.word	stance2pt_loop
			.byte	ANI_END



			.byte	SCRIPT
safty2pt_anim_rnd:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,8
			.word	SEQ_L_SAFTY
			.word	0

#			.byte	ANI_PREP_XITION
#			.word	4,0

#			.byte	ANI_INTTOSEQ,AA_VAR_COUNT
#			.word	SEQ_L_SAFTY
#			.byte	AA_VAR_FRAME

			.byte	ANI_ZEROVELS

#			.byte	ANI_SKIPFRAMES,AA_RAND,70,0
#			.byte	ANI_RUNTOEND,2

			.byte	ANI_INTSTREAM,34,70
			.byte	ANI_INTSTREAM,34,70
			.byte	ANI_INTSTREAM,34,70
			.byte	ANI_INTSTREAM,34,70



			
safty2pt_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_L_SAFTY

#			.byte	ANI_RUNTOEND,2
#			.byte	ANI_STREAMTOEND
#			.float	1.9

			.byte	ANI_INTSTREAM,34,70
			.byte	ANI_INTSTREAM,34,70
			.byte	ANI_INTSTREAM,34,70
			.byte	ANI_INTSTREAM,34,70


			.byte	ANI_GOTO
			.word	safty2pt_loop


#;;;;;;;;;
#;;;;;;;;; PUPPET MOVES
#;;;;;;;;;


			.globl	p_waist_drop_a_anim
			.globl	p_waist_drop_v_anim
			.byte	SCRIPT
p_waist_drop_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_NEWSEQ
			.word	SEQ_WSTDROPA
			
			.byte	ANI_SETVELS
			.float	0.0,1.2,.90

			.byte	ANI_INTSTREAM,11,6

			.byte	ANI_INTSTREAM,31,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,P_GRND_HIT1_SND,-1,0

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float .055

			.byte	ANI_RUNTOFRAME,64,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_INTSTREAM,88,55

			.byte	ANI_TURNHACK
			.word	512

#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR

#			.byte	ANI_INTTOSEQ,6
#			.word	SEQ_M_GU_C2
#			.byte	0

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
p_waist_drop_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPET

			.byte 	ANI_CODE
			.word	get_hit_snd,P_HIT2_SND,0,0
			.byte	ANI_CODE
			.word	attack_snd,P_THRW_PLR_SP,0,0
			
			.byte	ANI_NEWSEQ
			.word	SEQ_WSTDROPV

			.byte	ANI_INTSTREAM,11,6
			
			.byte	ANI_INTSTREAM,31,AA_TOGROUND

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_RUNTOFRAME,64,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

#			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END

#;;;;;;;;; 


			.globl	p_head_grab_a_anim
			.globl	p_head_grab_v_anim
			.byte	SCRIPT
p_head_grab_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT

			.byte	ANI_NEWSEQ
			.word	SEQ_HEDGRB_A

			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_SETVELS
			.float	0.0,0.55,1.2

			.byte	ANI_CODE
			.word	attack_snd,P_PUSH2_SND,0,0

			.byte	1

			.byte	ANI_INTSTREAM,30,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,P_GRND_HIT1_SND,0,0

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float .045

			.byte	ANI_RUNTOFRAME,133,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_NOCYL

			.byte	ANI_INTSTREAM,83,63

			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	-265

			.byte	ANI_CODE
			.word	general_run,0,0,0

#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0

			.byte	ANI_END


			.byte	SCRIPT
p_head_grab_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT

			.byte	ANI_NEWSEQ
			.word	SEQ_HEDGRB_V

	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam10_f
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex_nf
psv_nocam10_f:
	.endif
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPET

			.byte	ANI_ZEROVELS

			.byte 	ANI_CODE
			.word	get_hit_snd,P_HIT2_SND,0,0
			.byte	ANI_CODE
			.word	attack_snd,P_THRW_PLR_SP,0,0

			.byte	1

			.byte	ANI_INTSTREAM,30,AA_TOGROUND

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_RUNTOFRAME,133,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END



			.globl	p_hip_toss_a_anim
			.globl	p_hip_toss_v_anim
			.byte	SCRIPT
p_hip_toss_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_SETVELS
			.float	0.0,0.0,.75

			.byte	ANI_SETFRICTION
			.float .050
#			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	set_plyr_y_pos,0,0,0

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_HIPTOSSA

			.byte	ANI_RUNTOFRAME,37,1

			.byte	ANI_CODE
			.word	attack_snd,P_PUSH2_SND,0,0

			.byte	ANI_RUNTOFRAME,48,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_CODE
			.word	general_run,0,0,0

			.byte	ANI_END



			.byte	SCRIPT
p_hip_toss_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET



#; Test new camera stuff
	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam_4
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex
psv_nocam_4:
	.endif

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte 	ANI_CODE
			.word	get_hit_snd,P_HIT2_SND,0,0
			.byte	ANI_CODE
			.word	attack_snd,P_THRW_PLR_SP,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_HIPTOSSV

			.byte	1

			.byte	ANI_RUNTOFRAME,48,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_CODE
			.word	get_hit_snd,0,-1,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			
			.byte	ANI_SETVELS
#			.float	0.0,.15,-1.10
			.float	0.0,-.50,-1.40
			.byte	1

			.byte	ANI_INTSTREAM,12,AA_TOGROUND

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,P_GRND_HIT1_SND,0,0

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float .060

			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fu_anim

			.byte	ANI_GOTO
			.word	get_up_b_anim
			.byte	ANI_END

#;;;;;;;;;;;;;;;

			.globl	p_clothes_line_a_anim
			.globl	p_clothes_line_v_anim
			.byte	SCRIPT
p_clothes_line_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ|MODE_UNINT|MODE_PUPPET

			.byte	ANI_NEWSEQ
			.word	SEQ_CLOTHS_A

			.byte	ANI_INTSTREAM,13,6
			.byte	1
			.byte	ANI_INTSTREAM,29,AA_TOGROUND

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_TURNHACK
			.word	512
			
			.byte	ANI_GOTO
			.word	get_up_b_anim
			.byte	ANI_END



			.byte	SCRIPT
p_clothes_line_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPETEER




#; Test new camera stuff
	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam_6
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex
psv_nocam_6:
	.endif




			.byte	ANI_SETVELS
			.float	0.0,0.0,-.95

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_CLOTHS_V

			.byte	ANI_INTSTREAM,13,6
#			.byte	ANI_RUNTOFRAME,12,1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_PUPPETEER
			
			.byte	ANI_SETYVEL
			.float	0.40
			.byte	1
			.byte	ANI_INTSTREAM,29,AA_TOGROUND

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,P_GRND_HIT1_SND,-1,0

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float .045

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fu_anim

			.byte	ANI_GOTO
			.word	get_up_b_anim
			.byte	ANI_END



#;;;;;;;;;;;;;;;

			.globl	p_grab_jersey_a_anim
			.globl	p_grab_jersey_v_anim
			.byte	SCRIPT
p_grab_jersey_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ|MODE_UNINT|MODE_PUPPET

			.byte	ANI_NEWSEQ
			.word	SEQ_JERSY_A

			.byte	ANI_CODE
			.word	attack_snd,P_THRW_PLR_SP,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_TURNHACK
			.word	256

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END



			.byte	SCRIPT
p_grab_jersey_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPETEER

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.55

			.byte	ANI_NEWSEQ
			.word	SEQ_JERSY_V



	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam10_e
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex_nf
psv_nocam10_e:
	.endif




			.byte	ANI_SETFRICTION
			.float .015

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_NEWSEQ
			.word	SEQ_H_SPIN2

			.byte	ANI_SETVELS
			.float	0.0,0.0,-.95

#			.byte	ANI_SKIPFRAMES,4
			.byte	ANI_SKIPFRAMES,7

			.byte	ANI_CODE
			.word	get_hit_snd,-1,P_GET_THRWN1_SP,0

#			.byte	1,1,1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_SETYVEL
			.float	.65
			.byte	1
			.byte	ANI_INTSTREAM,41,AA_TOGROUND

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float	.032

			.byte	1

			.byte	ANI_SETYVEL
			.float	.43
			.byte	ANI_INTSTREAM,20,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,2
			.word	10

			.byte	ANI_CODE
			.word	tackle_spch,HS_HARD,-1,0

			.byte	ANI_TURNHACK
			.word	-64

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fu_anim

#			.byte	ANI_TURNHACK
#			.word	64

			.byte	ANI_CODE
			.word	maybe_get_up_b_hurt,0,0,0
			
			.byte	ANI_RUNTOEND,1

			.byte	ANI_GOTO
			.word	get_up_b_anim
			.byte	ANI_END




#;;;;;;;;; Puppet RUN sequences!!!
	
			.globl	p_run_drag_a_anim
			.globl	p_run_drag_v_anim
			.byte	SCRIPT
p_run_drag_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPET|MODE_NOCYL
draga_cyc:
			.byte	ANI_NEWSEQ
			.word	SEQ_P_DRAG_A

			.byte	ANI_RUNTOFRAME,10,1
			.byte	ANI_CODE
			.word	dust_cloud,1,0,0

			.byte	ANI_RUNTOFRAME,20,1
			.byte	ANI_CODE
			.word	dust_cloud,1,0,0

			.byte	ANI_RUNTOFRAME,30,1
			.byte	ANI_CODE
			.word	dust_cloud,2,0,0

			.byte	ANI_RUNTOFRAME,40,1
			.byte	ANI_CODE
			.word	dust_cloud,1,0,0

			.byte	ANI_RUNTOFRAME,50,1
			.byte	ANI_CODE
			.word	dust_cloud,1,0,0

			.byte	ANI_RUNTOFRAME,60,1
			.byte	ANI_CODE
			.word	dust_cloud,2,0,0

			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	draga_cyc


			.byte	SCRIPT
p_run_drag_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_PUPPETEER
dragv_cyc:
			.byte	ANI_NEWSEQ
			.word	SEQ_P_DRAG_V

			.byte	ANI_RUNTOFRAME,52,1

			.byte	ANI_CODE
			.word	taunt_speech,37,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_GOTO
			.word	dragv_cyc
			


			.globl	p_run_drag2_a_anim
			.globl	p_run_drag2_v_anim
			.byte	SCRIPT
p_run_drag2_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPET|MODE_NOCYL
draga2_cyc:
			.byte	ANI_NEWSEQ
			.word	SEQ_P_MNKY_A
			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	draga2_cyc


			.byte	SCRIPT
p_run_drag2_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_PUPPETEER
dragv2_cyc:
			.byte	ANI_NEWSEQ
			.word	SEQ_P_MNKY_V

			.byte	ANI_RUNTOFRAME,52,1

			.byte	ANI_CODE
			.word	taunt_speech,37,0,0

			.byte	ANI_RUNTOEND,1
			.byte	ANI_GOTO
			.word	dragv2_cyc


#;;;;;;;;;;;;

#			.globl	p_drag_arm_a_anim
#			.globl	p_drag_arm_v_anim
#			.byte	SCRIPT
#p_drag_arm_a_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORXZ|MODE_UNINT|MODE_PUPPETEER
#
#			.byte	ANI_SET_FACE_REL_VEL
#			.float	.65
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_ARMT_A
#
#			.byte	ANI_RUNTOFRAME,10,1
#			.byte	ANI_CODE
#			.word	dust_cloud,1,0,0
#
#			.byte	ANI_RUNTOFRAME,20,1
#			.byte	ANI_CODE
#			.word	dust_cloud,1,0,0
#
#			.byte	ANI_RUNTOFRAME,30,1
#			.byte	ANI_CODE
#			.word	dust_cloud,1,0,0
#
#			.byte	ANI_RUNTOFRAME,40,1
#			.byte	ANI_CODE
#			.word	dust_cloud,1,0,0
#
#			.byte	ANI_RUNTOFRAME,50,1
#			.byte	ANI_CODE
#			.word	dust_cloud,1,0,0
#
#			.byte	ANI_RUNTOFRAME,60,1
#			.byte	ANI_CODE
#			.word	dust_cloud,1,0,0
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_GOTO
#			.word	p_frol_a_anim
#			.byte	ANI_END
#
#
#			.byte	SCRIPT
#p_drag_arm_v_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORXZ|MODE_UNINT|MODE_REACTING|MODE_PUPPET
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_ARMT_V
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_GOTO
#			.word	p_frol_v_anim
#			.byte	ANI_END


#;;;;;;;;; Lineman/blocker spins around opponent

			.globl	spin_a_anim
			.globl	spin_v_anim
			.byte	SCRIPT
spin_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_SPIN_A

			.byte	ANI_SKIPFRAMES,1

			.byte	ANI_INTSTREAM,4,2

#			.byte	ANI_CODE
#			.word	get_hit_snd,P_THRW_PLR_SP,0,0

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_INTSTREAM,27,20

			.byte	ANI_PREP_XITION
			.word	8,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
spin_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_SPIN_V

			.byte	ANI_SKIPFRAMES,1

			.byte	ANI_INTSTREAM,4,2

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_INTSTREAM,45,34

			.byte	ANI_PREP_XITION
			.word	3,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; lineman/blocker tied-up

			.globl	tie1c_a_anim
			.globl	tie1c_v_anim
			.byte	SCRIPT
tie1c_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_TIE1C_A

			.byte 	ANI_CODE
			.word	get_hit_snd,P_HIT6_SND,0,0

			.byte	ANI_INTSTREAM,71,47

#			.byte 	ANI_CODE
#			.word	get_hit_snd,-1,0,0

			.byte	ANI_INTSTREAM,71,46

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_PREP_XITION
			.word	8,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
tie1c_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_TIE1C_V

			.byte 	ANI_CODE
			.word	get_hit_snd,10,0,0

			.byte	ANI_INTSTREAM,71,47

			.byte	ANI_INTSTREAM,71,46

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_PREP_XITION
			.word	3,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; lineman/blocker tied-up #2

			.globl	tie2c_a_anim
			.globl	tie2c_v_anim
			.byte	SCRIPT
tie2c_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_TIE2C_A

			.byte 	ANI_CODE
			.word	get_hit_snd,10,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_PREP_XITION
			.word	8,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
tie2c_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_TIE2C_V

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_PREP_XITION
			.word	3,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;; lineman/blocker tied-up #3

			.globl	tie3c_a_anim
			.globl	tie3c_v_anim
			.byte	SCRIPT
tie3c_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_TIE_LG_A

			.byte	ANI_SKIPFRAMES,1

			.byte 	ANI_CODE
			.word	get_hit_snd,10,0,0

			.byte	ANI_RUNTOFRAME,73,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_PREP_XITION
			.word	8,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
tie3c_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_TIE_LG_V

			.byte	ANI_SKIPFRAMES,1

#			.byte	ANI_RUNTOEND,1
			.byte	ANI_RUNTOFRAME,73,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_PREP_XITION
			.word	3,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;; lineman/blocker push away

			.globl	pushlg1a_anim
			.globl	pushlg1v_anim
			.byte	SCRIPT
pushlg1a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_PUSHLG1A

			.byte	ANI_SKIPFRAMES,1

			.byte	ANI_INTSTREAM,15,9

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_INTSTREAM,5,3

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_CANBLOCK

 			.byte	ANI_INTSTREAM,49,31

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

			.byte	SCRIPT
pushlg1v_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_PUSHLG1V

			.byte	ANI_SKIPFRAMES,1

			.byte	ANI_INTSTREAM,20,12

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_INTSTREAM,4,2

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING

			.byte	ANI_SETVELS
			.float	0.0,.65,0.95

			.byte	ANI_SETFRICTION
			.float .055

			.byte	ANI_INTSTREAM,44,27

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_PREP_XITION
			.word	3,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;  LINEMAN/BLOCKER throws victim aside - to get by 'em

			.globl	p_asidea_anim
			.globl	p_asidev_anim
			.byte	SCRIPT
p_asidea_anim:
			.byte	ANI_ZEROVELS
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_CODE
			.word	rotate_anim,576,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_P_ASIDEA

			.byte	ANI_CODE
			.word	attack_snd,P_PUSH2_SND,0,0

			.byte	ANI_RUNTOFRAME,26,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_INTSTREAM,31,22
#			.byte	ANI_RUNTOEND,1

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
p_asidev_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_P_ASIDEV

			.byte	ANI_RUNTOFRAME,26,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_RUNTOFRAME,64,1

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END



#;;;;;;;;;;;;  LINEMAN/BLOCKER throws victim aside (FLIPPED) - to get by 'em

			.globl	p_asidfa_anim
			.globl	p_asidfv_anim
			.byte	SCRIPT
p_asidfa_anim:
			.byte	ANI_ZEROVELS
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_CODE
			.word	rotate_anim,-576,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_P_ASIDFA

			.byte	ANI_CODE
			.word	attack_snd,P_PUSH2_SND,0,0

			.byte	ANI_RUNTOFRAME,26,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_INTSTREAM,31,22
#			.byte	ANI_RUNTOEND,1

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
p_asidfv_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_P_ASIDFV

			.byte	ANI_RUNTOFRAME,26,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_RUNTOFRAME,64,1

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;; Player head butts the opponent

#			.globl	p_hdbuta_anim
#			.globl	p_hdbutv_anim
#			.byte	SCRIPT
#p_hdbuta_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_HDBUTA
#
#			.byte	ANI_ZEROVELS
#
#			.byte 	ANI_CODE
#			.word	attack_snd,P_PUSH2_SND,0,0
#
#			.byte	ANI_SKIPFRAMES,15
#
#			.byte	ANI_RUNTOFRAME,70,1
#
#			.byte 	ANI_CODE
#			.word	get_hit_snd,P_THRW_PLR_SP,0,0
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CODE
#			.word	maybe_taunt,0,0,0
#			
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
#
#
#			.byte	SCRIPT
#p_hdbutv_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#			
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_HDBUTV
#
#			.byte	ANI_SKIPFRAMES,15
#
#			.byte	ANI_RUNTOFRAME,65,1
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,P_HARD_HIT2_SND,0,0
#
#			.byte	ANI_SHAKER,3
#			.word	20
#
#			.byte	ANI_RUNTOFRAME,70,1
#
#			.byte	ANI_CODE
#			.word	player_down,0,0,0
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_RUNTOFRAME,173,1
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,-1,-1
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_TURNHACK
#			.word	512
#
#			.byte	ANI_CODE_BNZ
#			.word	plyr_have_ball,0,0,0,m_wob_fd_anim
#
#			.byte	ANI_INTTOSEQ,2
#			.word	SEQ_GET_UP_C
#			.byte	0
#			.byte	ANI_GOTO
#			.word	get_up_c_anim


#;;;;;;;;; Player head butts the opponent then drop kicks em

#			.globl	p_hdbut2a_anim
#			.globl	p_hdbut2v_anim
#			.byte	SCRIPT
#p_hdbut2a_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_HDBUTA
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_CODE
#			.word	attack_snd,P_PUSH2_SND,0,0
#
#			.byte	ANI_SKIPFRAMES,15
#
#			.byte	ANI_RUNTOFRAME,70,1
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,P_THRW_PLR_SP,0,0
#
#			.byte	ANI_RUNTOFRAME,83,1
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_SETVELS
#			.float	0.0,0.0,-0.15
#			.byte	ANI_SETFRICTION
#			.float .035
#
#			.byte	ANI_RUNTOFRAME,118,1
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_INTTOSEQ,14
#			.word	SEQ_T_DRPKIC
#			.byte	0
#
#			.byte	ANI_CODE
#			.word	attack_snd,P_DIVESTRT_SND,0,0
#
#			.byte	ANI_RUNTOFRAME,10,1
#
#			.byte	ANI_ATTACK_ON,ATTMODE_KICK,JOINT_RANKLE,JOINT_LANKLE,15
#			.float	1.0,1.0,4.0
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	1
#			.byte	ANI_SETYVEL
#			.float	.52
#			.byte	ANI_INTSTREAM,43,AA_TOGROUND
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,0
#
#			.byte	ANI_CODE
#			.word	dust_cloud,0,0,0
#
#			.byte	ANI_SETFRICTION
#			.float .084
#
#			.byte	ANI_RUNTOFRAME,64,1
#
#			.byte	ANI_INTSTREAM,95,65
#
##			.byte	ANI_PREP_XITION
##			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
#
#
#			.byte	SCRIPT
#p_hdbut2v_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#			
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_HDBUTV
#
#			.byte	ANI_SKIPFRAMES,15
#
#			.byte	ANI_RUNTOFRAME,65,1
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,P_HARD_HIT2_SND,0,0
#
#			.byte	ANI_SHAKER,3
#			.word	20
#
#			.byte	ANI_RUNTOFRAME,83,1
#
#			.byte	ANI_CODE
#			.word	player_down,0,0,0
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_RUNTOFRAME,150,1
#
#			.byte	ANI_RUNTOFRAME,173,1
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,-1,-1
#
#			.byte	ANI_SHAKER,3
#			.word	10
#			
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_TURNHACK
#			.word	512
#
#			.byte	ANI_CODE_BNZ
#			.word	plyr_have_ball,0,0,0,m_wob_fd_anim
#
#			.byte	ANI_CODE
#			.word	maybe_get_up_c_hurt,0,0,0
#			
#			.byte	ANI_INTTOSEQ,2
#			.word	SEQ_GET_UP_C
#			.byte	0
#			.byte	ANI_GOTO
#			.word	get_up_c_anim


#;;;;;;;;; Player head butts the opponent then upper-cuts em

#			.globl	p_hdbut3a_anim
#			.globl	p_hdbut3v_anim
#			.byte	SCRIPT
#p_hdbut3a_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_HDBUTA
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_CODE
#			.word	attack_snd,P_PUSH2_SND,0,0
#
#			.byte	ANI_SKIPFRAMES,15
#
#			.byte	ANI_RUNTOFRAME,70,1
#
#			.byte	ANI_CODE
#			.word	attack_snd,P_HARD_HIT1_SND,0,0
#
#			.byte	ANI_RUNTOFRAME,83,1
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_RUNTOFRAME,118,1
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_INTTOSEQ,11
#			.word	SEQ_T_UPRCUT
#			.byte	0
#
#			.byte	ANI_CODE
#			.word	attack_snd,P_DIVESTRT_SND,0,0
#
#			.byte	ANI_CODE
#			.word	face_carrier,0,0,0
#
#			.byte	ANI_INTSTREAM,4,12
#
#			.byte	ANI_CODE
#			.word	attack_snd,P_HARD_HIT1_SND,0,0
#
#			.byte	ANI_INTSTREAM,6,16
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CODE
#			.word	maybe_taunt,0,0,0
#			
##			.byte	ANI_PREP_XITION
##			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
#
#
#			.byte	SCRIPT
#p_hdbut3v_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#			
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_HDBUTV
#
#			.byte	ANI_SKIPFRAMES,15
#
#			.byte	ANI_RUNTOFRAME,65,1
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,P_HARD_HIT2_SND,0,0
#
#			.byte	ANI_SHAKER,3
#			.word	20
#
#			.byte	ANI_RUNTOFRAME,83,1
#
#			.byte	ANI_CODE
#			.word	player_down,0,0,0
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_SETVELS
#			.float	0.0,0.0,-0.54
#			.byte	ANI_SETFRICTION
#			.float .020
#
#			.byte	ANI_INTSTREAM,74,62
#
## go into upper cut hit reaction
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_H_UPRCUT
#
#			.byte	ANI_CODE
#			.word	rotate_anim,512,0,0
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,0,-1,0
#
#			.byte	ANI_SKIPFRAMES,5
#
#			.byte	ANI_SET_FACE_REL_VEL
#			.float	.55
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING
#			.byte	ANI_SETYVEL
#			.float	1.7
#			.byte	1
#			.byte	ANI_INTSTREAM,34,AA_TOGROUND
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,-1
#
#			.byte	ANI_CODE
#			.word	dust_cloud,0,0,0
#
#			.byte	ANI_SHAKER,4
#			.word	20
#
#			.byte	ANI_SETFRICTION
#			.float	.075
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CODE_BNZ
#			.word	plyr_have_ball,0,0,0,m_wob_fu_anim
#
#			.byte	ANI_CODE
#			.word	maybe_get_up_b_hurt,0,0,0
#			
#			.byte	ANI_INTTOSEQ,4
#			.word	SEQ_GET_UP_B
#			.byte	0
#			.byte	ANI_GOTO
#			.word	get_up_b_anim
#			.byte	ANI_END

#;;;;;;;;; Player lifts opponent over head and throws him down

			.globl	p_thrw_a_anim
			.globl	p_thrw_v_anim
			.byte	SCRIPT
p_thrw_a_anim:
			.byte	ANI_ZEROVELS
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_P_THRW_A

			.byte	ANI_CODE
			.word	get_hit_snd,P_THRW_PLR_SP,0,0

			.byte	ANI_INTSTREAM,48,34

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_INTSTREAM,88,55

			.byte	ANI_TURNHACK
			.word	512
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
p_thrw_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_P_THRW_V

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	ANI_INTSTREAM,48,34

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_INTSTREAM,4,2

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,-1,-1

			.byte	ANI_SHAKER,4
			.word	20

			.byte	ANI_INTSTREAM,22,13

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

			.byte	ANI_CODE
			.word	maybe_get_up_c_hurt,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_GU_C2
			.byte	0
			
			.byte	ANI_GOTO
			.word	get_up_c_anim


#;;;;;;;;; Player lifts opponent and shakes em and throws him down

#			.globl	p_fshk_a_anim
#			.globl	p_fshk_v_anim
#			.byte	SCRIPT
#p_fshk_a_anim:
#			.byte	ANI_ZEROVELS
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_FSHK_A
#
#			.byte	ANI_RUNTOFRAME,6,1
#
#			.byte	ANI_CODE
#			.word	attack_snd,78,0,0
#
#			.byte	ANI_RUNTOFRAME,42,1
#
#			.byte	ANI_CODE
#			.word	attack_snd,P_RIP_LIMP_SP,0,0
#
#			.byte	ANI_RUNTOFRAME,139,1
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_TURNHACK
#			.word	512
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
#
#
#			.byte	SCRIPT
#p_fshk_v_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#			
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_FSHK_V
#
#			.byte	ANI_RUNTOFRAME,90,1
#
#			.byte	ANI_CODE
#			.word	player_down,0,0,0
#
#			.byte	ANI_RUNTOFRAME,139,1
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_SETVELS
#			.float	0.0,0.0,-.55
#
#			.byte	ANI_RUNTOFRAME,149,1
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,-1,-1
#
#			.byte	ANI_SHAKER,4
#			.word	20
#
#			.byte	ANI_SETYVEL
#			.float	.80
#
#			.byte	ANI_RUNTOFRAME,177,1
#
#			.byte	ANI_SETFRICTION
#			.float	.040
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,0
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CODE_BNZ
#			.word	plyr_have_ball,0,0,0,m_wob_fd_anim
#
##			.byte	ANI_CODE_BNZ
##			.word	maybe_dizzy,0,0,0,get_up_c_dizzy_anim
#			.byte	ANI_CODE
#			.word	maybe_get_up_c_hurt,0,0,0
#			
#			.byte	ANI_INTTOSEQ,4
#			.word	SEQ_GET_UP_C
#			.byte	0
#			.byte	ANI_GOTO
#			.word	get_up_c_anim


#;;;;;;;;; Player lifts opponent and throws him down (one handed) with bounce

#			.globl	p_thrw2_a_anim
#			.globl	p_thrw2_v_anim
#			.byte	SCRIPT
#p_thrw2_a_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_FSHK_A
#
#			.byte	ANI_CODE
#			.word	attack_snd,78,0,0
#
#			.byte	ANI_SKIPFRAMES,117
#
#			.byte	ANI_SETFRICTION
#			.float .25
#
#			.byte	ANI_SETYVEL
#			.float	.6
#
#			.byte	ANI_RUNTOFRAME,139,1
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_RUNTOFRAME,146,1
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_TURNHACK
#			.word	512
#			
#			.byte	ANI_CODE
#			.word	maybe_taunt,0,0,0
#			
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
#
#
#			.byte	SCRIPT
#p_thrw2_v_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_FSHK_V
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#			
#			.byte	ANI_SKIPFRAMES,117
#
#			.byte	ANI_RUNTOFRAME,139,1
#
#			.byte	ANI_CODE
#			.word	player_down,0,0,0
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_SETVELS
#			.float	0.0,0.0,-.55
#
#			.byte	ANI_RUNTOFRAME,149,1
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,-1,-1
#
#			.byte	ANI_SHAKER,4
#			.word	20
#
#			.byte	ANI_SETYVEL
#			.float	.95
#
#			.byte	ANI_RUNTOFRAME,177,1
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,0
#
#			.byte	ANI_SETFRICTION
#			.float	.040
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CODE_BNZ
#			.word	plyr_have_ball,0,0,0,m_wob_fd_anim
#
##			.byte	ANI_CODE_BNZ
##			.word	maybe_dizzy,0,0,0,get_up_c_dizzy_anim
#			.byte	ANI_CODE
#			.word	maybe_get_up_c_hurt,0,0,0
#
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_INTTOSEQ,6
#			.word	SEQ_M_GU_C2
#			.byte	0
#
#			.byte	ANI_GOTO
#			.word	get_up_c_anim
#

#;;;;;;;;; Player lifts opponent and throws him down (one handed)

#			.globl	p_thrw3_a_anim
#			.globl	p_thrw3_v_anim
#			.byte	SCRIPT
#p_thrw3_a_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_FSHK_A
#
#			.byte	ANI_CODE
#			.word	attack_snd,78,0,0
#
#			.byte	ANI_SKIPFRAMES,127
#
#			.byte	ANI_SETFRICTION
#			.float	.25
#
#			.byte	ANI_RUNTOFRAME,139,1
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_TURNHACK
#			.word	512
#			
#			.byte	ANI_CODE
#			.word	maybe_taunt,0,0,0
#			
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
#
#
#			.byte	SCRIPT
#p_thrw3_v_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#	.if DOSETCAM
#			.byte	ANI_CODE_BZ
#			.word	plyr_have_ball,0,0,0,psv_nocam8
#			
#			.byte	ANI_SETCAMBOT
#			.word	csi_rs_suplex
#psv_nocam8:
#	.endif
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_FSHK_V
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#			
#			.byte	ANI_SKIPFRAMES,127
#
#			.byte	ANI_RUNTOFRAME,139,1
#
#			.byte	ANI_CODE
#			.word	player_down,0,0,0
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_SETVELS
#			.float	0.0,0.0,-.55
#
#			.byte	ANI_RUNTOFRAME,149,1
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,-1,-1
#
#			.byte	ANI_SHAKER,4
#			.word	20
#
#			.byte	ANI_SETYVEL
#			.float	.95
#
#			.byte	ANI_RUNTOFRAME,177,1
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,0
#
#			.byte	ANI_SETFRICTION
#			.float .040
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CODE_BNZ
#			.word	plyr_have_ball,0,0,0,m_wob_fd_anim
#
##			.byte	ANI_CODE_BNZ
##			.word	maybe_dizzy,0,0,0,get_up_c_dizzy_anim
#			.byte	ANI_CODE
#			.word	maybe_get_up_c_hurt,0,0,0
#			
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_INTTOSEQ,6
#			.word	SEQ_M_GU_C2
#			.byte	0
#
#			.byte	ANI_GOTO
#			.word	get_up_c_anim


#;;;;;;;;; Player slams opponent to ground grabbing facemask

			.globl	p_fslm_a_anim
			.globl	p_fslm_v_anim
			.byte	SCRIPT
p_fslm_a_anim:
# FIX!!!  Don't zerovel, just cut in half the attackers running vels and let move happen
			.byte	ANI_ZEROVELS
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_P_FSLM_A

			.byte	ANI_CODE
			.word	attack_snd,65,0,0

			.byte	ANI_SKIPFRAMES,6

			.byte	ANI_SETYVEL
			.float	1.3

			.byte	ANI_RUNTOFRAME,38,1

			.byte	ANI_INTSTREAM,20,7

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_INTSTREAM,70,50

			.byte	ANI_TURNHACK
			.word	512
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
p_fslm_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_P_FSLM_V

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0
			
			.byte	ANI_SKIPFRAMES,6
			.byte	ANI_RUNTOFRAME,38,1

			.byte	ANI_INTSTREAM,20,7

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_SHAKER,3
			.word	20

			.byte	1,1,1

			.byte	ANI_SETVELS
			.float	0.0,.85,-.06

			.byte	1

			.byte	ANI_INTSTREAM,30,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_ZEROVELS

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END


#;;;;;;;;;;;;  Player diving and grabbing victim puppet

			.globl	p_dive_a_anim
			.globl	p_dive_v_anim
			.byte	SCRIPT
p_dive_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_NEWSEQ
			.word	SEQ_P_DIVE_A
			.byte	1

			.byte	ANI_INTSTREAM,16,AA_TOGROUND
			
			.byte	ANI_SETFRICTION
			.float	.070

			.byte	ANI_RUNTOFRAME,42,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0
			
			.byte	ANI_INTSTREAM,1,7

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END


			.byte	SCRIPT
p_dive_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_NEWSEQ
			.word	SEQ_P_DIVE_V
			.byte	1

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0
			
#			.byte	ANI_CODE
#			.word	player_down,0,0,0
			
	.if DOSETCAM
# If victim has ball, do some camera shit - otherwise, skip it
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,cont_dive
			.byte	ANI_SETCAMBOT
			.word	csi_sn_diveattack
#			.word	csi_zm_divetack
	.endif

cont_dive:
			.byte	ANI_INTSTREAM,16,AA_TOGROUND

# Put new LOS here, perhaps too far
			.byte	ANI_CODE
			.word	player_down,0,0,0
			
			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_RUNTOFRAME,42,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0
			
			.byte	ANI_INTSTREAM,1,12

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

			.byte	ANI_CODE
			.word	maybe_get_up_c_hurt,0,0,0
			
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_GOTO
			.word	get_up_c_anim

#;;;;;;;;;;;;  Player diving at feet and grabbing victim

			.globl	p_ankl_a_anim
			.globl	p_ankl_v_anim
			.byte	SCRIPT
p_ankl_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_NEWSEQ
			.word	SEQ_P_ANKL_A

			.byte	ANI_SETFRICTION
			.float	.055

			.byte	ANI_RUNTOFRAME,40,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
p_ankl_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_NEWSEQ
			.word	SEQ_P_ANKL_V

			.byte	ANI_SETFRICTION
			.float	.055

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_RUNTOFRAME,40,1

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1
			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_INTSTREAM,4,12

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

#			.byte	ANI_CODE_BNZ
#			.word	maybe_dizzy,0,0,0,get_up_c_dizzy_anim
			.byte	ANI_CODE
			.word	maybe_get_up_c_hurt,0,0,0
			
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_GOTO
			.word	get_up_c_anim


#;;;;;;;;;;;;  Player diving at feet and grabbing victim #2

			.globl	p_ank2_a_anim
			.globl	p_ank2_v_anim
			.byte	SCRIPT
p_ank2_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_NEWSEQ
			.word	SEQ_P_ANK2_A

			.byte	ANI_SETFRICTION
			.float	.055

			.byte	ANI_RUNTOFRAME,54,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_GOTO
			.word	get_up_c_anim
			
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
p_ank2_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_NEWSEQ
			.word	SEQ_P_ANK2_V

			.byte	ANI_SETFRICTION
			.float	.055

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_RUNTOFRAME,54,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0
			
			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1
			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_RUNTOEND,1
			
			.byte	ANI_TURNHACK
#			.word	512
			.word	384
			
			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fu_anim

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_GET_UP_B
			.byte	0
			.byte	ANI_GOTO
			.word	get_up_b_anim
			.byte	ANI_END
			


#;;;;;;;;;;;;  Player diving at feet and grabbing victim #2 (but gets away)

#			.globl	p_ank2_brk_a_anim
#			.globl	p_ank2_brk_v_anim
#			.byte	SCRIPT
#p_ank2_brk_a_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_ANK2_A
#
#			.byte	ANI_SETFRICTION
#			.float	.055
#
#			.byte	ANI_RUNTOFRAME,43,1
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_GOTO
#			.word	get_up_c_anim
#			
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END


#			.byte	SCRIPT
#p_ank2_brk_v_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_BT_ANKLE
##			.word	SEQ_P_ANK2_V
#
#			.byte	ANI_SETFRICTION
#			.float	.055
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#
#			.byte	ANI_RUNTOFRAME,43,1
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#			
#			.byte	ANI_RUNTOEND,1
#			
##			.byte	ANI_TURNHACK
##			.word	384
#			
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END


			
#;;;;;;;;;;;;  Player grabs victim and drops 'em

			.globl	p_tkdn_a_anim
			.globl	p_tkdn_v_anim
			.byte	SCRIPT
p_tkdn_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

#			.byte	ANI_TURNHACK
#			.word	128
#			.word	64

			.byte	ANI_NEWSEQ
			.word	SEQ_P_TKDWNA
			.byte	ANI_SKIPFRAMES,1

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,2,0,0,side

			.byte	ANI_SETVELS
			.float	-.70,1.0,.70
			.byte	ANI_GOTO
			.word	cont

side:
			.byte	ANI_SETVELS
			.float	-.20,1.2,.70
cont:
			.byte	ANI_RUNTOFRAME,25,1

			.byte	ANI_SETYVEL
			.float	-1.20

			.byte	ANI_RUNTOFRAME,30,1

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_SHAKER,2
			.word	10

			.byte	ANI_SETFRICTION
			.float .060

			.byte	1,1,1,1,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_RUNTOFRAME,73,1

			.byte	ANI_CODE_BNZ
			.word	plyr_rand,10,0,0,skip
# 1 out of five, play snd call

			.byte	ANI_CODE
			.word	attack_snd,P_OUTTA_MY_FACE_SP,0,0
skip:

# Get him off from the collision box of grounded player
			.byte	ANI_SETVELS
			.float	0,.20,-.20

			.byte	ANI_RUNTOEND,1
			.byte	ANI_ZEROVELS

			.byte	ANI_TURNHACK
			.word	92
#			.word	256

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0
			
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
p_tkdn_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_NEWSEQ
			.word	SEQ_P_TKDWNV

	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam9
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex
psv_nocam9:
	.endif

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0
			
			.byte	ANI_SKIPFRAMES,1

			.byte	ANI_RUNTOFRAME,30,1

			.byte	ANI_CODE
			.word	grnd_hit_snd,0,0,-1

			.byte	ANI_SETFRICTION
			.float	.043

			.byte	ANI_RUNTOFRAME,35,1

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_RUNTOFRAME,54,1

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fu_anim

			.byte	ANI_INTTOSEQ,8
			.word	SEQ_GET_UP_B
			.byte	0
			.byte	ANI_GOTO
			.word	get_up_b_anim


#;;;;;;;;;;;;  Player pile drives victim

#			.globl	p_pile_a_anim
#			.globl	p_pile_v_anim
#			.byte	SCRIPT
#p_pile_a_anim:
#			.byte	ANI_ZEROVELS
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER
#
##			.byte	ANI_CODE
##			.word	face_attacker,0,0,0
#
##			.byte	ANI_CODE
##			.word	rotate_anim,-256,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_PILE_A
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,P_THRW_PLR_SP,0,0
#
#			.byte	ANI_RUNTOFRAME,105,1
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CODE
#			.word	maybe_taunt,0,0,0
#			
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
#
#
#			.byte	SCRIPT
#p_pile_v_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_PILE_V
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,-1,0
#
#			.byte	ANI_RUNTOFRAME,105,1
#
#			.byte	ANI_CODE
#			.word	player_down,0,0,0
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,-1,-1
#
#			.byte	ANI_SHAKER,3
#			.word	20
#
#			.byte	ANI_RUNTOFRAME,115,1
#
#			.byte	ANI_SETYVEL
#			.float	.75
#			.byte	1
#			.byte	ANI_INTSTREAM,15,AA_TOGROUND
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,-1
#
#			.byte	ANI_SHAKER,3
#			.word	20
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_TURNHACK
#			.word	-128
#			
#			.byte	ANI_CODE_BNZ
#			.word	plyr_have_ball,0,0,0,m_wob_fd_anim
#
##			.byte	ANI_CODE_BNZ
##			.word	maybe_dizzy,0,0,0,get_up_c_dizzy_anim
#			.byte	ANI_CODE
#			.word	maybe_get_up_c_hurt,0,0,0
#			
#			.byte	ANI_GOTO
#			.word	get_up_b_anim
#			.byte	ANI_END


#;;;;;;;;;;;;  Player grabs victim and slams his face in the dirt

			.globl	p_face_a_anim
			.globl	p_face_v_anim
			.byte	SCRIPT
p_face_a_anim:
			.byte	ANI_ZEROVELS
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_P_FACE_A

			.byte	ANI_CODE
			.word	get_hit_snd,P_THRW_PLR_SP,0,0

			.byte	ANI_RUNTOFRAME,43,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	512
			
			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0
			
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
p_face_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_P_FACE_V

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	ANI_RUNTOFRAME,43,1

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_SHAKER,4
			.word	20

			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fu_anim

#			.byte	ANI_CODE_BNZ
#			.word	maybe_dizzy,0,0,0,get_up_b_dizzy_anim
			.byte	ANI_CODE
			.word	maybe_get_up_b_hurt,0,0,0
			
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_GET_UP_B
			.byte	0
			.byte	ANI_GOTO
			.word	get_up_b_anim


#;;;;;;;;;;;;  Player grabs victim from FRONT and suplexes him

#			.globl	p_supl_a_anim
#			.globl	p_supl_v_anim
#			.byte	SCRIPT
#p_supl_a_anim:
#			.byte	ANI_ZEROVELS
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER
#
#			.byte	ANI_TURNHACK
#			.word	512
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_SUPL_A
#
#			.byte	ANI_SKIPFRAMES,1
#
#			.byte	ANI_INTSTREAM,70,55
#
#			.byte	ANI_CODE
#			.word	attack_snd,P_KNEE_DUDE_SP,0,0
#
#			.byte	ANI_INTSTREAM,31,18
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_INTSTREAM,77,38
#
#			.byte	ANI_TURNHACK
#			.word	512
#
#			.byte	ANI_CODE
#			.word	maybe_taunt,0,0,0
#			
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
#
#
#			.byte	SCRIPT
#p_supl_v_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET
#
#	.if DOSETCAM
#			.byte	ANI_SETCAMBOT
#			.word	csi_rs_suplex
#	.endif
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_SUPL_V
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#	
#			.byte	ANI_SKIPFRAMES,1
#
#			.byte	ANI_INTSTREAM,70,55
#
#			.byte	ANI_INTSTREAM,31,18
#
#			.byte	ANI_CODE
#			.word	player_down,0,0,0
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,-1,-1
#			.byte	ANI_SHAKER,4
#			.word	12
#
#			.byte	1
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING
#
#			.byte	ANI_SET_FACE_REL_VEL
#			.float	-.50
#			.byte	ANI_SETYVEL
#			.float	.90
#			.byte	1
#			.byte	ANI_INTSTREAM,43,AA_TOGROUND
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,0
#
#			.byte	ANI_CODE
#			.word	dust_cloud,0,0,0
#
#			.byte	ANI_SHAKER,3
#			.word	10
#
#			.byte	ANI_SETFRICTION
#			.float	.037
#
#			.byte	ANI_INTSTREAM,15,10
#
#			.byte	ANI_CODE_BNZ
#			.word	plyr_have_ball,0,0,0,m_wob_fd_anim
#
##			.byte	ANI_CODE_BNZ
##			.word	maybe_dizzy,0,0,0,get_up_c_dizzy_anim
#			.byte	ANI_CODE
#			.word	maybe_get_up_c_hurt,0,0,0
#			
#			.byte	ANI_INTTOSEQ,4
#			.word	SEQ_GET_UP_C
#			.byte	0
#
#			.byte	ANI_GOTO
#			.word	get_up_c_anim
#			.byte	ANI_END



#;;;;;;;;;;;;  Player grabs victim from BEHIND and suplexes him

			.globl	p_suplb_a_anim
			.globl	p_suplb_v_anim
			.byte	SCRIPT
p_suplb_a_anim:
			.byte	ANI_ZEROVELS
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_SET_FACE_REL_VEL
			.float	.75

			.byte	ANI_SETFRICTION
			.float .022

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_SUPLEX_A

			.byte 	ANI_CODE
			.word	attack_snd,P_KNEE_DUDE_SP,0,0

#			.byte	ANI_RUNTOFRAME,59,1
			.byte	ANI_INTSTREAM,59,49

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_RUNTOEND,1
#			.byte	ANI_INTSTREAM,77,38

#			.byte	ANI_CODE
#			.word	maybe_taunt,0,0,0
			
			.byte	ANI_TURNHACK
			.word	512
	
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
p_suplb_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_SUPLEX_V

			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0
			
#			.byte	ANI_RUNTOFRAME,59,1
			.byte	ANI_INTSTREAM,59,49

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,-1,-1
			.byte	ANI_SHAKER,4
			.word	10
			.byte	ANI_CODE
			.word	dust_cloud,0,0,0


			.byte	1
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING

			.byte	ANI_SET_FACE_REL_VEL
			.float	-.50
			.byte	ANI_SETYVEL
			.float	.90
			.byte	1
			.byte	ANI_INTSTREAM,43,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float	.037

			.byte	ANI_RUNTOEND,1
#			.byte	ANI_INTSTREAM,15,15

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

			.byte	ANI_CODE
			.word	maybe_get_up_c_hurt,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			
			.byte	ANI_INTTOSEQ,8
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END


#;;;;;;;;;;;;  Player grabs victim by face and knees em TWICE and then kicks em

#			.globl	p_knee_a_anim
#			.globl	p_knee_v_anim
#			.byte	SCRIPT
#p_knee_a_anim:
#			.byte	ANI_ZEROVELS
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_KNEE_A
#
#			.byte	ANI_SKIPFRAMES,1
#
#			.byte	ANI_RUNTOFRAME,17,1
#			.byte	ANI_CODE
#			.word	attack_snd,P_KNEE_DUDE_SP,0,0
#
#			.byte	ANI_RUNTOFRAME,65,1
#			.byte	ANI_CODE
#			.word	attack_snd,P_KNEE_DUDE_SP,0,0
#
#			.byte	ANI_RUNTOFRAME,124,1
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CODE
#			.word	attack_snd,P_OUTTA_MY_FACE_SP,0,0
#
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
#
#
#			.byte	SCRIPT
#p_knee_v_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_KNEE_V
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#			
#			.byte	ANI_SKIPFRAMES,1
#
#			.byte	ANI_RUNTOFRAME,22,1
#			
#			.byte	ANI_CODE
#			.word	player_down,0,0,0
#			
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#			.byte	ANI_SHAKER,2
#			.word	10
#
#			.byte	ANI_RUNTOFRAME,72,1
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#			.byte	ANI_SHAKER,2
#			.word	10
#
#			.byte	ANI_RUNTOFRAME,124,1
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_RUNTOFRAME,125,1
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#			.byte	ANI_SHAKER,2
#			.word	10
#
#			.byte	1
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_REACTING
#
#			.byte	ANI_CODE
#			.word	rotate_anim,512,0,0
#
#			.byte	ANI_SETVELS
#			.float	0.0,0.0,-.75
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_H_HELI1
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#
#			.byte	1
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING
#			.byte	ANI_SETYVEL
#			.float	1.6
#			.byte	1
#			.byte	ANI_INTSTREAM,57,AA_TOGROUND
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,-1,-1
#
#			.byte	ANI_CODE
#			.word	dust_cloud,0,0,0
#
#			.byte	ANI_SETFRICTION
#			.float	.035
#
#			.byte	ANI_SHAKER,3
#			.word	10
#
#			.byte	ANI_INTSTREAM,19,10
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING
#			.byte	ANI_SETVELS
#			.float	0.0,.8,-0.50
#			.byte	1
#			.byte	ANI_INTSTREAM,15,AA_TOGROUND
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,0
#
#			.byte	ANI_SETFRICTION
#			.float	.055
#
#			.byte	ANI_TURNHACK
#			.word	512
#
#			.byte	ANI_CODE_BNZ
#			.word	plyr_have_ball,0,0,0,m_wob_fd_anim
#
##			.byte	ANI_CODE_BNZ
##			.word	maybe_dizzy,0,0,0,get_up_c_dizzy_anim
#			.byte	ANI_CODE
#			.word	maybe_get_up_c_hurt,0,0,0
#			
#		#do get-up here
#			.byte	ANI_INTTOSEQ,6
#			.word	SEQ_GET_UP_C
#			.byte	10
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END



#;;;;;;;;;;;;  Player grabs victim by face and knees em  ONCE and then kicks em

#			.globl	p_knee2_a_anim
#			.globl	p_knee2_v_anim
#			.byte	SCRIPT
#p_knee2_a_anim:
#			.byte	ANI_ZEROVELS
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_KNEE_A
#
#			.byte	ANI_SKIPFRAMES,56
#
#			.byte	ANI_RUNTOFRAME,65,1
#			.byte	ANI_CODE
#			.word	attack_snd,P_KNEE_DUDE_SP,0,0
#
#			.byte	ANI_RUNTOFRAME,69,1
#			.byte	ANI_SETYVEL
#			.float .68
#
#			.byte	ANI_RUNTOFRAME,118,1
#			.byte	ANI_CODE
#			.word	attack_snd,P_OUTTA_MY_FACE_SP,0,0
#
#			.byte	ANI_RUNTOFRAME,124,1
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CODE
#			.word	maybe_taunt,0,0,0
#			
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
#
#
#			.byte	SCRIPT
#p_knee2_v_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_KNEE_V
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#			
#			.byte	ANI_SKIPFRAMES,56
#
#			.byte	ANI_RUNTOFRAME,72,1
#			
#			.byte	ANI_CODE
#			.word	player_down,0,0,0
#			
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#			.byte	ANI_SHAKER,2
#			.word	10
#
#			.byte	ANI_RUNTOFRAME,124,1
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_RUNTOFRAME,125,1
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#			.byte	ANI_SHAKER,2
#			.word	10
#
#			.byte	1
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_REACTING
#
#			.byte	ANI_CODE
#			.word	rotate_anim,512,0,0
#
#			.byte	ANI_SETVELS
#			.float	0.0,0.0,-.85
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_H_FALNK1
#
#			.byte	ANI_SKIPFRAMES,3
#
##			.byte	ANI_CODE
##			.word	blood_spray,1,1,1
#
#			.byte	1,1
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING
#
#			.byte	ANI_SETYVEL
#			.float	1.2
#			.byte	1
#			.byte	ANI_INTSTREAM,43,AA_TOGROUND
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,-1,0
#
#			.byte	ANI_CODE
#			.word	dust_cloud,0,0,0
#
#			.byte	ANI_SHAKER,3
#			.word	10
#
#			.byte	ANI_SETFRICTION
#			.float	.037
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CODE_BNZ
#			.word	plyr_have_ball,0,0,0,m_wob_fd_anim
#
##			.byte	ANI_CODE_BNZ
##			.word	maybe_dizzy,0,0,0,get_up_c_dizzy_anim
#			.byte	ANI_CODE
#			.word	maybe_get_up_c_hurt,0,0,0
#			
#			.byte	ANI_INTTOSEQ,8
#			.word	SEQ_GET_UP_C
#			.byte	0
#
#			.byte	ANI_GOTO
#			.word	get_up_c_anim
#			.byte	ANI_END


#;;;;;;;;;;;;  Player grabs victim by face and knees em ONCE and then he flys away

#			.globl	p_knee3_a_anim
#			.globl	p_knee3_v_anim
#			.byte	SCRIPT
#p_knee3_a_anim:
#			.byte	ANI_ZEROVELS
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_KNEE_A
#
#			.byte	ANI_SKIPFRAMES,56
#
#			.byte	ANI_RUNTOFRAME,65,1
#			.byte	ANI_CODE
#			.word	attack_snd,P_KNEE_DUDE_SP,0,0
#
#			.byte	ANI_RUNTOFRAME,69,1
#			.byte	ANI_SETYVEL
#			.float	.68
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_RUNTOFRAME,103,1
#
#			.byte	ANI_CODE
#			.word	maybe_taunt,0,0,0
#			
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
#
#
#			.byte	SCRIPT
#p_knee3_v_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_KNEE_V
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#			
#			.byte	ANI_SKIPFRAMES,56
#
#			.byte	ANI_RUNTOFRAME,69,1
#			
#			.byte	ANI_CODE
#			.word	player_down,0,0,0
#			
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_RUNTOFRAME,72,1
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#			.byte	ANI_SHAKER,2
#			.word	10
#
#			.byte	ANI_RUNTOFRAME,75,1
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_REACTING
#
#			.byte	ANI_CODE
#			.word	rotate_anim,512,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_H_UPRCUT
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,-1,0
#
#			.byte	ANI_SKIPFRAMES,5
#
#			.byte	ANI_SETVELS
#			.float	0.0,0.0,-.65
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING
#			.byte	ANI_SETYVEL
#			.float	1.5
#			.byte	1
#			.byte	ANI_INTSTREAM,34,AA_TOGROUND
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,-1
#
#			.byte	ANI_CODE
#			.word	dust_cloud,0,0,0
#
#			.byte	ANI_SHAKER,3
#			.word	20
#
#			.byte	ANI_SETFRICTION
#			.float .055
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CODE_BNZ
#			.word	plyr_have_ball,0,0,0,m_wob_fu_anim
#
##			.byte	ANI_CODE_BNZ
##			.word	maybe_dizzy,0,0,0,get_up_b_dizzy_anim
#			.byte	ANI_CODE
#			.word	maybe_get_up_b_hurt,0,0,0
#			
#			.byte	ANI_INTTOSEQ,4
#			.word	SEQ_GET_UP_B
#			.byte	0
#			.byte	ANI_GOTO
#			.word	get_up_b_anim
#			.byte	ANI_END
			
			
# FIX!!!  Do alternate versions of this move - faster, more spins, etc.
#;;;;;;;;;;;;  Player grabs victim and spins him down to ground

			.globl	p_spin_a_anim
			.globl	p_spin_v_anim
			.byte	SCRIPT
p_spin_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_P_SPIN_A

			.byte	ANI_SETVELS
			.float	0.0,0.0,0.9

			.byte	ANI_SETFRICTION
			.float	.015

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	1
			
			.byte	ANI_INTSTREAM,86,55

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_INTSTREAM,20,13

#			.byte	ANI_CODE
#			.word	maybe_taunt,0,0,0

			.byte	ANI_TURNHACK
			.word	-128

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_GOTO
			.word	get_up_c_anim
			
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
p_spin_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_CODE
			.word	face_attacker,0,0,0


	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex
psv_nocam:
	.endif

			.byte	ANI_NEWSEQ
			.word	SEQ_P_SPIN_V

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0
			
			.byte	1
			
#			.byte	ANI_CODE
#			.word	player_down,0,0,0
			
			.byte	ANI_INTSTREAM,85,55

# Put new LOS here, perhaps too far
			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1
			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_INTSTREAM,39,25

			.byte	ANI_TURNHACK
			.word	-128

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

#			.byte	ANI_CODE_BNZ
#			.word	maybe_dizzy,0,0,0,get_up_c_dizzy_anim
			.byte	ANI_CODE
			.word	maybe_get_up_c_hurt,0,0,0
			
			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END

			
#;;;;;;;;;;;;  Player grabs victim and spins him down to ground

			.globl	p_csid_a_anim
			.globl	p_csid_v_anim
			.byte	SCRIPT
p_csid_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_P_CSID_A
			.byte 1

			.byte	ANI_SETVELS
			.float	0.0,0.0,.85
						
			.byte	ANI_CODE
			.word	attack_snd,-1,0,0

			.byte	ANI_INTSTREAM,118,70
			
			.byte	ANI_SETFRICTION
			.float	.070
			
			.byte	ANI_INTSTREAM,11,6

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_GOTO
			.word	get_up_c_anim
			
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
p_csid_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_CODE
			.word	face_attacker,0,0,0


	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam2
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex
psv_nocam2:
	.endif

			.byte	ANI_NEWSEQ
			.word	SEQ_P_CSID_V
			.byte	1

#			.byte	ANI_CODE
#			.word	player_down,0,0,0
			
			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0
			
			.byte	ANI_INTSTREAM,118,70

# Put new LOS here, perhaps too far
			.byte	ANI_CODE
			.word	player_down,0,0,0
			
			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1
			.byte	ANI_SHAKER,3
			.word	10
			
			.byte	ANI_INTSTREAM,11,6

			.byte	ANI_CODE
			.word	release_puppet,0,0,0
			
			.byte	ANI_SETVELS
			.float	0.0,0.60,.95
			
			.byte	ANI_SETFRICTION
			.float .025
			
			.byte	ANI_INTSTREAM,43,24

			.byte	ANI_TURNHACK
			.word	512
			
			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

			.byte	ANI_TURNHACK
			.word	0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_GU_C2
			.byte	0
			
			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END
			
			
#;;;;;;;;;;;;  Player grabs victim from behind and brings em down

			.globl	p_btac_a_anim
			.globl	p_btac_v_anim
			.byte	SCRIPT
p_btac_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_CODE
			.word	face_attacker,0,0,0
			
			.byte	ANI_NEWSEQ
			.word	SEQ_P_BTAC_A

			.byte	ANI_RUNTOFRAME,4,1

			.byte	ANI_SETFRICTION
			.float	0.035

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_SETYVEL
			.float	.84
			.byte	1
			.byte	ANI_INTSTREAM,40,AA_TOGROUND

			.byte	ANI_SHAKER,2
			.word	10

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_NOCATCH
			
			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END



			.byte	SCRIPT
p_btac_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_P_BTAC_V

			.byte	ANI_RUNTOFRAME,4,1

			.byte	ANI_SETFRICTION
			.float	0.035

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	1

			.byte	ANI_INTSTREAM,41,27

			.byte	ANI_RUNTOFRAME,45,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_NOCATCH
			
			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

#			.byte	ANI_CODE_BNZ
#			.word	maybe_dizzy,0,0,0,get_up_c_dizzy_anim
			.byte	ANI_CODE
			.word	maybe_get_up_c_hurt,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END


#;;;;;;;;;;;;  Player grabs victim from front and brings em down

			.globl	p_ftac_a_anim
			.globl	p_ftac_v_anim
			.byte	SCRIPT
p_ftac_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

	#!!!FIX
#			.byte	ANI_SET_FACE_REL_VEL
#			.float	-1.0
			
			.byte	ANI_TURNHACK
			.word	512
			
			.byte	ANI_NEWSEQ
			.word	SEQ_P_FTAC_A
			.byte	1

			.byte	ANI_INTSTREAM,31,AA_TOGROUND

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SETFRICTION
			.float .063

#			.byte	ANI_RUNTOFRAME,50,1
			.byte	ANI_RUNTOFRAME,49,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_NOCYL|MODE_NOCATCH

#			.byte	ANI_INTSTREAM,50,25
			.byte	ANI_INTSTREAM,51,25

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END



			.byte	SCRIPT
p_ftac_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0
			
			.byte	ANI_NEWSEQ
			.word	SEQ_P_FTAC_V
			.byte	1

			.byte	ANI_INTSTREAM,31,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,P_GRND_HIT1_SND,0,-1
			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SETFRICTION
			.float .063

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_NOCYL|MODE_NOCATCH

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fu_anim

#			.byte	ANI_CODE_BNZ
#			.word	maybe_dizzy,0,0,0,get_up_b_dizzy_anim
			.byte	ANI_CODE
			.word	maybe_get_up_b_hurt,0,0,0
			
			.byte	ANI_GOTO
			.word	get_up_b_anim
			.byte	ANI_END

			
#;;;;;;;;;;;;  Player grabs victim from behind and brings em down

			.globl	p_wtac_a_anim
			.globl	p_wtac_v_anim
			.byte	SCRIPT
p_wtac_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
			
			.byte	ANI_NEWSEQ
			.word	SEQ_P_WTAC_A

#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_SETYVEL
			.float	.35
#			.float	.65
			.byte	1
			.byte	ANI_INTSTREAM,29,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float	0.075
			
			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_RUNTOFRAME,80,1
			
			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING

			.byte	ANI_INTSTREAM,69,35
#			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0
			
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END
			
#			.byte	ANI_GOTO
#			.word	breath_anim
#			.byte	ANI_END


			.byte	SCRIPT
p_wtac_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_REACTING|MODE_PUPPET

#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0
			
			.byte	ANI_NEWSEQ
			.word	SEQ_P_WTAC_V

			.byte	1
			.byte	ANI_INTSTREAM,29,AA_TOGROUND

			.byte	ANI_CODE
			.word	player_down,0,0,0
			
			.byte	ANI_RUNTOFRAME,80,1
			
			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_NOCATCH
			
			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

#			.byte	ANI_CODE_BNZ
#			.word	maybe_dizzy,0,0,0,get_up_c_dizzy_anim
			.byte	ANI_CODE
			.word	maybe_get_up_c_hurt,0,0,0

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END
			
			
			

#;;;;;;;;;;;;  Player grabs victim by face and knees em ONCE and then he flys away

#			.globl	p_hdlcka_anim
#			.globl	p_hdlckv_anim
#			.byte	SCRIPT
#p_hdlcka_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_HDLCKA
#
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_INTSTREAM,61,40
#			.byte	ANI_CODE
#			.word	attack_snd,P_KNEE_DUDE_SP,0,0
#
#			.byte	ANI_INTSTREAM,10,6
#			.byte 	ANI_CODE
#			.word	get_hit_snd,P_HARD_HIT2_SND,0,0
#			.byte	ANI_SHAKER,2
#			.word	10
#
#			.byte	ANI_INTSTREAM,49,31
#			.byte	ANI_CODE
#			.word	attack_snd,P_KNEE_DUDE_SP,0,0
#
#			.byte	ANI_INTSTREAM,10,6
#			.byte 	ANI_CODE
#			.word	get_hit_snd,P_HARD_HIT2_SND,0,0
#			.byte	ANI_SHAKER,2
#			.word	10
#
#			.byte	ANI_INTSTREAM,47,30
#			.byte	ANI_CODE
#			.word	attack_snd,P_KNEE_DUDE_SP,0,0
#
#			.byte	ANI_INTSTREAM,10,6
#			.byte 	ANI_CODE
#			.word	get_hit_snd,P_HARD_HIT2_SND,0,0
#			.byte	ANI_SHAKER,3
#			.word	10
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_INTSTREAM,43,22
#
#			.byte	ANI_CODE
#			.word	maybe_taunt,0,0,0
#			
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
#
#
#			.byte	SCRIPT
#p_hdlckv_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET
#
#			.byte	ANI_CODE
#			.word	face_attacker,0,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_HDLCKV
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
#			
#			.byte	ANI_INTSTREAM,61,40
#			
#			.byte	ANI_CODE
#			.word	player_down,0,0,0
#			
#			.byte	ANI_INTSTREAM,10,6
#
#			.byte	ANI_INTSTREAM,49,31
#			.byte	ANI_INTSTREAM,10,6
#
#			.byte	ANI_INTSTREAM,47,30
#			.byte	ANI_INTSTREAM,11,6
#
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING
#
#			.byte	ANI_CODE
#			.word	rotate_anim,512,0,0
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_H_UPRCUT
#
#			.byte	ANI_CODE
#			.word	get_hit_snd,0,P_GET_THRWN1_SP,0
#
#			.byte	ANI_SKIPFRAMES,5
#
#			.byte	ANI_SET_FACE_REL_VEL
#			.float	.65
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING
#			.byte	ANI_SETYVEL
#			.float	1.5
#			.byte	1
#			.byte	ANI_INTSTREAM,34,AA_TOGROUND
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,-1
#
#			.byte	ANI_CODE
#			.word	dust_cloud,0,0,0
#
#			.byte	ANI_SHAKER,3
#			.word	20
#
#			.byte	ANI_SETFRICTION
#			.float .065
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CODE_BNZ
#			.word	plyr_have_ball,0,0,0,m_wob_fu_anim
#
##			.byte	ANI_CODE_BNZ
##			.word	maybe_dizzy,0,0,0,get_up_b_dizzy_anim
#			.byte	ANI_CODE
#			.word	maybe_get_up_b_hurt,0,0,0
#			
#			.byte	ANI_INTTOSEQ,4
#			.word	SEQ_GET_UP_B
#			.byte	0
#			.byte	ANI_GOTO
#			.word	get_up_b_anim
#			.byte	ANI_END

#;;;;;;;;;;;;  Player rolls on ground and tackles dude

			.globl	p_frol_a_anim
			.globl	p_frol_v_anim
			.byte	SCRIPT
p_frol_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_NEWSEQ
			.word	SEQ_P_FROL_A

			.byte	ANI_CODE
			.word	attack_snd,-1,0,0
			
			.byte	ANI_SETFRICTION
			.float	.055
			
			.byte	ANI_RUNTOFRAME,14,1
			
			.byte	ANI_RUNTOFRAME,75,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_INTSTREAM,100,70
			.byte	ANI_INTSTREAM,36,20
#			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	512
			
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
p_frol_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET

	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam12
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex
psv_nocam12:
	.endif

			.byte	ANI_NEWSEQ
			.word	SEQ_P_FROL_V

			.byte	ANI_SETFRICTION
			.float	.055
			
			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,-1,-1

			.byte	ANI_RUNTOFRAME,8,1
			
#			.byte	ANI_CODE
#			.word	get_hit_snd,-1,0,0
			
			.byte	ANI_RUNTOFRAME,14,1
			
			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0
			.byte	ANI_CODE
			.word	dust_cloud,0,0,0
			.byte	ANI_SHAKER,3
			.word	10
			
			.byte	ANI_CODE
			.word	player_down,0,0,0
			
			.byte	ANI_RUNTOFRAME,75,1

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_INTSTREAM,66,50
			
			.byte	ANI_TURNHACK
			.word	512
			
			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END
			
			
#;;;;;;;;;;;;  Player rolls on ground and tackles dude

			.globl	p_armg_a_anim
			.globl	p_armg_v_anim
			.byte	SCRIPT
p_armg_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_NEWSEQ
			.word	SEQ_P_ARMG_A

			.byte	ANI_ZEROVELS
			
			.byte	ANI_CODE
			.word	attack_snd,-1,0,0
			
			.byte	ANI_RUNTOFRAME,34,1
			
			.byte	ANI_CODE
			.word	attack_snd,P_THRW_PLR_SP,0,0
			
			.byte	ANI_RUNTOFRAME,61,1
			
			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
p_armg_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET


	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam3
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex
psv_nocam3:
	.endif

			.byte	ANI_NEWSEQ
			.word	SEQ_P_ARMG_V

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0
			
			.byte	ANI_RUNTOFRAME,56,1
			
			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0
			.byte	ANI_CODE
			.word	dust_cloud,0,0,0
			.byte	ANI_SHAKER,3
			.word	10
			
			.byte	ANI_RUNTOFRAME,61,1
			
			.byte	ANI_CODE
			.word	player_down,0,0,0
			
			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_RUNTOEND,1
			
			.byte	ANI_TURNHACK
			.word	606
						
			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END
			
			
#;;;;;;;;;;;;  Player rolls on ground and tackles dude

			.globl	p_armg2_a_anim
			.globl	p_armg2_v_anim
			.byte	SCRIPT
p_armg2_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_NEWSEQ
			.word	SEQ_P_ARMG_A

			.byte	ANI_ZEROVELS
			
			.byte	ANI_CODE
			.word	attack_snd,-1,0,0
			
			.byte	ANI_RUNTOFRAME,34,1
			
			.byte	ANI_CODE
			.word	attack_snd,P_THRW_PLR_SP,0,0
			
			.byte	ANI_RUNTOFRAME,42,1
			
			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
p_armg2_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET


#	.if DOSETCAM
#			.byte	ANI_CODE_BZ
#			.word	plyr_have_ball,0,0,0,psv_nocam3
#			
#			.byte	ANI_SETCAMBOT
#			.word	csi_rs_suplex
#psv_nocam3:
#	.endif

			.byte	ANI_NEWSEQ
			.word	SEQ_P_ARMG_V

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0
			
			.byte	ANI_RUNTOFRAME,42,1
			
			.byte	ANI_CODE
			.word	release_puppet,0,0,0
			
			.byte	ANI_CODE
			.word	player_down,0,0,0
			
			.byte	ANI_CODE
			.word	get_hit_snd,P_GET_THRWN1_SP,0,0
			
			.byte	ANI_SETVELS
			.float	0.0,1.2,0.90
			.byte 1

			.byte	ANI_INTSTREAM,18,AA_TOGROUND
			
			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0
			.byte	ANI_CODE
			.word	dust_cloud,0,0,0
			.byte	ANI_SHAKER,3
			.word	10
			
			.byte	ANI_SETFRICTION
			.float .045

			.byte	ANI_RUNTOEND,1
			
			.byte	ANI_TURNHACK
			.word	606
						
			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END
			




#;;;;;;;;;;;;  Player rolls on ground and tackles dude

			.globl	p_armg3_a_anim
			.globl	p_armg3_v_anim
			.byte	SCRIPT
p_armg3_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_NEWSEQ
			.word	SEQ_P_ARMG_A

			.byte	ANI_ZEROVELS
			
			.byte	ANI_CODE
			.word	attack_snd,-1,0,0
			
			.byte	ANI_RUNTOFRAME,34,1
			
			.byte	ANI_CODE
			.word	attack_snd,P_THRW_PLR_SP,0,0
			
			.byte	ANI_RUNTOFRAME,42,1
			
			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
p_armg3_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET



#; Test new camera stuff
	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam_1
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex
psv_nocam_1:
	.endif





			.byte	ANI_NEWSEQ
			.word	SEQ_P_ARMG_V

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0
			.byte	ANI_CODE
			.word	taunt_speech,11,0,0

			.byte	ANI_RUNTOFRAME,42,1
			
			.byte	ANI_CODE
			.word	release_puppet,0,0,0
			
			.byte	ANI_CODE
			.word	player_down,0,0,0
			
			.byte	ANI_CODE
			.word	get_hit_snd,P_GET_THRWN1_SP,0,0

			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT

			.byte	ANI_SET_FACE_REL_VEL
			.float	1.23

			.byte	ANI_INTTOSEQ,2
			.word	SEQ_H_SPIN2
			.byte	2

			.byte	ANI_RUNTOFRAME,6,1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_SETYVEL
			.float	.75
			.byte	1
			.byte	ANI_INTSTREAM,41,AA_TOGROUND

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float	.032

			.byte	1

			.byte	ANI_SETYVEL
			.float	.53
			.byte	ANI_INTSTREAM,20,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,2
			.word	10

			.byte	ANI_CODE
			.word	tackle_spch,HS_HARD,-1,0

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fu_anim

			.byte	ANI_CODE
			.word	maybe_get_up_b_hurt,0,0,0
			
			.byte	ANI_RUNTOEND,1

			.byte	ANI_GOTO
			.word	get_up_b_anim
			.byte	ANI_END


	
#;;;;;;;;;;;;  Player rolls on ground and tackles dude

			.globl	p_tbak_a_anim
			.globl	p_tbak_v_anim
			.byte	SCRIPT
p_tbak_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_NEWSEQ
			.word	SEQ_P_TBAK_A

			.byte	ANI_ZEROVELS
			
			.byte	ANI_RUNTOFRAME,20,1
			
			.byte	ANI_CODE
			.word	attack_snd,P_THRW_PLR_SP,0,0
			
			.byte	ANI_RUNTOFRAME,83,1
			
			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_INTSTREAM,100,60
			
			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	-256
			
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
p_tbak_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_NEWSEQ
			.word	SEQ_P_TBAK_V

			.byte	1
			
			.byte	ANI_CODE
			.word	get_hit_snd,-1,0,0
			
#			.byte	ANI_CODE
#			.word	player_down,0,0,0
			
			.byte	ANI_RUNTOFRAME,82,1
			
			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1
			.byte	ANI_CODE
			.word	dust_cloud,0,0,0
			.byte	ANI_SHAKER,3
			.word	10
			
# Put new LOS here, perhaps too far
			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_SETVELS
			.float	0.0,0.60,-.95
			
			.byte	ANI_SETFRICTION
			.float .025
			
			.byte	ANI_RUNTOEND,1
			
			.byte	ANI_TURNHACK
			.word	512
			
			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fd_anim

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_M_GU_C2
			.byte	0

			.byte	ANI_GOTO
			.word	get_up_c_anim
			.byte	ANI_END
	

#;;;;;;;;;;;;  Player helps dude on ground get up from lyin on his back

			.globl	p_hlpup_back_a_anim
			.globl	p_hlpup_back_v_anim
			.byte	SCRIPT
p_hlpup_back_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_NEWSEQ
			.word	SEQ_P_BHLP_A

			.byte	ANI_ZEROVELS
			
			.byte	ANI_CODE
			.word	drop_ball,0,0,0

			.byte	ANI_RUNTOFRAME,134,1
			
			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_RUNTOFRAME,136,1

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
p_hlpup_back_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET



#; Test new camera stuff
	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam_2
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex
psv_nocam_2:
	.endif




			.byte	ANI_NEWSEQ
			.word	SEQ_P_BHLP_V

			.byte	ANI_CLRFLAG
			.word	PF_LATEHITABLE

			.byte	ANI_RUNTOFRAME,134,1
			
			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	512
						
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;  Player helps dude on ground get up from layin on chest

#			.globl	p_hlpup_chest_a_anim
#			.globl	p_hlpup_chest_v_anim
#			.byte	SCRIPT
#p_hlpup_chest_a_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_CHLP_A
#
#			.byte	ANI_ZEROVELS
#			
#			.byte	ANI_CODE
#			.word	drop_ball,0,0,0
#
#			.byte	ANI_RUNTOFRAME,62,1
#			
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
#
#
#			.byte	SCRIPT
#p_hlpup_chest_v_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET
#
#
#
##; Test new camera stuff
#	.if DOSETCAM
#			.byte	ANI_CODE_BZ
#			.word	plyr_have_ball,0,0,0,psv_nocam_3
#			
#			.byte	ANI_SETCAMBOT
#			.word	csi_rs_suplex
#psv_nocam_3:
#	.endif
#
#
#
#
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_P_CHLP_V
#
#			.byte	ANI_CLRFLAG
#			.word	PF_LATEHITABLE
#
#			.byte	ANI_RUNTOFRAME,62,1
#			
#			.byte	ANI_CODE
#			.word	release_puppet,0,0,0
#
#			.byte	ANI_RUNTOEND,1
#			
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END


##;;;;;;;;;;  player gets flung over other players back

			.globl	p_flip_a_anim
			.globl	p_flip_v_anim
			.byte	SCRIPT
p_flip_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_ZEROVELS

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_P_FLIP_A

			.byte	ANI_CODE
			.word	attack_snd,78,0,0

			.byte	ANI_INTSTREAM,49,33

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	maybe_taunt,0,0,0
			
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


			.byte	SCRIPT
p_flip_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_REACTING|MODE_PUPPET

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_NEWSEQ
			.word	SEQ_P_FLIP_V

			.byte	ANI_CODE
			.word	get_hit_snd,-1,P_GET_THRWN1_SP,0
								  
			.byte	ANI_INTSTREAM,49,33

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_SETVELS
			.float	0.0,.90,-.80
#			.byte	ANI_SETYVEL
#			.float	.85
#			.byte	ANI_SET_FACE_REL_VEL
#			.float	.75

			.byte	1
			.byte	ANI_INTSTREAM,41,AA_TOGROUND

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,8
			.word	20

			.byte	ANI_SETFRICTION
			.float	0.030

			.byte	ANI_SETMODE
			.word	MODE_UNINT

			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_CODE_BNZ
			.word	plyr_have_ball,0,0,0,m_wob_fu_anim

			.byte	ANI_CODE
			.word	maybe_get_up_b_hurt,0,0,0
			
			.byte	ANI_INTTOSEQ,4
			.word	SEQ_GET_UP_B
			.byte	0
			.byte	ANI_GOTO
			.word	get_up_b_anim
			.byte	ANI_END

#;;;;;;;;;
#;;;;;;;;; CATCHES
#;;;;;;;;;

#;;;;;;;;;;  player dives and catches ball with both hands

			.globl	c_dive_for_ball_anim
			.byte	SCRIPT
c_dive_for_ball_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			
			.byte	ANI_NEWSEQ
			.word	SEQ_C_DIV2H

			.byte	ANI_CODE
			.word	face_ball,512,0,0

			.byte	ANI_INTSTREAM,11,5

			.byte	ANI_LEAPAT_TGTX
#			.byte	ANI_LEAPAT_BALL
		# max xzv, min xzv
#			.float	2.00,0.8
			.float	2.00,0.05
			
						
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY
			.byte	ANI_SETYVEL
			.float	.75

			.byte	ANI_RUNTOFRAME,23,1
		#catch ball

			.byte	ANI_INTSTREAM,10,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float	0.035

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0
			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_RUNTOEND,1
no_anim:
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  player jumps up and tries to intercept the ball

			.globl	c_jmpint_anim
			.byte	SCRIPT
c_jmpint_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_NOCYL

			.byte	ANI_NEWSEQ
			.word	SEQ_C_JMPINT

			.byte	ANI_CODE
			.word	rotate_to_ball,4,0,0

			.byte	ANI_INTSTREAM,11,5

#			.byte	ANI_LEAPAT_BALL
			.byte	ANI_LEAPAT_TGTX
			# max xzv, min xzv
#			.float	2.00,0.0
			.float	1.30,0.0

			.byte	ANI_SETYVEL
			.float	1.4
 
			.byte	ANI_INTSTREAM,11,15
		#catch ball here

			.byte	ANI_INTSTREAM,13,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

#			.byte	ANI_SETMODE
#			.word	MODE_ROTONLY

			.byte	ANI_RUNTOEND,1

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END



#;;;;;;;;;;  player jumps up and tries to intercept the ball

			.globl	c_jmpint2_anim
			.byte	SCRIPT
c_jmpint2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
#			.word	MODE_UNINT|MODE_ANCHOR|MODE_NOCYL

			.byte	ANI_NEWSEQ
			.word	SEQ_C_JMPIN3

			.byte	ANI_CODE
			.word	rotate_to_ball,4,0,0

			.byte	ANI_INTSTREAM,12,5

#			.byte	ANI_LEAPAT_BALL
			.byte	ANI_LEAPAT_TGTX
			# max xzv, min xzv
			.float	1.3,0.0
#			.float	1.85,0.0

			.byte	ANI_SETYVEL
			.float	1.4
 
			.byte	ANI_RUNTOFRAME,13,1
		#catch ball here

			.byte	ANI_INTSTREAM,20,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

#			.byte	ANI_SETMODE
#			.word	MODE_ROTONLY

			.byte	ANI_INTSTREAM,22,6
		#start forward progress

			.byte	ANI_INTSTREAM,19,8

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

			
#;;;;;;;;;;  player jumps up and tries to intercept the ball.
#;;;;;;;;;;  no aiming or vel changes.  only drones use this.

			.globl	c_jmpint3_anim
			.byte	SCRIPT
c_jmpint3_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_C_JMPINT

			.byte	ANI_CODE
			.word	rotate_to_ball,4,0,0

			.byte	ANI_INTSTREAM,11,5

			.byte	ANI_SETVELS
			.float	0.0,1.4,1.15

			.byte	ANI_INTSTREAM,11,15
		#catch ball here

			.byte	ANI_INTSTREAM,13,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_RUNTOEND,1

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END



#;;;;;;;;;;  player jumps up and catches ball (no XZ velocity) #1

			.globl	c_no_vel1_anim
			.byte	SCRIPT
c_no_vel1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_C_JMPIN3

			.byte	ANI_CODE
			.word	rotate_to_ball,4,0,0

			.byte	ANI_INTSTREAM,13,5

			.byte	ANI_SETYVEL
			.float	.75
 
			.byte	ANI_INTSTREAM,13,8
		#catch ball here

			.byte	ANI_INTSTREAM,11,AA_TOGROUND

#			.byte	ANI_SETMODE
#			.word	MODE_ROTONLY

			.byte	ANI_RUNTOFRAME,46,1

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END
			
			
#;;;;;;;;;;  player jumps up and tries to intercept the ball

#			.globl	c_no_vel2_anim
#			.byte	SCRIPT
#c_no_vel2_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_C_JMPINT
#
#			.byte	ANI_CODE
#			.word	rotate_to_ball,4,0,0
#			
#			.byte	ANI_INTSTREAM,11,5
#
#			.byte	ANI_SETYVEL
#			.float	.75
# 
#			.byte	ANI_INTSTREAM,5,13
#		#catch ball here
#
#			.byte	ANI_INTSTREAM,21,AA_TOGROUND
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END	

#;;;;;;;;;;  player jumps and catches ball in gut

			.globl	c_jgut1_anim
			.globl	c_jgut1f_anim
			.byte	SCRIPT
c_jgut1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_NEWSEQ
			.word	SEQ_C_JGUT1

			.byte	ANI_GOTO
			.word	finish_jgut1
			.byte	ANI_END

#;;;;;;;;;;  player jumps and catches ball in gut (flipped)

			.byte	SCRIPT
c_jgut1f_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_NEWSEQ
			.word	SEQ_C_JGUT1F
finish_jgut1:
			.byte	ANI_INTSTREAM,13,5

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY
			.byte	ANI_SETYVEL
			.float	1.20

			.byte	ANI_INTSTREAM,8,15
			.byte	ANI_INTSTREAM,24,AA_TOGROUND

#			.byte	ANI_PREP_XITION
#			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; jump low at catch ball

			.globl	c_jmp1_anim
			.globl	c_jmp1f_anim
			.byte	SCRIPT
c_jmp1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_NEWSEQ
			.word	SEQ_C_JMP1

			.byte	ANI_GOTO
			.word	finish_jmp1
			.byte	ANI_END
			
#;;;;;;;;;; jump low at catch ball (flipped)
			
			.byte	SCRIPT
c_jmp1f_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_NEWSEQ
			.word	SEQ_C_JMP1F
	
finish_jmp1:
			.byte	ANI_INTSTREAM,17,5

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY
			.byte	ANI_SETYVEL
			.float	1.20
	#in air for 30 ticks
			.byte	ANI_INTSTREAM,8,15
			.byte	ANI_INTSTREAM,20,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float .050
			
#			.byte	ANI_SETMODE
#			.word	MODE_ROTONLY

			.byte	ANI_RUNTOFRAME,52,1

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; jump low at catch ball #2

			.globl	c_jmp2_anim
			.globl	c_jmp2f_anim
			.byte	SCRIPT
c_jmp2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_NEWSEQ
			.word	SEQ_C_JMP2

			.byte	ANI_GOTO
			.word	finish_jmp2
			.byte	ANI_END
			
#;;;;;;;;;; jump low at catch ball (flipped) #2
			
			.byte	SCRIPT
c_jmp2f_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_NEWSEQ
			.word	SEQ_C_JMP2F
finish_jmp2:
			.byte	ANI_INTSTREAM,13,5

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY
			.byte	ANI_SETYVEL
			.float	1.30
	#in air for 30 ticks
			.byte	ANI_INTSTREAM,7,15
			.byte	ANI_INTSTREAM,20,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float .060
			
#			.byte	ANI_SETMODE
#			.word	MODE_ROTONLY

			.byte	ANI_RUNTOEND,1

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END
			
			
#;;;;;;;;;; catch ball doing splits on frame 25 in hands

			.globl	c_splits_anim
			.globl	c_splitsf_anim
			.byte	SCRIPT
c_splits_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_C_SPLITS
			.byte	0

			.byte	ANI_GOTO
			.word	finish_splits
			.byte	ANI_END

#;;;;;;;;;; catch ball doing splits on frame 25 in hands (flipped) 'coming from left of ball'

			.byte	SCRIPT
c_splitsf_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_C_SPLITF
			.byte	0
finish_splits:
			.byte	ANI_RUNTOFRAME,5,1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY
			.byte	ANI_SETYVEL
			.float	1.40

			.byte	ANI_INTSTREAM,17,15
			.byte	ANI_INTSTREAM,14,AA_TOGROUND

#			.byte	ANI_SETMODE
#			.word	MODE_ROTONLY

			.byte	ANI_INTSTREAM,38,19

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; catch ball with 1 hand

#			.globl	c_1hand1_anim
#			.globl	c_1hand1f_anim
#			.byte	SCRIPT
#c_1hand1_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#			.byte	ANI_INTTOSEQ,3
#			.word	SEQ_C_1HND1
#			.byte	0
#
#			.byte	ANI_GOTO
#			.word	finish_1hand1
#			.byte	ANI_END
#
##;;;;;;;;;; catch ball with 1 hand (flipped)
#
#			.byte	SCRIPT
#c_1hand1f_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#			.byte	ANI_INTTOSEQ,3
#			.word	SEQ_C_1HND1F
#			.byte	0
#finish_1hand1:
#			.byte	ANI_INTSTREAM,15,5
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY
#			.byte	ANI_SETYVEL
#			.float	1.35
#			.byte	ANI_INTSTREAM,6,15
#			.byte	ANI_INTSTREAM,12,AA_TOGROUND
#
##			.byte	ANI_SETMODE
##			.word	MODE_ROTONLY
#
#			.byte	ANI_INTSTREAM,13,10
#
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END


#;;;;;;;;;; catch ball by diving for it

#			.globl	c_dive1_anim
#			.byte	SCRIPT
#c_dive1_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#			.byte	ANI_NEWSEQ
#			.word	SEQ_C_DIVE1
#
#			.byte	ANI_INTSTREAM,14,5
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY
#			.byte	ANI_SETYVEL
#			.float	0.8
#
#			.byte	ANI_INTSTREAM,15,10
#			.byte	ANI_INTSTREAM,15,AA_TOGROUND
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,0
#
#			.byte	ANI_CODE
#			.word	dust_cloud,0,0,0
#
#			.byte	ANI_SETFRICTION
#			.float .055
#
#			.byte	ANI_INTSTREAM,5,13
#
#			.byte	ANI_GOTO
#			.word	get_up_c_anim


#;;;;;;;;;; catch ball by diving for it and landing on neck

#			.globl	c_ohednk_anim
#			.byte	SCRIPT
#c_ohednk_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_C_OHEDNK
#
#			.byte	ANI_SETYVEL
#			.float	1.20
#
#			.byte	ANI_RUNTOFRAME,15,1
#			.byte	ANI_INTSTREAM,28,AA_TOGROUND
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,-1,0
#
#			.byte	ANI_CODE
#			.word	dust_cloud,0,0,0
#
#			.byte	ANI_SETFRICTION
#			.float .055
#
##			.byte	ANI_SETMODE
##			.word	MODE_ROTONLY
#
##			.byte	ANI_INTSTREAM,97,53
#			.byte	ANI_INTSTREAM,97,33
#
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END


#;;;;;;;;;; catch ball by grabbing outta air...and continue running

			.globl	c_run1_anim
			.globl	c_run1f_anim
			.byte	SCRIPT
c_run1_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_C_RUN1
			.byte	0

			.byte	ANI_GOTO
			.word	finish_run1

#;;;;;;;;;; catch ball by grabbing outta air...and continue running (flipped)

#			.byte	SCRIPT
#c_run1f_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY
#
#			.byte	ANI_INTTOSEQ,3
#			.word	SEQ_C_RUN1F
#			.byte	0

finish_run1:
			.byte	ANI_INTSTREAM,24,5

			.byte	ANI_INTSTREAM,7,12
		#catch ball

			.byte	ANI_INTSTREAM,9,7
		#landed

			.byte	ANI_INTSTREAM,20,8

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; catch ball by grabbing outta air...and continue running

			.globl	c_run1a_anim
			.byte	SCRIPT
c_run1a_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_NEWSEQ
			.word	SEQ_C_RUN1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_INTSTREAM,22,5

			.byte	ANI_SETYVEL
			.float	1.65

			.byte	ANI_INTSTREAM,11,20
		#catch ball

			.byte	ANI_INTSTREAM,13,AA_TOGROUND

#			.byte	ANI_SETMODE
#			.word	MODE_ROTONLY

			.byte	ANI_INTSTREAM,19,10

#			.byte	ANI_PREP_XITION
#			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; catch ball tumbling and getting back up

			.globl	c_sidrol_anim
			.byte	SCRIPT
c_sidrol_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_C_SROL2
			.byte	0

			.byte	ANI_GOTO
			.word	finish_sidrol
			.byte	ANI_END

#;;;;;;;;;; catch ball tumbling and getting back up (flipped)

			.globl	c_sidrolf_anim
			.byte	SCRIPT
c_sidrolf_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_INTTOSEQ,3
			.word	SEQ_C_SROL2F
			.byte	0
finish_sidrol:
			.byte	ANI_INTSTREAM,11,5

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY
			.byte	ANI_SETYVEL
			.float	1.35
			.byte	ANI_RUNTOFRAME,25,1
			.byte	ANI_INTSTREAM,12,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SETFRICTION
			.float .040

#			.byte	ANI_SETMODE
#			.word	MODE_ROTONLY

#			.byte	ANI_INTSTREAM,82,23
			.byte	ANI_INTSTREAM,82,13

#			.byte	ANI_PREP_XITION
#			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END



#;;;;;;;;;; catch ball -- side-of gut

			.globl	c_sdgut_anim
			.byte	SCRIPT
c_sdgut_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_C_SDGUT
			.byte	9

			.byte	ANI_GOTO
			.word	finish_sgut

#;;;;;;;;;; catch ball -- side-of gut (flipped)

			.globl	c_sdgutf_anim
			.byte	SCRIPT
c_sdgutf_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_C_SDGUTF
			.byte	9
finish_sgut:
			.byte	ANI_RUNTOFRAME,15,1
		#catch ball

#			.byte	ANI_SETMODE
#			.word	MODE_ROTONLY

			.byte	ANI_INTSTREAM,24,13

#			.byte	ANI_PREP_XITION
#			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; catch ball -- then spin and run

			.globl	c_spin2_anim
			.byte	SCRIPT
c_spin2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_C_SPIN2
			.byte	0

			.byte	ANI_GOTO
			.word	finish_spin

#;;;;;;;;;; catch ball -- then spin and run (flipped)

			.globl	c_spin2f_anim
			.byte	SCRIPT
c_spin2f_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_INTTOSEQ,4
			.word	SEQ_C_SPIN2F
			.byte	0
finish_spin:
			.byte	ANI_INTSTREAM,11,5

			.byte	ANI_SETYVEL
			.float	1.40

			.byte	ANI_INTSTREAM,8,15
		#catch ball

			.byte	ANI_INTSTREAM,8,AA_TOGROUND

#			.byte	ANI_SETMODE
#			.word	MODE_ROTONLY

			.byte	ANI_INTSTREAM,40,18

#			.byte	ANI_PREP_XITION
#			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; catch ball -- then jump up...land...flip and run

			.globl	c_roll_anim
			.byte	SCRIPT
c_roll_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_C_ROLL
			.byte	ANI_SKIPFRAMES,1
#			.byte	ANI_INTTOSEQ,4
#			.word	SEQ_C_ROLL
#			.byte	1

			.byte	1,1
#			.byte	ANI_RUNTOFRAME,2,1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY
			.byte	ANI_SETYVEL
			.float	1.50

			.byte	ANI_RUNTOFRAME,14,1
		#catch ball

			.byte	ANI_INTSTREAM,21,AA_TOGROUND

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

#			.byte	ANI_SETMODE
#			.word	MODE_ROTONLY

			.byte	ANI_INTSTREAM,39,30

#			.byte	ANI_PREP_XITION
#			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;; catch and corksrew and run

#			.globl	c_jmp3_anim
#			.byte	SCRIPT
#c_jmp3_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY
#
#			.byte	ANI_INTTOSEQ,5
#			.word	SEQ_C_JMP3
#			.byte	22
#
#			.byte	ANI_GOTO
#			.word	finish_jmp3
#
##;;;;;;;;;; catch ball -- then spin and run (flipped)
#
#			.globl	c_jmp3f_anim
#			.byte	SCRIPT
#c_jmp3f_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY
#
#			.byte	ANI_INTTOSEQ,5
#			.word	SEQ_C_JMP3
#			.byte	22
#finish_jmp3:
#			.byte	ANI_RUNTOFRAME,25,1
#
#			.byte	ANI_SETYVEL
#			.float	1.45
#
#			.byte	ANI_INTSTREAM,15,10
#		#catch ball
#
#			.byte	ANI_INTSTREAM,31,AA_TOGROUND
#
##			.byte	ANI_SETMODE
##			.word	MODE_ROTONLY
#
#			.byte	ANI_INTSTREAM,5,3
#
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END


#;;;;;;;;;; catch ball -- jumping up..landing and then turning and running

#			.globl	c_jmpovr_anim
#			.byte	SCRIPT
#c_jmpovr_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY
#
#			.byte	ANI_INTTOSEQ,4
#			.word	SEQ_C_JMPOVR
#			.byte	0
#
#			.byte	ANI_INTSTREAM,11,5
#
#			.byte	ANI_SETYVEL
#			.float	1.45
#
#			.byte	ANI_INTSTREAM,13,16
#		#catch ball
#
#			.byte	ANI_INTSTREAM,25,AA_TOGROUND
#
#			.byte	ANI_INTSTREAM,37,15
#
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
#


#;;;;;;;;;;  player jumps and summersaults

#			.globl	c_flip_anim
#			.byte	SCRIPT
#c_flip_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_INTTOSEQ,3
#			.word	SEQ_C_FLIP
#			.byte	3
#
#			.byte	ANI_INTSTREAM,3,5
#
#			.byte	ANI_SETYVEL
#			.float	1.45
#
#			.byte	ANI_INTSTREAM,11,15
#		#catch ball
#
#			.byte	ANI_INTSTREAM,22,AA_TOGROUND
#
#			.byte	ANI_RUNTOEND,1
#
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END


#;;;;;;;;;;  player pedals bike and catches ball

			.globl	c_bicyc_anim
			.byte	SCRIPT
c_bicyc_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_C_BICYC
			.byte	0

			.byte	ANI_GOTO
			.word	finish_bcyc
			.byte	ANI_END

#;;;;;;;;;;  player pedals bike and catches ball (flipped)

			.globl	c_bicycf_anim
			.byte	SCRIPT
c_bicycf_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_C_BICYCF
			.byte	0
finish_bcyc:
			.byte	ANI_INTSTREAM,15,5

			.byte	ANI_SETYVEL
			.float	1.35

			.byte	ANI_INTSTREAM,10,15
		#catch ball

			.byte	ANI_INTSTREAM,23,AA_TOGROUND

			.byte	ANI_INTSTREAM,25,15

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;  player catches ball diving with 1 hand

#			.globl	c_div1h_anim
#			.byte	SCRIPT
#c_div1h_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_INTTOSEQ,3
#			.word	SEQ_C_DIV1H
#			.byte	0
#
#			.byte	ANI_GOTO
#			.word	finish_cdiv
#			.byte	ANI_END

#;;;;;;;;;;  player catches ball diving with 1 hand (flipped)
#
#			.globl	c_div1hf_anim
#			.byte	SCRIPT
#c_div1hf_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_INTTOSEQ,3
#			.word	SEQ_C_DIV1HF
#			.byte	0
#finish_cdiv:
#			.byte	ANI_INTSTREAM,14,5
#
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY
#			.byte	ANI_SETYVEL
#			.float	1.25
#
#			.byte	ANI_INTSTREAM,16,10
#		#catch ball
#
#			.byte	ANI_INTSTREAM,17,AA_TOGROUND
#
#			.byte	ANI_SETFRICTION
#			.float	0.035
#
#			.byte	ANI_CODE
#			.word	grnd_hit_snd,-1,0,0
#
#			.byte	ANI_CODE
#			.word	dust_cloud,0,0,0
#
#			.byte	ANI_INTSTREAM,60,40
#
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END

#;;;;;;;;;;  player dives and catches ball with both hands

			.globl	c_div2h_anim
			.byte	SCRIPT
c_div2h_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_INTTOSEQ,3
			.word	SEQ_C_DIV2H
			.byte	0

			.byte	ANI_INTSTREAM,10,5

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY
			.byte	ANI_SETYVEL
			.float	1.25

			.byte	ANI_RUNTOFRAME,23,1
		#catch ball

			.byte	ANI_INTSTREAM,10,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float	0.035

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,0

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_INTSTREAM,55,30
#			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;; player catches ball standing (quick kick version)

			.globl	c_standing_anim
			.byte	SCRIPT
c_standing_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_NEWSEQ
			.word	SEQ_C_STND1
			.byte	1

			.byte	ANI_WAITTRUE
			.word	is_inplay

			.byte	ANI_INTSTREAM,35,40
			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	general_run,0,0,0

			
#;;; ********** TWOPART catches **********

#;;;;;;;;;; catch ball by grabbing outta air...and continue running

			.globl	c2p_back_anim
			.byte	SCRIPT
c2p_back_anim:
			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART
			.word	MODE_ANCHORXZ|MODE_TWOPART

			.byte	ANI_NEWSEQ
			.word	SEQ_C_RUN1F
			
			.byte	ANI_CODE
			.word	rotate_torso,512,0,0
			
			.byte	ANI_INTSTREAM,30,8
		#catch ball

			.byte	ANI_INTSTREAM,35,10

		# turn off two_part
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0
			
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

			
#;;;;;;;;;; catch ball in side and continue running

			.globl	c2p_side_anim
			.byte	SCRIPT
c2p_side_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ|MODE_TWOPART
#			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART

			.byte	ANI_NEWSEQ
			.word	SEQ_C_SDGUT
			
			.byte	ANI_CODE
			.word	rotate_torso,512,0,0
			
			.byte	ANI_INTSTREAM,15,8
		#catch ball

			.byte	ANI_INTSTREAM,25,11

		# turn off two_part
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0
			
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END
			
#;;;;;;;;;; catch ball in side and continue running (fliped)

			.globl	c2p_sidef_anim
			.byte	SCRIPT
c2p_sidef_anim:
			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORXZ|MODE_TWOPART
			.word	MODE_ANCHORXZ|MODE_TWOPART

			.byte	ANI_NEWSEQ
			.word	SEQ_C_SDGUTF
		
			.byte	ANI_CODE
			.word	rotate_torso,512,0,0
			
			.byte	ANI_INTSTREAM,15,8
		#catch ball

			.byte	ANI_INTSTREAM,25,11

		# turn off two_part
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_CODE
			.word	set_unint_lowerhalf,0,0,0
			
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END
			
			
#;;;;;;;;;;;;; player catches ball standing (long kick version)

			.globl	c_standing2_anim
			.byte	SCRIPT
c_standing2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_NEWSEQ
			.word	SEQ_C_STND1
			.byte	1

			.byte	ANI_WAITTRUE
			.word	is_ball_kicked
			.byte	100
			.byte	35

			.byte	ANI_INTSTREAM,33,40
			.byte	ANI_RUNTOEND,1

			.byte	ANI_CODE
			.word	general_run,0,0,0

			.globl	run_hold_anim

			.byte	SCRIPT
run_hold_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
rha_loop:
			.byte	ANI_NEWSEQ
			.word	SEQ_RUN
			.byte	1
			.byte	ANI_WAITTRUE
			.word	is_inplay
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;; Milling around
			.globl	mill_random_anim
			.byte	SCRIPT
mill_random_anim:
			.byte	ANI_CODE
			.word	pick_random_mill,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;; Milling around

			.globl	fb_mill1
			.byte	SCRIPT
fb_mill1:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

#			.byte	ANI_CLRFLAG
#			.word	PF_LATEHITABLE

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_MILL_01
			.byte	0


			.byte	ANI_INTSTREAM,8,30
			.byte	ANI_CODE
			.word	dust_cloud,3,0,0


			.byte	ANI_INTSTREAM,21,84
			.byte	ANI_CODE
			.word	dust_cloud,4,0,0




			.byte	ANI_INTSTREAM,33,115



#; 66 in  230
#			.byte	ANI_INTSTREAM,33,115
#			.byte	ANI_INTSTREAM,33,115


			.byte	ANI_TURNHACK
			.word	128


			.byte	ANI_GOTO
			.word	mill_random_anim

#;;;;;;;;;;;;; Milling around

			.globl	fb_mill4
			.byte	SCRIPT
fb_mill4:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

#			.byte	ANI_CLRFLAG
#			.word	PF_LATEHITABLE

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_MILL_04
			.byte	16

			.byte	ANI_INTSTREAM,22,70
			.byte	ANI_INTSTREAM,22,70
			.byte	ANI_INTSTREAM,22,70
			.byte	ANI_INTSTREAM,22,70


			.byte	ANI_TURNHACK
			.word	64

#			.byte	ANI_GOTO
#			.word	stance_anim_int
#			.byte	ANI_END
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
			.byte	ANI_GOTO
			.word	mill_random_anim

#;;;;;;;;;;;;; Milling around

			.globl	fb_mill6
			.byte	SCRIPT
fb_mill6:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

#			.byte	ANI_CLRFLAG
#			.word	PF_LATEHITABLE

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_MILL_06
			.byte	0

			.byte	ANI_INTSTREAM,30,72
			.byte	ANI_INTSTREAM,30,72
			.byte	ANI_INTSTREAM,30,72
			.byte	ANI_INTSTREAM,30,72

			.byte	ANI_TURNHACK
			.word	-256

#			.byte	ANI_GOTO
#			.word	stance_anim_int
#			.byte	ANI_END

#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END

			.byte	ANI_GOTO
			.word	mill_random_anim



#;;;;;;;;;;;;; Milling around

			.globl	fb_mill7
			.byte	SCRIPT
fb_mill7:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

#			.byte	ANI_CLRFLAG
#			.word	PF_LATEHITABLE

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_MILL_07
			.byte	0

			.byte	ANI_INTSTREAM,30,72
			.byte	ANI_INTSTREAM,30,72
			.byte	ANI_INTSTREAM,30,72
#			.byte	ANI_INTSTREAM,30,72

			.byte	ANI_TURNHACK
			.word	-512

#			.byte	ANI_GOTO
#			.word	stance_anim_int
#			.byte	ANI_END
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END

			.byte	ANI_GOTO
			.word	mill_random_anim


#;;;;;;;;;;;;; Milling around

			.globl	fb_mill11
			.byte	SCRIPT
fb_mill11:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

#			.byte	ANI_CLRFLAG
#			.word	PF_LATEHITABLE

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_MILL_11
			.byte	0

			.byte	ANI_INTSTREAM,50,127
			.byte	ANI_INTSTREAM,50,127

#			.byte	ANI_GOTO
#			.word	stance_anim_int
#			.byte	ANI_END
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END

			.byte	ANI_GOTO
			.word	mill_random_anim


#;;;;;;;;;;;;; Milling around

			.globl	fb_mill12
			.byte	SCRIPT
fb_mill12:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

#			.byte	ANI_CLRFLAG
#			.word	PF_LATEHITABLE

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_MILL_12
			.byte	0

			.byte	ANI_INTSTREAM,24,100
			.byte	ANI_INTSTREAM,24,100
			.byte	ANI_INTSTREAM,24,100

			.byte	ANI_TURNHACK
			.word	512

#			.byte	ANI_GOTO
#			.word	stance_anim_int
#			.byte	ANI_END

#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END

			.byte	ANI_GOTO
			.word	mill_random_anim

#;;;;;;;;;;;;; Milling around

#			.globl	fb_mill13
#			.byte	SCRIPT
#fb_mill13:
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORXZ
#
#			.byte	ANI_INTTOSEQ,10
#			.word	SEQ_MILL_13
#			.byte	0
#
#			.byte	ANI_INTSTREAM,44,89
#			.byte	ANI_INTSTREAM,44,89
#
#			.byte	ANI_GOTO
#			.word	stance_anim_int
#			.byte	ANI_END


#;;;;;;;;;;;;; Milling around Exorcist!

			.globl	fb_mill15
			.byte	SCRIPT
fb_mill15:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

#			.byte	ANI_CLRFLAG
#			.word	PF_LATEHITABLE

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_MILL_15
			.byte	0

			.byte	ANI_INTSTREAM,35,71
			.byte	ANI_INTSTREAM,35,71
			.byte	ANI_INTSTREAM,35,71
			.byte	ANI_INTSTREAM,35,71

#			.byte	ANI_GOTO
#			.word	stance_anim_int
#			.byte	ANI_END

#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END

			.byte	ANI_GOTO
			.word	mill_random_anim

#;;;;;;;;;;;;; Defense Line Pre-Snap Stuff

			.globl	dline_01a
			.byte	SCRIPT
dline_01a:
			.byte	ANI_ZEROVELS
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_NEWSEQ
			.word	SEQ_DLINE_01

			.byte	ANI_INTSTREAM,55,112
			.byte	ANI_INTSTREAM,55,112

			.byte	ANI_GOTO
			.word	dline_01_loop

#-----
			.globl	dline_01b
			.byte	SCRIPT
dline_01b:
			.byte	ANI_ZEROVELS
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_NEWSEQ
			.word	SEQ_DLINE_01
			.byte	ANI_SKIPFRAMES,20

			.byte	ANI_INTSTREAM,45,112
			.byte	ANI_INTSTREAM,45,112

			.byte	ANI_GOTO
			.word	dline_01_loop

#-----
			.globl	dline_01c
			.byte	SCRIPT
dline_01c:
			.byte	ANI_ZEROVELS
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_NEWSEQ
			.word	SEQ_DLINE_01
			.byte	ANI_SKIPFRAMES,40

			.byte	ANI_INTSTREAM,35,102
			.byte	ANI_INTSTREAM,35,102
												
dline_01_loop:

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_DLINE_01
			.byte	0

			.byte	ANI_INTSTREAM,55,122
			.byte	ANI_INTSTREAM,55,122

			.byte	ANI_GOTO
			.word	dline_01_loop


#;;;;;;;;;;;;; Defense Line Pre-Snap Stuff

			.globl	dline_02a
			.byte	SCRIPT
dline_02a:
			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_DLINE_02

			.byte	ANI_INTSTREAM,44,117
			.byte	ANI_INTSTREAM,44,117
			.byte	ANI_INTSTREAM,44,117
			.byte	ANI_INTSTREAM,44,117

			.byte	ANI_GOTO
			.word	dline_02_loop

			.globl	dline_02b
			.byte	SCRIPT
dline_02b:
			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_DLINE_02

			.byte	ANI_SKIPFRAMES,20

			.byte	ANI_INTSTREAM,39,112
			.byte	ANI_INTSTREAM,39,112
			.byte	ANI_INTSTREAM,39,112
			.byte	ANI_INTSTREAM,39,112

			.byte	ANI_GOTO
			.word	dline_02_loop

			.globl	dline_02c
			.byte	SCRIPT
dline_02c:
			.byte	ANI_ZEROVELS

			.byte	ANI_NEWSEQ
			.word	SEQ_DLINE_02

			.byte	ANI_SKIPFRAMES,40

			.byte	ANI_INTSTREAM,34,107
			.byte	ANI_INTSTREAM,34,107
			.byte	ANI_INTSTREAM,34,107
			.byte	ANI_INTSTREAM,34,107

			.byte	ANI_GOTO
			.word	dline_02_loop

dline_02_loop:

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_DLINE_02
			.byte	0

			.byte	ANI_INTSTREAM,44,117
			.byte	ANI_INTSTREAM,44,117
			.byte	ANI_INTSTREAM,44,117
			.byte	ANI_INTSTREAM,44,117

			.byte	ANI_GOTO
			.word	dline_02_loop


#;;;;;;;;;;;;; Defense Line Pre-Snap Stuff

			.globl	dline_03a
			.byte	SCRIPT
dline_03a:
			.byte	ANI_ZEROVELS
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_NEWSEQ
			.word	SEQ_DLINE_03

			.byte	ANI_INTSTREAM,36,117
			.byte	ANI_INTSTREAM,36,117
			.byte	ANI_INTSTREAM,36,117
			.byte	ANI_INTSTREAM,35,117

			.byte	ANI_GOTO
			.word	dline_03_loop
					
			.globl	dline_03b
			.byte	SCRIPT
dline_03b:
			.byte	ANI_ZEROVELS
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_NEWSEQ
			.word	SEQ_DLINE_03
			.byte	ANI_SKIPFRAMES,20

			.byte	ANI_INTSTREAM,31,107
			.byte	ANI_INTSTREAM,31,107
			.byte	ANI_INTSTREAM,31,107
			.byte	ANI_INTSTREAM,31,107

			.byte	ANI_GOTO
			.word	dline_03_loop

			.globl	dline_03c
			.byte	SCRIPT
dline_03c:
			.byte	ANI_ZEROVELS
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_NEWSEQ
			.word	SEQ_DLINE_03
			.byte	ANI_SKIPFRAMES,40

			.byte	ANI_INTSTREAM,26,100
			.byte	ANI_INTSTREAM,26,100
			.byte	ANI_INTSTREAM,26,100
			.byte	ANI_INTSTREAM,26,100

dline_03_loop:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_DLINE_03
			.byte	0

			.byte	ANI_INTSTREAM,36,117
			.byte	ANI_INTSTREAM,36,117
			.byte	ANI_INTSTREAM,36,117
			.byte	ANI_INTSTREAM,35,117
	
			.byte	ANI_GOTO
			.word	dline_03_loop


#;;;;;;;;;;;;; Defense Line Pre-Snap Stuff

			.globl	dline_04a
			.byte	SCRIPT
dline_04a:
			.byte	ANI_ZEROVELS
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_NEWSEQ
			.word	SEQ_DLINE_04

			.byte	ANI_INTSTREAM,34,102
			.byte	ANI_INTSTREAM,34,102
			.byte	ANI_INTSTREAM,34,102
			.byte	ANI_INTSTREAM,34,102

			.byte	ANI_GOTO
			.word	dline_04_loop

			.globl	dline_04b
			.byte	SCRIPT
dline_04b:
			.byte	ANI_ZEROVELS
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_NEWSEQ
			.word	SEQ_DLINE_04

			.byte	ANI_SKIPFRAMES, 20

			.byte	ANI_INTSTREAM,29,92
			.byte	ANI_INTSTREAM,29,92
			.byte	ANI_INTSTREAM,29,92
			.byte	ANI_INTSTREAM,29,92

			.byte	ANI_GOTO
			.word	dline_04_loop

			.globl	dline_04c
			.byte	SCRIPT
dline_04c:
			.byte	ANI_ZEROVELS
			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_NEWSEQ
			.word	SEQ_DLINE_04

			.byte	ANI_SKIPFRAMES, 40

			.byte	ANI_INTSTREAM,24,77
			.byte	ANI_INTSTREAM,24,77
			.byte	ANI_INTSTREAM,24,77
			.byte	ANI_INTSTREAM,24,77

dline_04_loop:

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_DLINE_04
			.byte	0

			.byte	ANI_INTSTREAM,69,127
			.byte	ANI_INTSTREAM,69,127

			.byte	ANI_GOTO
			.word	dline_04_loop

#;;;;;;;;;;;;; Taunt Thinks about stomping the guy

			.globl	m_nostomp
			.byte	SCRIPT
m_nostomp:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ

			.byte	ANI_ZEROVELS
			
			.byte	ANI_CODE
			.word	face_carrier,256,0,0

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_M_NOSTMP
			.byte	0

# Do a kick ass camera move!
	.if DOSETCAM
# If victim has ball, do some camera shit - otherwise, skip it
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,nostomp_cont
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex_nf
	.endif
nostomp_cont:

			.byte	ANI_CODE
			.word	taunt_speech,-3,0,0
			
			.byte	ANI_STREAMTOEND
			.float	0.80
			
#			.byte	ANI_GOTO
#			.word	stance_anim_int
#			.byte	ANI_END
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#; B2000 Endzone Dance - Back Handspring
#			.globl endz_02
#			.byte	SCRIPT
#endz_02:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_SETFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_INTTOSEQ,6
#			.word	SEQ_ENDZ_02
#			.byte	0
#			
#			.byte	ANI_CODE
#			.word	drop_ball,0,0,0
#
#			.byte	ANI_CODE
#			.word	taunt_speech,0,0,0
#			
#			.byte	ANI_INTSTREAM,24,72			
#			.byte	ANI_INTSTREAM,24,72
#
#			.byte	ANI_CLRFLAG
#			.word	PF_CELEBRATE
#
#			.byte	ANI_GOTO
#			.word	breath_anim
#			.byte	ANI_END


			.globl endz_08
			.byte	SCRIPT
endz_08:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_ENDZ_08
			.byte	4

	.if DOSETCAM
#			.byte	ANI_SETCAMBOT
#			.word	csi_zm_divetack
# If victim has ball, do some camera shit - otherwise, skip it
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,endz_08_cont
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex_nf
	.endif
endz_08_cont:
			
			.byte	ANI_CODE
			.word	drop_ball,0,0,0

			.byte	ANI_CODE
			.word	taunt_speech,0,0,0
			
			.byte	ANI_INTSTREAM,69,122

			.byte	ANI_CLRFLAG
			.word	PF_CELEBRATE

			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END

			.globl endz_06
			.byte	SCRIPT
endz_06:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE


			.byte	ANI_INTTOSEQ,10
			.word	SEQ_ENDZ_06
			.byte	0

	.if DOSETCAM
#			.byte	ANI_SETCAMBOT
#			.word	csi_zm_divetack
# If victim has ball, do some camera shit - otherwise, skip it
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,endz_06_cont
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex
	.endif
endz_06_cont:

			.byte	ANI_ZEROVELS

			
			.byte	ANI_CODE
			.word	drop_ball,0,0,0

			.byte	ANI_CODE
			.word	taunt_speech,0,0,0


			.byte	ANI_INTSTREAM,40,65
			.byte	ANI_INTSTREAM,40,65
			.byte	ANI_INTSTREAM,40,65
			.byte	ANI_INTSTREAM,40,65

			.byte	ANI_CLRFLAG
			.word	PF_CELEBRATE

			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END


#;---------------- Man2man plays pick a from random shift tables!!
			.globl	db_man2man_random_anim
			.byte	SCRIPT
db_man2man_random_anim:
			.byte	ANI_CODE
			.word	pick_random_db_m2m_anim,0,0,0
			.byte	ANI_END

#;---------------- Zone plays pick a from random shift tables!!

			.globl	db_zone_random_anim
			.byte	SCRIPT
db_zone_random_anim:
			.byte	ANI_CODE
			.word	pick_random_db_zone_anim,0,0,0
			.byte	ANI_END

#;---------------- Shift
			.globl	shift_03
			.byte	SCRIPT
shift_03:
			
			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_SHIFT_03
			.byte	0

			.byte	ANI_INTSTREAM,43,92
			.byte	ANI_INTSTREAM,43,92
			.byte	ANI_INTSTREAM,43,92
			.byte	ANI_INTSTREAM,42,92

			.byte	ANI_GOTO
			.word	db_random_anim
		
			.byte	ANI_END

#;---------------- Shift
#			.globl	shift_05
#			.byte	SCRIPT
#shift_05:
#			
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORXZ
#
#			.byte	ANI_INTTOSEQ,10
#			.word	SEQ_SHIFT_05
#			.byte	0
#
#			.byte	ANI_INTSTREAM,42,72
#
#			.byte	ANI_GOTO
#			.word	db_random_anim
#		
#			.byte	ANI_END

#;---------------- Shift
#			.globl	shift_06
#			.byte	SCRIPT
#shift_06:
#			
#			.byte	ANI_ZEROVELS
#
#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORXZ
#
#			.byte	ANI_INTTOSEQ,10
#			.word	SEQ_SHIFT_06
#			.byte	0
#
#			.byte	ANI_INTSTREAM,42,72
#
#			.byte	ANI_GOTO
#			.word	db_random_anim
#		
#			.byte	ANI_END

#;---------------- Shift
			.globl	shift_08
			.byte	SCRIPT
shift_08:
			
			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_SHIFT_08
			.byte	0

			.byte	ANI_INTSTREAM,36,100
			.byte	ANI_INTSTREAM,36,100
			.byte	ANI_INTSTREAM,36,100
			.byte	ANI_INTSTREAM,35,100

			.byte	ANI_GOTO
			.word	db_random_anim
		
			.byte	ANI_END


#;---------------- Shift
			.globl	shift_09
			.byte	SCRIPT
shift_09:
			
			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_SHIFT_09
			.byte	0

			.byte	ANI_INTSTREAM,35,80
			.byte	ANI_INTSTREAM,35,80
			.byte	ANI_INTSTREAM,35,80
			.byte	ANI_INTSTREAM,34,80

			.byte	ANI_GOTO
			.word	db_random_anim
		
			.byte	ANI_END

#;---------------- Shift - Shift backwards then forwards
			.globl	shift_10
			.byte	SCRIPT
shift_10:
			
			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_SHIFT_10
			.byte	0

			.byte	ANI_INTSTREAM,34,67
			.byte	ANI_INTSTREAM,34,67
			.byte	ANI_INTSTREAM,34,67 
			.byte	ANI_INTSTREAM,34,67

			.byte	ANI_GOTO
			.word	db_random_anim
			.byte	ANI_END


#;---------------- fall_04
			.globl	fall_04
			.byte	SCRIPT
fall_04:
			
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

#			.byte	ANI_SETFLAG
#			.word	PF_LATEHITABLE


			.byte	ANI_NEWSEQ
			.word	SEQ_FALL_04


	.if DOSETCAM
# If victim has ball, do some camera shit - otherwise, skip it
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,fall_04_cont
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex
#			.word	csi_zm_divetack
	.endif

fall_04_cont:

			.byte	ANI_SET_FACE_REL_VEL
			.float	1.2

			.byte	1,1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_SETYVEL
			.float	1.20

			.byte	1

			.byte	ANI_INTSTREAM,9,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float	.080

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_SHAKER,2
			.word	10

#			.byte	ANI_INTSTREAM,15,25

#			.byte	ANI_STREAMTOEND
#			.float	1.0
			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	-128

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;---------------- fall_04
			.globl	fall_04f
			.byte	SCRIPT
fall_04f:
			
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

#			.byte	ANI_SETFLAG
#			.word	PF_LATEHITABLE

			.byte	ANI_NEWSEQ
			.word	SEQ_FALL_04F


	.if DOSETCAM
# If victim has ball, do some camera shit - otherwise, skip it
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,fall_04f_cont
			.byte	ANI_SETCAMBOT
#			.word	csi_rs_suplex
			.word	csi_zm_divetack
	.endif

fall_04f_cont:

			.byte	ANI_SET_FACE_REL_VEL
			.float	1.2

			.byte	1,1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_SETYVEL
			.float	1.20

			.byte	1

			.byte	ANI_INTSTREAM,9,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float	.080

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_SHAKER,2
			.word	10

#			.byte	ANI_INTSTREAM,15,25

#			.byte	ANI_STREAMTOEND
#			.float	1.0

			.byte	ANI_RUNTOEND,1


			.byte	ANI_TURNHACK
			.word	128

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;---------------- Hit in front, getup and shake head
			.globl	fall_10
			.byte	SCRIPT
fall_10:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

#			.byte	ANI_SETFLAG
#			.word	PF_LATEHITABLE

			.byte	ANI_NEWSEQ
			.word	SEQ_FALL_10

	.if DOSETCAM
# If victim has ball, do some camera shit - otherwise, skip it
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,fall_10_cont
			.byte	ANI_SETCAMBOT
			.word	csi_sn_diveattack
#			.word	csi_zm_divetack
	.endif

fall_10_cont:

			.byte	ANI_SET_FACE_REL_VEL
			.float	1.2

			.byte	ANI_RUNTOFRAME,3,1


			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_SETYVEL
			.float	0.90

			.byte	1

			.byte	ANI_INTSTREAM,9,AA_TOGROUND

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float .060

			.byte	ANI_INTSTREAM,35,35

#			.byte	ANI_NEWSEQ
#			.word	SEQ_FALL_10

#			.byte	ANI_SETMODE
#			.word	MODE_ANCHORXZ

 #			.byte	ANI_SKIPFRAMES,47


			.byte	ANI_SETMODE
			.word	0


			.byte	ANI_STREAMTOEND
			.float	2.2

			.byte	ANI_TURNHACK
			.word	-64


			.byte	ANI_GOTO
			.word	stance_anim_int
			.byte	ANI_END

#			.byte	ANI_SETMODE
#			.word	MODE_UNINT

#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END


#;---------------- Hit in back, getup off his ass
			.globl	fall_12
			.byte	SCRIPT
fall_12:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

#			.byte	ANI_SETFLAG
#			.word	PF_LATEHITABLE

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

			.byte	ANI_NEWSEQ
			.word	SEQ_FALL_12

	.if DOSETCAM
# If victim has ball, do some camera shit - otherwise, skip it
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,fall_12_cont
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex_nf
#			.word	csi_zm_divetack
	.endif

fall_12_cont:

			.byte	ANI_SET_FACE_REL_VEL
			.float	1.2

			.byte	ANI_RUNTOFRAME,2,1

			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHORY
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_SETYVEL
			.float	1.00

			.byte	1

			.byte	ANI_INTSTREAM,11,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float	.045

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_SHAKER,2
			.word	10

			.byte	ANI_INTSTREAM,69,90

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND	
			.float	2.0

			.byte	ANI_TURNHACK
			.word	-256

			.byte	ANI_GOTO
			.word	stance_anim_int
			.byte	ANI_END









#;---------------- Hit in front, getup and shake head
			.globl	fall_18
			.byte	SCRIPT
fall_18:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

#			.byte	ANI_SETFLAG
#			.word	PF_LATEHITABLE

			.byte	ANI_NEWSEQ
			.word	SEQ_FALL_18

	.if DOSETCAM
# If victim has ball, do some camera shit - otherwise, skip it
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,fall_18_cont
			.byte	ANI_SETCAMBOT
			.word	csi_sn_diveattack
#			.word	csi_zm_divetack
	.endif

fall_18_cont:

			.byte	ANI_SET_FACE_REL_VEL
			.float	1.1

#			.byte	ANI_SETVELS
#			.float	0.0,0.0,0.60

			.byte	ANI_RUNTOFRAME,3,1


			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH


			.byte	ANI_SETYVEL
			.float	0.95

			.byte	1

			.byte	ANI_INTSTREAM,8,AA_TOGROUND

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float .035


			.byte	ANI_INTSTREAM,44,75

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND
			.float	1.7

			.byte	ANI_TURNHACK
			.word	-256

			.byte	ANI_GOTO
			.word	stance_anim_int
			.byte	ANI_END



#;;;;;;;;;;;;
			.globl	catch_01
			.byte	SCRIPT
catch_01:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_CATCH_01
	
			.byte	ANI_INTSTREAM,18,5

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_SETYVEL
			.float	1.20

	#in air for 30 ticks
			.byte	ANI_INTSTREAM,10,15
			.byte	ANI_INTSTREAM,28,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float .050
			
#			.byte	ANI_SETMODE
#			.word	MODE_ROTONLY

			.byte	ANI_RUNTOEND,1
#			.byte	ANI_INTSTREAM,30, 20

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;
			.globl	catch_02
			.byte	SCRIPT
catch_02:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_NEWSEQ
			.word	SEQ_CATCH_02
	
			.byte	ANI_INTSTREAM,23,5

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_SETYVEL
			.float	1.20

	#in air for 30 ticks
			.byte	ANI_INTSTREAM,11,15
			.byte	ANI_INTSTREAM,16,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float .050
			
#			.byte	ANI_SETMODE
#			.word	MODE_ROTONLY

#			.byte	ANI_RUNTOEND,1
			.byte	ANI_INTSTREAM,14,7

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

			.globl	catch_2f
			.byte	SCRIPT
catch_2f:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_NEWSEQ
			.word	SEQ_CATCH_2F

			.byte	ANI_SETFLAG
			.word	PF_FLIP
	
			.byte	ANI_INTSTREAM,23,5

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_SETYVEL
			.float	1.20

	#in air for 30 ticks
			.byte	ANI_INTSTREAM,11,15
			.byte	ANI_INTSTREAM,16,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float .050
			
#			.byte	ANI_SETMODE
#			.word	MODE_ROTONLY

#			.byte	ANI_RUNTOEND,1
			.byte	ANI_INTSTREAM,14,7

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;
			.globl	catch_03
			.byte	SCRIPT
catch_03:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_CATCH_03
	
			.byte	ANI_INTSTREAM,10,5

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_SETYVEL
			.float	1.50
	#in air for 30 ticks
			.byte	ANI_INTSTREAM,23,15
			.byte	ANI_INTSTREAM,10,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float .050
			
#			.byte	ANI_SETMODE
#			.word	MODE_ROTONLY

#			.byte	ANI_RUNTOFRAME,52,1
#			.byte	ANI_RUNTOEND,1

			.byte	ANI_INTSTREAM,11,6

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;
			.globl	catch_03f
			.byte	SCRIPT
catch_03f:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_CATCH_3B
	
			.byte	ANI_INTSTREAM,10,5

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_SETYVEL
			.float	1.50
	#in air for 30 ticks
			.byte	ANI_INTSTREAM,23,15
			.byte	ANI_INTSTREAM,10,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float .050
			
#			.byte	ANI_SETMODE
#			.word	MODE_ROTONLY

#			.byte	ANI_RUNTOFRAME,52,1
#			.byte	ANI_RUNTOEND,1

			.byte	ANI_INTSTREAM,11,6

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;
			.globl	catch_05
			.byte	SCRIPT
catch_05:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_CATCH_05
	
			.byte	ANI_INTSTREAM,11,5

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_SETYVEL
			.float	1.50
	#in air for 30 ticks
			.byte	ANI_INTSTREAM,10,15
			.byte	ANI_INTSTREAM,15,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float .050
			
#			.byte	ANI_SETMODE
#			.word	0
#			.word	MODE_ROTONLY

#			.byte	ANI_RUNTOFRAME,52,1
#			.byte	ANI_RUNTOEND,1

#			.byte	ANI_INTSTREAM,11,7
			.byte	ANI_INTSTREAM,67,25

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;
			.globl	catch_05f
			.byte	SCRIPT
catch_05f:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_CATCH_5B
	
			.byte	ANI_INTSTREAM,11,5

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_SETYVEL
			.float	1.50
	#in air for 30 ticks
			.byte	ANI_INTSTREAM,10,15
			.byte	ANI_INTSTREAM,15,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float .050
			
#			.byte	ANI_SETMODE
#			.word	0

#			.byte	ANI_RUNTOFRAME,52,1
#			.byte	ANI_RUNTOEND,1

#			.byte	ANI_INTSTREAM,11,7
			.byte	ANI_INTSTREAM,67,25

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;;;;
			.globl	catch_06
			.byte	SCRIPT
catch_06:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_CATCH_6B
	
			.byte	ANI_INTSTREAM,6,5

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY

			.byte	ANI_SETYVEL
			.float	0.70

#			.byte	ANI_INTSTREAM,12,12
			.byte	ANI_INTSTREAM,4,2
			.byte	ANI_INTSTREAM,8,AA_TOGROUND

#			.byte	ANI_SETFRICTION
#			.float .050
			
#			.byte	ANI_SETMODE
#			.word	MODE_ROTONLY

#			.byte	ANI_RUNTOFRAME,52,1
#			.byte	ANI_RUNTOEND,1

			.byte	ANI_INTSTREAM,23,8

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


#;;;;;;;;;;;;;;;;;;;;;;;;;	Bitch at each other

			.globl	p_argue_a_anim
			.globl	p_argue_v_anim
			.byte	SCRIPT
p_argue_a_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPETEER

			.byte	ANI_CODE
			.word	face_attacker,0,0,0

			.byte	ANI_ZEROVELS

	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,argue_cont
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex_nf
argue_cont:
	.endif

			.byte	ANI_NEWSEQ
			.word	SEQ_ARGUE_A


#			.byte	ANI_INTTOSEQ,6
#			.word	SEQ_ARGUE_A
#			.byte	0


#			.byte	ANI_STREAMTOEND
#			.float	1.5

#			.byte	ANI_INTSTREAM,179,268
#			.byte	ANI_INTSTREAM,89,134

			.byte	ANI_INTSTREAM,59,89
			.byte	ANI_INTSTREAM,59,89
			.byte	ANI_INTSTREAM,59,89									

			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_GOTO
			.word	mill_random_anim
			.byte	ANI_END
			

			.byte	SCRIPT
p_argue_v_anim:
			.byte	ANI_SETMODE
			.word	MODE_ANCHOR|MODE_UNINT|MODE_PUPPET

			.byte	ANI_CODE
			.word	face_attacker,0,0,0


			.byte	ANI_NEWSEQ
			.word	SEQ_ARGUE_V

#			.byte	ANI_INTTOSEQ,6
#			.word	SEQ_ARGUE_V
#			.byte	0


#			.byte	ANI_STREAMTOEND
#			.float	1.5

			.byte	ANI_INTSTREAM,59,89
			.byte	ANI_INTSTREAM,59,89
			.byte	ANI_INTSTREAM,59,89									


			.byte	ANI_CODE
			.word	release_puppet,0,0,0

			.byte	ANI_GOTO
			.word	mill_random_anim
			.byte	ANI_END

#;;;;;;;;;; TURBO just another hurdle
#			
#			.globl	turbo_hurd01_anim
#			.byte	SCRIPT
#turbo_hurd01_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_HURD_01
#
#			.byte	1
#			.byte	ANI_CODE
#			.word	maybe_dive,0,0,0
#			.byte	1
#			.byte	ANI_CODE
#			.word	maybe_dive,0,0,0
#			.byte	1
#			.byte	ANI_CODE
#			.word	maybe_dive,0,0,0
#			.byte	1
#			.byte	ANI_CODE
#			.word	maybe_dive,0,0,0
#			.byte	1
#			.byte	ANI_CODE
#			.word	maybe_dive,0,0,0
#			.byte	1
#			.byte	ANI_CODE
#			.word	maybe_dive,0,0,0
#			.byte	1
#			.byte	ANI_CODE
#			.word	maybe_dive,0,0,0
#			
#			.byte	ANI_CODE
#			.word	attack_snd,P_HURDLE_SND,0,0
#
#			.byte	ANI_SETYVEL
#			.float	1.25
#			
#			.byte	ANI_INTSTREAM,30,AA_TOGROUND
#
#			.byte	ANI_ATTACK_OFF
#			
#			.byte	ANI_INTSTREAM,14,6
#
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
#		
#
#;;;;;;;;;; NON-TURBO just another hurdle
#
#			.globl	hurd01_anim
#			.byte	SCRIPT
#hurd01_anim:
#			.byte	ANI_SETMODE
#			.word	MODE_UNINT|MODE_ANCHOR
#
#			.byte	ANI_NEWSEQ
#			.word	SEQ_HURD_01
#			
#			.byte	1
#			.byte	ANI_CODE
#			.word	maybe_dive,0,0,0
#			.byte	1
#			.byte	ANI_CODE
#			.word	maybe_dive,0,0,0
#			.byte	1
#			.byte	ANI_CODE
#			.word	maybe_dive,0,0,0
#			.byte	1
#			.byte	ANI_CODE
#			.word	maybe_dive,0,0,0
#			.byte	1
#			.byte	ANI_CODE
#			.word	maybe_dive,0,0,0
#			.byte	1
#			.byte	ANI_CODE
#			.word	maybe_dive,0,0,0
#			.byte	1
#			.byte	ANI_CODE
#			.word	maybe_dive,0,0,0
#			
#			.byte	ANI_CODE
#			.word	attack_snd,P_HURDLE_SND,0,0
#			
#			.byte	ANI_SETYVEL
#			.float	.85
#		
#			.byte	ANI_INTSTREAM,30,AA_TOGROUND
#
#			.byte	ANI_ATTACK_OFF
#
#			.byte	ANI_INTSTREAM,14,6
#			
#			.byte	ANI_PREP_XITION
#			.word	4,0
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END


#;;;;;;;;;; TURBO just another hurdle
			
			.globl	turbo_hurd03_anim
			.byte	SCRIPT
turbo_hurd03_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_HURD_03

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			
			.byte	ANI_CODE
			.word	attack_snd,P_HURDLE_SND,0,0

			.byte	ANI_SETYVEL
			.float	1.25
		
			.byte	ANI_INTSTREAM,36,AA_TOGROUND

			.byte	ANI_ATTACK_OFF
			
			.byte	ANI_INTSTREAM,15,4

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END
		

#;;;;;;;;;; NON-TURBO just another hurdle

			.globl	hurd03_anim
			.byte	SCRIPT
hurd03_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_HURD_03
			
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			
			.byte	ANI_CODE
			.word	attack_snd,P_HURDLE_SND,0,0
			
			.byte	ANI_SETYVEL
			.float	.85
				
			.byte	ANI_INTSTREAM,36,AA_TOGROUND

			.byte	ANI_ATTACK_OFF

			.byte	ANI_INTSTREAM,15,5
			
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;; TURBO just another hurdle
			
			.globl	turbo_hurd04_anim
			.byte	SCRIPT
turbo_hurd04_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_HURD_04

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			
			.byte	ANI_CODE
			.word	attack_snd,P_HURDLE_SND,0,0

			.byte	ANI_SETYVEL
			.float	1.25

			.byte	ANI_INTSTREAM,27,AA_TOGROUND

			.byte	ANI_ATTACK_OFF
			
			.byte	ANI_INTSTREAM,13,5

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END
		

#;;;;;;;;;; NON-TURBO just another hurdle

			.globl	hurd04_anim
			.byte	SCRIPT
hurd04_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_HURD_04
			
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			
			.byte	ANI_CODE
			.word	attack_snd,P_HURDLE_SND,0,0
			
			.byte	ANI_SETYVEL
			.float	.85
			
			.byte	ANI_INTSTREAM,27,AA_TOGROUND

			.byte	ANI_ATTACK_OFF

			.byte	ANI_INTSTREAM,13,5
			
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;; TURBO just another hurdle
			
			.globl	turbo_hurd07_anim
			.byte	SCRIPT
turbo_hurd07_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_HURD_07

			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			
			.byte	ANI_CODE
			.word	attack_snd,P_HURDLE_SND,0,0

			.byte	ANI_SETYVEL
			.float	1.25

			.byte	ANI_INTSTREAM,34,AA_TOGROUND

			.byte	ANI_ATTACK_OFF
			
			.byte	ANI_INTSTREAM,11,4

			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END
		

#;;;;;;;;;; NON-TURBO just another hurdle

			.globl	hurd07_anim
			.byte	SCRIPT
hurd07_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_HURD_07
			
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			
			.byte	ANI_CODE
			.word	attack_snd,P_HURDLE_SND,0,0
			
			.byte	ANI_SETYVEL
			.float	.85
			
			.byte	ANI_INTSTREAM,34,AA_TOGROUND

			.byte	ANI_ATTACK_OFF

			.byte	ANI_INTSTREAM,11,4
			
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END

#;;;;;;;;;; NON-TURBO just another hurdle

			.globl	hurdflip_anim
			.byte	SCRIPT
hurdflip_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_HURDFLIP
			
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			
			.byte	ANI_CODE
			.word	attack_snd,P_HURDLE_SND,0,0
			
			.byte	ANI_SETYVEL
			.float	1.3
			
			.byte	ANI_INTSTREAM,20,AA_TOGROUND

			.byte	ANI_ATTACK_OFF

			.byte	ANI_INTSTREAM,4,2
			
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END



			.globl	turbo_hurdflip_anim
			.byte	SCRIPT
turbo_hurdflip_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_NEWSEQ
			.word	SEQ_HURDFLIP
			
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			.byte	1
			.byte	ANI_CODE
			.word	maybe_dive,0,0,0
			
			.byte	ANI_CODE
			.word	attack_snd,P_HURDLE_SND,0,0
			
			.byte	ANI_SETYVEL
			.float	1.4
			
			.byte	ANI_INTSTREAM,20,AA_TOGROUND

			.byte	ANI_ATTACK_OFF

			.byte	ANI_INTSTREAM,4,2
			
			.byte	ANI_PREP_XITION
			.word	4,0
			.byte	ANI_CODE
			.word	general_run,0,0,0
			.byte	ANI_END


");

//-------------------------------------------------------------------------------------------------
// Sequences that are removed for SEATTLE VERSION!
//-------------------------------------------------------------------------------------------------

#if 1
asm("

#;;;;;;;;;;  player does a JOHN DANCE

			.globl	end_zone18_anim
			.byte	SCRIPT
end_zone18_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE


			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_EZJON1
			.byte	0

	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam10_d
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex
psv_nocam10_d:
	.endif


			
			.byte	ANI_CODE
			.word	drop_ball,0,0,0

			.byte	ANI_CODE
			.word	taunt_speech,0,0,0
			
			.byte	ANI_INTSTREAM,37,85
			.byte	ANI_INTSTREAM,37,85
			.byte	ANI_INTSTREAM,37,85
			.byte	ANI_INTSTREAM,37,85

#			.byte	ANI_RUNTOEND,1
			
			.byte	ANI_CLRFLAG
			.word	PF_CELEBRATE

			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END



#;;;;;;;;;;  player does a JOHN DANCE

			.globl	end_zone19_anim
			.byte	SCRIPT
end_zone19_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_EZJON2
			.byte	0
			

	.if DOSETCAM
#			.byte	ANI_SETCAMBOT
#			.word	csi_zm_divetack
# If victim has ball, do some camera shit - otherwise, skip it
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,cont_dive2
			.byte	ANI_SETCAMBOT
			.word	csi_zm_divetack
	.endif
cont_dive2:
			.byte	ANI_CODE
			.word	drop_ball,0,0,0

			.byte	ANI_CODE
			.word	taunt_speech,0,0,0
			
			.byte	ANI_INTSTREAM,37,85
			.byte	ANI_INTSTREAM,37,85
			.byte	ANI_INTSTREAM,37,85
			.byte	ANI_INTSTREAM,37,85
#			.byte	ANI_RUNTOEND,1
			
			.byte	ANI_CLRFLAG
			.word	PF_CELEBRATE

			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END

#;;;;;;;;;;  player does a end-zone (touchdown) dance

			.globl	end_zone2_anim
			.byte	SCRIPT
end_zone2_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ
			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_INZONE2
			.byte	0
			
			.byte	ANI_RUNTOFRAME,31,1
			
			.byte	ANI_CODE
			.word	taunt_speech,3,0,0
			
			.byte	ANI_RUNTOFRAME,82,1
			
			.byte	ANI_CODE
			.word	taunt_speech,3,0,0
			
			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	-320
			
			.byte	ANI_CLRFLAG
			.word	PF_CELEBRATE
			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END


#;;;;;;;;;;  player does a cart-wheel (touchdown) dance

			.globl	end_zone3_anim
			.byte	SCRIPT
end_zone3_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_ZEROVELS
			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE

#			.byte	ANI_NEWSEQ
#			.word	SEQ_INZONE3

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_INZONE3
			.byte	0

			.byte	ANI_CODE
			.word	drop_ball,0,0,0
			
#			.byte	ANI_CODE
#			.word	taunt_speech,1,0,0
			
			.byte	ANI_INTSTREAM,100,90
			.byte	ANI_INTSTREAM,100,90
			.byte	ANI_INTSTREAM,100,90
			
			.byte	ANI_CODE
			.word	taunt_speech,5,0,0
			
			.byte	ANI_INTSTREAM,87,77

#			.byte	ANI_RUNTOEND,1
			.byte	ANI_CLRFLAG
			.word	PF_CELEBRATE
			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END

#;;;;;;;;;;  player does a cart-wheel (touchdown) dance

			.globl	end_zone4_anim
			.byte	SCRIPT
end_zone4_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_ZEROVELS
			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_CELCHE
			.byte	0


			.byte	ANI_RUNTOFRAME,68,1
			
#			.byte	ANI_CODE
#			.word	taunt_speech,0,0,0
			
			.byte	ANI_RUNTOFRAME,120,1
			
#			.byte	ANI_CODE
#			.word	taunt_speech,2,0,0
			
			.byte	ANI_RUNTOFRAME,211,1
			
#			.byte	ANI_CODE
#			.word	taunt_speech,1,0,0
#			
			.byte	ANI_RUNTOEND,1
			
			.byte	ANI_CLRFLAG
			.word	PF_CELEBRATE

			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END


#;;;;;;;;;;  player pushs arms up to crowd

			.globl	end_zone13_anim
			.byte	SCRIPT
end_zone13_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_ZEROVELS
			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_EZPUSH
			.byte	0



	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam10_h
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex_nf
psv_nocam10_h:
	.endif

			.byte	ANI_INTSTREAM,36,85
			.byte	ANI_INTSTREAM,36,85
			.byte	ANI_INTSTREAM,36,85
			.byte	ANI_INTSTREAM,36,85

			.byte	ANI_TURNHACK
			.word	-576

			.byte	ANI_CLRFLAG
			.word	PF_CELEBRATE

			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END


#;;;;;;;;;;  player does the ercal shuffule (jason dance)

			.globl	end_zone14_anim
			.byte	SCRIPT
end_zone14_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORXZ
			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_EZJASN
			.byte	0

			.byte	ANI_RUNTOFRAME,14,1

			.byte	ANI_INTSTREAM,41,120
			.byte	ANI_INTSTREAM,41,120

			.byte	ANI_TURNHACK
			.word	-576

			.byte	ANI_CLRFLAG
			.word	PF_CELEBRATE

			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END


#;;;;;;;;;;  player just dances

			.globl	end_zone15_anim
			.byte	SCRIPT
end_zone15_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_ZEROVELS
			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_EZDNCE
			.byte	0


	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam10_i
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex
psv_nocam10_i:
	.endif

			.byte	ANI_INTSTREAM,46,105
			.byte	ANI_INTSTREAM,46,105
			.byte	ANI_INTSTREAM,46,105
			.byte	ANI_INTSTREAM,46,105

#			.byte	ANI_RUNTOEND,1

			.byte	ANI_TURNHACK
			.word	-768

			.byte	ANI_CLRFLAG
			.word	PF_CELEBRATE

			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END

#;;;;;;;;;;  player shrugs shoulders

			.globl	end_zone7_anim
			.byte	SCRIPT
end_zone7_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_ZEROVELS
			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_EZCWHA
			.byte	0

			.byte	ANI_INTSTREAM,100,95
			.byte	ANI_INTSTREAM,100,95
			.byte	ANI_INTSTREAM,73,71
			
#			.byte	ANI_RUNTOEND,1
			
			.byte	ANI_CLRFLAG
			.word	PF_CELEBRATE
			
			.byte	ANI_TURNHACK
			.word	512
			
			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END
			
#;;;;;;;;;;  player waves to crowd #2

			.globl	end_zone8_anim
			.byte	SCRIPT
end_zone8_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_ZEROVELS
			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE

#			.byte	ANI_NEWSEQ
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_EZWAV1
			.byte	0

			.byte	ANI_CODE
			.word	taunt_speech,3,0,0
			
			.byte	ANI_RUNTOEND,1
			
			.byte	ANI_CLRFLAG
			.word	PF_CELEBRATE
			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END

#;;;;;;;;;;  player spins himself dizzy

			.globl	end_zone10_anim
			.byte	SCRIPT
end_zone10_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			.byte	ANI_ZEROVELS
			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE
		
			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_EZCSPN
			.byte	0

	.if DOSETCAM
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,psv_nocam10_x
			
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex_nf
psv_nocam10_x:
	.endif


			.byte	ANI_INTSTREAM,100,80
			.byte	ANI_INTSTREAM,100,80
			
			.byte	ANI_CODE
			.word	taunt_speech,19,0,0
			
			.byte	ANI_INTSTREAM,100,80
			
			.byte	ANI_INTSTREAM,24,18
			
			.byte	ANI_CLRFLAG
			.word	PF_CELEBRATE
			
			.byte	ANI_TURNHACK
			.word	256
			
			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END


#;;;;;;;;;;  player DOES A HORSEY DANCE

			.globl	end_zone11_anim
			.byte	SCRIPT
end_zone11_anim:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR
			
			.byte	ANI_ZEROVELS
			
			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE
			
#			.byte	ANI_NEWSEQ
#			.word	SEQ_M_EZCHRS

			.byte	ANI_INTTOSEQ,6
			.word	SEQ_M_EZCHRS
			.byte	0
			
#			.byte	ANI_CODE
#			.word	attack_snd,179,0,0
			
			.byte	ANI_INTSTREAM,37,90
			.byte	ANI_INTSTREAM,38,90
			.byte	ANI_INTSTREAM,38,90
			.byte	ANI_INTSTREAM,37,90
			.byte	ANI_RUNTOEND,1
			
			.byte	ANI_CLRFLAG
			.word	PF_CELEBRATE
			
			.byte	ANI_TURNHACK
			.word	512
			
			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END

#;;;;;;;;;;;;; Milling around

			.globl	fb_mill3
			.byte	SCRIPT
fb_mill3:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

#			.byte	ANI_CLRFLAG
#			.word	PF_LATEHITABLE

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_MILL_03
			.byte	0

#			.byte	ANI_RUNTOEND,1

			.byte	ANI_INTSTREAM,39,120
			.byte	ANI_INTSTREAM,39,120
			.byte	ANI_INTSTREAM,39,120
			.byte	ANI_INTSTREAM,39,120


			.byte	ANI_TURNHACK
			.word	384

#			.byte	ANI_GOTO
#			.word	stance_anim_int
#			.byte	ANI_END
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END

			.byte	ANI_GOTO
			.word	mill_random_anim


#;;;;;;;;;;;;; Milling around

			.globl	fb_mill5
			.byte	SCRIPT
fb_mill5:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

#			.byte	ANI_CLRFLAG
#			.word	PF_LATEHITABLE

			.byte	ANI_INTTOSEQ,14
			.word	SEQ_MILL_05
			.byte	0

			.byte	ANI_INTSTREAM,47,70
			.byte	ANI_INTSTREAM,46,70

			.byte	ANI_TURNHACK
#			.word	192
			.word	256

#			.byte	ANI_GOTO
#			.word	stance_anim_int
#			.byte	ANI_END

#			.byte	ANI_PREP_XITION
#			.word	4,0

#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END
			.byte	ANI_GOTO
			.word	mill_random_anim

#;;;;;;;;;;;;; Milling around

			.globl	fb_mill10
			.byte	SCRIPT
fb_mill10:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

#			.byte	ANI_CLRFLAG
#			.word	PF_LATEHITABLE

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_MILL_10
			.byte	0

			.byte	ANI_INTSTREAM,30,91
			.byte	ANI_INTSTREAM,30,91

			.byte	ANI_TURNHACK
			.word	512

#			.byte	ANI_GOTO
#			.word	stance_anim_int
#			.byte	ANI_END
#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END

			.byte	ANI_GOTO
			.word	mill_random_anim

#;;;;;;;;;;;;; Milling around

			.globl	fb_mill14
			.byte	SCRIPT
fb_mill14:
			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

#			.byte	ANI_CLRFLAG
#			.word	PF_LATEHITABLE

			.byte	ANI_INTTOSEQ,14
			.word	SEQ_MILL_14
			.byte	0

			.byte	ANI_INTSTREAM,52,127

			.byte	ANI_TURNHACK
			.word	512

#			.byte	ANI_GOTO
#			.word	stance_anim_int
#			.byte	ANI_END

#			.byte	ANI_CODE
#			.word	general_run,0,0,0
#			.byte	ANI_END

			.byte	ANI_GOTO
			.word	mill_random_anim


#; Cartwheel
			.globl endz_20
			.byte	SCRIPT
endz_20:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR

			.byte	ANI_ZEROVELS

			.byte	ANI_SETFLAG
			.word	PF_CELEBRATE

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_ENDZ_20
			.byte	0

	.if DOSETCAM
#			.byte	ANI_SETCAMBOT
#			.word	csi_zm_divetack
# If victim has ball, do some camera shit - otherwise, skip it
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,endz_20_cont
			.byte	ANI_SETCAMBOT
			.word	csi_zm_divetack
	.endif
endz_20_cont:
			
			.byte	ANI_CODE
			.word	drop_ball,0,0,0

			.byte	ANI_CODE
			.word	taunt_speech,0,0,0


			.byte	ANI_INTSTREAM,42,60
			.byte	ANI_INTSTREAM,42,60
			.byte	ANI_INTSTREAM,42,60
			.byte	ANI_INTSTREAM,42,60

			.byte	ANI_TURNHACK
			.word	256

			.byte	ANI_CLRFLAG
			.word	PF_CELEBRATE

			.byte	ANI_GOTO
			.word	breath_anim
			.byte	ANI_END

#;---------------- Shift
			.globl	shift_01
			.byte	SCRIPT
shift_01:
			
			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_SHIFT_01
			.byte	0

			.byte	ANI_INTSTREAM,36,68 
			.byte	ANI_INTSTREAM,36,68 
			.byte	ANI_INTSTREAM,36,68 
			.byte	ANI_INTSTREAM,36,68 

#			.byte	ANI_INTSTREAM,72,126
#			.byte	ANI_INTSTREAM,72,126

			.byte	ANI_GOTO
#			.word	db_random_anim
			.word	shift_01
		
			.byte	ANI_END


#;---------------- Shift
			.globl	shift_02
			.byte	SCRIPT
shift_02:
			
			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_SHIFT_02
			.byte	0

			.byte	ANI_INTSTREAM,43,78
			.byte	ANI_INTSTREAM,43,78
			.byte	ANI_INTSTREAM,43,78
			.byte	ANI_INTSTREAM,43,78

			.byte	ANI_GOTO
			.word	db_random_anim
		
			.byte	ANI_END


#;---------------- Shift
			.globl	shift_04
			.byte	SCRIPT
shift_04:
			
			.byte	ANI_ZEROVELS

			.byte	ANI_SETMODE
			.word	MODE_ANCHORXZ

			.byte	ANI_INTTOSEQ,10
			.word	SEQ_SHIFT_04
			.byte	0

			.byte	ANI_INTSTREAM,43,95
			.byte	ANI_INTSTREAM,43,95
			.byte	ANI_INTSTREAM,43,95
			.byte	ANI_INTSTREAM,42,95

			.byte	ANI_GOTO
			.word	db_random_anim
		
			.byte	ANI_END

#;---------------- Hit in back, getup and kick
			.globl	fall_13
			.byte	SCRIPT
fall_13:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0

#			.byte	ANI_SETFLAG
#			.word	PF_LATEHITABLE

			.byte	ANI_NEWSEQ
			.word	SEQ_FALL_13

	.if DOSETCAM
# If victim has ball, do some camera shit - otherwise, skip it
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,fall_13_cont
			.byte	ANI_SETCAMBOT
			.word	csi_sn_diveattack
#			.word	csi_zm_divetack
	.endif

fall_13_cont:

			.byte	ANI_SET_FACE_REL_VEL
			.float	1.20

			.byte	ANI_RUNTOFRAME,2,1

			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH

			.byte	ANI_SETYVEL
			.float	0.80

			.byte	1

			.byte	ANI_INTSTREAM,7,AA_TOGROUND

			.byte	ANI_SETFRICTION
			.float	.045

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_SHAKER,2
			.word	10

			.byte	ANI_INTSTREAM,55,60

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND	
			.float	1.6

			.byte	ANI_TURNHACK
			.word	256


			.byte	ANI_GOTO
			.word	stance_anim_int
			.byte	ANI_END


#;---------------- Hit in back, getup and turn around ankle soar
			.globl	fall_16
			.byte	SCRIPT
fall_16:
			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHOR|MODE_REACTING

			.byte	ANI_CODE
			.word	get_hit_snd,-1,-1,0


#			.byte	ANI_SETFLAG
#			.word	PF_LATEHITABLE


			.byte	ANI_NEWSEQ
			.word	SEQ_FALL_16

	.if DOSETCAM
# If victim has ball, do some camera shit - otherwise, skip it
			.byte	ANI_CODE_BZ
			.word	plyr_have_ball,0,0,0,fall_16_cont
			.byte	ANI_SETCAMBOT
			.word	csi_rs_suplex_nf
#			.word	csi_zm_divetack
	.endif

fall_16_cont:
			.byte	ANI_SET_FACE_REL_VEL
			.float	1.2

			.byte	ANI_RUNTOFRAME,3,1


			.byte	ANI_SETMODE
			.word	MODE_UNINT|MODE_ANCHORY|MODE_REACTING|MODE_NOCATCH


			.byte	ANI_SETYVEL
			.float	0.80

			.byte	1

			.byte	ANI_INTSTREAM,8,AA_TOGROUND

			.byte	ANI_CODE
			.word	player_down,0,0,0

			.byte	ANI_CODE
			.word	grnd_hit_snd,-1,0,-1

			.byte	ANI_CODE
			.word	dust_cloud,0,0,0

			.byte	ANI_SHAKER,3
			.word	10

			.byte	ANI_SETFRICTION
			.float .060

			.byte	ANI_INTSTREAM,47,80

			.byte	ANI_SETMODE
			.word	0

			.byte	ANI_STREAMTOEND	
			.float	2.5

#			.byte	ANI_INTSTREAM,40,90
#			.byte	ANI_INTSTREAM,40,90

			.byte	ANI_TURNHACK
			.word	512

			.byte	ANI_GOTO
			.word	stance_anim_int
			.byte	ANI_END



");

#endif
//-------------------------------------------------------------------------------------------------