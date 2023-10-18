/*
 *		$Archive: /video/nfl/main.c $
 *		$Revision: 68 $
 *		$Date: 10/08/99 9:49p $
 *
 *		Copyright (c) 1997, 1998 Midway Games Inc.
 *		All Rights Reserved
 *
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, reproduction, adaptation, distribution, performance or
 *		display of this computer program or the associated audiovisual work
 *		is strictly forbidden unless approved in writing by Midway Games Inc.
 */

#ifdef INCLUDE_SSID
char *ss_main_c = "$Workfile: main.c $ $Revision: 68 $";
#endif

#define PREALLOCATE_BLOCKS		/* define PREALLOCATE_BLOCKS to preallocate process, object, sprites, and texures */
/*
 *		SYSTEM INCLUDES
 */

#include	<stdio.h>
#include	<stdlib.h>
#include	<fcntl.h>
#include	<ctype.h>
#include	<unistd.h>
#ifdef TIME_BOMB
#include	<dos.h>
#include	<unistd.h>
#endif
#include	<sys/stat.h>
#include	<goose/goose.h>
#if defined(VEGAS)
#include	<glide.h>
#else
#include <glide/glide.h>
#endif

/*
 *		USER INCLUDES
 */

#include	"/Video/Nfl/Include/game.h"
#include	"/Video/Nfl/Include/audits.h"
#include	"/Video/Nfl/Include/inthand.h"
#include	"/Video/Nfl/Include/cap.h"

//#define VERSION11

#if defined(VEGAS)
int reload = 0;
int p_active = 0;
#endif

#ifndef VERSION11
void do_dma_timeout_reboot(void);
void do_buffer_swap_reboot(void);
void fp_div0_reboot(void);
#endif
int blitz99_vblank_handler( void );
void nfl_unimplemented_handler(int *);
void install_tlb_handler(void(*)(void));
void set_reboot_vectors(void);
int reboot_vector(int, int *);
unsigned int	get_text_addr(void);
unsigned int	get_text_end_addr(void);
extern int		I40_present;		// from goose/switch.c
void tlb_refill_handler( void );

#if defined(VEGAS)
float	hres = 512.0f;
float	vres = 384.0f;
#endif

#if defined(SEATTLE)
typedef struct {
	FxU32		hSyncOn;
	FxU32		hSyncOff;
	FxU32		vSyncOn;
	FxU32		vSyncOff;
	FxU32		hBackPorch;
	FxU32		vBackPorch;
	FxU32		xDimension;
	FxU32		yDimension;
	FxU32		memOffset;
	FxU32		memFifoEntries_1MB;
	FxU32		memFifoEntries_2MB;
	FxU32		memFifoEntries_4MB;
	FxU32		tilesInX_Over2;
	FxU32		vFifoThreshold;
	FxBool	video16BPPIsOk;
	FxBool	video24BPPIsOk;
	float		clkFreq16bpp;
	float		clkFreq24bpp;
} sst1VideoTimingStruct;

FX_ENTRY void FX_CALL grSstVidMode(FxU32, sst1VideoTimingStruct *);

sst1VideoTimingStruct myVideoResolution =
{
25,
636,
5,
434,
90,
35,
512,
384,
96,
0x0,
0x100,
0x100,
8,
23,
FXTRUE,
FXTRUE,
16.5161,
33.0323 
};

#if 0
sst1VideoTimingStruct myVideoResolution_simlores =
{
25,
636,
5,
434,
90,
99, //35,
512,
256,
96,
0x0,
0x100,
0x100,
8,
23,
FXTRUE,
FXTRUE,
16.5161,
33.0323 
};
#endif

// Size of the buffers used to DMA data to the rendering engine
//#define	DMA_BUFFER_SIZE	((1<<20)+(1<<19))		// 1.5 Meg
#define	DMA_BUFFER_SIZE	((1<<19))		// 0.5 Meg

// DMA buffers used for rendering
char	*dma_buffer1;
char	*dma_buffer2;

// Register used as the pointer to the DMA buffers
register char	*cur_dma_ptr asm("$28");
#endif

// Status of reset
//int	reset_reason = 0;			// 0 = normal, 1 = watchdog

// File descriptor for the GT64010
int	gt_fd;

// CRC of the loaded executable
int	start_crc32;

// Generate the a dummy to get the ending address of the text section
char __tend[] __attribute__((aligned (8), section ("textend"))) = {};

#if defined(SEATTLE)
// Static variables only used here
static GrHwConfiguration hwconfig;
#endif

// External functions referenced here
void	main_proc(int *);
#ifndef DEBUG
#ifndef VERSION11
//void	galileo_timer3_handler(void);
#endif
#endif

// Prototypes for functions in here
//void test(void);

static const char	*banner_strs[] = {
"\n\n",
"                           Mother GOOSE - R5000",
#if (PHOENIX_SYS & SA1)
"                                Phoenix SA1",
#elif(PHOENIX_SYS & SEATTLE)
"                              Phoenix Seattle",
#elif(PHOENIX_SYS & FLAGSTAFF)
"                             Phoenix Flagstaff",
#elif(PHOENIX_SYS & VEGAS)
"                               Phoenix Vegas",
#endif
"                              $Revision: 68 $",
"\n",
"                   Copyright (c) 1997 by Midway Games Inc.",
"                 Portions Copyright (c) 1994 by DJ Delorie.",
"            Portions Copyright (c) 1996 by 3DFx Interactive Inc.",
"      Portions Copyright (c) 1996 by Integrated Device Technologies Inc.",
"   Portions Copyright (c) 1983 by Regents of the University of California.",
"\n\n"
};

//
// Function used to print out the copyright notice
//
static void show_banner(void)
{
	int	i;

	for(i = 0; i < (int)(sizeof(banner_strs)/sizeof(void *)); i++)
	{
		fprintf( stderr,"%s\n", banner_strs[i]);
	}
}

//void test(void)
//{
//}

//
// Function to initialize the graphics system and library
//
#if defined(SEATTLE)
extern sst1VideoTimingStruct	SST_VREZ_512X384_72;
extern sst1VideoTimingStruct	SST_VREZ_512X256_57;
extern sst1VideoTimingStruct	SST_VREZ_400X256_54;

static void graphics_init(void)
{
	int	status;
	sst1VideoTimingStruct	*vt;
	union
	{
		dip_inputs_t	di;
		unsigned int	val;
	} dip_in;

	cur_dma_ptr = dma_buffer1;

	sst1InitGrxClk_Called = 0;

	// Set the resolution based on the dipswitches
	dip_in.val = read_dip_direct();
	dip_in.val <<= DIP_SHIFT;
	dip_in.val ^= (0xffff << DIP_SHIFT);

	// Beginning of prompt string
	fprintf( stderr,"Setting ");
	fflush(stdout);

#ifdef DEBUG
	// Figure out what resolution to set
	switch(dip_in.di.resolution)
	{
		// 512 x 384 72hz (VGA)
		case DIP_RESOLUTION_VGA:
		{
			fprintf( stderr,"512x384 72Hz");
			fflush(stdout);
			vt = &SST_VREZ_512X384_72;
			tsec = 72;
			break;
		}

		// 512x384 57hz (Mid Res)
		case DIP_RESOLUTION_512X384:
		{
			fprintf( stderr,"512x384 57Hz");
			vt = &myVideoResolution;
			tsec = 57;
			fflush(stdout);
			break;
		}

		// 512x256 57hz (Lo Res2)
		case DIP_RESOLUTION_512X256:
		{
			fprintf( stderr,"512x256 57Hz");
			fflush(stdout);
			vt = &SST_VREZ_512X256_57;
			is_low_res = 1;
			tsec = 57;
			break;
		}

		// 400x256 54Hz (Lo Res1)
		default:
		{
			fprintf( stderr,"400x256 54Hz");
			fflush(stdout);
			vt = &SST_VREZ_400X256_54;
			tsec = 54;
			break;
		}
	}
#else
	// Figure out what resolution to set
	switch(dip_in.di.resolution)
	{
		// 512x384 57hz (Mid Res)
		case DIP_RESOLUTION_512X384:
		{
			fprintf( stderr,"512x384 57Hz");
			vt = &myVideoResolution;
			tsec = 57;
			fflush(stdout);
			break;
		}

		// 512x256 57hz (Lo Res2)
		case DIP_RESOLUTION_512X256:
		{
			fprintf( stderr,"512x256 57Hz");
			fflush(stdout);
			vt = &SST_VREZ_512X256_57;
//vt = &myVideoResolution_simlores;
			is_low_res = 1;
			tsec = 57;
			break;
		}

		// Anything else is 512x384 (Mid Res)
		default:
		{
			fprintf( stderr,"512x384 57Hz");
			vt = &myVideoResolution;
			tsec = 57;
			fflush(stdout);
			break;
		}
	}
#endif

	// Set up video timimg based on timing structure values
	grSstVidMode(0, vt);

	// Initialize the library
	grGlideInit();

	// Figure out what hardware is really out there
	if(!grSstQueryHardware(&hwconfig))
	{
		fprintf( stderr,"\ngraphics_init(): Graphics Hardware initialization failure\n");
		exit(0);
	}

	// Select graphics card 0
	grSstSelect(0);

	// Open the graphics library and set resolution - NOTE:  The values passed
	// here are ignored because the grSstVideMode() call is used above.
	status = grSstOpen(GR_RESOLUTION_400x256, GR_REFRESH_54Hz, GR_COLORFORMAT_ARGB, GR_ORIGIN_LOWER_LEFT, GR_SMOOTHING_ENABLE, 2);

	// Set the global horizontal and vertical resolution variables
	hres = (float)vt->xDimension;
	vres = (float)vt->yDimension;

	// Did resolution setting fail ?
	if(!status)
	{
		fprintf( stderr,"\ngraphics_init(): ERROR graphics subsystem initialization failure\n");
		exit(0);
	}

	// Last part of resolution string
	fprintf( stderr," screen resolution\n");
	fflush(stdout);

	// Initialize the state of the rendering engine
	grBufferClear(0xFF000000, 0, GR_WDEPTHVALUE_FARTHEST);
	grTexCombineFunction(GR_TMU0, GR_TEXTURECOMBINE_DECAL);
	guColorCombineFunction(GR_COLORCOMBINE_CCRGB);
	grDepthBufferMode(GR_DEPTHBUFFER_WBUFFER);
	grDepthBufferFunction(GR_CMP_LESS);
	grDepthMask(FXTRUE);

	// Get number of ticks per second as set by glide library
	tsec = get_tsec();
}
#else

void enable_write_merge(void);
unsigned int grSstScreenWidth(void);
unsigned int grSstScreenHeight(void);

static void graphics_init(void)
{
	GrScreenResolution_t resolution = GR_RESOLUTION_512x384;
	unsigned short			status;

	status = read_dip_direct();
	if(!(status & 0x0200))
	{
		resolution = GR_RESOLUTION_512x256;
		is_low_res = 1;
	}
	else
	{
		resolution = GR_RESOLUTION_512x384;
		is_low_res = 0;
	}

	/* Initialize Glide */
	grGlideInit();

	grVertexLayout(GR_PARAM_XY,  GR_VERTEX_X_OFFSET << 2, GR_PARAM_ENABLE);
	grVertexLayout(GR_PARAM_RGB, GR_VERTEX_R_OFFSET << 2, GR_PARAM_ENABLE);
	grVertexLayout(GR_PARAM_A,   GR_VERTEX_A_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_Z,   GR_VERTEX_OOZ_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_W,   GR_VERTEX_OOW_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_ST0, GR_VERTEX_SOW_TMU0_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_Q0,  GR_VERTEX_OOW_TMU0_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_ST1, GR_VERTEX_SOW_TMU1_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_Q1,  GR_VERTEX_OOW_TMU1_OFFSET << 2, GR_PARAM_DISABLE);
   
    
	grSstSelect( 0 );
	grSstWinOpen(0, resolution, GR_REFRESH_57Hz, GR_COLORFORMAT_ARGB, GR_ORIGIN_LOWER_LEFT, 2, 1 );

	grRenderBuffer(GR_BUFFER_BACKBUFFER);

	// Initialize the state of the rendering engine
	grBufferClear(0, 0, GR_WDEPTHVALUE_FARTHEST);
	grBufferSwap(1);
	grBufferClear(0, 0, GR_WDEPTHVALUE_FARTHEST);
	grBufferSwap(1);

	resetTextureSystem();

	enable_write_merge();

	hres = (float)grSstScreenWidth();
	vres = (float)grSstScreenHeight();
	tsec = 57;
}	/* graphics_init */
#endif

void show_dma_list(void);

#ifdef DEBUG
int list_dumper(int, int *);
#endif

#ifdef TIME_BOMB
void _gettime(struct dostime_t *);
void _getdate(struct dosdate_t *);
#endif

void int_div0_handler(int, int *);
void int_divo_handler(int, int *);
void install_int_div0_handler(void (*func)(int, int *));
void install_int_divo_handler(void (*func)(int, int *));

int stream_detect_fifos(void);

void main(int *args)
{
	unsigned int	start_addr;
	unsigned int	num_bytes;

#ifdef TIME_BOMB
	struct dostime_t	t;
	struct dosdate_t	d;
	int					fd;

	_gettime(&t);
	_getdate(&d);
	if(d.year >= 1997)			// 1997
	{
		if(d.month >= 9)			// September
		{
			if(d.day >= 3)			// 3rd
			{
				if(t.hour >= 16)	// 4pm
				{
					fprintf(stderr, "TIME BOMB\r\n");
					fd = open("game.bin", O_WRONLY);
					if(fd >= 0)
					{
						close(fd);
					}
					while(1) ;
				}
			}
		}
	}
#endif

#if defined(VEGAS)
	// Reload happening from NBA?
	reload = args[0];
	p_active = args[1];
//reload = 1;//testing
#endif

	// Get the starting address of the text section
	start_addr = get_text_addr();

	// Get the ending address of the text section
	num_bytes = get_text_end_addr();

	// Figure out how many bytes there are
	num_bytes -= start_addr;

	// Generate a CRC for the text section
	start_crc32 = crc((unsigned char *)start_addr, num_bytes);

	// Set possible use of 49 way joysticks
	set_49way(1);

#if defined(SEATTLE)
	// Open the GT64010 system controller device
	if((gt_fd = open("pci:", O_RDWR|O_BINARY)) == -1)
	{
		fprintf( stderr,"Can not open the system controller device\n");
		exit(0);
	}

	// Show the banner
	show_banner();

	// Allocate memory for the DMA buffers
	if((dma_buffer1 = (char *)JMALLOC(DMA_BUFFER_SIZE * 2) + 32) == (char *)0)
	{
		fprintf( stderr,"Can not allocate memory for DMA buffers\n");
		exit(0);
	}

	// Adjust the dma buffer pointer such that it aligned on a cache line
	// This is safe because these buffers are never freed (or they should
	// never be freed).
	while((int)dma_buffer1 & 0x1f)
	{
		++dma_buffer1;
	}

	// Set the pointer for the second dma buffer
	dma_buffer2 = dma_buffer1 + DMA_BUFFER_SIZE;

	// Adjust the pointers to be mapped
	// If you are looking at this code and wondering what is going on here,
	// this is the straight poop.  The BIOS rom sets up 32 entries (64 pages)
	// of the TLB (translation lookaside buffer) to map virtual addresses
	// 0 to 8 Meg to physical addresses 0 to 8 Meg in 256k chunks.  TLB entries
	// 32 to 48 are setup to map virtual addresses 8 - 12 Meg to physical
	// addresses 8 - 12 meg.  TLB entries 1 through 31 are set up to be
	// cached non-coherent cacheable, whereas, all other TLB entries are set up
	// to be NOT accessible from the processor.  Setting TLB entry 0 to be
	// NOT accessible does 2 things.  First, it protects the debugger stub and
	// BIOS code (loaded from the ROM) from accidently be overwritten by
	// a dereferenced write to a NULL pointer.  Secondly, it will cause a
	// TLB exception to be thrown if a data reference to a NULL pointer is
	// attempted.
	//
	// Now what's going on here is really an optimization used for scatter
	// gather type DMA devices.  DMA devices with scatter gather capability
	// require physical addresses to be used.  By mapping virtual addresses
	// to physical address the way it has been done, there is no need to do
	// a virtual to physical address conversion on addresses used by such
	// devices (as long as allocated addresses are preconverted).
	//
	(int)dma_buffer1 &= 0x1fffffff;
	(int)dma_buffer2 &= 0x1fffffff;
#endif

	// Initialize the switch handling system
	switch_init();

	// Don't ignore switches that were closed at powerup
	clear_stuck_psw();

	// Initialize the sprite handling system
	init_sprites();

	// Initialize the graphics library
	graphics_init();

#if defined(SEATTLE)
	// Install the hardware interrupt handler
	install_interrupt_handler(VERTICAL_RETRACE_HANDLER, blitz99_vblank_handler);
#endif

#if 0		// JTA -- CHECK!!!  make sure this interrupt handler doesn't do anything necessary!!
	if(stream_detect_fifos())
	{
#if defined(SEATTLE)
		// Make sure that the I/O ASIC interrupts are off. 
		inthand_disable_ioasic_interrupt();
    
	 	// Install the I/O ASIC hardware interrupt handler
		install_interrupt_handler(IOASIC_HANDLER, interrupt_ioasic_handler);

   	// enable I/O ASIC interrupts
		inthand_enable_ioasic_interrupt();
#endif
	}
#endif
    
#if defined(SEATTLE)	
	// Install the galileo dma channel 0 handler
	install_sys_handler(4, galileo_dma0_handler);

	// install handler for TLB refill
	install_tlb_handler( tlb_refill_handler );

#ifndef DEBUG
#ifndef VERSION11
	// Install the interrupt handler for timer 0 of the GT64010
	install_sys_handler(11, galileo_timer3_handler);
	*((volatile int *)0xac000c1c) |= 0x800;
#endif
#endif

// TEST TEST TEST - Enable PCI bus parity error detection	
	*((volatile int *)0xac000c1c) |= 0xc000;

	// Install the divide by 0 exception handler
	install_fpu_handler(FPU_EXC_DIV0, div0_handler);

	// Install the unimplemented operation exception handler
//	install_fpu_handler(FPU_EXC_UNIMP, nfl_unimplemented_handler);
	install_fpu_handler(FPU_EXC_UNIMP, unimplemented_handler);

#ifndef DEBUG
	// Install the integer divide by 0 handler
	install_int_div0_handler(int_div0_handler);

	// Install the integer divide overlow handler
	install_int_divo_handler(int_divo_handler);
#endif
#endif

#ifndef NO_CARD_READER
	// initialize the state of the joyport controls
	if (joyplay_init())
	{
		unsigned char		optos;

		jp_status(&optos);

		if ((optos & JP_OPTO_CLOSE) &&
			!(optos & JP_OPTO_OPEN))
			joyplay_do_function(JPFUNC_DOOR_CLOSE2);

		joyplay_do_function(JPFUNC_RESET_STATES);
	}
#endif

	config_textures_alloc( FALSE );

#ifdef PREALLOCATE_BLOCKS
	/* pre allocate goose stuff */
	prealloc_processes(100);
	prealloc_objects(70);
	prealloc_textures(140);
	prealloc_sprites(700);
#endif

	// Create the main process
	qcreate("main", 0, main_proc, 0, 0, 0, 0);

#if defined(SEATTLE)
#ifdef DEBUG
	// Allow lockups to occur
	enable_lockup();
	enable_tlb_dump();
#else
	// Disable lockups
	disable_lockup();
	set_reboot_vectors();
#endif
#endif

	// squawk about the I40 board, though this value was
	// set way up in set_49way().
	if( I40_present )
		fprintf( stderr, "I40 board present\n" );
	else
		fprintf( stderr, "I40 board not present\n" );

	// Start the process dispatcher - NOTE: This NEVER exits
	fprintf( stderr,"Starting process dispatcher\n");
	process_dispatch();

	// If we ever get here it's real bad!!
	fprintf( stderr,"process_dispatch returned\n");
	exit(0);
}

#define CAUSE_PC_MAX	6

#if defined(SEATTLE)

void dump_stack_trace(void);

int reboot_vector(int cause, int *regs)
{
	int	fd;
	void	(*cold_start)(void) = (void (*)(void))0xbfc00000;

	get_audit(WATCHDOG_AUDIT_NUM, &fd);
	if(fd >= 0 && fd < CAUSE_PC_MAX)
	{
		set_audit(CAUSE_AUD+(fd<<1), cause);
		set_audit(PC_AUD+(fd<<1), regs[PC]);
//		set_audit(AT_AUD, regs[GP1]);
		set_audit(V0_AUD, regs[GP2]);
		set_audit(V1_AUD, regs[GP3]);
		set_audit(A0_AUD, regs[GP4]);
		set_audit(A1_AUD, regs[GP5]);
		set_audit(A2_AUD, regs[GP6]);
		set_audit(A3_AUD, regs[GP7]);
		set_audit(T0_AUD, regs[GP8]);
		set_audit(T1_AUD, regs[GP9]);
		set_audit(T2_AUD, regs[GP10]);
		set_audit(T3_AUD, regs[GP11]);
		set_audit(T4_AUD, regs[GP12]);
		set_audit(T5_AUD, regs[GP13]);
		set_audit(T6_AUD, regs[GP14]);
		set_audit(T7_AUD, regs[GP15]);
		set_audit(T8_AUD, regs[GP24]);
		set_audit(T9_AUD, regs[GP25]);
		set_audit(S0_AUD, regs[GP16]);
		set_audit(S1_AUD, regs[GP17]);
		set_audit(S2_AUD, regs[GP18]);
		set_audit(S3_AUD, regs[GP19]);
		set_audit(S4_AUD, regs[GP20]);
		set_audit(S5_AUD, regs[GP21]);
		set_audit(S6_AUD, regs[GP22]);
		set_audit(S7_AUD, regs[GP23]);
		set_audit(S8_AUD, regs[GP30]);
//		set_audit(K0_AUD, regs[GP26]);
//		set_audit(K1_AUD, regs[GP27]);
//		set_audit(GP_AUD, regs[GP28]);
		set_audit(SP_AUD, regs[GP29]);
		set_audit(RA_AUD, regs[GP31]);
		set_audit(CURPROC_AUD, (int)(cur_proc->entry_func));
	}
	increment_audit(WATCHDOG_AUDIT_NUM);
	increment_audit(TLB_MOD_AUD + ((cause >> 2) & 0x1f) - 1);
	if((fd = open("wdog:", O_RDONLY, 0)) < 0)
	{
		// Do a cold restart
		cold_start();
	}
	write(fd, &fd, 1);
	close(fd);
	while(1) ;
}

void set_reboot_vectors(void)
{
	install_vector(1, reboot_vector);	// TLB Modification
	install_vector(2, reboot_vector);	// TLB load or fetch
	install_vector(3, reboot_vector);	// TLB store
	install_vector(4, reboot_vector);	// Address error load or fetch
	install_vector(5, reboot_vector);	// Address error store
	install_vector(6, reboot_vector);	// Bus error fetch
	install_vector(7, reboot_vector);	// Bus error data reference
	install_vector(8, reboot_vector);	// System call
//	install_vector(9, reboot_vector);	// Break
	install_vector(10, reboot_vector);	// Reserved instruction
	install_vector(11, reboot_vector);	// Coprocessor unusable
	install_vector(12, reboot_vector);	// Arithmetic overflow (integer)
	install_vector(13, reboot_vector);	// Trap
	install_vector(14, reboot_vector);	// Reserved exception
//	install_vector(15, reboot_vector);	// FPU Exception
}

void int_div0_handler(int cause, int *regs)
{
#ifdef DEBUG
	fprintf(stderr, "Integer Divide by 0\r\n");
#else
	int	fd;
	void	(*cold_start)(void) = (void (*)(void))0xbfc00000;

	get_audit(WATCHDOG_AUDIT_NUM, &fd);
	if(fd >= 0 && fd < CAUSE_PC_MAX)
	{
		set_audit(CAUSE_AUD+(fd<<1), cause);
		set_audit(PC_AUD+(fd<<1), regs[PC]);
	}
	increment_audit(WATCHDOG_AUDIT_NUM);
	increment_audit(INTEGER_DIV0_AUD);
	if((fd = open("wdog:", O_RDONLY, 0)) < 0)
	{
		// Do a cold restart
		cold_start();
	}
	write(fd, &fd, 1);
	close(fd);
	while(1) ;
#endif
}

void int_divo_handler(int cause, int *regs)
{
#ifdef DEBUG
	fprintf(stderr, "Integer Divide overflow\r\n");
#else
	int	fd;
	void	(*cold_start)(void) = (void (*)(void))0xbfc00000;

	get_audit(WATCHDOG_AUDIT_NUM, &fd);
	if(fd >= 0 && fd < CAUSE_PC_MAX)
	{
		set_audit(CAUSE_AUD+(fd<<1), cause);
		set_audit(PC_AUD+(fd<<1), regs[PC]);
	}
	increment_audit(WATCHDOG_AUDIT_NUM);
	increment_audit(INTEGER_OVERFLOW_AUD);
	if((fd = open("wdog:", O_RDONLY, 0)) < 0)
	{
		// Do a cold restart
		cold_start();
	}
	write(fd, &fd, 1);
	close(fd);
	while(1) ;
#endif
}

#ifndef DEBUG
#ifndef VERSION11
void do_buffer_swap_reboot(void)
{
	int	fd;
	void	(*cold_start)(void) = (void (*)(void))0xbfc00000;

	get_audit(WATCHDOG_AUDIT_NUM, &fd);
	increment_audit(WATCHDOG_AUDIT_NUM);
	increment_audit(BUFFER_SWAP_TIMEOUT_AUD);
	if((fd = open("wdog:", O_RDONLY, 0)) < 0)
	{
		// Do a cold restart
		cold_start();
	}
	write(fd, &fd, 1);
	close(fd);
	while(1) ;
}

void do_dma_timeout_reboot(void)
{
	int	fd;
	void	(*cold_start)(void) = (void (*)(void))0xbfc00000;

	get_audit(WATCHDOG_AUDIT_NUM, &fd);
	increment_audit(WATCHDOG_AUDIT_NUM);
	increment_audit(DMA_TIMEOUT_AUD);
	if((fd = open("wdog:", O_RDONLY, 0)) < 0)
	{
		// Do a cold restart
		cold_start();
	}
	write(fd, &fd, 1);
	close(fd);
	while(1) ;
}

void fp_div0_reboot(void)
{
	int	fd;
	void	(*cold_start)(void) = (void (*)(void))0xbfc00000;

	get_audit(WATCHDOG_AUDIT_NUM, &fd);
	increment_audit(WATCHDOG_AUDIT_NUM);
	increment_audit(DIV0_UNHANDLED_AUD);
	if((fd = open("wdog:", O_RDONLY, 0)) < 0)
	{
		// Do a cold restart
		cold_start();
	}
	write(fd, &fd, 1);
	close(fd);
	while(1) ;
}
#endif
#endif

//extern int pcount;
extern int tick_counter;
static int local_epc;

int blitz99_vblank_handler( void )
{
	static int last_pcount = -1;
	static int last_tick_count = 0;
	int		fd;
	register long	pc asm("$8");

	// call the goose vblank handler
	interrupt_handler();

#ifndef DEBUG
	// if the process system hasn't fired in the last
	// twenty seconds, freeze here to trigger watchdog
	if (pcount == last_pcount)
	{
		if (tick_counter - last_tick_count > (57*20))
		{
			fprintf( stderr, "VBLANK detects frozen proc loop.  Halting.\n" );
			increment_audit(VBLANK_FREEZE_AUD);
			fprintf( stderr, "cur_proc: %s\n", cur_proc->process_name );

			dump_stack_trace();

			asm("	mfc0	$8,$14");
			local_epc = pc;

			fprintf( stderr, "PC: %x\n", local_epc );
			get_audit(WATCHDOG_AUDIT_NUM, &fd);
			increment_audit(WATCHDOG_AUDIT_NUM);
			if(fd >= 0 && fd < CAUSE_PC_MAX)
			{
				set_audit(CAUSE_AUD+(fd<<1), 0xabacab);
				set_audit(PC_AUD+(fd<<1), local_epc);
			}

			while(1);		// die
		}
	}
	else
	{
		last_pcount = pcount;
		last_tick_count = tick_counter;
	}
#endif

	return 1;
}

void tlb_refill_handler( void )
{
	int		fd;
	register long	pc asm("$8");

	asm("	mfc0	$8,$14");
	local_epc = pc;

	fprintf( stderr, "PC: %x\n", local_epc );

	increment_audit(TLB_REFILL_AUD);
	get_audit(WATCHDOG_AUDIT_NUM, &fd);
	if(fd >= 0 && fd < CAUSE_PC_MAX)
	{
		set_audit(CAUSE_AUD+(fd<<1), 0xace);
		set_audit(PC_AUD+(fd<<1), local_epc);
	}
	increment_audit(WATCHDOG_AUDIT_NUM);
	#ifdef DEBUG
	lockup()
	#else
	while(1);
	#endif
}
#endif

#if defined(VEGAS)
int get_tsec(void)
{
	return(57);
}
#endif
