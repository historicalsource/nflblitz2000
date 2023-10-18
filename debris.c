//
// debris.c - Source for debris functions
//
// $Revision: 7 $
//
// $Author: Janderso $
//

#if 0		// NOT USED!


#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	<math.h>
#include	<goose\goose.h>
#include	"\Video\Nfl\Include\ani3D.h"
#include	"\Video\Nfl\Include\debris.h"
#include	"\Video\Nfl\Include\id.h"
#include	"\Video\Nfl\Include\game.h"
#include	"\Video\Nfl\Include\CamBot.h"

// Prototypes for static functions
static void _delete_debris(debris_control_t *);
static void draw_debris(void *);
static void init_matrix(float *);
static float _rmagv(float *);
static void normalize(float *);

// Static data used in this module
static float		x_rot_mat[16];		// X rotation matrix
static float		y_rot_mat[16];		// Y rotation matrix
static float		z_rot_mat[16];		// Z rotation matrix
static float		trans_mat[16];		// Translation matrix
static float		comp_mat[16]; 		// Composite transform matrix
static float		cam_mat[16];		// Camera matrix
static float		i_mat[16];			// Intermediate working matrix
static MidVertex	dp_vert[3];			// Transformed vertices for the debris piece
static int			mat_init = 1;		// Flag used to determine of matrices need
												// initializing

static Lvert_t		_debris_vert[3] = {
{0.0f,   1.0f, 0.0f, 1.0f},
{-1.0f, -1.0f, 0.0f, 1.0f},
{1.0f,  -1.0f, 0.0f, 1.0f}
};


// Sets a matrix to an identity matrix
static void init_matrix(float *mat)
{
	register int	i;

	for(i = 0; i < 16; i++)
	{
		mat[i] = 0.0f;
	}
	mat[0] = mat[5] = mat[10] = mat[15] = 1.0f;
}

__asm__("
	.set		noreorder
_rsqrt:
	jr			$31
	rsqrt.s	$f0,$f12
	.set		reorder
");

float	_rsqrt(float);

static float _rmagv(float *v)
{
	register float f;

	f = v[0] * v[0];
	f += v[1] * v[1];
	f += v[2] * v[2];
	return(_rsqrt(f));
}


static void normalize(float *v)
{
	register float m = _rmagv(v);

	v[0] *= m;
	v[1] *= m;
	v[2] *= m;
}

// This function is used to create debris objects
struct object_node *make_debris(debris_config_t *dct)
{
	debris_control_t		*dc;
	int						i;
	struct object_node	*dobj;

	if(mat_init)
	{
		init_matrix(x_rot_mat);
		init_matrix(y_rot_mat);
		init_matrix(z_rot_mat);
		init_matrix(trans_mat);
		init_matrix(comp_mat);
		init_matrix(cam_mat);
		init_matrix(i_mat);

		mat_init = 0;
	}

	// Allocate space for the debris control structure
	if((dc = (debris_control_t *)JMALLOC(sizeof(debris_control_t))) == NULL)
	{
		fprintf(stderr, "Can not allocate memory for debris control structure\r\n");
		return(NULL);
	}

	// Allocate space for the debris particles
	if((dc->dp = (debris_particle_t *)JMALLOC(dct->num_particles * sizeof(debris_particle_t))) == NULL)
	{
		JFREE(dc);
		fprintf(stderr, "Can not allocate memory for debris particle structure(s)\r\n");
		return(NULL);
	}

	// Initialize the friction, gravity, ticks, number of particles, and state
	dc->friction = dct->friction;
	dc->gravity = dct->gravity;
	dc->ticks = 0;
	dc->num = dct->num_particles;
	dc->state = DEBRIS_STATE_INIT;

	// Initialize the x, y, and z positions of each of the particles
	for(i = 0; i < dct->num_particles; i++)
	{
		// Set current position
		dc->dp[i].x = dct->x;
		dc->dp[i].y = dct->y;
		dc->dp[i].z = dct->z;

		// Set the current x, y, and z axis rotations
		dc->dp[i].x_ang = 0.0f;
		dc->dp[i].y_ang = 0.0f;
		dc->dp[i].z_ang = 0.0f;

		// Generate random x, y, and z axis rotations (degrees)
		if(dct->x_ang)
		{
			dc->dp[i].x_rot = (float)randrng(dct->x_ang);
		}
		else
		{
			dc->dp[i].x_rot = 0;
		}
		if(dct->y_ang)
		{
			dc->dp[i].y_rot = (float)randrng(dct->y_ang);
		}
		else
		{
			dc->dp[i].y_rot = 0;
		}
		if(dct->z_ang)
		{
			dc->dp[i].z_rot = (float)randrng(dct->z_ang);
		}
		else
		{
			dc->dp[i].z_rot = 0;
		}

		// Convert to radians
		dc->dp[i].x_rot = DEG2RAD(dc->dp[i].x_rot);
		dc->dp[i].y_rot = DEG2RAD(dc->dp[i].y_rot);
		dc->dp[i].z_rot = DEG2RAD(dc->dp[i].z_rot);

		// Generate a random vector for the direction of the particle
		dc->dp[i].dx = (50.0f - (float)randrng(101));
		dc->dp[i].dy = (50.0f - (float)randrng(101));
		dc->dp[i].dz = (50.0f - (float)randrng(101));

		// Normalize the vector
		normalize((void *)&dc->dp[i].dx);

		// Set the momentum vector
		dc->dp[i].mx = dct->mx;
		dc->dp[i].my = dct->my;
		dc->dp[i].mz = dct->mz;

		if(dct->mx != 0.0f && dct->my != 0.0f && dct->mz != 0.0f)
		{
			// Normalize the momemtum vector
			normalize((void *)&dc->dp[i].mx);

			// Set the momentum velocity
			dc->dp[i].mvelocity = dct->mvelocity;
		}
		else
		{
			dc->dp[i].mvelocity = 0.0f;
		}

		// Set the velocity of the particle
		dc->dp[i].velocity = dct->velocity;
		dc->dp[i].velocity -= (dct->velocity * ((float)randrng(71) / 100.0f));

		// Set the gravity of the particle
		dc->dp[i].gravity = dct->gravity;

		// Assign a random tick to start this particle moving
		dc->dp[i].tick = randrng(dct->create_ticks);

		// Assume NO texture for this particle
		dc->dp[i].tn = NULL;
		dc->dp[i].tbuffer = NULL;

		// Is there a texture list pointer at all ?
		if(dct->texture_list)
		{
			// YES - is there a texture for this particle ?
			if(dct->texture_list[i])
			{
				// Create the memory needed to retain the texture

				// YES - Create the texture node for the particle
				dc->dp[i].tn = create_texture_from_memory(dct->texture_list[i],
					dc->dp[i].tbuffer,
					0,
					0,
					CREATE_NORMAL_TEXTURE,
					GR_TEXTURECLAMP_CLAMP,
					GR_TEXTURECLAMP_CLAMP,
					GR_TEXTUREFILTER_BILINEAR,
					GR_TEXTUREFILTER_BILINEAR);
			}
		}
	}

	// Create the drawing object to draw the debris
//	dobj = create_object("debris", 0, 0, DRAWORDER_DEBRIS, dc, draw_debris);
	dobj = create_object("debris", PLAYER_PID, 0, DRAWORDER_DEBRIS, dc, draw_debris);

	// Did the object create OK ?
	if(!dobj)
	{
		// NOPE - free up the resources
		_delete_debris(dc);
		fprintf(stderr, "Could not create debris object\r\n");
	}

	// Return the object pointer
	return(dobj);
}

// 4x4 matrix multiply (cross product)
static void _mxm(register float *r, register float *m1, register float *m2)
{
	register int	i;

	i = 12;
	while(i >= 0)
	{
		r[i]   = m1[i]*m2[0] + m1[i+1]*m2[4] + m1[i+2]*m2[8] + m1[i+3]*m2[12];
		r[i+1] = m1[i]*m2[1] + m1[i+1]*m2[5] + m1[i+2]*m2[9] + m1[i+3]*m2[13];
		r[i+2] = m1[i]*m2[2] + m1[i+1]*m2[6] + m1[i+2]*m2[10] + m1[i+3]*m2[14];
		r[i+3] = m1[i]*m2[3] + m1[i+1]*m2[7] + m1[i+2]*m2[11] + m1[i+3]*m2[15];
		i -= 4;
	}
}

__asm__("
	.set	noreorder
recip:
	jr			$31
	recip.s	$f0,$f12
	.set	reorder
");

float recip(float);

// Cross product between a 4x4 and a 4x1 array
static void mxv(void)
{
	register int	i;

	for(i = 0; i < 3; i++)
	{
		dp_vert[i].x   = _debris_vert[i].x*comp_mat[0] + _debris_vert[i].y*comp_mat[1] + _debris_vert[i].z*comp_mat[2]  + _debris_vert[i].w*comp_mat[3];
		dp_vert[i].y   = _debris_vert[i].x*comp_mat[4] + _debris_vert[i].y*comp_mat[5] + _debris_vert[i].z*comp_mat[6]  + _debris_vert[i].w*comp_mat[7];
		dp_vert[i].sow = _debris_vert[i].x*comp_mat[8] + _debris_vert[i].y*comp_mat[9] + _debris_vert[i].z*comp_mat[10] + _debris_vert[i].w*comp_mat[11];
		dp_vert[i].oow = recip(dp_vert[i].sow);

		dp_vert[i].x *= (-2.0f * dp_vert[0].oow * (HRES/2.0f));
		dp_vert[i].y *= (2.0f * dp_vert[0].oow * (HRES/2.0f));
	}
}

// This function is used to draw debris objects
static void draw_debris(void *od)
{
	register debris_control_t	*dc;
	register debris_particle_t	*dp;
	register int					i;
	register int					num_at_end = 0;
	register float					sin_a;
	register float					cos_a;

	// Get the debris control struction pointer
	dc = (debris_control_t *)od;

	// Make sure it's NOT null
	if(!dc)
	{
		return;
	}

	// Set the state
	dc->state = DEBRIS_STATE_RUN;

	// Set up the rendering engine state
	grDepthBufferFunction(GR_CMP_LESS);
	grDepthBufferMode(GR_DEPTHBUFFER_WBUFFER);
	grDepthMask(FXTRUE);
	grTexCombineFunction(0, GR_TEXTURECOMBINE_ONE);
	grConstantColorValue(0xffa0a0a0);
	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB);
	grDepthBiasLevel(0.0f);

	// Get pointer to debris particle array
	dp = dc->dp;

	// Generate the camera transform matrix

	// Process all of the debris particles
	for(i = 0; i < dc->num; i++)
	{
		// Are we suppose to draw this one yet ?
		if(dp->tick <= dc->ticks)
		{
			// YES - Is the particle above ground ?
			if(dp->y > -_debris_vert[0].x)
			{
				// YES - Generate the x, y, and z angles
				dp->x_ang += dp->x_rot;
				dp->y_ang += dp->y_rot;
				dp->z_ang += dp->z_rot;

				// Clamp the angles
				if(dp->x_ang >= (2*M_PI))
				{
					dp->x_ang -= (2*M_PI);
				}
				else if(dp->x_ang <= (-2*M_PI))
				{
					dp->x_ang += (2*M_PI);
				}

				if(dp->y_ang >= (2*M_PI))
				{
					dp->y_ang -= (2*M_PI);
				}
				else if(dp->y_ang <= (-2*M_PI))
				{
					dp->y_ang += (2*M_PI);
				}

				if(dp->z_ang >= (2*M_PI))
				{
					dp->z_ang -= (2*M_PI);
				}
				else if(dp->z_ang <= (-2*M_PI))
				{
					dp->z_ang += (2*M_PI);
				}

				// Apply new velocities to the current position
				dp->x += ((dp->dx + dp->mx) * (dp->velocity - (dc->friction * dp->velocity)));
				dp->y += ((dp->dy + dp->my) * (dp->velocity - (dc->friction * dp->velocity)));
				dp->z += ((dp->dz + dp->mz) * (dp->velocity - (dc->friction * dp->velocity)));

				// Apply gravity to the particle
				dp->y -= dp->gravity;

				// Generate next gravity value
				dp->gravity += dc->gravity;

				// Calculate next velocity
				dp->velocity -= (dc->friction * dp->velocity);

				// Calculate next velocity
				dp->mvelocity -= (dc->friction * dp->mvelocity);

				// Is is now below ground
				if(dp->y <= -_debris_vert[0].y)
				{
					// YES - Don't bother
					num_at_end++;
					continue;
				}

				// Generate the camera matrix
				memcpy(cam_mat, cambot.xform, sizeof(float)*12);

				// Generate the local X axis rotation matrix
				cos_a = fcos(dp->x_ang);
				sin_a = fsin(dp->x_ang);
				x_rot_mat[5] = x_rot_mat[10] = cos_a;
				x_rot_mat[6] = -sin_a;
				x_rot_mat[9] = sin_a;
	
				// Generate the local Y axis rotation matrix
				cos_a = fcos(dp->y_ang);
				sin_a = fsin(dp->y_ang);
				y_rot_mat[0] = y_rot_mat[10] = cos_a;
				y_rot_mat[8] = -sin_a;
				y_rot_mat[2] = sin_a;

				// Generate the local Z axis rotation matrix
				cos_a = fcos(dp->z_ang);
				sin_a = fsin(dp->z_ang);
				z_rot_mat[0] = z_rot_mat[5] = cos_a;
				z_rot_mat[1] = -sin_a;
				z_rot_mat[4] = sin_a;

				// Generate the Translation matrix
				trans_mat[3] = dp->x - cambot.x;
				trans_mat[7] = dp->y - cambot.y;
				trans_mat[11] = dp->z - cambot.z;

				// Contatenate the matrices (this gets the piece oriented and
				// positioned in the world
//				_mxm(i_mat, x_rot_mat, cam_mat);
//				_mxm(comp_mat, y_rot_mat, i_mat);
//				_mxm(i_mat, z_rot_mat, comp_mat);
//				_mxm(comp_mat, trans_mat, i_mat);
_mxm(i_mat, cam_mat, trans_mat);
_mxm(comp_mat, i_mat, x_rot_mat);
_mxm(i_mat, comp_mat, y_rot_mat);
_mxm(comp_mat, i_mat, z_rot_mat);

				// Generate the 3 transformed veritces
				mxv();

				// Is the triangle in front of the camera ?
				if(dp_vert[0].sow > 10.0f && dp_vert[1].sow >= 10.0f && dp_vert[2].sow >= 10.0f)
				{
					// YES - Is there a texture for the triangle ?
					if(dp->tn)
					{
						// YES - Set the texture source
						guTexSource(dp->tn->texture_handle);
					}

					dp_vert[0].x += (hres / 2.0f);
					dp_vert[0].y += (vres / 2.0f);

					// Snap the vertices to .25
					dp_vert[0].x += (float)(1<<21);
					dp_vert[0].y += (float)(1<<21);
					dp_vert[0].x -= (float)(1<<21);
					dp_vert[0].y -= (float)(1<<21);

					dp_vert[1].x += (hres / 2.0f);
					dp_vert[1].y += (vres / 2.0f);

					dp_vert[1].x += (float)(1<<21);
					dp_vert[1].y += (float)(1<<21);
					dp_vert[1].x -= (float)(1<<21);
					dp_vert[1].y -= (float)(1<<21);

					dp_vert[2].x += (hres / 2.0f);
					dp_vert[2].y += (vres / 2.0f);

					dp_vert[2].x += (float)(1<<21);
					dp_vert[2].y += (float)(1<<21);
					dp_vert[2].x -= (float)(1<<21);
					dp_vert[2].y -= (float)(1<<21);

					// Generate the sow and tow values (assumes square texture!!)
					// Not really needed if NOT textured!!!
					dp_vert[0].sow = 0.0f;
					dp_vert[0].tow = 0.5f * 256.0f * dp_vert[0].oow;

					dp_vert[1].sow = 1.0f * 256.0f * dp_vert[1].oow;
					dp_vert[1].tow = 0.0f;

					dp_vert[2].sow = 1.0f * 256.0f * dp_vert[2].oow;
					dp_vert[2].tow = 1.0f * 256.0f * dp_vert[2].oow;

					// Draw the triangle
					grDrawTriangleDma(&dp_vert[0], &dp_vert[1], &dp_vert[2], 0);
				}
			}

			// Particle is below ground - Increment below ground count
			else
			{
				num_at_end++;
			}
		}
		dp++;
	}

	// Increment the elapsed ticks
	dc->ticks++;

	// Are all of the particles below the gound ?
	if(num_at_end >= dc->num)
	{
		// YES - Set the debis state to done
		dc->state = DEBRIS_STATE_DONE;
	}
}

// This function is used to free resources used by a debris object
static void _delete_debris(debris_control_t *dc)
{
	register int	i;

	// Is it a valid pointer ?
	if(dc)
	{
		// YES - Is the debris particle array pointer valid ?
		if(dc->dp)
		{
			// YES - Free all of the textures used for this debris object
			for(i = 0; i < dc->num; i++)
			{
				// Is the texture node pointer valid ?
				if(dc->dp[i].tn)
				{
					// YES - delete the texture
					delete_texture(dc->dp[i].tn);
				}

				// Is the texture buffer pointer valid ?
				if(dc->dp[i].tbuffer)
				{
					// YES - free the memory
					JFREE(dc->dp[i].tbuffer);
				}
			}

			// Free the memory for the debris particle array
			JFREE(dc->dp);
		}

		// Free the memory for the debris control structure
		JFREE(dc);
	}
}

// This function is used to delete a debris object
void delete_debris(struct object_node *dobj)
{
	debris_control_t	*dc;

	// Get the debris object control structure pointer
	dc = (debris_control_t *)dobj->object_data;

	// Free up the resources
	_delete_debris(dc);

	// Delete the object too
	delete_object(dobj);
}


// This function is used to retrieve the state of a debris object
int get_debris_status(struct object_node *dobj)
{
	return(((debris_control_t *)dobj->object_data)->state);
}
#endif