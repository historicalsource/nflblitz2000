#------------------------------------------------------------------------------
#
# Makefile for IMG2D Directory
#
# $Revision: 49 $
#
#------------------------------------------------------------------------------

# Project name; various usage including pathnames
#
PRJ = 		NFL

# Pathname for where header files end up
#
INCPATH =	\video\$(PRJ)\include

# Pathname for image source files
#
IMGPATH =	\video\$(PRJ)\img2d

# List of things to be built by this makefile
#
TARGETS =\
	$(INCPATH)\sysfont.h\
	$(INCPATH)\general.h\
	$(INCPATH)\plaq.h\
	$(INCPATH)\tmsel.h\
	$(INCPATH)\statpg.h\
	$(INCPATH)\plyrinfo.h\
	$(INCPATH)\bigfnt.h\
	$(INCPATH)\smlogos.h\
	$(INCPATH)\flash.h\
	$(INCPATH)\vsscreen.h\
	$(INCPATH)\winmsg.h\
	$(INCPATH)\credits.h\
	$(INCPATH)\makeplay.h\
	$(INCPATH)\newfeatr.h\
	$(INCPATH)\trivia.h\
	$(INCPATH)\gamesel.h\
	$(INCPATH)\plays.h\
	$(INCPATH)\sweepstk.h\
	$(INCPATH)\custgame.h

#	$(INCPATH)\mugshot.h		Removed because we have no license for 'em
#	$(INCPATH)\ending.h\		Dan made a better ending than these dumb 2d screens

# Phony make targets that don't want any implicit dependencies
#
.PHONY:	all prompt clobber clean

#---------------------------------------
#
# Useful macros
#
define NEWLINE


endef

# Target Object File Name (base.ext)
#
define TOFN
$(notdir $(subst \,/,$@))
endef

# Target Object Base Name (base)
#
define TOBN
$(basename $(TOFN))
endef

# Target Object Log Name
#
define TOLN
$(basename $(notdir $(subst \,/,$(TRG)))).log
endef

# Default LOD file process
#  Use $(subst ...) to invoke, subing PARMS with any additional Composit
#  command line options or with a null-string if none are needed
#
#  Ex: $(subst PARMS,-clipon,$(LODPROC))
#      $(subst PARMS,-t -oneh,$(LODPROC))
#      $(subst PARMS,,$(LODPROC))
#
define LODPROC
echo Generating `$(TOFN)'
if EXIST $@ attrib -r $@
if EXIST $@ del $@
if EXIST $(TOBN).bat del $(TOBN).bat
c -newpack -quiet_batch_file PARMS -lfile $(TOBN).lod >$(TOBN).log
if NOT EXIST $(TOBN).bat fail
call $(TOBN)
del *.3df
endef

#------------------------------------------------------------------------------
#
# Rules and dependencies
#
# What we are going to build
#
all:	prompt $(TARGETS)
	@$(foreach TRG, $(TARGETS),\
		find "ERROR" $(TOLN)|if not errorlevel 1 find "ERROR" $(TOLN)$(NEWLINE)\
		echo >nul Y|find "Y"$(NEWLINE))
	@echo MAKING `$(PRJ)\IMG2D' DONE

prompt:
	@echo MAKING `$(PRJ)\IMG2D'

clobber:
	@echo \VIDEO\$(PRJ)\IMG2D CLOBBER
	@$(foreach TRG, $(TARGETS), if exist $(TRG) attrib -r $(TRG)$(NEWLINE))
	@$(foreach TRG, $(TARGETS), if exist $(TRG) del $(TRG)$(NEWLINE))
	@echo \VIDEO\$(PRJ)\IMG2D CLOBBER DONE

clean:
	@echo \VIDEO\$(PRJ)\IMG2D CLEAN
	@$(MAKE) --no-print-directory -f img2d.mak clobber
	@if exist *.3df attrib -r *.3df
	@if exist *.3df del *.3df
	@if exist *.bat attrib -r *.bat
	@if exist *.bat del *.bat
	@if exist *.log attrib -r *.log
	@if exist *.log del *.log
	@if exist *.tga attrib -r *.tga
	@if exist *.tga del *.tga
	@if exist *.wms attrib -r *.wms
	@if exist *.wms del *.wms
	@echo \VIDEO\$(PRJ)\IMG2D CLEAN DONE

#------------------------------------------------------------------------------
# List of images needed by sysfont.lod
#------------------------------------------------------------------------------
SYSFONT_IMGS =	bastfont.wif scplate.wif

$(INCPATH)\sysfont.h:	sysfont.lod $(SYSFONT_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#------------------------------------------------------------------------------
# List of images needed by general.lod
#------------------------------------------------------------------------------
GENERAL_IMGS =	brush50.wif nflogo.wif teaminfo.wif colbars.wif bkgrnd.wif\
				hiscore.wif

$(INCPATH)\general.h:	general.lod $(GENERAL_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#------------------------------------------------------------------------------
# List of images needed by plaq.lod
#------------------------------------------------------------------------------
PLAQ_IMGS =		ynplaq3.wif stats.wif bkgrnd.wif ynplaqu2.wif flash.wif\
				hiscore.wif

$(INCPATH)\plaq.h:		plaq.lod $(PLAQ_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#------------------------------------------------------------------------------
# List of images needed by tmsel.lod
#------------------------------------------------------------------------------
TMSEL_IMGS =	teamsel2.wif qbs.wif tmsel_lr.wif teamlogo.wif bkgrnd.wif\
				rost_ari.wif rost_atl.wif rost_bal.wif rost_buf.wif\
				rost_car.wif rost_chi.wif rost_cin.wif rost_cle.wif\
				rost_dal.wif rost_den.wif rost_det.wif rost_gbp.wif\
				rost_ind.wif rost_jac.wif rost_kan.wif rost_mia.wif\
				rost_min.wif rost_nen.wif rost_nor.wif rost_nyg.wif\
				rost_nyj.wif rost_oak.wif rost_phi.wif rost_pit.wif\
				rost_snd.wif rost_snf.wif rost_stl.wif rost_sea.wif\
				rost_tam.wif rost_ten.wif rost_was.wif

$(INCPATH)\tmsel.h:		tmsel.lod $(TMSEL_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#------------------------------------------------------------------------------
# List of images needed by statpg.lod
#------------------------------------------------------------------------------
STATPG_IMGS =	halfstat.wif

$(INCPATH)\statpg.h:	statpg.lod $(STATPG_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#------------------------------------------------------------------------------
# List of images needed by plyrinfo.lod
#------------------------------------------------------------------------------
PLYRINFO_IMGS =	plyrinfo.wif scplate.wif scpla_LR.wif firebar.wif

$(INCPATH)\plyrinfo.h:	plyrinfo.lod $(PLYRINFO_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#------------------------------------------------------------------------------
# List of images needed by bigfnt.lod
#------------------------------------------------------------------------------
BIGFNT_IMGS =	bastbig.wif bastfont.wif 

$(INCPATH)\bigfnt.h:	bigfnt.lod $(BIGFNT_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#------------------------------------------------------------------------------
# List of images needed by smlogos.lod
#------------------------------------------------------------------------------
SMLOGOS_IMGS =	plyrinfo.wif

$(INCPATH)\smlogos.h:	smlogos.lod $(SMLOGOS_IMGS) \video\tools\c.exe
	@$(subst PARMS,-l -clipon,$(LODPROC))

#------------------------------------------------------------------------------
# List of images needed by flash.lod
#------------------------------------------------------------------------------
FLASH_IMGS =	flash.wif nflgrafx.wif tipspg.wif

$(INCPATH)\flash.h:		flash.lod $(FLASH_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#------------------------------------------------------------------------------
# List of images needed by vsscreen.lod
#------------------------------------------------------------------------------
VSSCREEN_IMGS =	vsscreen.wif ynplaq3.wif helmets.wif

$(INCPATH)\vsscreen.h:	vsscreen.lod $(VSSCREEN_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#------------------------------------------------------------------------------
# List of images needed by winmsg.lod
#------------------------------------------------------------------------------
WINMSG_IMGS =	winteams.wif

$(INCPATH)\winmsg.h:	winmsg.lod $(WINMSG_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#------------------------------------------------------------------------------
# List of images needed by credits.lod
#------------------------------------------------------------------------------
CREDITS_IMGS =	credits.wif

$(INCPATH)\credits.h:	credits.lod $(CREDITS_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#------------------------------------------------------------------------------
# List of images needed by makeplay.lod
#------------------------------------------------------------------------------
MAKEPLAY_IMGS =	makeplay.wif ynplaq3.wif

$(INCPATH)\makeplay.h:	makeplay.lod $(MAKEPLAY_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#------------------------------------------------------------------------------
# List of images needed by newfeatr.lod
#------------------------------------------------------------------------------
NEWFEATR_IMGS =	newfeatr.wif

$(INCPATH)\newfeatr.h:	newfeatr.lod $(NEWFEATR_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#------------------------------------------------------------------------------
# List of images needed by trivia.lod
#------------------------------------------------------------------------------
TRIVIA_IMGS =	trivia.wif

$(INCPATH)\trivia.h:	trivia.lod $(TRIVIA_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#------------------------------------------------------------------------------
# List of images needed by ending.lod
#------------------------------------------------------------------------------
ENDING_IMGS =	ending.wif

$(INCPATH)\ending.h:	ending.lod $(ENDING_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#------------------------------------------------------------------------------
# List of images needed by gamesel.lod
#------------------------------------------------------------------------------
GAMESEL_IMGS =		gamesel.wif

$(INCPATH)\gamesel.h:	gamesel.lod $(GAMESEL_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#------------------------------------------------------------------------------
# List of images needed by plays.lod
#------------------------------------------------------------------------------
PLAYS_IMGS =	plays_o.wif plays_d.wif plays_t.wif

$(INCPATH)\plays.h:		plays.lod $(PLAYS_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#------------------------------------------------------------------------------
# List of images needed by sweepstk.lod
#------------------------------------------------------------------------------
SWEEPS_IMGS =	sweepstk.wif

$(INCPATH)\sweepstk.h:	sweepstk.lod $(SWEEPS_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#------------------------------------------------------------------------------
# List of images needed by custgame.lod
#------------------------------------------------------------------------------
CUSTGAME_IMGS =	custgame.wif

$(INCPATH)\custgame.h:	custgame.lod $(CUSTGAME_IMGS) \video\tools\c.exe
	@$(subst PARMS,-clipon,$(LODPROC))

#------------------------------------------------------------------------------
# List of images needed by mugshot.lod
#------------------------------------------------------------------------------
#MUGSHOT_IMGS =	mugshot.wif
#
#$(INCPATH)\mugshot.h:	mugshot.lod $(MUGSHOT_IMGS) \video\tools\c.exe
#	@$(subst PARMS,-clipon,$(LODPROC))
