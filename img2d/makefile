#
# $Revision: 1 $
#

all:
	@echo \VIDEO\NFL\IMG2D TOP LEVEL
	@$(MAKE) --no-print-directory -f img2d.mak all
	@echo \VIDEO\NFL\IMG2D TOP LEVEL DONE

clobber:
	@echo \VIDEO\NFL\IMG2D TOP LEVEL CLOBBER
	@$(MAKE) --no-print-directory -f img2d.mak clobber
	@echo \VIDEO\NFL\IMG2D TOP LEVEL CLOBBER DONE

clean:
	@echo \VIDEO\NFL\IMG2D TOP LEVEL CLEAN DONE
	@$(MAKE) --no-print-directory -f img2d.mak clean
	@echo \VIDEO\NFL\IMG2D TOP LEVEL CLEAN DONE

rebuild:
	@echo \VIDEO\NFL\IMG2D TOP LEVEL REBUILD
	@$(MAKE) --no-print-directory clean
	@$(MAKE) --no-print-directory all
	@echo \VIDEO\NFL\IMG2D TOP LEVEL REBUILD DONE

get:
ifeq	($(SSUSER),)
	@echo Source Server Login Name Environment Variable is NOT set
	@echo Do "set SSUSER=xxxx" where xxxx is your Netware login name
else
	@echo \VIDEO\NFL\IMG2D GET
	$(SS) cp $$/video/nfl/img2d
	$(SS) get img2d -gcd
	@echo \VIDEO\NFL\IMG2D GET DONE
endif
