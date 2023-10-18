#
# Image directory makefile
#


#
# Team uniforms
#

# UNI2 is for teams that have the same lghl for home and away
UNI1 =		$(addsuffix .wms,jerfh jerfa lghlh lghla svmkh svmka num num2)
UNI2 =		$(addsuffix .wms,jerfh jerfa lghlh svmkh svmka num num2)

BEA =		$(addprefix BEA,$(UNI1))	# bears
BEN =		$(addprefix BEN,$(UNI2))	# bengals
BIL =		$(addprefix BIL,$(UNI2))	# bills
BRO =		$(addprefix BRO,$(UNI1))	# broncos
BUC =		$(addprefix BUC,$(UNI2))	# buccaneers
CAR =		$(addprefix CAR,$(UNI1))	# cardinals
CHA =		$(addprefix CHA,$(UNI1))	# chargers
CHI =		$(addprefix CHI,$(UNI1))	# chiefs
COL =		$(addprefix COL,$(UNI2))	# colts
COW =		$(addprefix COW,$(UNI1))	# cowboys
DOL =		$(addprefix DOL,$(UNI1))	# dolphins
EAG =		$(addprefix EAG,$(UNI1))	# eagles
FAL =		$(addprefix FAL,$(UNI2))	# falcons
FOR =		$(addprefix FOR,$(UNI2))	# 49ers
GIA =		$(addprefix GIA,$(UNI2))	# giants
JAG =		$(addprefix JAG,$(UNI2))	# jaguars
JET =		$(addprefix JET,$(UNI1))	# jets
LIO =		$(addprefix LIO,$(UNI2))	# lions
TIT =		$(addprefix TIT,$(UNI1))	# titans
PAC =		$(addprefix PAC,$(UNI2))	# packers
PAN =		$(addprefix PAN,$(UNI1))	# panthers
PAT =		$(addprefix PAT,$(UNI2))	# patriots
RAI =		$(addprefix RAI,$(UNI2))	# raiders
RAM =		$(addprefix RAM,$(UNI2))	# rams
RAV =		$(addprefix RAV,$(UNI1))	# ravens
RED =		$(addprefix RED,$(UNI1))	# redskins
SAI =		$(addprefix SAI,$(UNI2))	# saints
SEA =		$(addprefix SEA,$(UNI2))	# seahawks
STE =		$(addprefix STE,$(UNI2))	# steelers
VIK =		$(addprefix VIK,$(UNI2))	# vikings
BRW =		$(addprefix BRW,$(UNI1))	# browns

TEAMS = BEA BEN BIL BRO BUC CAR CHA CHI COL COW DOL EAG FAL FOR GIA \
		JAG JET LIO TIT PAC PAN PAT RAI RAM RAV RED SAI SEA STE VIK \
		BRW

UNIFORMS =	$(BEA) $(BEN) $(BIL) $(BRO) $(BUC) $(CAR) $(CHA) $(CHI) $(COL) \
		$(COW) $(DOL) $(EAG) $(FAL) $(FOR) $(GIA) $(JAG) $(JET) $(LIO) \
		$(TIT) $(PAC) $(PAN) $(PAT) $(RAI) $(RAM) $(RAV) $(RED) $(SAI) \
		$(SEA) $(STE) $(VIK) $(BRW) armhnd.wms armhnd2.wms hedshu.wms hedshu2.wms

HELMETS  =	liovs.wms benvs.wms bilvs.wms brovs.wms bucvs.wms carvs.wms \
		chavs.wms chivs.wms colvs.wms cowvs.wms dolvs.wms eagvs.wms \
		falvs.wms forvs.wms giavs.wms jagvs.wms jetvs.wms beavs.wms \
		titvs.wms panvs.wms patvs.wms raivs.wms ramvs.wms ravvs.wms \
		redvs.wms saivs.wms seavs.wms stevs.wms vikvs.wms pacvs.wms \
		brwvs.wms

MISC =		off_x.wms off.wms ball.wms c_o_red.wms c_o_blu.wms \
		c_xo_red.wms c_xo_blu.wms c_x_yel.wms logo.wms \
		pslimb.wms pstorso.wms psshould.wms pshand.wms \
		playsel8.wms numtx_3d.wms \
        trail1.wms trail2.wms trail3.wms trail4.wms trail5.wms \
        trail6.wms trail7.wms trail8.wms trail9.wms trail10.wms \
        trail11.wms trail12.wms trail13.wms trail14.wms trail15.wms \
        trail16.wms trail17.wms \
        dirt1.wms dirt2.wms dirt3.wms dirt4.wms dirt5.wms dirt6.wms \
        dirt7.wms dirt8.wms dirt9.wms dirt10.wms \
        psarrow1.wms psarrow2.wms psarrow3.wms psarrow4.wms \
        psarrow5.wms psarrow6.wms psarrow7.wms psarrow8.wms \
		mpline.wms
#        blood1.wms blood2.wms blood3.wms blood4.wms blood5.wms blood6.wms \
#        blood7.wms blood8.wms blood9.wms blood10.wms \
#        btrail1.wms btrail2.wms btrail3.wms btrail4.wms btrail5.wms \
#        btrail6.wms btrail7.wms btrail8.wms btrail9.wms btrail10.wms \
#        btrail11.wms

HDS =	mark bran dan dude jasn jeff jen jim luis radn root \
	shnk skul thug sal danf jonc mik bleb palo
HDS1 =	$(addprefix hed_,$(HDS))

HEADS =	$(addsuffix .wms,$(HDS1))


#
# Endzones
#

ENDZONES =	BEAbg.wms BEAez1.wms BEAez2.wms BEAez3.wms \
		BENbg.wms BENez1.wms BENez2.wms BENez3.wms \
		BILbg.wms BILez1.wms BILez2.wms BILez3.wms \
		BRObg.wms BROez1.wms BROez2.wms BROez3.wms \
		BUCbg.wms BUCez1.wms BUCez2.wms BUCez3.wms \
		CARbg.wms CARez1.wms CARez2.wms CARez3.wms \
		CHAbg.wms CHAez1.wms CHAez2.wms CHAez3.wms \
		CHIbg.wms CHIez1.wms CHIez2.wms CHIez3.wms \
		COLbg.wms COLez1.wms COLez2.wms COLez3.wms \
		COWbg.wms COWez1.wms COWez2.wms COWez3.wms \
		DOLbg.wms DOLez1.wms DOLez2.wms DOLez3.wms \
		EAGbg.wms EAGez1.wms EAGez2.wms EAGez3.wms \
		FALbg.wms FALez1.wms FALez2.wms FALez3.wms \
		FORbg.wms FORez1.wms FORez2.wms FORez3.wms \
		GIAbg.wms GIAez1.wms GIAez2.wms GIAez3.wms \
		JAGbg.wms JAGez1.wms JAGez2.wms JAGez3.wms \
		JETbg.wms JETez1.wms JETez2.wms JETez3.wms \
		LIObg.wms LIOez1.wms LIOez2.wms LIOez3.wms \
		TITbg.wms TITez1.wms TITez2.wms TITez3.wms \
		PACbg.wms PACez1.wms PACez2.wms PACez3.wms \
		PANbg.wms PANez1.wms PANez2.wms PANez3.wms \
		PATbg.wms PATez1.wms PATez2.wms PATez3.wms \
		RAIbg.wms RAIez1.wms RAIez2.wms RAIez3.wms \
		RAMbg.wms RAMez1.wms RAMez2.wms RAMez3.wms \
		RAVbg.wms RAVez1.wms RAVez2.wms RAVez3.wms \
		REDbg.wms REDez1.wms REDez2.wms REDez3.wms \
		SAIbg.wms SAIez1.wms SAIez2.wms SAIez3.wms \
		SEAbg.wms SEAez1.wms SEAez2.wms SEAez3.wms \
		STEbg.wms STEez1.wms STEez2.wms STEez3.wms \
		VIKbg.wms VIKez1.wms VIKez2.wms VIKez3.wms \
		BRWbg.wms BRWez1.wms BRWez2.wms BRWez3.wms

SELBASE = $(addsuffix jerft,$(TEAMS)) \
		  $(addsuffix lghlt,$(TEAMS)) \
		  $(addsuffix svmkt,$(TEAMS)) \
		  $(addsuffix numt,$(TEAMS))

SELECT = $(addsuffix .wms,$(SELBASE))


#---------------------------
# ALL
#	Makefile entry point (default)
#---------------------------

all:
	@echo IMG3D
	@$(MAKE) --no-print-directory -f img3d.mak select
	@$(MAKE) --no-print-directory -f img3d.mak $(ENDZONES)
	@$(MAKE) --no-print-directory -f img3d.mak $(MISC)
	@$(MAKE) --no-print-directory -f img3d.mak $(HELMETS)
	@$(MAKE) --no-print-directory -f img3d.mak $(UNIFORMS)
	@$(MAKE) --no-print-directory -f img3d.mak $(HEADS)
	@$(MAKE) --no-print-directory -f img3d.mak $(SELECT)
	@echo IMG3D DONE


#---------------------------
# SELECT
#	Makefile entry point
#---------------------------

select:	$(addsuffix .tga,$(SELBASE)) 

%jerft.tga:		%jerfh.tga
	@echo Copying $< to $@
	@copy $< $@

%lghlt.tga:		%lghlh.tga
	@echo Copying $< to $@
	@copy $< $@

%svmkt.tga:		%svmkh.tga
	@echo Copying $< to $@
	@copy $< $@

%numt.tga:		%num.tga
	@echo Copying $< to $@
	@copy $< $@

%.wms:	%.3df
	@echo Converting $< to $@
	@3df2wms $<

%.3df:	%.tga
	@echo Converting $< to $@
	-texus -o $@ $<

$(addsuffix jerfa.3df,$(TEAMS)):
	@echo Converting $(basename $@).tga to $@
	@-texus -t rgb565 -o $@ $(basename $@).tga

$(addsuffix jerfh.3df,$(TEAMS)):
	@echo Converting $(basename $@).tga to $@
	@-texus -t rgb565 -o $@ $(basename $@).tga

$(addsuffix lhgla.3df,$(TEAMS)):
	@echo Converting $(basename $@).tga to $@
	@-texus -t rgb565 -o $@ $(basename $@).tga

$(addsuffix lhglh.3df,$(TEAMS)):
	@echo Converting $(basename $@).tga to $@
	@-texus -t rgb565 -o $@ $(basename $@).tga

$(addsuffix svmka.3df,$(TEAMS)):
	@echo Converting $(basename $@).tga to $@
	@-texus -t argb1555 -o $@ $(basename $@).tga

$(addsuffix svmkh.3df,$(TEAMS)):
	@echo Converting $(basename $@).tga to $@
	@-texus -t argb1555 -o $@ $(basename $@).tga

$(addsuffix num.3df,$(TEAMS)):
	@echo Converting $(basename $@).tga to $@
	@-texus -t argb1555 -o $@ $(basename $@).tga

$(addsuffix num2.3df,$(TEAMS)):
	@echo Converting $(basename $@).tga to $@
	@-texus -t argb1555 -o $@ $(basename $@).tga


# generate HOME uniforms with no MIP-MAPS
# NEW BLOCK

$(addsuffix jerft.3df,$(TEAMS)):
	@echo Converting $(basename $@).tga to $@
	@-texus -mn -o $@ $(basename $@).tga

$(addsuffix lghlt.3df,$(TEAMS)):
	@echo Converting $(basename $@).tga to $@
	@-texus -mn -o $@ $(basename $@).tga

$(addsuffix svmkt.3df,$(TEAMS)):
	@echo Converting $(basename $@).tga to $@
	@-texus -mn -t argb4444 -o $@ $(basename $@).tga

$(addsuffix numt.3df,$(TEAMS)):
	@echo Converting $(basename $@).tga to $@
	@-texus -mn -t argb1555 -o $@ $(basename $@).tga

# NEW BLOCK

c_o_red.3df c_o_blu.3df c_xo_red.3df c_xo_blu.3df c_x_yel.3df mpline.3df:
	@echo Converting $(basename $@).tga to $@
	@-texus -t argb4444 -mn -o $@ $(basename $@).tga

numtx_3d.3df off_x.3df off.3df pslimb.3df pstorso.3df psshould.3df pshand.3df:
	@echo Converting $(basename $@).tga to $@
	@-texus -mn -t argb4444 -o $@ $(basename $@).tga

psarrow1.3df psarrow2.3df psarrow3.3df psarrow4.3df \
psarrow5.3df psarrow6.3df psarrow7.3df psarrow8.3df :
	@echo Converting $(basename $@).tga to $@
	@-texus -mn -t argb1555 -o $@ $(basename $@).tga

playsel8.3df:
	@echo Converting $(basename $@).tga to $@
#	@-texus -mn -t rgb565 -o $@ $(basename $@).tga
	@-texus -mn -t argb1555 -o $@ $(basename $@).tga
#	@-texus -mn -t argb4444 -o $@ $(basename $@).tga

trail1.3df trail2.3df trail3.3df trail4.3df trail5.3df \
trail6.3df trail7.3df trail8.3df trail9.3df trail10.3df \
trail11.3df trail12.3df trail13.3df trail14.3df trail15.3df \
trail16.3df trail17.3df \
dirt1.3df dirt2.3df dirt3.3df dirt4.3df dirt5.3df dirt6.3df \
dirt7.3df dirt8.3df dirt9.3df dirt10.3df :
#blood1.3df blood2.3df blood3.3df blood4.3df blood5.3df blood6.3df \
#blood7.3df blood8.3df blood9.3df blood10.3df \
#btrail1.3df btrail2.3df btrail3.3df btrail4.3df btrail5.3df \
#btrail6.3df btrail7.3df btrail8.3df btrail9.3df btrail10.3df \
#btrail11.3df :
	@echo Converting $(basename $@).tga to $@
	@-texus -t argb8332 -o $@ $(basename $@).tga

$(addsuffix .3df,$(HDS1)) :
	@echo Converting $(basename $@).tga to $@
	@-texus -t argb1555 -o $@ $(basename $@).tga

#flash3.3df :
#	@echo Converting $(basename $@).tga to $@
#	@-texus -t rgb332 -o $@ $(basename $@).tga
