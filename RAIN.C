#include	<stdio.h>
#include	<stdlib.h>
#include	<goose/goose.h>

#include "include/ani3D.h"
#include "include/game.h"
#include "include/player.h"
#include "include/id.h"

//void grDrawTriangleDma(MidVertex *, MidVertex *, MidVertex *, int);

void do_rain(void);

typedef struct	particle
{
	float	x;
	float	y;
	float	z;
	float	x_vel;
	float	y_vel;
	MidVertex	verts[3];
} particle_t;

static particle_t	*particle;
static int			num_particles;

static float	lx[] = {-1.0f, -1.0f, -0.75f, -0.75f, -0.5f, -0.5f, -0.25f, -0.25f};
static float	rx[] = { 1.0f, 0.75f,  0.75f,  0.5f,   0.5f, 0.25f,  0.25f, -0.0f};

static void init_particle(particle_t *p)
{
	// Get a random starting position for the particle
	p->x = (float)randrng((int)hres);
	p->y = vres;
	p->z = (float)randrng(16);

	// Set the velocity of the particle base on the particles Z
	p->x_vel = 0.0f;
	p->y_vel = ((20.0f - p->z) / 4.0f) + 2.0f;

	// Generate the verticies for the particle
	// Vary the size of the particle base on it's Z value
	p->verts[0].x = p->x;
	p->verts[0].y = p->y + (20.0f * (((19.0f - p->z) / 19.0f)) + 1.0f);
	p->verts[0].oow = 1.0f/(150.0f + (p->z * 10.0f));
	p->verts[0].sow = 0.0f;
	p->verts[0].tow = 0.0f;

//	p->verts[1].x = p->x - (.025f * (((19.0f - p->z) / 19.0f)) + 1.0f);
	p->verts[1].x = p->x + lx[(int)p->z / 2];
	p->verts[1].y = p->y;
	p->verts[1].oow = 1.0f/(150.0f + (p->z * 10.0f));
	p->verts[1].sow = 0.0f;
	p->verts[1].tow = 0.0f;

//	p->verts[2].x = p->x + (0.25f * (((19.0f - p->z) / 19.0f)) + 1.0f);
	p->verts[2].x = p->x + rx[(int)p->z / 2];
	p->verts[2].y = p->y;
	p->verts[2].oow = 1.0f/(150.0f + (p->z * 10.0f));
	p->verts[2].sow = 0.0f;
	p->verts[2].tow = 0.0f;

	if(p->verts[2].x - p->verts[1].x < 0.25f)
	{
		p->verts[1].x = p->verts[2].x - 0.25f;
	}

	if(p->verts[0].y - p->verts[1].y < 0.25f)
	{
		p->verts[0].y = p->verts[1].y + 0.25f;
	}

	p->verts[0].x += (float)(1<<21);
	p->verts[1].x += (float)(1<<21);
	p->verts[2].x += (float)(1<<21);
	p->verts[0].y += (float)(1<<21);
	p->verts[1].y += (float)(1<<21);
	p->verts[2].y += (float)(1<<21);

	p->verts[0].x -= (float)(1<<21);
	p->verts[1].x -= (float)(1<<21);
	p->verts[2].x -= (float)(1<<21);
	p->verts[0].y -= (float)(1<<21);
	p->verts[1].y -= (float)(1<<21);
	p->verts[2].y -= (float)(1<<21);
}

static int init_particles(int num)
{
	int	i;

	// Allocate memory for the number of particles requested
	particle = (particle_t *)malloc(sizeof(particle_t) * num);

	// ERROR ?
	if(!particle)
	{
		// YES - print out message and return fail
		fprintf(stderr, "Can not allocate memory for %d particles\r\n", num);
		return(0);
	}

	// Initialize each of the particles
	for(i = 0; i < num; i++)
	{
		init_particle(&particle[i]);
	}

	// Set the number of particles
	num_particles = num;

	// Return OK
	return(1);
}

static void do_particles(void)
{
	int	i;
	int	j;
	particle_t	*p;

	p = particle;
	for(j = 0; j < num_particles; j++)
	{
		for(i = 0; i < 3; i++)
		{
			p->verts[i].x -= p->x_vel;
			p->verts[i].y -= p->y_vel;
			p->verts[i].oow = 1.0f/((150.0f + (p->z * 10.0f)));
		}

		if(p->verts[0].y < 0.0f || p->verts[0].x < 0.0f)
		{
			init_particle(p);
		}

		grDrawTriangleDma(&p->verts[0], &p->verts[1], &p->verts[2], 1);

		++p;
	}
}


static void draw_particles(void *parts)
{
	grTexCombineFunction(0, GR_TEXTURECOMBINE_ONE);
	guAlphaSource(GR_ALPHASOURCE_CC_ALPHA);
	grConstantColorValue(0x80a0a0a0);
	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB);
	grAlphaBlendFunction(GR_BLEND_SRC_ALPHA,
		GR_BLEND_ONE_MINUS_SRC_ALPHA,
		GR_BLEND_ONE,
		GR_BLEND_ZERO);
	grDepthBufferFunction(GR_CMP_LESS);
	grDepthBiasLevel(0.0f);

	do_particles();

}

void do_rain(void)
{
	if(!init_particles(randrng(200) + 400))
	{
		fprintf(stderr, "do_rain(): could not initialize rain drops\r\n");
		return;
	}
	create_object("rain", 0, 0, 0x00058000, particle, draw_particles);
}
