//
// debris.h - Include file for debris
//
// $Revision: 1 $
//
// $Author: Mlynch $
//
#ifndef	__DEBRIS_H__
#define	__DEBRIS_H__

#ifndef	__SPRITES_H__
#include	<goose\sprites.h>
#endif

// Structure definition of a debris partical
typedef struct debris_particle
{
	int				tick;			// Tick to start drawing particle
	float				velocity;	// Current velocity of the particle
	float				mvelocity;	// Momentive velocity of the particle
	float				gravity;		// Current gravity velocity of particle
	float				x;				// Current X position of particle
	float				y;				// Current Y position of particle
	float				z;				// Current Z position of particle
	float				dx;			// Current X velocity of particle
	float				dy;			// Currnet Y velocity of particle
	float				dz;			// Current Z velocity of particle
	float				mx;			// Current X Momentum of paritcle
	float				my;			// Current X Momentum of paritcle
	float				mz;			// Current X Momentum of paritcle
	float				x_ang;		// Current x axis rotation of the particle
	float				y_ang;		// Current y axis rotation of the particle
	float				z_ang;		// Current z axis rotation of the particle
	float				x_rot;		// X axis rotation of the particle
	float				y_rot;		// Y axis rotation of the particle
	float				z_rot;		// Z axis rotation of the particle
	texture_node_t	*tn;			// Texture node for this particle
	void				*tbuffer;	// Memory used to hold the texture for particle
} debris_particle_t;


// Structure definition of a debris control structure
typedef struct debris_control
{
	float					friction;	// Frictional deceleration force
	float					gravity;		// Gravitational acceleration force
	int					ticks;		// Elapsed ticks since start
	int					num;			// Number of particles
	int					state;		// State of the debris object
	debris_particle_t	*dp;			// Array of particles
} debris_control_t;

// Structure definition of a vertex
typedef struct Lvert
{
	float	x;
	float	y;
	float	z;
	float	w;
} Lvert_t;

// Definitions of debris states
#define	DEBRIS_STATE_INIT	0
#define	DEBRIS_STATE_RUN	1
#define	DEBRIS_STATE_DONE	2

// Structure definition of a debris configuration structure
typedef struct debris_config
{
	float	x;						// Starting world x position of debris pieces
	float	y;						// Starting world y position of debris pieces
	float	z;						// Starting world z position of debris pieces
	float	mx;					// Momemtum in x direction
	float	my;					// Momentum in y direction
	float	mz;					// Momentum in z direction
	float	velocity;			// Initial velocity of a piece of debris
	float	mvelocity;			// Momentum velocity
	float	friction;			// Frictional force used to decelerate debris peices
	float	gravity;				// Gravitational force used to drop pieces
	int	x_ang;				// Max random x axis rotation angle for debris pieces (degrees)
	int	y_ang;				// Max random y axis rotation angle for debris pieces (degrees)
	int	z_ang;				// Max random z axis rotation angle for debris pieces (degrees)
	int	num_particles;		// Number of pieces of debris to create
	int	create_ticks;		// Random count with which to create debris pieces
	char	**texture_list;	// List of textures to put on debris pieces
} debris_config_t;

// Function prototypes
void delete_debris(struct object_node *);
struct object_node *make_debris(debris_config_t *);
int get_debris_status(struct object_node *);


#endif
