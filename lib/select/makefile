#
# Copyright (c) 1997 by Midway Video Inc.
# All rights reserved
#
# Makefile for select library for football
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
CF_FILE=	select.cf
endif

ifneq	($(TIMING),)
TIME_STATS=		-DTIME_STATS
endif

#---------------------------------------
# localset.mak isn't under version control.  Everyone's is different.
# It's for defining things like NODRONES or NODOWNS and such.
ifneq	($(wildcard /video/nfl/*.mak),)
include /video/nfl/localset.mak
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
CFLAGS=	$(DBG) $(LOCALS) $(TIME_STATS) -D$(TARGET_SYS) -D$(GRX_HARDWARE) -D$(BUILDMODE) @$(CF_FILE)
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
CSRCS =	record.c select.c vsscreen.c game_sel.c game_cfg.c

#
# ASM Sources
#
ASRCS =

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
all: /video/nfl/lib/select.a($(OBJS))
	@echo $(DBG_MSG) VERSION OF \VIDEO\NFL\LIB\SELECT LIBRARY DONE


#
# Dependencies and rules for object modules
#
%.o:	%.s
	@echo $< to $@
	$(AS) $<,$@

%.o:	%.c
	@echo $< to $@ and $(basename $<).d
	$(CC3) -o $@ $<

select.o:	select.c
	@echo $< to $@ and $(basename $<).d
	$(CC1) -o $@ $<

record.o:	record.c
	@echo $< to $@ and $(basename $<).d
	$(CC1) -o $@ $<

vsscreen.o:	vsscreen.c
	@echo $< to $@ and $(basename $<).d
	$(CC1) -o $@ $<

ifneq	($(wildcard /video/nfl/*.mak),)
vsscreen.o : /video/nfl/localset.mak

endif

#
# Dependency files
#
ifneq	($(wildcard *.d),)
include	$(wildcard *.d)
endif

#
# How to clean up for a complete rebuild
#
clean:
	@echo \VIDEO\NFL\LIB\SELECT CLEAN

	@$(MAKE) --no-print-directory clobber

ifneq	($(wildcard *.o),)
	@attrib -r *.o
	@del *.o
endif

ifneq	($(wildcard *.d),)
	@attrib -r *.d
	@del *.d
endif

	@echo \VIDEO\NFL\LIB\SELECT CLEAN DONE


#
# How to clean up to regenerate the archive
#
clobber:
	@echo \VIDEO\NFL\LIB\SELECT CLOBBER
	@if EXIST ..\select.a attrib -r ..\select.a
	@if EXIST ..\select.a del ..\select.a
	@echo \VIDEO\NFL\LIB\SELECT CLOBBER DONE

#
# What to do for a complete rebuild
#
rebuild:
	@echo \VIDEO\NFL\LIB\SELECT REBUILD
	@$(MAKE) --no-print-directory clean
	@$(MAKE) --no-print-directory all
	@echo \VIDEO\NFL\LIB\SELECT REBUILD DONE
