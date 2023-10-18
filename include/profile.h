// misc definitions
#define NUM_PROFILES	30
#define PFNAMELEN		20

// profile defintions
#define PRF_ABLOCK		0
#define PRF_COLLIS		1
#define PRF_SHADOW		2
#define PRF_LASTPLAYER	3
#define PRF_FIELD1		4
#define PRF_FIELD2		5
#define PRF_PLAYER1		6
#define PRF_PLAYER14	19
#define PRF_RDATA		20

// declarations
typedef struct _xtime_t
{
	char	name[PFNAMELEN];// function name
	long	high;			// highest execution time
	long	low;			// lowest execution time
	long	mean;			// mean execution time
	long	n;				// data points represented by mean
	long	lstart;			// timer value at last start
	long	lstop;			// timer value at last stop
	long	state;			// 0 if waiting for start, 1 if waiting for stop
} xtime_t;

#ifdef PROFILE
extern xtime_t fprofiles[NUM_PROFILES];

// prototypes
void profile_dump( int, int );
void reset_profiles( int, int );
void init_profile( char *, int );
void pftimer_start( int );
void pftimer_end( int );
#else
#define profile_dump(x,y)	;
#define reset_profiles(x,y)	;
#define init_profile(x,y)	;
#define pftimer_start(x)	;
#define pftimer_end(x)		;
#endif