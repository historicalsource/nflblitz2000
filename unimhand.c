#if 0	// was commented out in main.c
/****************************************************************************/
/*                                                                          */
/* unimhand.c - Unimplementd Operation Exception Handler.                   */
/*                                                                          */
/* Copyright (c) 1996 by Midway Video Inc.                                  */
/* All rights reserved                                                      */
/*                                                                          */
/* Written by:  Michael J. Lynch                                            */
/* $Revision: 3 $                                                             */
/*                                                                          */
/****************************************************************************/
#include	<stdio.h>
#include	<goose/goose.h>

#define	FPU_CAUSE_SPECIAL					(0<<16)
#define	FPU_CAUSE_COP1						(1<<16)
#define	FPU_CAUSE_COP1X_MADDS			(2<<16)
#define	FPU_CAUSE_COP1X_NMADDS			(3<<16)
#define	FPU_CAUSE_COP1X_MSUBS			(4<<16)
#define	FPU_CAUSE_COP1X_NMSUBS			(5<<16)
#define	FPU_CAUSE_COP1X_MADDD			(6<<16)
#define	FPU_CAUSE_COP1X_MSUBD			(7<<16)
#define	FPU_CAUSE_COP1X_NMADDD			(8<<16)
#define	FPU_CAUSE_COP1X_NMSUBD			(9<<16)
#define	FPU_CAUSE_COP1X_LWXC1			(10<<16)
#define	FPU_CAUSE_COP1X_LDXC1			(11<<16)
#define	FPU_CAUSE_COP1X_SWXC1			(12<<16)
#define	FPU_CAUSE_COP1X_SDXC1			(13<<16)
#define	FPU_CAUSE_COP1X_PREFIX			(14<<16)
#define	FPU_CAUSE_COP1X_UNRECOGNIZED	(15<<16)
#define	FPU_CAUSE_LWC1						(16<<16)
#define	FPU_CAUSE_LDC1						(17<<16)
#define	FPU_CAUSE_SWC1						(18<<16)
#define	FPU_CAUSE_SDC1						(19<<16)
#define	FPU_CAUSE_NOT_FPU_INSTRUCTION	(20<<16)

#ifdef SEATTLE
#ifndef DEBUG
int reboot_vector(int, int *);
#endif
#endif

void nfl_unimplemented_handler(int *);

void nfl_unimplemented_handler(int *r_save)
{
	unsigned	instruction;
	int		reg;
	int		ft_val;
	int		fs_val;
	int		fr_val;

	// At this point we determine whether or not the instruction is a
	// COP1 instruction and if so we look specifically for a madd or
	// msub instruction.  If either of these instructions caused the
	// exception we attempt to determine if the exception was possibly
	// caused by an intermediate de-normalized result.  (E.G.  The result
	// of the multiply portion of the instruction resulted in a denormalized
	// result).  If so we, simply move the fr register to the fd register
	// and set the PC to the next instruction.

	instruction = *((unsigned *)r_save[PC]);

	switch((instruction >> 26))
	{
		case 0x00:	// Special
		{
#ifdef DEBUG
			fprintf(stderr, "unimplemented_handler() - Special Instruction\r\n");
#endif
			reg = FPU_CAUSE_SPECIAL;
			break;
		}
		case 0x11:	// COP1
		{
#ifdef DEBUG
			fprintf(stderr, "unimplemented_handler() - COP1 Instruction\r\n");
#endif
			reg = FPU_CAUSE_COP1;
			break;
		}
		case 0x13:	// COP1X
		{
			if((instruction & 0x3f) == 0x20)		// madd.s
			{
				reg = instruction >> 16;
				reg &= 0x1f;
				reg <<= 1;
				reg += FGR0;
				ft_val = r_save[reg];
				ft_val >>= 23;
				ft_val &= 0x7f;
				ft_val -= 127;

				reg = instruction >> 11;
				reg &= 0x1f;
				reg <<= 1;
				reg += FGR0;
				fs_val = r_save[reg];
				fs_val >>= 23;
				fs_val &= 0x7f;
				fs_val -= 127;

				ft_val += fs_val;

				if(ft_val <= -127)
				{
					reg = instruction >> 21;
					reg &= 0x1f;
					reg <<= 1;
					reg += FGR0;
					fr_val = r_save[reg];

					reg = instruction >> 6;
					reg &= 0x1f;
					reg <<= 1;
					reg += FGR0;
					r_save[reg] = fr_val;

					r_save[PC] += 4;
					return;
				}
				else
				{
#ifdef DEBUG
					fprintf(stderr, "unimplemented_handler() - madd.s unable to fix\r\n");
#endif
					reg = FPU_CAUSE_COP1X_MADDS;
				}
			}
			else if((instruction & 0x3f) == 0x30)		// nmadd.s
			{
				reg = instruction >> 16;
				reg &= 0x1f;
				reg <<= 1;
				reg += FGR0;
				ft_val = r_save[reg];
				ft_val >>= 23;
				ft_val &= 0x7f;
				ft_val -= 127;

				reg = instruction >> 11;
				reg &= 0x1f;
				reg <<= 1;
				reg += FGR0;
				fs_val = r_save[reg];
				fs_val >>= 23;
				fs_val &= 0x7f;
				fs_val -= 127;

				ft_val += fs_val;

				if(ft_val <= -127)
				{
					reg = instruction >> 21;
					reg &= 0x1f;
					reg <<= 1;
					reg += FGR0;
					fr_val = r_save[reg];

					reg = instruction >> 6;
					reg &= 0x1f;
					reg <<= 1;
					reg += FGR0;
					r_save[reg] = fr_val | 0x80000000;

					r_save[PC] += 4;
					return;
				}
				else
				{
#ifdef DEBUG
					fprintf(stderr, "unimplemented_handler() - nmadd.s unable to fix\r\n");
#endif
					reg = FPU_CAUSE_COP1X_NMADDS;
				}
			}
			else if((instruction & 0x3f) == 0x28)	// msub.s
			{
				reg = instruction >> 16;
				reg &= 0x1f;
				reg <<= 1;
				reg += FGR0;
				ft_val = r_save[reg];
				ft_val >>= 23;
				ft_val &= 0x7f;
				ft_val -= 127;

				reg = instruction >> 11;
				reg &= 0x1f;
				reg <<= 1;
				reg += FGR0;
				fs_val = r_save[reg];
				fs_val >>= 23;
				fs_val &= 0x7f;
				fs_val -= 127;

				ft_val += fs_val;
		
				if(ft_val <= -127)
				{
					reg = instruction >> 21;
					reg &= 0x1f;
					reg <<= 1;
					reg += FGR0;
					fr_val = r_save[reg];
		
					reg = instruction >> 6;
					reg &= 0x1f;
					reg <<= 1;
					reg += FGR0;
					r_save[reg] = fr_val | 0x80000000;
		
					r_save[PC] += 4;
					return;
				}
				else
				{
#ifdef DEBUG
					fprintf(stderr, "unimplemented_handler() - msub.s unable to fix\r\n");
#endif
					reg = FPU_CAUSE_COP1X_MSUBS;
				}
			}
			else if((instruction & 0x3f) == 0x38)	// nmsub.s
			{
				reg = instruction >> 16;
				reg &= 0x1f;
				reg <<= 1;
				reg += FGR0;
				ft_val = r_save[reg];
				ft_val >>= 23;
				ft_val &= 0x7f;
				ft_val -= 127;

				reg = instruction >> 11;
				reg &= 0x1f;
				reg <<= 1;
				reg += FGR0;
				fs_val = r_save[reg];
				fs_val >>= 23;
				fs_val &= 0x7f;
				fs_val -= 127;

				ft_val += fs_val;
		
				if(ft_val <= -127)
				{
					reg = instruction >> 21;
					reg &= 0x1f;
					reg <<= 1;
					reg += FGR0;
					fr_val = r_save[reg];
		
					reg = instruction >> 6;
					reg &= 0x1f;
					reg <<= 1;
					reg += FGR0;
					r_save[reg] = fr_val;
		
					r_save[PC] += 4;
					return;
				}
				else
				{
#ifdef DEBUG
					fprintf(stderr, "unimplemented_handler() - nmsub.s unable to fix\r\n");
#endif
					reg = FPU_CAUSE_COP1X_NMSUBS;
				}
			}
			else if((instruction & 0x3f) == 0x21)	// madd.d
			{
#ifdef DEBUG
				fprintf(stderr, "unimplemented_handler() - madd.d unable to fix\r\n");
#endif
				reg = FPU_CAUSE_COP1X_MADDD;
			}
			else if((instruction & 0x3f) == 0x29)	// msub.d
			{
#ifdef DEBUG
				fprintf(stderr, "unimplemented_handler() - msub.d unable to fix\r\n");
#endif
				reg = FPU_CAUSE_COP1X_MSUBD;
			}
			else if((instruction & 0x3f) == 0x31)	// nmadd.d
			{
#ifdef DEBUG
				fprintf(stderr, "unimplemented_handler() - nmadd.d unable to fix\r\n");
#endif
				reg = FPU_CAUSE_COP1X_NMADDD;
			}
			else if((instruction & 0x3f) == 0x39)	// nmsub.d
			{
#ifdef DEBUG
				fprintf(stderr, "unimplemented_handler() - nmsub.d unable to fix\r\n");
#endif
				reg = FPU_CAUSE_COP1X_NMSUBD;
			}
			else if((instruction & 0x3f) == 0x00)	// lwxc1
			{
#ifdef DEBUG
				fprintf(stderr, "unimplemented_handler() - lwxc1 unable to fix\r\n");
#endif
				reg = FPU_CAUSE_COP1X_LWXC1;
			}
			else if((instruction & 0x3f) == 0x01)	// ldxc1
			{
#ifdef DEBUG
				fprintf(stderr, "unimplemented_handler() - ldxc1 unable to fix\r\n");
#endif
				reg = FPU_CAUSE_COP1X_LDXC1;
			}
			else if((instruction & 0x3f) == 0x08)	// swxc1
			{
#ifdef DEBUG
				fprintf(stderr, "unimplemented_handler() - swxc1 unable to fix\r\n");
#endif
				reg = FPU_CAUSE_COP1X_SWXC1;
			}
			else if((instruction & 0x3f) == 0x09)	// sdxc1
			{
#ifdef DEBUG
				fprintf(stderr, "unimplemented_handler() - sdxc1 unable to fix\r\n");
#endif
				reg = FPU_CAUSE_COP1X_SDXC1;
			}
			else if((instruction & 0x3f) == 0x0f)	// prefix
			{
#ifdef DEBUG
				fprintf(stderr, "unimplemented_handler() - prefix unable to fix\r\n");
#endif
				reg = FPU_CAUSE_COP1X_PREFIX;
			}
			else
			{
#ifdef DEBUG
				fprintf(stderr, "unimplemented_handler() - unrecognized COP1X instruction\r\n");
#endif
				reg = FPU_CAUSE_COP1X_UNRECOGNIZED;
			}
			break;
		}
		case 0x31:	// LWC1
		{
#ifdef DEBUG
			fprintf(stderr, "unimplemented_handler() - LWC1 Instruction\r\n");
#endif
			reg = FPU_CAUSE_LWC1;
			break;
		}
		case 0x35:	// LDC1
		{
#ifdef DEBUG
			fprintf(stderr, "unimplemented_handler() - LDC1 Instruction\r\n");
#endif
			reg = FPU_CAUSE_LDC1;
			break;
		}
		case 0x39:	// SWC1
		{
#ifdef DEBUG
			fprintf(stderr, "unimplemented_handler() - SWC1 Instruction\r\n");
#endif
			reg = FPU_CAUSE_SWC1;
			break;
		}
		case 0x3d:	// SDC1
		{
#ifdef DEBUG
			fprintf(stderr, "unimplemented_handler() - SDC1 Instruction\r\n");
#endif
			reg = FPU_CAUSE_SDC1;
			break;
		}
		default:
		{
#ifdef DEBUG
			fprintf(stderr, "unimplemented_handler() - NOT FPU Instruction\r\n");
#endif
			reg = FPU_CAUSE_NOT_FPU_INSTRUCTION;
			break;
		}
	}
#ifdef DEBUG
	while(1) ;
#else
#ifdef SEATTLE
	reg |= (15<<2);
	reboot_vector(reg, r_save);
#endif
#endif
}


#endif