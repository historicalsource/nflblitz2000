#
# The default LOD Bias, Format, and Filter modes to use
#
BIAS=	0.0
FORMAT=	rgb565
FILTER=	GR_MIPMAP_NEAREST
HANDTOUCHED=	NO

GRASS_BIAS=	-0.9

#
# Command to use to delete files
#
RM=	ckdel

#
# TGAs with no alpha channel
#
#NA_TGA=	ads98.tga

#ifeq	($(HANDTOUCHED),YES)
#G1_TGA=	grass1a.tga grass1b.tga grass1c.tga grass1d.tga
#
#G2_TGA=	grass2a.tga grass2b.tga grass2c.tga grass2d.tga
#	
#G3_TGA=	grass3a.tga grass3b.tga grass3c.tga grass3d.tga
#else
#G1_TGA=	grass198.tga
#
#G2_TGA=	grass298.tga
#	
#G3_TGA=	grass398.tga
#endif

G1_TGA=	grass198.tga
G2_TGA=	grass298.tga
G3_TGA=	grass398.tga
G4_TGA=	grass1_s.tga
G5_TGA=	grass2_s.tga
G6_TGA=	grass3_s.tga
G7_TGA=	grass1_t.tga
G8_TGA=	grass2_t.tga
G9_TGA=	grass3_t.tga
G10_TGA= grass1_d.tga
G11_TGA= grass2_d.tga
G12_TGA= grass3_d.tga
G13_TGA= grass1_a.tga
G14_TGA= grass2_a.tga
G15_TGA= grass3_a.tga
G16_TGA= grass1_m.tga
G17_TGA= grass2_m.tga
G18_TGA= grass3_m.tga



#
# TGAs with alpha channel
#
A_TGA=	nfl98.tga nums98.tga blitz98.tga \
		nflpa98.tga roof98.tga ads98.tga \
		nfl_s.tga nums_s.tga blitz_s.tga \
		nflpa_s.tga roof_s.tga ads_s.tga \
		blitz_d.tga roof_d.tga ads_d.tga \
		nums_t.tga nums_d.tga nums_a.tga nums_m.tga \
		blitz_a.tga nfl_a.tga nflpa_a.tga\
		blitz_m.tga nfl_m.tga nflpa_m.tga
				
A2_TGA= post98.tga post_s.tga post_d.tga
	
#
# All of the TGAs
#
TGA=	$(NA_TGA) $(A_TGA) $(A2_TGA)


#
# WMS Files with no alpha channel
#
NA_WMS=	$(NA_TGA:.tga=.wms)


#
# WMS Files with alpha channel (4444)
#
A_WMS=	$(A_TGA:.tga=.wms)

#
# WMS Files with more bits for alpha channel (8332)
#
A2_WMS=	$(A2_TGA:.tga=.wms)


#
# Tell it not to delete the 3DF files
#
#.PRECIOUS:	$(TGA:.tga=.3df)


#---------------------------
# ALL
#	Makefile entry point (default)
#---------------------------

all:	
	@echo FIELD2 TEXTURES
	@$(MAKE) --no-print-directory -f field2.mak $(TGA)
	@$(MAKE) --no-print-directory -f field2.mak alpha FORMAT=argb4444 BIAS=-1.0
	@$(MAKE) --no-print-directory -f field2.mak alpha2 FORMAT=argb4444 BIAS=-1.0
	@$(MAKE) --no-print-directory -f field2.mak nonalpha BIAS=-1.0
	@$(MAKE) --no-print-directory -f field2.mak grass198.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@$(MAKE) --no-print-directory -f field2.mak grass298.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@$(MAKE) --no-print-directory -f field2.mak grass398.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@$(MAKE) --no-print-directory -f field2.mak grass1_s.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@$(MAKE) --no-print-directory -f field2.mak grass2_s.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@$(MAKE) --no-print-directory -f field2.mak grass3_s.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@$(MAKE) --no-print-directory -f field2.mak grass1_t.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@$(MAKE) --no-print-directory -f field2.mak grass2_t.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@$(MAKE) --no-print-directory -f field2.mak grass3_t.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@$(MAKE) --no-print-directory -f field2.mak grass1_d.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@$(MAKE) --no-print-directory -f field2.mak grass2_d.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@$(MAKE) --no-print-directory -f field2.mak grass3_d.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@$(MAKE) --no-print-directory -f field2.mak grass1_a.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@$(MAKE) --no-print-directory -f field2.mak grass2_a.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@$(MAKE) --no-print-directory -f field2.mak grass3_a.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@$(MAKE) --no-print-directory -f field2.mak grass1_m.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@$(MAKE) --no-print-directory -f field2.mak grass2_m.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@$(MAKE) --no-print-directory -f field2.mak grass3_m.wms BIAS=$(GRASS_BIAS) FILTER=GR_MIPMAP_NEAREST
	@echo FIELD2 TEXTURES DONE

#
# Alpha Textures
#
alpha:	$(A_WMS)

#
# extra Alpha Textures
#
alpha2:	$(A2_WMS)

#
# Non-Alpha Textures
#
nonalpha:	$(NA_WMS)

#
# Grass textures
#
grass198.wms: $(G1_TGA) field2.mak
	@-texus -m5 -t $(FORMAT) -o grass198.3df grass198.tga
	@echo Converting grass198.3df to grass198.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass198.3df

grass298.wms: $(G2_TGA) field2.mak
	@-texus -m5 -t $(FORMAT) -o grass298.3df grass298.tga
	@echo Converting grass298.3df to grass298.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass298.3df

grass398.wms: $(G3_TGA) field2.mak
	@-texus -m5 -t $(FORMAT) -o grass398.3df grass398.tga
	@echo Converting grass398.3df to grass398.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass398.3df

grass1_s.wms: $(G4_TGA) field2.mak
	@-texus -m5 -t $(FORMAT) -o grass1_s.3df grass1_s.tga
	@echo Converting grass1_s.3df to grass1_s.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass1_s.3df

grass2_s.wms: $(G5_TGA) field2.mak
	@-texus -m5 -t $(FORMAT) -o grass2_s.3df grass2_s.tga
	@echo Converting grass2_s.3df to grass2_s.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass2_s.3df

grass3_s.wms: $(G6_TGA) field2.mak
	@-texus -m5 -t $(FORMAT) -o grass3_s.3df grass3_s.tga
	@echo Converting grass3_s.3df to grass3_s.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass3_s.3df

grass1_t.wms: $(G7_TGA) field2.mak
	@-texus -m5 -t $(FORMAT) -o grass1_t.3df grass1_t.tga
	@echo Converting grass1_t.3df to grass1_t.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass1_t.3df

grass2_t.wms: $(G8_TGA) field2.mak
	@-texus -m5 -t $(FORMAT) -o grass2_t.3df grass2_t.tga
	@echo Converting grass2_t.3df to grass2_t.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass2_t.3df

grass3_t.wms: $(G9_TGA) field2.mak
	@-texus -m5 -t $(FORMAT) -o grass3_t.3df grass3_t.tga
	@echo Converting grass3_t.3df to grass3_t.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass3_t.3df

grass1_d.wms: $(G10_TGA) field2.mak
	@-texus -m5 -t $(FORMAT) -o grass1_d.3df grass1_d.tga
	@echo Converting grass1_d.3df to grass1_d.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass1_d.3df

grass2_d.wms: $(G11_TGA) field2.mak
	@-texus -m5 -t $(FORMAT) -o grass2_d.3df grass2_d.tga
	@echo Converting grass2_d.3df to grass2_d.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass2_d.3df

grass3_d.wms: $(G12_TGA) field2.mak
	@-texus -m5 -t $(FORMAT) -o grass3_d.3df grass3_d.tga
	@echo Converting grass3_d.3df to grass3_d.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass3_d.3df

grass1_a.wms: $(G13_TGA) field2.mak
	@-texus -m5 -t $(FORMAT) -o grass1_a.3df grass1_a.tga
	@echo Converting grass1_a.3df to grass1_a.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass1_a.3df

grass2_a.wms: $(G14_TGA) field2.mak
	@-texus -m5 -t $(FORMAT) -o grass2_a.3df grass2_a.tga
	@echo Converting grass2_a.3df to grass2_a.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass2_a.3df

grass3_a.wms: $(G15_TGA) field2.mak
	@-texus -m5 -t $(FORMAT) -o grass3_a.3df grass3_a.tga
	@echo Converting grass3_a.3df to grass3_a.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass3_a.3df

grass1_m.wms: $(G16_TGA) field2.mak
	@-texus -m5 -t $(FORMAT) -o grass1_m.3df grass1_m.tga
	@echo Converting grass1_m.3df to grass1_m.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass1_m.3df

grass2_m.wms: $(G17_TGA) field2.mak
	@-texus -m5 -t $(FORMAT) -o grass2_m.3df grass2_m.tga
	@echo Converting grass2_m.3df to grass2_m.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass2_m.3df

grass3_m.wms: $(G18_TGA) field2.mak
	@-texus -m5 -t $(FORMAT) -o grass3_m.3df grass3_m.tga
	@echo Converting grass3_m.3df to grass3_m.wms: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -f $(FILTER) -b $(BIAS) grass3_m.3df

#
# How to covert a 3DF file to a WMS file
#
%.wms:	%.3df
	@echo Converting $< to $@: bias = $(BIAS) - filter = $(FILTER)
	@3df2wms -b $(BIAS) -f $(FILTER) $<

#
# How to convert a TGA file to a 3DF file
#
%.3df:	%.tga field2.mak
	@echo Converting $< to $@: format = $(FORMAT)
	@-texus -m2 -t $(FORMAT) -o $@ $<
