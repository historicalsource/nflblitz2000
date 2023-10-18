#
# The default LOD Bias, Format, and Filter modes to use
#
BIAS=	0.0
FORMAT=	rgb565
FILTER=	GR_MIPMAP_NEAREST
HANDTOUCHED=	NO

GRASS_BIAS=	-0.9

#
# TGAs with no alpha channel
#
NA_TGA=	ezstripe.tga posthaf.tga

ifeq	($(HANDTOUCHED),YES)
G1_TGA=	grass1a.tga grass1b.tga grass1c.tga grass1d.tga

G2_TGA=	grass2a.tga grass2b.tga grass2c.tga grass2d.tga
	
G3_TGA=	grass3a.tga grass3b.tga grass3c.tga grass3d.tga
else
G1_TGA=	grass1a.tga

G2_TGA=	grass2a.tga
	
G3_TGA=	grass3a.tga
endif

#
# TGAs with alpha channel
#
A_TGA=	nfl.tga shadow.tga numbers.tga signs.tga blitz.tga nshadow.tga \
	nsigns.tga


#
# All of the TGAs
#
TGA=	$(NA_TGA) $(A_TGA) $(G1_TGA) $(G2_TGA) $(G3_TGA)


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
	@echo FIELD TEXTURES
	@$(MAKE) --no-print-directory -f field.mak $(TGA)
	@$(MAKE) --no-print-directory -f field.mak alpha FORMAT=argb4444 BIAS=-1.0
	@$(MAKE) --no-print-directory -f field.mak nonalpha BIAS=-1.0
	@$(MAKE) --no-print-directory -f field.mak grass1.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@$(MAKE) --no-print-directory -f field.mak grass2.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@$(MAKE) --no-print-directory -f field.mak grass3.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@echo FIELD TEXTURES DONE


#
# Alpha Textures
#
alpha:	$(A_WMS)

#
# Non-Alpha Textures
#
nonalpha:	$(NA_WMS)

#
# Grass textures
#
ifeq ($(HANDTOUCHED), YES)
grass1.wms:	$(G1_TGA:.tga=.wms)
	@echo Combining $(G1_TGA:.tga=.wms) into $@
	@mwms2wms $(G1_TGA:.tga=.wms) grass1.wms > NUL:
else
grass1.wms:	$(G1_TGA) field.mak
	@echo Converting grass1a.tga to grass1.3df: format = $(FORMAT)
	@-texus -m5 -t $(FORMAT) -o grass1.3df grass1a.tga
	@echo Converting grass1.3df to grass1.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass1.3df
endif

ifeq ($(HANDTOUCHED), YES)
grass2.wms:	$(G2_TGA:.tga=.wms)
	@echo Combining $(G2_TGA:.tga=.wms) into $@
	@mwms2wms $(G2_TGA:.tga=.wms) grass2.wms > NUL:
else
grass2.wms:	$(G2_TGA) field.mak
	@echo Converting grass2a.tga to grass2.3df: format = $(FORMAT)
	@-texus -m5 -t $(FORMAT) -o grass2.3df grass2a.tga
	@echo Converting grass2.3df to grass2.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass2.3df
endif

ifeq ($(HANDTOUCHED), YES)
grass3.wms:	$(G3_TGA:.tga=.wms)
	@echo Combining $(G3_TGA:.tga=.wms) into $@
	@mwms2wms $(G3_TGA:.tga=.wms) grass3.wms > NUL:
else
grass3.wms:	$(G3_TGA) field.mak
	@echo Converting grass3a.tga to grass3.3df: format = $(FORMAT)
	@-texus -m5 -t $(FORMAT) -o grass3.3df grass3a.tga
	@echo Converting grass3.3df to grass3.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass3.3df
endif

grass1a.wms:	grass1a.3df
	@echo Converting $< to $@: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass1a.3df

grass1b.wms:	grass1b.3df
	@echo Converting $< to $@: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass1b.3df

grass1c.wms:	grass1c.3df
	@echo Converting $< to $@: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass1c.3df

grass1d.wms:	grass1d.3df
	@echo Converting $< to $@: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass1d.3df

grass1e.wms:	grass1e.3df
	@echo Converting $< to $@: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass1e.3df

grass2a.wms:	grass2a.3df
	@echo Converting $< to $@: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass2a.3df

grass2b.wms:	grass2b.3df
	@echo Converting $< to $@: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass2b.3df

grass2c.wms:	grass2c.3df
	@echo Converting $< to $@: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass2c.3df

grass2d.wms:	grass2d.3df
	@echo Converting $< to $@: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass2d.3df

grass2e.wms:	grass2e.3df
	@echo Converting $< to $@: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass2e.3df

grass3a.wms:	grass3a.3df
	@echo Converting $< to $@: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass3a.3df

grass3b.wms:	grass3b.3df
	@echo Converting $< to $@: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass3b.3df

grass3c.wms:	grass3c.3df
	@echo Converting $< to $@: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass3c.3df

grass3d.wms:	grass3d.3df
	@echo Converting $< to $@: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass3d.3df

grass3e.wms:	grass3e.3df
	@echo Converting $< to $@: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass3e.3df



grass1a.3df:	grass1a.tga
	@echo Converting $< to $@: format = $(FORMAT)
	@-texus -mn -t $(FORMAT) -o grass1a.3df grass1a.tga

grass1b.3df:	grass1b.tga
	@echo Converting $< to $@: format = $(FORMAT)
	@-texus -mn -t $(FORMAT) -o grass1b.3df grass1b.tga

grass1c.3df:	grass1c.tga
	@echo Converting $< to $@: format = $(FORMAT)
	@-texus -mn -t $(FORMAT) -o grass1c.3df grass1c.tga

grass1d.3df:	grass1d.tga
	@echo Converting $< to $@: format = $(FORMAT)
	@-texus -mn -t $(FORMAT) -o grass1d.3df grass1d.tga

grass1e.3df:	grass1e.tga
	@echo Converting $< to $@: format = $(FORMAT)
	@-texus -mn -t $(FORMAT) -o grass1e.3df grass1e.tga

grass2a.3df:	grass2a.tga
	@echo Converting $< to $@: format = $(FORMAT)
	@-texus -mn -t $(FORMAT) -o grass2a.3df grass2a.tga

grass2b.3df:	grass2b.tga
	@echo Converting $< to $@: format = $(FORMAT)
	@-texus -mn -t $(FORMAT) -o grass2b.3df grass2b.tga

grass2c.3df:	grass2c.tga
	@echo Converting $< to $@: format = $(FORMAT)
	@-texus -mn -t $(FORMAT) -o grass2c.3df grass2c.tga

grass2d.3df:	grass2d.tga
	@echo Converting $< to $@: format = $(FORMAT)
	@-texus -mn -t $(FORMAT) -o grass2d.3df grass2d.tga

grass2e.3df:	grass2e.tga
	@echo Converting $< to $@: format = $(FORMAT)
	@-texus -mn -t $(FORMAT) -o grass2e.3df grass2e.tga

grass3a.3df:	grass3a.tga
	@echo Converting $< to $@: format = $(FORMAT)
	@-texus -mn -t $(FORMAT) -o grass3a.3df grass3a.tga

grass3b.3df:	grass3b.tga
	@echo Converting $< to $@: format = $(FORMAT)
	@-texus -mn -t $(FORMAT) -o grass3b.3df grass3b.tga

grass3c.3df:	grass3c.tga
	@echo Converting $< to $@: format = $(FORMAT)
	@-texus -mn -t $(FORMAT) -o grass3c.3df grass3c.tga

grass3d.3df:	grass3d.tga
	@echo Converting $< to $@: format = $(FORMAT)
	@-texus -mn -t $(FORMAT) -o grass3d.3df grass3d.tga

grass3e.3df:	grass3e.tga
	@echo Converting $< to $@: format = $(FORMAT)
	@-texus -mn -t $(FORMAT) -o grass3e.3df grass3e.tga



#
# How to covert a 3DF file to a WMS file
#
%.wms:	%.3df
	@echo Converting $< to $@: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -b $(BIAS) -f $(FILTER) $<

#
# How to convert a TGA file to a 3DF file
#
%.3df:	%.tga field.mak
	@echo Converting $< to $@: format = $(FORMAT)
	@-texus -m2 -t $(FORMAT) -o $@ $<
