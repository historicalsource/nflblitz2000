#
# The default LOD Bias, Format, and Filter modes to use
#
BIAS=	0.0
FORMAT=	rgb565
FILTER=	GR_MIPMAP_NEAREST

#
# TGAs with no alpha channel
#
NA_TGA=	crowdd.tga crowdl.tga dotmtrx.tga jumbo.tga sky.tga \
	ncrowdd.tga ncrowdl.tga ndtmtrx1.tga ndtmtrx2.tga njumbo.tga nsky.tga

#
# TGAs with alpha channel
#
A_TGA=	shadow.tga trim.tga backwall.tga banners.tga \
	nshadow.tga ntrim.tga nbckwall.tga nbanners.tga


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
	@echo STADIUM TEXTURES
	@$(MAKE) --no-print-directory -f stadium.mak $(TGA)
	@$(MAKE) --no-print-directory -f stadium.mak alpha FORMAT=argb4444 BIAS=-1.0
	@$(MAKE) --no-print-directory -f stadium.mak nonalpha BIAS=-1.0
	@echo STADIUM TEXTURES DONE


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
