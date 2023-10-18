#
# The default LOD Bias, Format, and Filter modes to use
#
BIAS=	0.0
FORMAT=	rgb565
FILTER=	GR_MIPMAP_NEAREST

#
# TGAs with no alpha channel
#
NA_TGA= ceil98.tga crowdd98.tga crowdl98.tga wall98.tga\
		ceil_s.tga crowdd_s.tga crowdl_s.tga wall_s.tga\
		ceil_d.tga crowdd_d.tga crowdl_d.tga wall_d.tga
		
#
# TGAs with alpha channel
#
A_TGA=	skybox98.tga trim98.tga\
		skybox_s.tga trim_s.tga\
		skybox_d.tga trim_d.tga

#
# All of the TGAs
#
TGA=	$(NA_TGA) $(A_TGA)


#
# WMS Files with no alpha channel
#
NA_WMS=	$(NA_TGA:.tga=.wms)


#
# WMS Files with alpha channel
#
A_WMS=	$(A_TGA:.tga=.wms)


#
# Tell it not to delete the 3DF files
#
#.PRECIOUS:	$(TGA:.tga=.3df)


#---------------------------
# ALL
#	Makefile entry point (default)
#---------------------------

all:
	@echo STADIUM2 TEXTURES
	@$(MAKE) --no-print-directory -f stadium2.mak $(TGA)
	@$(MAKE) --no-print-directory -f stadium2.mak alpha FORMAT=argb4444 BIAS=-1.0
	@$(MAKE) --no-print-directory -f stadium2.mak nonalpha BIAS=-1.0
	@echo STADIUM2 TEXTURES DONE


#
# Alpha Textures
#
alpha:	$(A_WMS)

#
# Non-Alpha Textures
#
nonalpha:	$(NA_WMS)


#
# How to covert a 3DF file to a WMS file
#
%.wms:	%.3df
	@echo Converting $< to $@
	@3df2wms -b $(BIAS) -f $(FILTER) $<

#
# How to convert a TGA file to a 3DF file
#
%.3df:	%.tga
	@echo Converting $< to $@
	@-texus -t $(FORMAT) -o $@ $<
