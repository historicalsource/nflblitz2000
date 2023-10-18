#
# $Revision: 28 $
#

TARGS = loball.h ballshad.h plyr266.h plyr266f.h plyr498.h plyr498f.h \
	helmet.h medball.h loball2.h medball2.h logo.h field.h stadpie.h \
	plyslct.h nums_3d.h spfx.h field2.h stadium2.h head.h helm.h \
#	xheads.h xheads2.h xheads3.h

all:
	@echo MODELS
	@$(MAKE) --no-print-directory -f models.mak $(TARGS)
	@echo MODELS DONE

plyr266.h: plyr266.sdl
	@echo Creating $@
	@cvtmod -r -f 1.2 -square -name plyr266 -append 266 < plyr266.sdl > plyr266.h

plyr266f.h: plyr266f.sdl
	@echo Creating $@
	@cvtmod -r -f 1.2 -square -name plyr266f -append 266f < plyr266f.sdl > plyr266f.h

plyr498.h: plyr498.sdl
	@echo Creating $@
	@cvtmod -r -f 1.2 -square -name plyr498 -append 498 < plyr498.sdl > plyr498.h

plyr498f.h: plyr498f.sdl
	@echo Creating $@
	@cvtmod -r -f 1.2 -square -name plyr498f -append 498f < plyr498f.sdl > plyr498f.h

#stadium.h:	stadium.sdl
#	cvtmod -name stadium -f 65.0 -fz 57.5 < stadium.sdl > stadium.h
stadpie.h:	stadpie.sdl
	cvtmod -name stadium -f 65.0 -fz 57.5 < stadpie.sdl > stadpie.h

field.h:	field.sdl
# Used to be 50!!!!!!!!!!!
	cvtmod -name field -f 65.0 -fz 57.5 < field.sdl > field.h

loball.h:	loball.sdl
	@echo Creating $@
	@cvtmod -name loball < loball.sdl > loball.h

medball.h:	medball.sdl
	@echo Creating $@
	@cvtmod -name medball < medball.sdl > medball.h

loball2.h:	loball2.sdl
	@echo Creating $@
	@cvtmod -name loball2 -append _big < loball2.sdl > loball2.h

medball2.h:	medball2.sdl
	@echo Creating $@
	@cvtmod -name medball2 -append _big < medball2.sdl > medball2.h

ballshad.h:	ballshad.sdl
	@echo Creating $@
	@cvtmod -name ballshad 	< ballshad.sdl > ballshad.h

helmet.h:	helmet.sdl
	@echo Creating $@
	@cvtmod -name helmet < helmet.sdl > helmet.h

logo.h:	logo.sdl
	@echo Creating $@
	@cvtmod -name logo < logo.sdl > logo.h

plyslct.h: plyslct.sdl
	@echo Creating $@
	@cvtmod -z -q -name plyslct < plyslct.sdl > plyslct.h

helm.h: phelm.sdl
	@echo Creating $@
	@cvtmod -f 1.2 -z -l -t < phelm.sdl > helm.h

head.h: phead.sdl
	@echo Creating $@
	@cvtmod -f 1.2 -z -l -s -t < phead.sdl > head.h

# xheads.h: xheads.sdl
#	@echo Creating $@
#	@cvtmod -f 1.2 -z -x < xheads.sdl > xheads.h

# xheads2.h: xheads2.sdl
#	@echo Creating $@
#	@cvtmod -f 1.2 -z -x < xheads2.sdl > xheads2.h

# xheads3.h: xheads3.sdl
#	@echo Creating $@
#	@cvtmod -f 1.2 -z -x < xheads3.sdl > xheads3.h

nums_3d.h: nums_3d.sdl
	@echo Creating $@
	@cvtmod -name nums_3d < nums_3d.sdl > nums_3d.h

spfx.h: spfx.sdl
	@echo Creating $@
	@cvtmod -f 4.0 -name spfx < spfx.sdl > spfx.h

field2.h:	field2.sdl
	cvtmod -name field2 -f 65.0 -fz 57.5 < field2.sdl > field2.h

stadium2.h:	stadium2.sdl
	cvtmod -name stadium2 -f 65.0 -fz 57.5 < stadium2.sdl > stadium2.h
