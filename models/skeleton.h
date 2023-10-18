SNODE fbplyr_skel[] =
{
	/* 0 */
	{0.0f, 0.0f, 0.0f,		/* Prot */
	0,						/* flags */
	{1, 9, 12}},	 		/* children */

	/* 1 */
	{0.0000f, 0.6465f, -0.0118f,		/* torso */
	0,						/* flags */
	{2-1, 5-1, 8-1}},		/* children */

	/* 2 */
	{-2.9075f, 5.3818f, -0.0111f,	/* right shoulder */
	0,						/* flags */
	{3-2, 0, 0}},			/* children */

	/* 3 */
	{-2.9190f, 9.3556f, -0.0150f, /* right elbow */
	0,						/* flags */
	{4-3, 0, 0}},			/* children */

	/* 4 */
	{-2.8549f, 12.0003f, -0.0040f,	/* right wrist */
	0,						/* flags */
	{0, 0, 0}},				/* children */

	/* 5 */
	{2.9075f, 5.3818f, -0.0111f,	/* left shoulder */
	0,						/* flags */
	{6-5, 0, 0}},			/* children */

	/* 6 */
	{2.9190f, 9.3556f, -0.0150f,	/* left elbow */
	0,						/* flags */
	{7-6, 0, 0}},			/* children */

	/* 7 */
	{2.8549f, 12.0003f, -0.0040f,	/* left wrist */
	0,						/* flags */
	{0, 0, 0}},				/* children */

	/* 8 */
	{0.0f, 5.5752f, 0.0017f,		/* head */
	SN_HEADLIMB,			/* flags */
	{0, 0, 0}},				/* children */

	/* 9 */
	{-1.0824f, -0.5195f, 0.0f,		/* right hip */
	0,						/* flags */
	{10-9, 0, 0}},			/* children */

	/* 10 */
	{-1.1531f, 4.9568f, -0.0226f,	/* right knee */
	0,						/* flags */
	{11-10, 0, 0}},			/* children */

	/* 11 */
	{-1.0085f, 10.6092f, -0.0098f,	/* right ankle */
	0,						/* flags */
	{0, 0, 0}},				/* children */

	/* 12 */
	{1.0824f, -0.51948f, 0.0f,		/* left hip */
	0,						/* flags */
	{13-12, 0, 0}},			/* children */

	/* 13 */
	{1.1531f, 4.9568f, -0.0226f,	/* left knee */
	0,						/* flags */
	{14-13, 0, 0}},			/* children */

	/* 14 */
	{1.0085f, 10.6092f, -0.0098f,	/* left ankle */
	0,						/* flags */
	{0, 0, 0}},				/* children */
};


SNODE tophalf_skel[] =
{
	/* 0 */
	{0.0f, 0.0f, 0.0f,		/* Prot */
	SN_NODRAW,				/* flags */
	{1, 0, 0}},		 		/* children */

	/* 1 */
	{0.0000f, 0.6465f, -0.0118f,		/* torso */
	0,						/* flags */
	{2-1, 5-1, 8-1}},		/* children */

	/* 2 */
	{-2.9075f, 5.3818f, -0.0111f,	/* right shoulder */
	0,						/* flags */
	{3-2, 0, 0}},			/* children */

	/* 3 */
	{-2.9190f, 9.3556f, -0.0150f, /* right elbow */
	0,						/* flags */
	{4-3, 0, 0}},			/* children */

	/* 4 */
	{-2.8549f, 12.0003f, -0.0040f,	/* right wrist */
	0,						/* flags */
	{0, 0, 0}},				/* children */

	/* 5 */
	{2.9075f, 5.3818f, -0.0111f,	/* left shoulder */
	0,						/* flags */
	{6-5, 0, 0}},			/* children */

	/* 6 */
	{2.9190f, 9.3556f, -0.0150f,	/* left elbow */
	0,						/* flags */
	{7-6, 0, 0}},			/* children */

	/* 7 */
	{2.8549f, 12.0003f, -0.0040f,	/* left wrist */
	0,						/* flags */
	{0, 0, 0}},				/* children */

	/* 8 */
	{0.0f, 5.5752f, 0.0017f,		/* head */
	SN_HEADLIMB,			/* flags */
	{0, 0, 0}},				/* children */
};

SNODE bothalf_skel[] =
{
	/* 0 */
	{0.0f, 0.0f, 0.0f,		/* Prot */
	0,						/* flags */
	{9, 12, 0}},	 		/* children */

	/* 1-8 */
	{0.0f, 0.0f, 0.0f, 0, {0, 0, 0}},
	{0.0f, 0.0f, 0.0f, 0, {0, 0, 0}},
	{0.0f, 0.0f, 0.0f, 0, {0, 0, 0}},
	{0.0f, 0.0f, 0.0f, 0, {0, 0, 0}},
	{0.0f, 0.0f, 0.0f, 0, {0, 0, 0}},
	{0.0f, 0.0f, 0.0f, 0, {0, 0, 0}},
	{0.0f, 0.0f, 0.0f, 0, {0, 0, 0}},
	{0.0f, 0.0f, 0.0f, 0, {0, 0, 0}},


	/* 9 */
	{-1.0824f, -0.5195f, 0.0f,		/* right hip */
	0,						/* flags */
	{10-9, 0, 0}},			/* children */

	/* 10 */
	{-1.1531f, 4.9568f, -0.0226f,	/* right knee */
	0,						/* flags */
	{11-10, 0, 0}},			/* children */

	/* 11 */
	{-1.0085f, 10.6092f, -0.0098f,	/* right ankle */
	0,						/* flags */
	{0, 0, 0}},				/* children */

	/* 12 */
	{1.0824f, -0.51948f, 0.0f,		/* left hip */
	0,						/* flags */
	{13-12, 0, 0}},			/* children */

	/* 13 */
	{1.1531f, 4.9568f, -0.0226f,	/* left knee */
	0,						/* flags */
	{14-13, 0, 0}},			/* children */

	/* 14 */
	{1.0085f, 10.6092f, -0.0098f,	/* left ankle */
	0,						/* flags */
	{0, 0, 0}},				/* children */
};


// standard models

LIMB *model266_limbs[] =
{
	&limb_pelvis_obj266,
	&limb_torso_obj266,
	&limb_rtuarm_obj266,
	&limb_rtlarm_obj266,
	&limb_rthand_obj266,
	&limb_lfuarm_obj266,
	&limb_lflarm_obj266,
	&limb_lfhand_obj266,
	&limb_head_obj266,
	&limb_rtthigh_obj266,
	&limb_rtcalf_obj266,
	&limb_rtfoot_obj266,
	&limb_lfthigh_obj266,
	&limb_lfcalf_obj266,
	&limb_lffoot_obj266
};

LIMB *model266f_limbs[] =
{
	&limb_pelvis_obj266f,
	&limb_torso_obj266f,
	&limb_rtuarm_obj266f,
	&limb_rtlarm_obj266f,
	&limb_rthand_obj266f,
	&limb_lfuarm_obj266f,
	&limb_lflarm_obj266f,
	&limb_lfhand_obj266f,
	&limb_head_obj266f,
	&limb_rtthigh_obj266f,
	&limb_rtcalf_obj266f,
	&limb_rtfoot_obj266f,
	&limb_lfthigh_obj266f,
	&limb_lfcalf_obj266f,
	&limb_lffoot_obj266f
};

LIMB *model498_limbs[] =
{
	&limb_pelvis_obj498,
	&limb_torso_obj498,
	&limb_rtuarm_obj498,
	&limb_rtlarm_obj498,
	&limb_rthand_obj498,
	&limb_lfuarm_obj498,
	&limb_lflarm_obj498,
	&limb_lfhand_obj498,
	&limb_head_obj498,
	&limb_rtthigh_obj498,
	&limb_rtcalf_obj498,
	&limb_rtfoot_obj498,
	&limb_lfthigh_obj498,
	&limb_lfcalf_obj498,
	&limb_lffoot_obj498
};

LIMB *model498f_limbs[] =
{
	&limb_pelvis_obj498f,
	&limb_torso_obj498f,
	&limb_rtuarm_obj498f,
	&limb_rtlarm_obj498f,
	&limb_rthand_obj498f,
	&limb_lfuarm_obj498f,
	&limb_lflarm_obj498f,
	&limb_lfhand_obj498f,
	&limb_head_obj498f,
	&limb_rtthigh_obj498f,
	&limb_rtcalf_obj498f,
	&limb_rtfoot_obj498f,
	&limb_lfthigh_obj498f,
	&limb_lfcalf_obj498f,
	&limb_lffoot_obj498f
};

// ball in left hand

LIMB *model266bl_limbs[] =
{
	&limb_pelvis_obj266,
	&limb_torso_obj266,
	&limb_rtuarm_obj266,
	&limb_rtlarm_obj266,
	&limb_rthand_obj266,
	&limb_lfuarm_obj266,
	&limb_lflarm_obj266,
	&limb_lfhand_obj001b,
	&limb_head_obj266,
	&limb_rtthigh_obj266,
	&limb_rtcalf_obj266,
	&limb_rtfoot_obj266,
	&limb_lfthigh_obj266,
	&limb_lfcalf_obj266,
	&limb_lffoot_obj266
};

LIMB *model266fbl_limbs[] =
{
	&limb_pelvis_obj266f,
	&limb_torso_obj266f,
	&limb_rtuarm_obj266f,
	&limb_rtlarm_obj266f,
	&limb_rthand_obj266f,
	&limb_lfuarm_obj266f,
	&limb_lflarm_obj266f,
	&limb_lfhand_obj001b,
	&limb_head_obj266f,
	&limb_rtthigh_obj266f,
	&limb_rtcalf_obj266f,
	&limb_rtfoot_obj266f,
	&limb_lfthigh_obj266f,
	&limb_lfcalf_obj266f,
	&limb_lffoot_obj266f
};

LIMB *model498bl_limbs[] =
{
	&limb_pelvis_obj498,
	&limb_torso_obj498,
	&limb_rtuarm_obj498,
	&limb_rtlarm_obj498,
	&limb_rthand_obj498,
	&limb_lfuarm_obj498,
	&limb_lflarm_obj498,
	&limb_lfhand_obj001b,
	&limb_head_obj498,
	&limb_rtthigh_obj498,
	&limb_rtcalf_obj498,
	&limb_rtfoot_obj498,
	&limb_lfthigh_obj498,
	&limb_lfcalf_obj498,
	&limb_lffoot_obj498
};

LIMB *model498fbl_limbs[] =
{
	&limb_pelvis_obj498f,
	&limb_torso_obj498f,
	&limb_rtuarm_obj498f,
	&limb_rtlarm_obj498f,
	&limb_rthand_obj498f,
	&limb_lfuarm_obj498f,
	&limb_lflarm_obj498f,
	&limb_lfhand_obj001b,
	&limb_head_obj498f,
	&limb_rtthigh_obj498f,
	&limb_rtcalf_obj498f,
	&limb_rtfoot_obj498f,
	&limb_lfthigh_obj498f,
	&limb_lfcalf_obj498f,
	&limb_lffoot_obj498f
};

// ball in right hand

LIMB *model266br_limbs[] =
{
	&limb_pelvis_obj266,
	&limb_torso_obj266,
	&limb_rtuarm_obj266,
	&limb_rtlarm_obj266,
	&limb_rthand_obj001b,
	&limb_lfuarm_obj266,
	&limb_lflarm_obj266,
	&limb_lfhand_obj266,
	&limb_head_obj266,
	&limb_rtthigh_obj266,
	&limb_rtcalf_obj266,
	&limb_rtfoot_obj266,
	&limb_lfthigh_obj266,
	&limb_lfcalf_obj266,
	&limb_lffoot_obj266
};

LIMB *model266fbr_limbs[] =
{
	&limb_pelvis_obj266f,
	&limb_torso_obj266f,
	&limb_rtuarm_obj266f,
	&limb_rtlarm_obj266f,
	&limb_rthand_obj001b,
	&limb_lfuarm_obj266f,
	&limb_lflarm_obj266f,
	&limb_lfhand_obj266f,
	&limb_head_obj266f,
	&limb_rtthigh_obj266f,
	&limb_rtcalf_obj266f,
	&limb_rtfoot_obj266f,
	&limb_lfthigh_obj266f,
	&limb_lfcalf_obj266f,
	&limb_lffoot_obj266f
};

LIMB *model498br_limbs[] =
{
	&limb_pelvis_obj498,
	&limb_torso_obj498,
	&limb_rtuarm_obj498,
	&limb_rtlarm_obj498,
	&limb_rthand_obj001b,
	&limb_lfuarm_obj498,
	&limb_lflarm_obj498,
	&limb_lfhand_obj498,
	&limb_head_obj498,
	&limb_rtthigh_obj498,
	&limb_rtcalf_obj498,
	&limb_rtfoot_obj498,
	&limb_lfthigh_obj498,
	&limb_lfcalf_obj498,
	&limb_lffoot_obj498
};

LIMB *model498fbr_limbs[] =
{
	&limb_pelvis_obj498f,
	&limb_torso_obj498f,
	&limb_rtuarm_obj498f,
	&limb_rtlarm_obj498f,
	&limb_rthand_obj001b,
	&limb_lfuarm_obj498f,
	&limb_lflarm_obj498f,
	&limb_lfhand_obj498f,
	&limb_head_obj498f,
	&limb_rtthigh_obj498f,
	&limb_rtcalf_obj498f,
	&limb_rtfoot_obj498f,
	&limb_lfthigh_obj498f,
	&limb_lfcalf_obj498f,
	&limb_lffoot_obj498f
};

// JUMBO ball in left hand

LIMB *model266jbl_limbs[] =
{
	&limb_pelvis_obj266,
	&limb_torso_obj266,
	&limb_rtuarm_obj266,
	&limb_rtlarm_obj266,
	&limb_rthand_obj266,
	&limb_lfuarm_obj266,
	&limb_lflarm_obj266,
	&limb_lfhand_obj002b,
	&limb_head_obj266,
	&limb_rtthigh_obj266,
	&limb_rtcalf_obj266,
	&limb_rtfoot_obj266,
	&limb_lfthigh_obj266,
	&limb_lfcalf_obj266,
	&limb_lffoot_obj266
};

LIMB *model266fjbl_limbs[] =
{
	&limb_pelvis_obj266f,
	&limb_torso_obj266f,
	&limb_rtuarm_obj266f,
	&limb_rtlarm_obj266f,
	&limb_rthand_obj266f,
	&limb_lfuarm_obj266f,
	&limb_lflarm_obj266f,
	&limb_lfhand_obj002b,
	&limb_head_obj266f,
	&limb_rtthigh_obj266f,
	&limb_rtcalf_obj266f,
	&limb_rtfoot_obj266f,
	&limb_lfthigh_obj266f,
	&limb_lfcalf_obj266f,
	&limb_lffoot_obj266f
};

LIMB *model498jbl_limbs[] =
{
	&limb_pelvis_obj498,
	&limb_torso_obj498,
	&limb_rtuarm_obj498,
	&limb_rtlarm_obj498,
	&limb_rthand_obj498,
	&limb_lfuarm_obj498,
	&limb_lflarm_obj498,
	&limb_lfhand_obj002b,
	&limb_head_obj498,
	&limb_rtthigh_obj498,
	&limb_rtcalf_obj498,
	&limb_rtfoot_obj498,
	&limb_lfthigh_obj498,
	&limb_lfcalf_obj498,
	&limb_lffoot_obj498
};

LIMB *model498fjbl_limbs[] =
{
	&limb_pelvis_obj498f,
	&limb_torso_obj498f,
	&limb_rtuarm_obj498f,
	&limb_rtlarm_obj498f,
	&limb_rthand_obj498f,
	&limb_lfuarm_obj498f,
	&limb_lflarm_obj498f,
	&limb_lfhand_obj002b,
	&limb_head_obj498f,
	&limb_rtthigh_obj498f,
	&limb_rtcalf_obj498f,
	&limb_rtfoot_obj498f,
	&limb_lfthigh_obj498f,
	&limb_lfcalf_obj498f,
	&limb_lffoot_obj498f
};

// JUMBO ball in right hand

LIMB *model266jbr_limbs[] =
{
	&limb_pelvis_obj266,
	&limb_torso_obj266,
	&limb_rtuarm_obj266,
	&limb_rtlarm_obj266,
	&limb_rthand_obj002b,
	&limb_lfuarm_obj266,
	&limb_lflarm_obj266,
	&limb_lfhand_obj266,
	&limb_head_obj266,
	&limb_rtthigh_obj266,
	&limb_rtcalf_obj266,
	&limb_rtfoot_obj266,
	&limb_lfthigh_obj266,
	&limb_lfcalf_obj266,
	&limb_lffoot_obj266
};

LIMB *model266fjbr_limbs[] =
{
	&limb_pelvis_obj266f,
	&limb_torso_obj266f,
	&limb_rtuarm_obj266f,
	&limb_rtlarm_obj266f,
	&limb_rthand_obj002b,
	&limb_lfuarm_obj266f,
	&limb_lflarm_obj266f,
	&limb_lfhand_obj266f,
	&limb_head_obj266f,
	&limb_rtthigh_obj266f,
	&limb_rtcalf_obj266f,
	&limb_rtfoot_obj266f,
	&limb_lfthigh_obj266f,
	&limb_lfcalf_obj266f,
	&limb_lffoot_obj266f
};

LIMB *model498jbr_limbs[] =
{
	&limb_pelvis_obj498,
	&limb_torso_obj498,
	&limb_rtuarm_obj498,
	&limb_rtlarm_obj498,
	&limb_rthand_obj002b,
	&limb_lfuarm_obj498,
	&limb_lflarm_obj498,
	&limb_lfhand_obj498,
	&limb_head_obj498,
	&limb_rtthigh_obj498,
	&limb_rtcalf_obj498,
	&limb_rtfoot_obj498,
	&limb_lfthigh_obj498,
	&limb_lfcalf_obj498,
	&limb_lffoot_obj498
};

LIMB *model498fjbr_limbs[] =
{
	&limb_pelvis_obj498f,
	&limb_torso_obj498f,
	&limb_rtuarm_obj498f,
	&limb_rtlarm_obj498f,
	&limb_rthand_obj002b,
	&limb_lfuarm_obj498f,
	&limb_lflarm_obj498f,
	&limb_lfhand_obj498f,
	&limb_head_obj498f,
	&limb_rtthigh_obj498f,
	&limb_rtcalf_obj498f,
	&limb_rtfoot_obj498f,
	&limb_lfthigh_obj498f,
	&limb_lfcalf_obj498f,
	&limb_lffoot_obj498f
};
