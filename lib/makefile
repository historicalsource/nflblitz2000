#
# Top level makefile for game libraries
#
# $Revision: $
#

#
# Default target
#
all:
	@echo \VIDEO\LIBRARY TOP LEVEL
	@$(MAKE) --no-print-directory -C ani3d
	@$(MAKE) --no-print-directory -C select
	@echo \VIDEO\LIBRARY TOP LEVEL DONE

#
# How to clean up for a relink
#
clobber:
	@echo \VIDEO\LIBRARY TOP LEVEL CLOBBER
	@$(MAKE) --no-print-directory -C ani3d clobber
	@$(MAKE) --no-print-directory -C select clobber
	@echo \VIDEO\LIBRARY TOP LEVEL CLOBBER DONE


#
# How to clean up for a recompile
#
clean:
	@echo \VIDEO\LIBRARY TOP LEVEL CLEAN
	@$(MAKE) --no-print-directory -C ani3d clean
	@$(MAKE) --no-print-directory -C select clean
	@echo \VIDEO\LIBRARY TOP LEVEL CLEAN DONE


#
# How to clean up everything and rebuild
#
rebuild:
	@echo \VIDEO\LIBRARY TOP LEVEL REBUILD
	@$(MAKE) --no-print-directory -C ani3d rebuild
	@$(MAKE) --no-print-directory -C select rebuild
	@echo \VIDEO\LIBRARY TOP LEVEL REBUILD DONE
