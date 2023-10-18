VERTEX fieldline_obj_vtx[] =
{
	{-1.2f,0.0f,182.0f},
	{-1.2f,0.0f,-182.0f},
	{1.2f,0.0f,182.0f},
	{1.2f,0.0f,-182.0f}
};

ST fieldline_obj_st[] =
{
	{0.49f,0.51f},
	{0.49f,0.49f},
	{0.51f,0.51f},
	{0.51f,0.49f}
};

TRI fieldline_obj_tris[] =
{
	{0,3,6, 0,1,2, 0},
	{9,6,3, 3,255,255, -12},
	{0, 0, 0, 0, 0, 0, -20}
};

LIMB limb_fieldline_obj = 
{
	4,			/* vertex count */
	2,			/* triangle count */
	NULL,			/* vertex normals */
	fieldline_obj_vtx,	/* vertices */
	fieldline_obj_st,	/* texture coordinates */
	fieldline_obj_tris,	/* triangles */
};
