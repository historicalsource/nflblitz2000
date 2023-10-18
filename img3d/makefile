#
# $Revision:  $
#

#---------------------------
# ALL
#	Makefile entry point (default)
#---------------------------

all:
	@echo \VIDEO\NFL\IMG3D TOP LEVEL

	@$(MAKE) --no-print-directory -f img3d.mak all

	@echo UNIFORMS
	c_hack plyr.ld
	uni.bat			# convert the player uniforms
	@echo UNIFORMS DONE

	@echo FIELD
	@$(MAKE) --no-print-directory -f field.mak all

	@echo STADIUM
	@$(MAKE) --no-print-directory -f stadium.mak all

	@echo FIELD2
	@$(MAKE) --no-print-directory -f field2.mak all

	@echo IMG3D TOP LEVEL DONE

	@echo STADIUM2
	@$(MAKE) --no-print-directory -f stadium2.mak all

	@echo \VIDEO\NFL\IMG3D TOP LEVEL DONE


#---------------------------
# CLOBBER
#	Makefile entry point
#---------------------------

clobber:
	@echo IMG3D TOP LEVEL CLOBBER

# Clean all the generated intermediate files (*.WMS)
ifneq	($(wildcard *.wms),)
	@attrib -r *.wms
	@del *.wms
endif

# Clean all the generated intermediate files (*.3DF)
ifneq	($(wildcard *.3df),)
	@attrib -r *.3df
	@del *.3df
endif

# Clean the old oilers textures (Oilers changed to Titans for 2000)
ifneq	($(wildcard oil*.tga),)
	@attrib -r oil*.tga
	@del oil*.tga
endif

	@echo IMG3D TOP LEVEL CLOBBER DONE


#---------------------------
# CLEAN
#	Makefile entry point
#---------------------------

clean:
	@echo \VIDEO\NFL\IMG3D TOP LEVEL CLEAN
	@$(MAKE) --no-print-directory clobber
	@echo \VIDEO\NFL\IMG3D TOP LEVEL CLEAN DONE


#---------------------------
# REBUILD
#	Makefile entry point
#---------------------------

rebuild:
	@echo \VIDEO\NFL\IMG3D TOP LEVEL REBUILD
	@$(MAKE) --no-print-directory clean
	@$(MAKE) --no-print-directory all
	@echo \VIDEO\NFL\IMG3D TOP LEVEL REBUILD
