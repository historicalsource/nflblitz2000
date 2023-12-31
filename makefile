#-------------------------------------------------------------------------------------------------------------------
#
# Top level Makefile for NFL Game
#
# $Revision: 1 $
#
#-------------------------------------------------------------------------------------------------------------------


#-------------------------------------------------------------------------------------------------------------------
# HERE ARE THE COMMON ENTRIES FOR THIS MAKEFILE
#	NOTE: Cleaning means erasing all files generated by the make process (i.e. EXEs, OBJs, WMS files, header files, libs, etc.)


# rebuild_nfl
#	Cleans and rebuilds everything for NFL (all art and NFL code)
#	Use this if you want a fresh build of all the NFL stuff
#	NOTE: The tools & low-level libs must already be made, and will remain unchanged
#	NOTE: This does not include NFL movies

# rebuild
#	Cleans and rebuilds all the NFL-specific code (no art)
#	Use this if you want a fresh build of the NFL code 
#	NOTE: The tools, low-level libs, and art must already be made, and will remain unchanged
#	NOTE: This includes all subdirectories that contain NFL source (i.e. VIDEO\NFL\LIB, VIDEO\NFL\LIB\SELECT, etc.)

# clean_nfl
#	Cleans everything for NFL (all art and code)
#	NOTE: This does not include NFL movies

# clean
#	Cleans all the NFL-specific code (does not clean art)

# all_nfl
#	Builds (updates) everything for NFL (all art and NFL code)
#	The tools, low-level libs, and art must already be made
#	NOTE: This does not include NFL movies

# allart
#	Makes any art that has changed in the IMG2D, IMG3D, MODELS, and ANIM directories
#	NOTE: This is useful when artists make simple changes to the artwork.

# all
#	(default makefile entry)
#	Builds (updates) all the NFL-specific code (does not build any art)
#	The tools, low-level libs, and art must already be made
#	This is the default entry that is run when you just type 'make' at the command line (with no entry specified)

# copy_data
#	copies all data used for the game to the target hard drive (including NFL artwork, sound banks, etc.)
#	NOTE: This does not copy the movies, but should copy everything else that's needed
#	NOTE: hardware must be properly setup to copy the data
#-------------------------------------------------------------------------------------------------------------------



#-------------------------------------------------------------------------------------------------------------------
# Environment variables used by the makefiles
#
# TARGET_SYS = {SA1, SEATTLE, VEGAS}
# BUILDMODE = {DEBUG, RELEASE}
# GAME = {NFL, NBA}
# GRX_HARDWARE = {BANSHEE, VOODOO2}
# DJGPP = {C:/DJGPP/DJGPP.ENV}
# TBIOS_ADR = {380, 390}
# NFL_DRIVE = {PHX0:, PHX1:}
#-------------------------------------------------------------------------------------------------------------------

PHX :=	phx:

ifeq	($(DUAL_BOOT),YES)
ifndef	NFL_DRIVE
include NFL_DRIVE.must.be.defined!!
else
PHX :=	$(NFL_DRIVE)
endif
endif


ifeq	($(BUILDMODE), RELEASE)
NAME_SUFFIX=	r
DBG_MSG=		RELEASE
DBG=			
else
NAME_SUFFIX=	d
DBG_MSG=		DEBUG
DBG=			-DDEBUG
endif

ifeq	($(TARGET_SYS),VEGAS)
SOUND_LIB=		/video/lib/vsound$(NAME_SUFFIX).a
LNK_FILE=		vnfl.lnk
CF_FILE=		vnfl.cf
NAME_PREFIX=	v
ifeq	($(GRX_HARDWARE),BANSHEE)
CF_FILE=		bnfl.cf
NAME_PREFIX=	b
endif
else
SOUND_LIB=		/video/lib/ssound$(NAME_SUFFIX).a
LNK_FILE=		snfl.lnk
CF_FILE=		nfl.cf
NAME_PREFIX=	s
endif

ifneq	($(TIMING),)
TIME_STATS=		-DTIME_STATS
endif

BASE=	$(NAME_PREFIX)nfl$(NAME_SUFFIX)

SS=		@c:/vss/win32/ss

#---------------------------------------
# Set to Movie File Groups to be processed
#  Ex: movies/*.wms
#      movies/*.cnt
#
MFG_WMS=	movies/*.wms
MFG_CNT=	movies/*.cnt

#---------------------------------------
# localset.mak isn't under version control.  Everyone's is different.
# It's for defining things like NODRONES or NODOWNS and such.
ifneq	($(wildcard *.mak),)
include localset.mak
endif

#
# Assembler command line
#
AS2=	@asmmips /q /of.rdata /zd /l /oc+

#
# Linker command line
#
LINK=	@psylink /s /c /m /wl /wm

#
# C flags
#
CFLAGS_TMP=	$(DBG) $(LOCALS) $(LOCALS2) $(TIME_STATS) -D$(TARGET_SYS)\
-D$(GRX_HARDWARE) -DNFL -DNO_CARD_READER @$(CF_FILE) -O

CFLAGS=	$(strip $(CFLAGS_TMP))

#
# Compiler command lines
#
CC3=	@ccmips $(CFLAGS)3
CC2=	@ccmips $(CFLAGS)2
CC1=	@ccmips $(CFLAGS)1
CC0=	@ccmips $(CFLAGS)0


#
# C Source modules
#
CSRCS=	main.c attract.c coin.c field.c plyrseq.c \
	sysfont.c teamdata.c playbook.c sndinfo.c mainproc.c cambot.c \
	nfl.c data.c picinfo.c drone.c detect.c player.c plyrinfo.c \
	sounds.c movie.c audinfo.c support.c adjust.c \
	playsel.c result.c data2.c freegame.c showpnam.c \
	debris.c gameover.c handicap.c pmisc.c filechk.c unimhand.c cap.c \
	trivia.c jmalloc.c sweeps.c playstuf.c game.c \
	camdata.c grab.c

#rain.c opmsg.c stream.c inthand.c


#
# Assembly source modules
#
ASRCS=	getgaddr.s

#
# Object files
#
OBJS=	$(CSRCS:.c=.o) $(ASRCS:.s=.o)

#
# Phony target for prompt
#
#.PHONY:	prompt

#
# Tell make NOT to delete .o files when done
#
.PRECIOUS:	$(OBJS)

#
# What we are going to build
#
ifndef	CPE
$(BASE):	prompt $(BASE).cpe $(BASE).exe done
	@echo >nul

else
ifeq	($(CPE), 0)
$(BASE):	prompt delcpe $(BASE).exe done
	@echo >nul

else
ifeq	($(CPE), 1)
$(BASE):	prompt delbin $(BASE).cpe $(BASE).exe done
	@echo >nul

else
ifeq	($(CPE), 2)
$(BASE):	prompt delcpe delexe $(BASE).bin done
	@echo >nul

else
$(BASE):
	@echo Aborting -- if specified, CPE environment variable must be:
	@echo 	0 to make .BIN & .EXE
	@echo 	1 to make .CPE & .EXE & .MAP & .SYM
	@echo 	2 to make .BIN
endif
endif
endif
endif

bin:	prompt $(BASE).bin done

cpe:	prompt $(BASE).cpe done

exe:	prompt $(BASE).exe done

delbin:
	@echo DELETING $(BASE).BIN
	@if EXIST $(BASE).bin del $(BASE).bin

delcpe:
	@echo DELETING $(BASE).CPE
	@if EXIST $(BASE).cpe del $(BASE).cpe

delexe:
	@echo DELETING $(BASE).EXE
	@if EXIST $(BASE).exe del $(BASE).exe

prompt:
	@echo $(DBG_MSG) VERSION OF \VIDEO\NFL

done:
	@echo $(DBG_MSG) VERSION OF \VIDEO\NFL DONE


do_catcnts:
ifneq	($(MFG_CNT),)
	@$(MAKE) --no-print-directory -C movies catcnts
endif


filesys_chk:
	@genfchk anim/*.ani img2d/*.wms img3d/*.wms sounds/*.bnk trivia/*.txt $(MFG_WMS) $(MFG_CNT) ../diag/img2d/*.wms ../diag/*.bnk ../lib/sound/*.bin

rfilesys_chk:
	@genfchk anim/*.ani img2d/*.wms img3d/*.wms sounds/*.bnk trivia/*.txt $(MFG_WMS) $(MFG_CNT) ../diag/img2d/*.wms ../diag/*.bnk ../lib/sound/*.bin


#
# Primary Targets
#
$(BASE).cpe:	$(OBJS) lib\select.a lib\ani3d.a /video/lib/$(NAME_PREFIX)goose$(NAME_SUFFIX).a /video/lib/$(NAME_PREFIX)glide$(NAME_SUFFIX).a $(SOUND_LIB)
	@if EXIST nflink.lnk del nflink.lnk
	@copy	$(LNK_FILE) nflink.lnk > NUL:	 
	@echo 	inclib	"/video/lib/$(NAME_PREFIX)goose$(NAME_SUFFIX).a" >> nflink.lnk
	@echo 	inclib	"/video/lib/$(NAME_PREFIX)glide$(NAME_SUFFIX).a" >> nflink.lnk
	@echo 	inclib	"$(SOUND_LIB)" >> nflink.lnk
	@echo LINKING $(BASE).CPE
	$(LINK) @nflink.lnk,$(BASE).cpe,$(BASE).sym,$(BASE).map

$(BASE).bin:	$(OBJS) lib\select.a lib\ani3d.a /video/lib/$(NAME_PREFIX)goose$(NAME_SUFFIX).a /video/lib/$(NAME_PREFIX)glide$(NAME_SUFFIX).a $(SOUND_LIB)
	@if EXIST nflink.lnk del nflink.lnk
	@copy	$(LNK_FILE) nflink.lnk > NUL:	 
	@echo 	inclib	"/video/lib/$(NAME_PREFIX)goose$(NAME_SUFFIX).a" >> nflink.lnk
	@echo 	inclib	"/video/lib/$(NAME_PREFIX)glide$(NAME_SUFFIX).a" >> nflink.lnk
	@echo 	inclib	"$(SOUND_LIB)" >> nflink.lnk
	@echo LINKING $(BASE).BIN
	$(LINK) /p @nflink.lnk,$(BASE).bin

#
# Dependencies and rules
#
%.o:	%.c
	@echo $< to $@ and $(basename $<).d
	$(CC3) -o $@ $< 

%.o:	%.s
	@echo $< to $@
	$(AS2) $<,$@

$(BASE).exe:	$(BASE).bin
#if
ifeq	($(TARGET_SYS),SEATTLE)
	@bin2exe $(BASE).bin 0x800c4000
else
	@bin2exe $(BASE).bin 0x80100000
endif

cambot.o:	cambot.c
	@echo $< to $@ and $(basename $<).d
	$(CC1) -o $@ $< 

pmisc.o:	pmisc.c
	@echo $< to $@ and $(basename $<).d
	$(CC1) -o $@ $< 

showpnam.o:	showpnam.c
	@echo $< to $@ and $(basename $<).d
	$(CC1) -o $@ $< 

player.o:	player.c
	@echo $< to $@ and $(basename $<).d
	$(CC1) -o $@ $< 

nfl.o:	nfl.c
	@echo $< to $@ and $(basename $<).d
	$(CC1) -o $@ $< 

detect.o:	detect.c
	@echo $< to $@ and $(basename $<).d
	$(CC1) -o $@ $< 

attract.o:	attract.c
	@echo $< to $@ and $(basename $<).d
	$(CC1) -o $@ $< 

playsel.o:	playsel.c
	@echo $< to $@ and $(basename $<).d
	$(CC1) -o $@ $< 

drone.o:	drone.c
	@echo $< to $@ and $(basename $<).d
	$(CC1) -o $@ $< 

plyrinfo.o:	plyrinfo.c
	@echo $< to $@ and $(basename $<).d
	$(CC1) -o $@ $< 

audinfo.o:	audinfo.c
	@echo $< to $@ and $(basename $<).d
	$(CC1) -o $@ $< 

trivia.o:	trivia.c
	@echo $< to $@ and $(basename $<).d
	$(CC1) -o $@ $< 

anim/genseq.equ:
	@$(MAKE) --no-print-directory -C anim all

plyrseq.o:	anim/genseq.equ

ifneq	($(wildcard *.mak),)
attract.o : localset.mak
cambot.o  : localset.mak
detect.o  : localset.mak
drone.o   : localset.mak
handicap.o:	localset.mak
mainproc.o: localset.mak
nfl.o     : localset.mak
player.o  : localset.mak
playsel.o : localset.mak
plyrinfo.o: localset.mak
plyrseq.o : localset.mak
sndinfo.o : localset.mak
trivia.o  : localset.mak
playstuf.o: localset.mak

endif

#
# Dependency files
#
ifneq	($(wildcard *.d),)
include $(wildcard *.d)
endif


#  NOTE: Target drive files
#  Primary & backup files are clumped in order to get the best possible
#  separation (with PCOPY anyway) on the target drive


#---------------------------
# ALL
#	Makefile entry point
#---------------------------

all:
	@echo MAKING ALL
	@$(MAKE) --no-print-directory -C lib all
	@$(MAKE) --no-print-directory $(BASE)
	@echo ALL MADE


#---------------------------
# GET
#	Makefile entry point
#	Gets source from the source safe
#---------------------------

get:
ifeq	($(SSUSER),)
	@echo Source Server Login Name Environment Variable is NOT set
	@echo Do "set SSUSER=xxxx" where xxxx is your Netware login name
else
	@echo GETTING \VIDEO\NFL
	$(SS) cp $$/video/nfl
	$(SS) get nfl -gcd
	@echo GETTING \VIDEO\NFL DONE
endif


#---------------------------
# GET_ALL
#	Makefile entry point
#	Retrieves all game source from the source safe
#---------------------------

get_all:
ifeq	($(SSUSER),)
	@echo Source Server Login Name Environment Variable is NOT set
	@echo Do "set SSUSER=xxxx" where xxxx is your Netware login name
else
	@echo GETTING \VIDEO\NFL AND SUBDIRECTORIES
	$(SS) cp $$/video/nfl
	$(SS) get nfl -r -gcd
	@echo GETTING \VIDEO\NFL AND SUBDIRECTORIES DONE
endif


#---------------------------
# UPDATE_DISK
#	Makefile entry point
#---------------------------

update_disk:
	@$(MAKE) --no-print-directory do_catcnts
	@$(MAKE) --no-print-directory copy_data

#---------------------------
# UPDATE_GAME
#	Makefile entry point
#---------------------------

update_game:	 
	@$(MAKE) --no-print-directory do_catcnts
	@$(MAKE) --no-print-directory copy_game

#---------------------------
# UPDATE_ALL
#	Makefile entry point
#---------------------------

update_all:
	@$(MAKE) --no-print-directory do_catcnts
	@$(MAKE) --no-print-directory copy_game
	@$(MAKE) --no-print-directory copy_data

#---------------------------
# RELEASE_DISK
#	Makefile entry point
#---------------------------

release_disk:
	@tdel *.*
	@tdel *
	@$(MAKE) --no-print-directory do_catcnts
	@$(MAKE) --no-print-directory copy_movies
	@$(MAKE) --no-print-directory copy_data
	@$(MAKE) --no-print-directory copy_game
	@echo Remember to add the .REV file that you make with CREATREV.EXE
	@echo if you want to do EPROM revs.

#---------------------------
# COPY_DATA
#	Makefile entry point
#---------------------------

copy_data:
	@pcopy anim/*.ani   $(PHX) /u /c
	@pcopy img2d/*.wms  $(PHX) /u /c
	@pcopy img3d/*.wms  $(PHX) /u /c
	@pcopy sounds/*.bnk $(PHX) /u /c
	@pcopy trivia/*.txt $(PHX) /u /c
	@pcopy sweeps/*.txt $(PHX) /u /c
ifneq	($(MFG_WMS),)
	@pcopy $(MFG_WMS)   $(PHX) /u /c
endif
ifneq	($(MFG_CNT),)
	@pcopy $(MFG_CNT)   $(PHX) /u
endif
	@pcopy ../diag/img2d/*.wms $(PHX) /u /c
	@pcopy ../diag/*.bnk       $(PHX) /u /c
	@pcopy ../lib/sound/*.bin  $(PHX) /u /c

copy_data_bak:
	@pcopy anim/*.ani   $(PHX)*.bak /u /c
	@pcopy img2d/*.wms  $(PHX)*.bak /u /c
	@pcopy img3d/*.wms  $(PHX)*.bak /u /c
	@pcopy sounds/*.bnk $(PHX)*.bak /u /c
	@pcopy trivia/*.txt $(PHX)*.bak /u /c
ifneq	($(MFG_WMS),)
	@pcopy $(MFG_WMS)   $(PHX)*.bak /u /c
endif
ifneq	($(MFG_CNT),)
	@pcopy $(MFG_CNT)   $(PHX)*.bak /u
endif
	@pcopy ../diag/img2d/*.wms $(PHX)*.bak /u /c
	@pcopy ../diag/*.bnk       $(PHX)*.bak /u /c
	@pcopy ../lib/sound/*.bin  $(PHX)*.bak /u /c

#---------------------------
# COPY_GAME
#	Makefile entry point
#---------------------------

copy_game:
	@pcopy ../diag/$(BASE).exe $(PHX)diag.exe /u /c
	@pcopy         $(BASE).exe $(PHX)game.exe /u /c

#---------------------------
# COPY_GAME_BAK
#	Makefile entry point
#---------------------------

copy_game_bak:
	@pcopy ../diag/$(BASE).exe $(PHX)diag.bak /u /c
	@pcopy         $(BASE).exe $(PHX)game.bak /u /c

#---------------------------
# COPY_MOVIES
#	Makefile entry point
#---------------------------

copy_movies:
ifneq	($(MFG_WMS),)
	@$(MAKE) --no-print-directory -C movies update_disk
endif


#---------------------------
# ALLART
#	Makefile entry point
#---------------------------

allart:
	@echo MAKING ALLART
	@$(MAKE) --no-print-directory -C models
	@$(MAKE) --no-print-directory -C img2d
	@$(MAKE) --no-print-directory -C img3d
	@$(MAKE) --no-print-directory -C anim
	@echo ALLART MADE


#---------------------------
# ALL_NFL
#	Makefile entry point
#	Builds everything for NFL (including all art)
#---------------------------

all_nfl:
	@echo MAKING ALL NFL
	@$(MAKE) --no-print-directory allart
	@$(MAKE) --no-print-directory all
	@echo ALL NFL MADE


#---------------------------
# CLEAN
#	Makefile entry point
#	Cleans up for a rebuild
#---------------------------

clean:
	@echo \VIDEO\NFL CLEAN
	@$(MAKE) --no-print-directory clobber
	@echo \VIDEO\NFL CLEAN
ifneq	($(wildcard *.o),)
	@del *.o
endif
ifneq	($(wildcard *.d),)
	@del *.d
endif
	@echo \VIDEO\NFL CLEAN DONE


#---------------------------
# CLOBBER
#	Makefile entry point
#	Cleans up for a relink
#---------------------------

clobber:
	@echo \VIDEO\NFL CLOBBER
	@if EXIST $(BASE).cpe del $(BASE).cpe
	@if EXIST $(BASE).bin del $(BASE).bin
	@if EXIST $(BASE).sym del $(BASE).sym
	@if EXIST $(BASE).map del $(BASE).map
	@echo \VIDEO\NFL CLOBBER DONE


#---------------------------
# RELINK
#	Makefile entry point
#---------------------------

relink:
	@echo \VIDEO\NFL RELINK
	@$(MAKE) --no-print-directory clobber
	@$(MAKE) --no-print-directory all
	@echo \VIDEO\NFL RELINK DONE


#---------------------------
# REBUILD
#	Makefile entry point
#	Rebuilds just NFL source directories (does not rebuild any art)
#---------------------------

rebuild:
	@echo \VIDEO\NFL REBUILD
	@$(MAKE) --no-print-directory clean
	@$(MAKE) --no-print-directory all
	@echo \VIDEO\NFL REBUILD DONE


#---------------------------
# CLOBBER_NFL
#	Makefile entry point
#	Clobbers just the game directories
#---------------------------

clobber_nfl:
	@echo CLOBBERING NFL AND SUBDIRECTORIES
	@$(MAKE) --no-print-directory -C models clobber
	@$(MAKE) --no-print-directory -C img2d clobber
	@$(MAKE) --no-print-directory -C img3d clobber
	@$(MAKE) --no-print-directory -C anim clobber
	@$(MAKE) --no-print-directory -C lib clobber
	@$(MAKE) --no-print-directory clobber
	@echo NFL AND SUBDIRECTORIES CLOBBERED


#---------------------------
# CLEAN_NFL
#	Makefile entry point
#	Cleans just the game directories
#---------------------------

clean_nfl: 
	@echo CLEANING NFL AND SUBDIRECTORIES
	@$(MAKE) --no-print-directory -C models clean
	@$(MAKE) --no-print-directory -C img2d clean
	@$(MAKE) --no-print-directory -C img3d clean
	@$(MAKE) --no-print-directory -C anim clean
	@$(MAKE) --no-print-directory -C lib clean
	@$(MAKE) --no-print-directory clean
	@echo NFL AND SUBDIRECTORIES CLEANED


#---------------------------
# REBUILD_NFL
#	Makefile entry point
#	Rebuilds just the game directories
#---------------------------

rebuild_nfl:
	@echo REBUILDING NFL AND SUBDIRECTORIES
	@$(MAKE) --no-print-directory clean_nfl
	@$(MAKE) --no-print-directory -C models rebuild
	@$(MAKE) --no-print-directory -C img2d rebuild
	@$(MAKE) --no-print-directory -C img3d rebuild
	@$(MAKE) --no-print-directory -C anim rebuild
	@$(MAKE) --no-print-directory -C lib rebuild
	@$(MAKE) --no-print-directory rebuild
	@echo NFL AND SUBDIRECTORIES REBUILT


#---------------------------
# TOOLS
#	Makefile entry point
#	Builds all the tools and low-level libs needed to build NFL
#---------------------------

tools:
	@$(MAKE) --no-print-directory -C /video/tools/putil
	@$(MAKE) --no-print-directory -C /video/tools
	@$(MAKE) --no-print-directory -C /video/lib/libc
	@$(MAKE) --no-print-directory -C /video/lib


.PHONY:	run

run:
	prun	$(BASE).exe

