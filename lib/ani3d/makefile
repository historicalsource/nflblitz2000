#
# Copyright (c) 1997 by Midway Video Inc.
# All rights reserved
#
# Makefile for ani3d library for football
#
# $Revision: $
#
ifneq	($(BUILDMODE), RELEASE)
DBG=	-DDEBUG
DBG_MSG=	DEBUG
else
DBG=
DBG_MSG=	RELEASE
endif

ifeq	($(TARGET_SYS),VEGAS)
CF_FILE=	/video/nfl/lib/vlib.cf
ifeq	($(GRX_HARDWARE),BANSHEE)
CF_FILE=	/video/nfl/lib/blib.cf
else
endif
endif
ifeq	($(TARGET_SYS),SEATTLE)
CF_FILE=	ani3d.cf
endif

ifneq	($(TIMING),)
TIME_STATS=		-DTIME_STATS
endif

#
# Assembler command line
#
AS=	@asmmips /q /of.rdata /zd /l /oc+

#
# Command line options for archiver
#
ARFLAGS= /u

#
# Archiver
#
AR=	@psylib


#
# Command line options for C compiler
CFLAGS=	$(DBG) $(TIME_STATS) -D$(TARGET_SYS) -D$(GRX_HARDWARE) -D$(BUILDMODE) @$(CF_FILE)
#
# C compiler command lines
#
CC3 = 	@ccmips $(CFLAGS) -O3
CC2 = 	@ccmips $(CFLAGS) -O2
CC1 = 	@ccmips $(CFLAGS) -O1
CC0 = 	@ccmips $(CFLAGS) -O0


#
# C Sources
#
CSRCS =	pipe3d.c anim.c

#
# ASM Sources
#
ASRCS =	geom.s

#
# Object modules
#
OBJS = 	$(ASRCS:.s=.o) $(CSRCS:.c=.o)

#
# Tell make NOT to delete .o files when done
#
.PRECIOUS:	$(OBJS)

#
# What we are going to build
#
all: /video/nfl/lib/ani3d.a($(OBJS))
	@echo $(DBG_MSG) VERSION OF \VIDEO\NFL\LIB\ANI3D LIBRARY DONE


#
# Dependancies and rules for object modules
#
%.o:	%.s
	@echo $< to $@
	$(AS) $<,$@

%.o:	%.c
	@echo $< to $@ and $(basename $<).d
	$(CC3) -o $@ $<

genseq.o:	genseq.s \video\nfl\anim\genseq.ani
	@echo $< to $@
	$(AS) $<,$@


#
# Dependency files
#
ifneq	($(wildcard *.d),)
include	$(wildcard *.d)
endif

#
# How to clean up for a complete rebuild
#
clean:	clobber
	@echo \VIDEO\NFL\LIB\ANI3D CLEAN
ifneq	($(wildcard *.o),)
	@-del *.o
endif
ifneq	($(wildcard *.d),)
	@-del *.d
endif
	@echo \VIDEO\NFL\LIB\ANI3D CLEAN DONE


#
# How to clean up to regenerate the archive
#
clobber:
	@echo \VIDEO\NFL\LIB\ANI3D CLOBBER
	@if EXIST ..\ani3d.a attrib -r ..\ani3d.a
	@if EXIST ..\ani3d.a del ..\ani3d.a
	@echo \VIDEO\NFL\LIB\ANI3D CLOBBER DONE

#
# What to do for a complete rebuild
#
rebuild:
	@echo \VIDEO\NFL\LIB\ANI3D REBUILD
	@$(MAKE) --no-print-directory clean
	@$(MAKE) --no-print-directory all
	@echo \VIDEO\NFL\LIB\ANI3D REBUILD DONE
