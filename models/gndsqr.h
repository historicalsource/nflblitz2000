VERTEX gndsquare_obj_vtx[] =
{
	{-7.0f,0.0f,7.0f},
	{-7.0f,0.0f,-7.0f},
	{7.0f,0.0f,7.0f},
	{7.0f,0.0f,-7.0f}
};

ST gndsquare_obj_st[] =
{
	{0.0f,1.0f},
	{0.0f,0.0f},
	{1.0f,1.0f},
	{1.0f,0.0f}
};

TRI gndsquare_obj_tris[] =
{
	{0,3,6, 0,1,2, 0},
	{9,6,3, 3,255,255, -12},
	{0, 0, 0, 0, 0, 0, -20}
};

LIMB limb_gndsquare_obj = 
{
	4,				/* vertex count */
	2,				/* triangle count */
	NULL,	/* vertex normals */
	gndsquare_obj_vtx,	/* vertices */
	gndsquare_obj_st,	/* texture coordinates */
	gndsquare_obj_tris,	/* triangles */
};
