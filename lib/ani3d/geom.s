;;
;; geom.S	Geometry pipeline.
;; draw_tris_fb is r5000-approved.  xform_limb_fs needs work.
;;
;; $Header: /video/nfl/lib/ani3d/geom.s 10    2/22/99 12:37p Bdabelst $

;; structure definitions

limb_nvtx=	0
limb_ntris=	4
limb_pvtx=	12
limb_pst=	16
limb_ptri=	20
limb_size=	28

midv_x=		0
midv_y=		4
midv_oow=	8
midv_sow=	12
midv_tow=	16
midv_size=	24

tri_v1=		0
tri_v2=		2
tri_v3=		4
tri_t1=		6
tri_t2=		8
tri_t3=		10
tri_texture=	12
tri_size=	14
ntri_texture=	tri_texture+tri_size

st_s=		0
st_t=		4
st_size=	8

vtx_x=		0
vtx_y=		4
vtx_z=		8
vtx_size=	12

	xref	guTexSource
	xref	grTexLodBiasValue
	xref	grDrawTriangleDma
	xref	guDrawTriangleWithClip

	section	.text

;;############################################################################
;; void xform_limb_fs( LIMB *limb, float *transform, MidVertex *dest, int low_res );
;; Transforms a flat-shaded limb.
;; note: *dest should include space for one junk entry at the beginning
;; note: this will bomb if the limb contains only one vertex.  But no limb
;;       should have fewer than three anyway.
;; note: Nothing is saved on the stack.

	xdef	xform_limb_fs

;; int reg usage:
;; t0 & t1 - temps
;; t2, t3, t4, t5 - save values of f20, f22, f24, and f26
;; t6 - src pointer
;; unneeded -> t7 - replacement val for z=0
;; t8 - loop counter
;; t9 - holds status register

VLCNT	equr	t8
DEST	equr	a2
SRC		equr	t6

;; fp reg usage:
;;  f0-f11 - xform matrix
;;  f12-f14 - px, py, pz
;;  f15-f17 - tx, ty, tz (work)
;;  f18-f20 - x, y, oow (final)

;;  f23 - k0 (HRES * oow)
;;  f24 - (float)(HRES)				0x4400 0000
;;  f25 - (float)(1<<21 + HRES/2)	0x4a00 0400
;;  f26 - (float)(1<<21 + VRES/2)	0x4a00 0300
;;  f27 - (float)(1<<21)			0x4a00 0000
;;  f28 - aspect ratio

xform_limb_fs:

	;; set FPU for 32-register mode (set bit 26 of status register)
	mfc0	t9,r12
	lui		t0,0x0400
	or		t0,t0,t9
	mtc0	t0,r12

	;; save FPU registers
	mfc1	t2,f20
	mfc1	t3,f22
	mfc1	t4,f24
	mfc1	t5,f26

	;; init src ptr
	lw		SRC,limb_pvtx(a0)

	;; init loop counter
	lw		VLCNT,limb_nvtx(a0)

	;; load the matrix
	lwc1	f0,0(a1)
	lwc1	f1,4(a1)
	lwc1	f2,8(a1)
	lwc1	f3,12(a1)
	lwc1	f4,16(a1)
	lwc1	f5,20(a1)
	lwc1	f6,24(a1)
	lwc1	f7,28(a1)
	lwc1	f8,32(a1)
	lwc1	f9,36(a1)
	lwc1	f10,40(a1)
	lwc1	f11,44(a1)

	;; init constants
;;	lui		t7,0x3586
;;	ori		t7,0x37BD
;;	lui		t0,0x4400		;;512
;;	lui		t0,0x4440		;;768
	lui		t0,0x4426		;;664
	mtc1	t0,f24
	lui		t0,0x4a00
	mtc1	t0,f27
	ori		t1,t0,0x0400
	mtc1	t1,f25

	;; after this point, t1 can no longer be used as
	;; a temp.  It holds the save value of f28
	bne		a3,zero,low_res
	mfc1	t1,f28

med_res:
	ori		t0,t0,0x0300
	mtc1	t0,f26

	lui		t0,0x3f80
	j		xres
	ori		t0,t0,0x0000

low_res:
	ori		t0,t0,0x0200
	mtc1	t0,f26

	lui		t0,0x3f2a
	ori		t0,t0,0xaaab

xres:
	mtc1	t0,f28

	lwc1	f12,vtx_x(SRC)		;; get px
	lwc1	f13,vtx_y(SRC)		;; get py
	lwc1	f14,vtx_z(SRC)		;; get pz

	;; first vertex (xform & recip only)

	madd.s	f17,f11,f12,f8		;; tz = m11 + px * m8;
	madd.s	f16,f7,f12,f4		;; ty = m7 + px * m4;
	madd.s	f15,f3,f12,f0		;; tx = m3 + px * m0;
	;nop
	madd.s	f17,f17,f13,f9		;; tz += py * m9;
	madd.s	f16,f16,f13,f5		;; ty += py * m5;
	madd.s	f15,f15,f13,f1		;; tx += py * m1;
	;nop
	madd.s	f17,f17,f14,f10		;; tz += pz * m10;
	madd.s	f16,f16,f14,f6		;; ty += pz * m6;
	nmadd.s	f15,f15,f14,f2		;; tx += pz * m2;
	addi	VLCNT,VLCNT,-1
	recip.s	f20,f17
	mov.s	f19,f16
	mov.s	f18,f15
	addi	SRC,SRC,vtx_size	;; inc src
	lwc1	f12,vtx_x(SRC)		;; get px
	lwc1	f13,vtx_y(SRC)		;; get py
	lwc1	f14,vtx_z(SRC)		;; get pz
	;many nops

	;; main loop (project, clamp, & write n-1, xform & recip n)

vtx_loop:

	madd.s	f17,f11,f12,f8		;; tz = m11 + px * m8;
	madd.s	f16,f7,f12,f4		;; ty = m7 + px * m4;
	madd.s	f15,f3,f12,f0		;; tx = m3 + px * m0;

	mul.s	f23,f20,f24			;;;; k0 = HRES * oow

	mul.s	f19,f19,f28			;; y *= aspect ratio

	madd.s	f17,f17,f13,f9		;; tz += py * m9;
	madd.s	f16,f16,f13,f5		;; ty += py * m5;
	madd.s	f15,f15,f13,f1		;; tx += py * m1;

	madd.s	f18,f25,f18,f23		;;;; x = tx * k0 + (HRES/2 + clamp)
	madd.s	f19,f26,f19,f23		;;;; y = ty * k0 + (VRES/2 + clamp)

	madd.s	f17,f17,f14,f10		;; tz += pz * m10;
	madd.s	f16,f16,f14,f6		;; ty += pz * m6;
	nmadd.s	f15,f15,f14,f2		;; tx += pz * m2;

	sub.s	f18,f18,f27			;;;; x -= clamp
	sub.s	f19,f19,f27			;;;; y -= clamp

	swc1	f20,midv_oow(DEST)	;;;; write oow

	recip.s	f20,f17
	swc1	f18,midv_x(DEST)	;;;; write x
	swc1	f19,midv_y(DEST)	;;;; write y
	mov.s	f19,f16
	mov.s	f18,f15
	addi	VLCNT,VLCNT,-1
	addi	SRC,SRC,vtx_size	;; inc src
	lwc1	f12,vtx_x(SRC)		;; get px
	lwc1	f13,vtx_y(SRC)		;; get py
	lwc1	f14,vtx_z(SRC)		;; get pz

	bne		VLCNT,zero,vtx_loop
	addi	DEST,DEST,midv_size	;;;; inc dest

	;; final vertex (project, clamp, & write only)

	mul.s	f19,f19,f28			;; y *= aspect ratio
	mul.s	f23,f20,f24			;;;; k0 = HRES * oow
	madd.s	f18,f25,f18,f23		;;;; x = tx * k0 + (HRES/2 + clamp)
	madd.s	f19,f26,f19,f23		;;;; y = ty * k0 + (VRES/2 + clamp)
	sub.s	f18,f18,f27			;;;; x -= clamp
	sub.s	f19,f19,f27			;;;; y -= clamp
	swc1	f18,midv_x(DEST)	;;;; write x
	swc1	f19,midv_y(DEST)	;;;; write y
	swc1	f20,midv_oow(DEST)	;;;; write oow

	;; put FPU back in 16-register mode
	mtc0	t9,r12

	;; restore FPU registers
	mtc1	t2,f20
	mtc1	t3,f22
	mtc1	t4,f24
	mtc1	t1,f28

	jr		ra			;; done

	mtc1	t5,f26

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; float fsqrt( float f )
;
;	xdef	fsqrt
;
;fsqrt:
;	jr	ra
;	sqrt.s	f0,f12


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; void draw_tris_noar( LIMB *limb, GrMipMipID_t *decals, MidVertex *vtx, int draw_mode )
;; draw triangles, skip aspect ratio calculation


	xdef	draw_tris_noar

PTRI	equr	s0
DECALS	equr	s1
JVAL	equr	s2
TTEX	equr	s3
TABLE	equr	s4
PVTX	equr	s5
PST		equr	s6

VERT1	equr	a2
VERT2	equr	a1
VERT3	equr	a0

ST1		equr	t3
ST2		equr	t4
ST3		equr	t5

	dw		dtf_done						;; code == -5
	dw		no_new_verts					;; code == -4
	dw		one_new_vert					;; code == -3
	dw		two_new_verts					;; code == -2
	dw		three_new_verts					;; code == -1
tc_functions:
	dw		all_new,all_new,all_new,all_new	;; code >= 0
	dw		all_new,all_new,all_new,all_new
	dw		all_new,all_new,all_new,all_new
	dw		all_new,all_new,all_new,all_new

draw_tris_noar:

	addi	sp,sp,-32
	sw		s0,0(sp)
	sw		s1,4(sp)
	sw		s2,8(sp)
	sw		s3,12(sp)
	sw		s4,16(sp)
	sw		s5,20(sp)
	sw		s6,24(sp)
	sw		ra,28(sp)

	;; a3, draw mode, gets passed right on to grDrawTriangle
	lw		PTRI,limb_ptri(a0)	;; load triangle pointer
	move	DECALS,a1
	la		s4,tc_functions
	move	PVTX,a2				;; save vtx base ptr

	;; get texture, then fall through into all-new case
	lh		TTEX,tri_texture(PTRI)
	lw		PST,limb_pst(a0)	;; load st base ptr


all_new:
	add		TTEX,TTEX,DECALS	;; get decals[TTEX]
	jal		guTexSource
	lw		a0,0(TTEX)
	;; fall through into 3-new case


three_new_verts:
	;; vertex 1
	lhu		ST1,tri_t1(PTRI)
	lhu		VERT1,tri_v1(PTRI)
	sll		ST1,ST1,3
	sll		VERT1,VERT1,3
	addu	ST1,ST1,PST
	addu	VERT1,VERT1,PVTX
	lwc1	f6,st_s(ST1)
	lwc1	f4,midv_oow(VERT1)
	lwc1	f8,st_t(ST1)
	mul.s	f6,f6,f4
	mul.s	f8,f8,f4

	lhu		VERT2,tri_v2(PTRI)
	lhu		ST2,tri_t2(PTRI)

	swc1	f6,midv_sow(VERT1)
	swc1	f8,midv_tow(VERT1)

	;; vertex 2
	sll		VERT2,VERT2,3
	sll		ST2,ST2,3
	addu	VERT2,VERT2,PVTX
	addu	ST2,ST2,PST
	lwc1	f4,midv_oow(VERT2)
	lwc1	f6,st_s(ST2)
	lwc1	f8,st_t(ST2)
	mul.s	f6,f6,f4
	mul.s	f8,f8,f4

	lhu		VERT3,tri_v3(PTRI)
	lhu		ST3,tri_t3(PTRI)

	swc1	f6,midv_sow(VERT2)
	swc1	f8,midv_tow(VERT2)

	;; vertex 3
	sll		VERT3,VERT3,3
	sll		ST3,ST3,3
	addu	VERT3,VERT3,PVTX
	addu	ST3,ST3,PST
	lwc1	f4,midv_oow(VERT3)
	lwc1	f6,st_s(ST3)
	lwc1	f8,st_t(ST3)
	mul.s	f6,f6,f4
	mul.s	f8,f8,f4

	lh		TTEX,ntri_texture(PTRI)	;; get next tri texture
	add		JVAL,TTEX,TABLE

	swc1	f6,midv_sow(VERT3)
	swc1	f8,midv_tow(VERT3)

	jal		grDrawTriangleDma
	lw		JVAL,0(JVAL)

	jr		JVAL
	addi	PTRI,PTRI,tri_size


one_new_vert:
	lhu		ST1,tri_t1(PTRI)
	lhu		VERT1,tri_v1(PTRI)
	sll		ST1,ST1,3
	sll		VERT1,VERT1,3
	addu	ST1,ST1,PST
	addu	VERT1,VERT1,PVTX
	lwc1	f6,st_s(ST1)
	lwc1	f4,midv_oow(VERT1)
	lwc1	f8,st_t(ST1)
	mul.s	f6,f6,f4
	mul.s	f8,f8,f4

	lhu		VERT2,tri_v2(PTRI)
	lhu		VERT3,tri_v3(PTRI)
	sll		VERT2,VERT2,3
	addu	VERT2,VERT2,PVTX
	sll		VERT3,VERT3,3
	addu	VERT3,VERT3,PVTX

	swc1	f6,midv_sow(VERT1)

	lh		TTEX,ntri_texture(PTRI)	;; get next tri texture
	swc1	f8,midv_tow(VERT1)
	add		JVAL,TTEX,TABLE

	jal		grDrawTriangleDma
	lw		JVAL,0(JVAL)

	jr		JVAL
	addi	PTRI,PTRI,tri_size


two_new_verts:
	lhu		ST1,tri_t1(PTRI)
	lhu		VERT1,tri_v1(PTRI)
	sll		ST1,ST1,3
	sll		VERT1,VERT1,3
	addu	ST1,ST1,PST
	addu	VERT1,VERT1,PVTX
	lwc1	f6,st_s(ST1)
	lwc1	f4,midv_oow(VERT1)
	lwc1	f8,st_t(ST1)
	mul.s	f6,f6,f4
	mul.s	f8,f8,f4

	lhu		ST2,tri_t2(PTRI)
	lhu		VERT2,tri_v2(PTRI)
	sll		ST2,ST2,3
	sll		VERT2,VERT2,3
	addu	ST2,ST2,PST
	addu	VERT2,VERT2,PVTX

	swc1	f6,midv_sow(VERT1)
	swc1	f8,midv_tow(VERT1)

	lhu		VERT3,tri_v3(PTRI)

	lwc1	f4,midv_oow(VERT2)
	lwc1	f6,st_s(ST2)
	lwc1	f8,st_t(ST2)
	mul.s	f6,f6,f4
	mul.s	f8,f8,f4

	sll		VERT3,VERT3,3
	addu	VERT3,VERT3,PVTX
	lh		TTEX,ntri_texture(PTRI)	;; get next tri texture

	swc1	f6,midv_sow(VERT2)
	swc1	f8,midv_tow(VERT2)

	add		JVAL,TTEX,TABLE

	jal		grDrawTriangleDma
	lw		JVAL,0(JVAL)

	jr		JVAL
	addi	PTRI,PTRI,tri_size


no_new_verts:
	lhu		VERT1,tri_v1(PTRI)
	lhu		VERT2,tri_v2(PTRI)
	lhu		VERT3,tri_v3(PTRI)
	sll		VERT1,VERT1,3
	addu	VERT1,VERT1,PVTX
	sll		VERT2,VERT2,3
	addu	VERT2,VERT2,PVTX
	sll		VERT3,VERT3,3
	lh		TTEX,ntri_texture(PTRI)	;; get next tri texture
	addu	VERT3,VERT3,PVTX
	add		JVAL,TTEX,TABLE

	jal		grDrawTriangleDma
	lw		JVAL,0(JVAL)

	jr		JVAL
	addi	PTRI,PTRI,tri_size


dtf_done:
	lw		s0,0(sp)
	lw		s1,4(sp)
	lw		s2,8(sp)
	lw		s3,12(sp)
	lw		s4,16(sp)
	lw		s5,20(sp)
	lw		s6,24(sp)
	lw		ra,28(sp)
	addi	sp,sp,32

	jr		ra
	nop		;; branch delay

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; void draw_tris( LIMB *limb, GrMipMipID_t *decals, int *ratios, MidVertex *vtx, int draw_mode )
;; draw triangles, perform aspect ratio calculations

	xdef	draw_tris

;PTRI	equr	s0
;DECALS	equr	s1
;JVAL	equr	s2
;TTEX	equr	s3
;TABLE	equr	s4
;PVTX	equr	s5
;PST	equr	s6
RATIOS	equr	s7

;VERT1	equr	a2
;VERT2	equr	a1
;VERT3	equr	a0

;ST1	equr	t3
;ST2	equr	t4
;ST3	equr	t5

;SF		equr	f20
;TF		equr	f22

	dw		dt_done							;; code == -5
	dw		dt_no_new_verts					;; code == -4
	dw		dt_one_new_vert					;; code == -3
	dw		dt_two_new_verts  				;; code == -2
	dw		dt_three_new_verts				;; code == -1
dtc_functions:
	dw		dt_all_new,dt_all_new,dt_all_new,dt_all_new	;; code >= 0
	dw		dt_all_new,dt_all_new,dt_all_new,dt_all_new
	dw		dt_all_new,dt_all_new,dt_all_new,dt_all_new
	dw		dt_all_new,dt_all_new,dt_all_new,dt_all_new
	dw		dt_all_new,dt_all_new,dt_all_new,dt_all_new
	dw		dt_all_new,dt_all_new,dt_all_new,dt_all_new
	dw		dt_all_new,dt_all_new,dt_all_new,dt_all_new
	dw		dt_all_new,dt_all_new,dt_all_new,dt_all_new

ar_values:
	ieee32	256.0,	32.0
	ieee32	256.0,	64.0
	ieee32	256.0,	128.0
	ieee32	256.0,	256.0
	ieee32	128.0,	256.0
	ieee32	64.0,	256.0
	ieee32	32.0,	256.0

draw_tris:

	addi	sp,sp,-48
	sw		s0,0(sp)
	sw		s1,4(sp)
	sw		s2,8(sp)
	sw		s3,12(sp)
	sw		s4,16(sp)
	sw		s5,20(sp)
	sw		s6,24(sp)
	sw		s7,28(sp)
	swc1	f20,32(sp)
	swc1	f22,36(sp)
	sw		ra,40(sp)

	;; v0, draw mode, gets written to a3 & passed right on to grDrawTriangle
	lw		PTRI,limb_ptri(a0)	;; load triangle pointer
	move	DECALS,a1
	move	RATIOS,a2
	la		TABLE,dtc_functions
	move	PVTX,a3				;; save vtx base ptr
	move	a3,v0

	;; get texture, then fall through into all-new case
	lh		TTEX,tri_texture(PTRI)
	lw		PST,limb_pst(a0)	;; load st base ptr


dt_all_new:
	add		a0,TTEX,DECALS	;; get decals[TTEX]
	jal		guTexSource
	lw		a0,0(a0)

	;; get new sf & tf
	;; fall through into 3-new case
	add		t0,TTEX,RATIOS
	lw		t0,0(t0)
	la		t1,ar_values
	sll		t0,t0,3
	add		t0,t0,t1
	lwc1	f20,0(t0)
	lwc1	f22,4(t0)


dt_three_new_verts:
	;; vertex 1
	lhu		ST1,tri_t1(PTRI)
	lhu		VERT1,tri_v1(PTRI)
	sll		ST1,ST1,3
	sll		VERT1,VERT1,3
	addu	ST1,ST1,PST
	addu	VERT1,VERT1,PVTX
	lwc1	f6,st_s(ST1)
	lwc1	f4,midv_oow(VERT1)
	lwc1	f8,st_t(ST1)
	mul.s	f6,f6,f4
	mul.s	f8,f8,f4
	mul.s	f6,f6,f20
	mul.s	f8,f8,f22

	lhu		VERT2,tri_v2(PTRI)
	lhu		ST2,tri_t2(PTRI)

	swc1	f6,midv_sow(VERT1)
	swc1	f8,midv_tow(VERT1)

	;; vertex 2
	sll		VERT2,VERT2,3
	sll		ST2,ST2,3
	addu	VERT2,VERT2,PVTX
	addu	ST2,ST2,PST
	lwc1	f4,midv_oow(VERT2)
	lwc1	f6,st_s(ST2)
	lwc1	f8,st_t(ST2)
	mul.s	f6,f6,f4
	mul.s	f8,f8,f4
	mul.s	f6,f6,f20
	mul.s	f8,f8,f22

	lhu		VERT3,tri_v3(PTRI)
	lhu		ST3,tri_t3(PTRI)

	swc1	f6,midv_sow(VERT2)
	swc1	f8,midv_tow(VERT2)

	;; vertex 3
	sll		VERT3,VERT3,3
	sll		ST3,ST3,3
	addu	VERT3,VERT3,PVTX
	addu	ST3,ST3,PST
	lwc1	f4,midv_oow(VERT3)
	lwc1	f6,st_s(ST3)
	lwc1	f8,st_t(ST3)
	mul.s	f6,f6,f4
	mul.s	f8,f8,f4
	mul.s	f6,f6,f20
	mul.s	f8,f8,f22

	lh		TTEX,ntri_texture(PTRI)	;; get next tri texture
	swc1	f6,midv_sow(VERT3)
	add		JVAL,TTEX,TABLE
	lw		JVAL,0(JVAL)

	jal		guDrawTriangleWithClip
	swc1	f8,midv_tow(VERT3)

	jr		JVAL
	addi	PTRI,PTRI,tri_size


dt_one_new_vert:
	lhu		ST1,tri_t1(PTRI)
	lhu		VERT1,tri_v1(PTRI)
	sll		ST1,ST1,3
	sll		VERT1,VERT1,3
	addu	ST1,ST1,PST
	addu	VERT1,VERT1,PVTX
	lwc1	f6,st_s(ST1)
	lwc1	f4,midv_oow(VERT1)
	lwc1	f8,st_t(ST1)
	mul.s	f6,f6,f4
	mul.s	f8,f8,f4
	mul.s	f6,f6,f20
	mul.s	f8,f8,f22

	lhu		VERT2,tri_v2(PTRI)
	lhu		VERT3,tri_v3(PTRI)
	sll		VERT2,VERT2,3
	addu	VERT2,VERT2,PVTX
	sll		VERT3,VERT3,3


	lh		TTEX,ntri_texture(PTRI)	;; get next tri texture
	addu	VERT3,VERT3,PVTX
	add		JVAL,TTEX,TABLE
	swc1	f6,midv_sow(VERT1)
	lw		JVAL,0(JVAL)

	jal		guDrawTriangleWithClip
	swc1	f8,midv_tow(VERT1)

	jr		JVAL
	addi	PTRI,PTRI,tri_size


dt_two_new_verts:
	lhu		ST1,tri_t1(PTRI)
	lhu		VERT1,tri_v1(PTRI)
	sll		ST1,ST1,3
	sll		VERT1,VERT1,3
	addu	ST1,ST1,PST
	addu	VERT1,VERT1,PVTX
	lwc1	f6,st_s(ST1)
	lwc1	f4,midv_oow(VERT1)
	lwc1	f8,st_t(ST1)
	mul.s	f6,f6,f4
	mul.s	f8,f8,f4
	mul.s	f6,f6,f20
	mul.s	f8,f8,f22

	lhu		ST2,tri_t2(PTRI)
	lhu		VERT2,tri_v2(PTRI)
	sll		ST2,ST2,3
	sll		VERT2,VERT2,3
	addu	ST2,ST2,PST
	addu	VERT2,VERT2,PVTX

	swc1	f6,midv_sow(VERT1)
	swc1	f8,midv_tow(VERT1)

	lhu		VERT3,tri_v3(PTRI)

	lwc1	f4,midv_oow(VERT2)
	lwc1	f6,st_s(ST2)
	lwc1	f8,st_t(ST2)
	mul.s	f6,f6,f4
	mul.s	f8,f8,f4
	mul.s	f6,f6,f20
	mul.s	f8,f8,f22

	sll		VERT3,VERT3,3
	addu	VERT3,VERT3,PVTX
	lh		TTEX,ntri_texture(PTRI)	;; get next tri texture

	swc1	f6,midv_sow(VERT2)
	add		JVAL,TTEX,TABLE
	lw		JVAL,0(JVAL)

	jal		guDrawTriangleWithClip
	swc1	f8,midv_tow(VERT2)

	jr		JVAL
	addi	PTRI,PTRI,tri_size


dt_no_new_verts:
	lhu		VERT1,tri_v1(PTRI)
	lhu		VERT2,tri_v2(PTRI)
	lhu		VERT3,tri_v3(PTRI)
	sll		VERT1,VERT1,3
	addu	VERT1,VERT1,PVTX
	sll		VERT2,VERT2,3
	lh		TTEX,ntri_texture(PTRI)	;; get next tri texture
	addu	VERT2,VERT2,PVTX
	add		JVAL,TTEX,TABLE
	sll		VERT3,VERT3,3
	lw		JVAL,0(JVAL)

	jal		guDrawTriangleWithClip
	addu	VERT3,VERT3,PVTX

	jr		JVAL
	addi	PTRI,PTRI,tri_size


dt_done:
	lw		s0,0(sp)
	lw		s1,4(sp)
	lw		s2,8(sp)
	lw		s3,12(sp)
	lw		s4,16(sp)
	lw		s5,20(sp)
	lw		s6,24(sp)
	lw		s7,28(sp)
	lwc1	f20,32(sp)
	lwc1	f22,36(sp)
	lw		ra,40(sp)
	addi	sp,sp,48

	jr		ra
	nop		;; branch delay

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; void draw_tris_noclip( LIMB *limb, GrMipMipID_t *decals, int *ratios, MidVertex *vtx, int draw_mode )
;; draw triangles, perform aspect ratio calculations

	xdef	draw_tris_noclip

	dw		dtn_done							;; code == -5
	dw		dtn_no_new_verts					;; code == -4
	dw		dtn_one_new_vert					;; code == -3
	dw		dtn_two_new_verts  				;; code == -2
	dw		dtn_three_new_verts				;; code == -1
dtnc_functions:
	dw		dtn_all_new,dtn_all_new,dtn_all_new,dtn_all_new	;; code >= 0
	dw		dtn_all_new,dtn_all_new,dtn_all_new,dtn_all_new
	dw		dtn_all_new,dtn_all_new,dtn_all_new,dtn_all_new
	dw		dtn_all_new,dtn_all_new,dtn_all_new,dtn_all_new
	dw		dtn_all_new,dtn_all_new,dtn_all_new,dtn_all_new
	dw		dtn_all_new,dtn_all_new,dtn_all_new,dtn_all_new
	dw		dtn_all_new,dtn_all_new,dtn_all_new,dtn_all_new
	dw		dtn_all_new,dtn_all_new,dtn_all_new,dtn_all_new

draw_tris_noclip:

	addi	sp,sp,-48
	sw		s0,0(sp)
	sw		s1,4(sp)
	sw		s2,8(sp)
	sw		s3,12(sp)
	sw		s4,16(sp)
	sw		s5,20(sp)
	sw		s6,24(sp)
	sw		s7,28(sp)
	swc1	f20,32(sp)
	swc1	f22,36(sp)
	sw		ra,40(sp)

	;; v0, draw mode, gets written to a3 & passed right on to grDrawTriangle
	lw		PTRI,limb_ptri(a0)	;; load triangle pointer
	move	DECALS,a1
	move	RATIOS,a2
	la		TABLE,dtnc_functions
	move	PVTX,a3				;; save vtx base ptr
	move	a3,v0

	;; get texture, then fall through into all-new case
	lh		TTEX,tri_texture(PTRI)
	lw		PST,limb_pst(a0)	;; load st base ptr


dtn_all_new:
	add		a0,TTEX,DECALS	;; get decals[TTEX]
	jal		guTexSource
	lw		a0,0(a0)

	;; get new sf & tf
	;; fall through into 3-new case
	add		t0,TTEX,RATIOS
	lw		t0,0(t0)
	la		t1,ar_values
	sll		t0,t0,3
	add		t0,t0,t1
	lwc1	f20,0(t0)
	lwc1	f22,4(t0)


dtn_three_new_verts:
	;; vertex 1
	lhu		ST1,tri_t1(PTRI)
	lhu		VERT1,tri_v1(PTRI)
	sll		ST1,ST1,3
	sll		VERT1,VERT1,3
	addu	ST1,ST1,PST
	addu	VERT1,VERT1,PVTX
	lwc1	f6,st_s(ST1)
	lwc1	f4,midv_oow(VERT1)
	lwc1	f8,st_t(ST1)
	mul.s	f6,f6,f4
	mul.s	f8,f8,f4
	mul.s	f6,f6,f20
	mul.s	f8,f8,f22

	lhu		VERT2,tri_v2(PTRI)
	lhu		ST2,tri_t2(PTRI)

	swc1	f6,midv_sow(VERT1)
	swc1	f8,midv_tow(VERT1)

	;; vertex 2
	sll		VERT2,VERT2,3
	sll		ST2,ST2,3
	addu	VERT2,VERT2,PVTX
	addu	ST2,ST2,PST
	lwc1	f4,midv_oow(VERT2)
	lwc1	f6,st_s(ST2)
	lwc1	f8,st_t(ST2)
	mul.s	f6,f6,f4
	mul.s	f8,f8,f4
	mul.s	f6,f6,f20
	mul.s	f8,f8,f22

	lhu		VERT3,tri_v3(PTRI)
	lhu		ST3,tri_t3(PTRI)

	swc1	f6,midv_sow(VERT2)
	swc1	f8,midv_tow(VERT2)

	;; vertex 3
	sll		VERT3,VERT3,3
	sll		ST3,ST3,3
	addu	VERT3,VERT3,PVTX
	addu	ST3,ST3,PST
	lwc1	f4,midv_oow(VERT3)
	lwc1	f6,st_s(ST3)
	lwc1	f8,st_t(ST3)
	mul.s	f6,f6,f4
	mul.s	f8,f8,f4
	mul.s	f6,f6,f20
	mul.s	f8,f8,f22

	lh		TTEX,ntri_texture(PTRI)	;; get next tri texture
	swc1	f6,midv_sow(VERT3)
	add		JVAL,TTEX,TABLE
	lw		JVAL,0(JVAL)

	jal		grDrawTriangleDma
	swc1	f8,midv_tow(VERT3)

	jr		JVAL
	addi	PTRI,PTRI,tri_size


dtn_one_new_vert:
	lhu		ST1,tri_t1(PTRI)
	lhu		VERT1,tri_v1(PTRI)
	sll		ST1,ST1,3
	sll		VERT1,VERT1,3
	addu	ST1,ST1,PST
	addu	VERT1,VERT1,PVTX
	lwc1	f6,st_s(ST1)
	lwc1	f4,midv_oow(VERT1)
	lwc1	f8,st_t(ST1)
	mul.s	f6,f6,f4
	mul.s	f8,f8,f4
	mul.s	f6,f6,f20
	mul.s	f8,f8,f22

	lhu		VERT2,tri_v2(PTRI)
	lhu		VERT3,tri_v3(PTRI)
	sll		VERT2,VERT2,3
	addu	VERT2,VERT2,PVTX
	sll		VERT3,VERT3,3


	lh		TTEX,ntri_texture(PTRI)	;; get next tri texture
	addu	VERT3,VERT3,PVTX
	add		JVAL,TTEX,TABLE
	swc1	f6,midv_sow(VERT1)
	lw		JVAL,0(JVAL)

	jal		grDrawTriangleDma
	swc1	f8,midv_tow(VERT1)

	jr		JVAL
	addi	PTRI,PTRI,tri_size


dtn_two_new_verts:
	lhu		ST1,tri_t1(PTRI)
	lhu		VERT1,tri_v1(PTRI)
	sll		ST1,ST1,3
	sll		VERT1,VERT1,3
	addu	ST1,ST1,PST
	addu	VERT1,VERT1,PVTX
	lwc1	f6,st_s(ST1)
	lwc1	f4,midv_oow(VERT1)
	lwc1	f8,st_t(ST1)
	mul.s	f6,f6,f4
	mul.s	f8,f8,f4
	mul.s	f6,f6,f20
	mul.s	f8,f8,f22

	lhu		ST2,tri_t2(PTRI)
	lhu		VERT2,tri_v2(PTRI)
	sll		ST2,ST2,3
	sll		VERT2,VERT2,3
	addu	ST2,ST2,PST
	addu	VERT2,VERT2,PVTX

	swc1	f6,midv_sow(VERT1)
	swc1	f8,midv_tow(VERT1)

	lhu		VERT3,tri_v3(PTRI)

	lwc1	f4,midv_oow(VERT2)
	lwc1	f6,st_s(ST2)
	lwc1	f8,st_t(ST2)
	mul.s	f6,f6,f4
	mul.s	f8,f8,f4
	mul.s	f6,f6,f20
	mul.s	f8,f8,f22

	sll		VERT3,VERT3,3
	addu	VERT3,VERT3,PVTX
	lh		TTEX,ntri_texture(PTRI)	;; get next tri texture

	swc1	f6,midv_sow(VERT2)
	add		JVAL,TTEX,TABLE
	lw		JVAL,0(JVAL)

	jal		grDrawTriangleDma
	swc1	f8,midv_tow(VERT2)

	jr		JVAL
	addi	PTRI,PTRI,tri_size


dtn_no_new_verts:
	lhu		VERT1,tri_v1(PTRI)
	lhu		VERT2,tri_v2(PTRI)
	lhu		VERT3,tri_v3(PTRI)
	sll		VERT1,VERT1,3
	addu	VERT1,VERT1,PVTX
	sll		VERT2,VERT2,3
	lh		TTEX,ntri_texture(PTRI)	;; get next tri texture
	addu	VERT2,VERT2,PVTX
	add		JVAL,TTEX,TABLE
	sll		VERT3,VERT3,3
	lw		JVAL,0(JVAL)

	jal		grDrawTriangleDma
	addu	VERT3,VERT3,PVTX

	jr		JVAL
	addi	PTRI,PTRI,tri_size


dtn_done:
	lw		s0,0(sp)
	lw		s1,4(sp)
	lw		s2,8(sp)
	lw		s3,12(sp)
	lw		s4,16(sp)
	lw		s5,20(sp)
	lw		s6,24(sp)
	lw		s7,28(sp)
	lwc1	f20,32(sp)
	lwc1	f22,36(sp)
	lw		ra,40(sp)
	addi	sp,sp,48

	jr		ra
	nop		;; branch delay

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; void draw_tris_noz( LIMB *limb, GrMipMipID_t *decals, int *ratios, MidVertex *vtx, int draw_mode )
;; draw triangles, perform aspect ratio calculations, but oow is always 1.0f

	xdef	draw_tris_noz

	dw		dtz_done							;; code == -5
	dw		dtz_no_new_verts					;; code == -4
	dw		dtz_one_new_vert					;; code == -3
	dw		dtz_two_new_verts  				;; code == -2
	dw		dtz_three_new_verts				;; code == -1
dtzc_functions:
	dw		dtz_all_new,dtz_all_new,dtz_all_new,dtz_all_new	;; code >= 0
	dw		dtz_all_new,dtz_all_new,dtz_all_new,dtz_all_new
	dw		dtz_all_new,dtz_all_new,dtz_all_new,dtz_all_new
	dw		dtz_all_new,dtz_all_new,dtz_all_new,dtz_all_new
	dw		dtz_all_new,dtz_all_new,dtz_all_new,dtz_all_new
	dw		dtz_all_new,dtz_all_new,dtz_all_new,dtz_all_new
	dw		dtz_all_new,dtz_all_new,dtz_all_new,dtz_all_new
	dw		dtz_all_new,dtz_all_new,dtz_all_new,dtz_all_new

ar_half_values:
	ieee32	128.0,	16.0
	ieee32	128.0,	32.0
	ieee32	128.0,	64.0
	ieee32	128.0,	128.0
	ieee32	64.0,	128.0
	ieee32	32.0,	128.0
	ieee32	16.0,	128.0

draw_tris_noz:

	addi	sp,sp,-48
	sw		s0,0(sp)
	sw		s1,4(sp)
	sw		s2,8(sp)
	sw		s3,12(sp)
	sw		s4,16(sp)
	sw		s5,20(sp)
	sw		s6,24(sp)
	sw		s7,28(sp)
	swc1	f20,32(sp)
	swc1	f22,36(sp)
	sw		ra,40(sp)

	;; v0, draw mode, gets written to a3 & passed right on to grDrawTriangle
	lw		PTRI,limb_ptri(a0)	;; load triangle pointer
	move	DECALS,a1
	move	RATIOS,a2
	la		TABLE,dtzc_functions
	move	PVTX,a3				;; save vtx base ptr
	move	a3,v0

	;; get texture, then fall through into all-new case
	lh		TTEX,tri_texture(PTRI)
	lw		PST,limb_pst(a0)	;; load st base ptr


dtz_all_new:
	add		a0,TTEX,DECALS	;; get decals[TTEX]
	jal		guTexSource
	lw		a0,0(a0)

	;; get new sf & tf
	;; fall through into 3-new case
	add		t0,TTEX,RATIOS
	lw		t0,0(t0)
	la		t1,ar_half_values
	sll		t0,t0,3
	add		t0,t0,t1
	lwc1	f20,0(t0)
	lwc1	f22,4(t0)


dtz_three_new_verts:
	;; vertex 1
	lhu		ST1,tri_t1(PTRI)
	lhu		VERT1,tri_v1(PTRI)
	sll		ST1,ST1,3
	sll		VERT1,VERT1,3
	addu	ST1,ST1,PST
	addu	VERT1,VERT1,PVTX
	lwc1	f6,st_s(ST1)
	lwc1	f8,st_t(ST1)
	mul.s	f6,f6,f20
	mul.s	f8,f8,f22

	lhu		VERT2,tri_v2(PTRI)
	lhu		ST2,tri_t2(PTRI)

	swc1	f6,midv_sow(VERT1)
	swc1	f8,midv_tow(VERT1)

	;; vertex 2
	sll		VERT2,VERT2,3
	sll		ST2,ST2,3
	addu	VERT2,VERT2,PVTX
	addu	ST2,ST2,PST
	lwc1	f6,st_s(ST2)
	lwc1	f8,st_t(ST2)
	mul.s	f6,f6,f20
	mul.s	f8,f8,f22

	lhu		VERT3,tri_v3(PTRI)
	lhu		ST3,tri_t3(PTRI)

	swc1	f6,midv_sow(VERT2)
	swc1	f8,midv_tow(VERT2)

	;; vertex 3
	sll		VERT3,VERT3,3
	sll		ST3,ST3,3
	addu	VERT3,VERT3,PVTX
	addu	ST3,ST3,PST
	lwc1	f6,st_s(ST3)
	lwc1	f8,st_t(ST3)
	mul.s	f6,f6,f20
	mul.s	f8,f8,f22

	lh		TTEX,ntri_texture(PTRI)	;; get next tri texture
	swc1	f6,midv_sow(VERT3)
	add		JVAL,TTEX,TABLE
	lw		JVAL,0(JVAL)

	jal		grDrawTriangleDma
	swc1	f8,midv_tow(VERT3)

	jr		JVAL
	addi	PTRI,PTRI,tri_size


dtz_one_new_vert:
	lhu		ST1,tri_t1(PTRI)
	lhu		VERT1,tri_v1(PTRI)
	sll		ST1,ST1,3
	sll		VERT1,VERT1,3
	addu	ST1,ST1,PST
	addu	VERT1,VERT1,PVTX
	lwc1	f6,st_s(ST1)
	lwc1	f8,st_t(ST1)
	mul.s	f6,f6,f20
	mul.s	f8,f8,f22

	lhu		VERT2,tri_v2(PTRI)
	lhu		VERT3,tri_v3(PTRI)
	sll		VERT2,VERT2,3
	addu	VERT2,VERT2,PVTX
	sll		VERT3,VERT3,3


	lh		TTEX,ntri_texture(PTRI)	;; get next tri texture
	addu	VERT3,VERT3,PVTX
	add		JVAL,TTEX,TABLE
	swc1	f6,midv_sow(VERT1)
	lw		JVAL,0(JVAL)

	jal		grDrawTriangleDma
	swc1	f8,midv_tow(VERT1)

	jr		JVAL
	addi	PTRI,PTRI,tri_size


dtz_two_new_verts:
	lhu		ST1,tri_t1(PTRI)
	lhu		VERT1,tri_v1(PTRI)
	sll		ST1,ST1,3
	sll		VERT1,VERT1,3
	addu	ST1,ST1,PST
	addu	VERT1,VERT1,PVTX
	lwc1	f6,st_s(ST1)
	lwc1	f8,st_t(ST1)
	mul.s	f6,f6,f20
	mul.s	f8,f8,f22

	lhu		ST2,tri_t2(PTRI)
	lhu		VERT2,tri_v2(PTRI)
	sll		ST2,ST2,3
	sll		VERT2,VERT2,3
	addu	ST2,ST2,PST
	addu	VERT2,VERT2,PVTX

	swc1	f6,midv_sow(VERT1)
	swc1	f8,midv_tow(VERT1)

	lhu		VERT3,tri_v3(PTRI)

	lwc1	f6,st_s(ST2)
	lwc1	f8,st_t(ST2)
	mul.s	f6,f6,f20
	mul.s	f8,f8,f22

	sll		VERT3,VERT3,3
	addu	VERT3,VERT3,PVTX
	lh		TTEX,ntri_texture(PTRI)	;; get next tri texture

	swc1	f6,midv_sow(VERT2)
	add		JVAL,TTEX,TABLE
	lw		JVAL,0(JVAL)

	jal		grDrawTriangleDma
	swc1	f8,midv_tow(VERT2)

	jr		JVAL
	addi	PTRI,PTRI,tri_size


dtz_no_new_verts:
	lhu		VERT1,tri_v1(PTRI)
	lhu		VERT2,tri_v2(PTRI)
	lhu		VERT3,tri_v3(PTRI)
	sll		VERT1,VERT1,3
	addu	VERT1,VERT1,PVTX
	sll		VERT2,VERT2,3
	lh		TTEX,ntri_texture(PTRI)	;; get next tri texture
	addu	VERT2,VERT2,PVTX
	add		JVAL,TTEX,TABLE
	sll		VERT3,VERT3,3
	lw		JVAL,0(JVAL)

	jal		grDrawTriangleDma
	addu	VERT3,VERT3,PVTX

	jr		JVAL
	addi	PTRI,PTRI,tri_size


dtz_done:
	lw		s0,0(sp)
	lw		s1,4(sp)
	lw		s2,8(sp)
	lw		s3,12(sp)
	lw		s4,16(sp)
	lw		s5,20(sp)
	lw		s6,24(sp)
	lw		s7,28(sp)
	lwc1	f20,32(sp)
	lwc1	f22,36(sp)
	lw		ra,40(sp)
	addi	sp,sp,48

	jr		ra
	nop		;; branch delay

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	xdef	rsqrt
;rsqrt:
;	jr			r31
;	rsqrt.s	f0,f12
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

