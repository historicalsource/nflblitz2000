REM Converts all textures for player uniforms
REM TGA -> 3df -> WMS

REM All ???H.WMS and ???A.WMS files are the uniforms used for the team select screen
REM All ???HLOD.WMS and ???ALOD.WMS files are the uniforms used during the game

REM 5/27/99 PERRIN
REM All files in STAGE1 used to be converted to 1555 format. This is now changed to 4444 format.
REM (i.e. texus -t argb1555 -mn -o BEAH.3df BEAH.tga)

REM ---------------
REM		STAGE 1
REM ---------------

texus -t argb4444 -mn -o BEAH.3df BEAH.tga
3df2wms BEAH.3df
texus -t argb4444 -mn -o BEAA.3df BEAA.tga
3df2wms BEAA.3df

texus -t argb4444 -mn -o BENH.3df BENH.tga
3df2wms BENH.3df
texus -t argb4444 -mn -o BENA.3df BENA.tga
3df2wms BENA.3df

texus -t argb4444 -mn -o BILH.3df BILH.tga
3df2wms BILH.3df
texus -t argb4444 -mn -o BILA.3df BILA.tga
3df2wms BILA.3df

texus -t argb4444 -mn -o BROH.3df BROH.tga
3df2wms BROH.3df
texus -t argb4444 -mn -o BROA.3df BROA.tga
3df2wms BROA.3df

texus -t argb4444 -mn -o BUCH.3df BUCH.tga
3df2wms BUCH.3df
texus -t argb4444 -mn -o BUCA.3df BUCA.tga
3df2wms BUCA.3df

texus -t argb4444 -mn -o CARH.3df CARH.tga
3df2wms CARH.3df
texus -t argb4444 -mn -o CARA.3df CARA.tga
3df2wms CARA.3df

texus -t argb4444 -mn -o CHAH.3df CHAH.tga
3df2wms CHAH.3df
texus -t argb4444 -mn -o CHAA.3df CHAA.tga
3df2wms CHAA.3df

texus -t argb4444 -mn -o CHIH.3df CHIH.tga
3df2wms CHIH.3df
texus -t argb4444 -mn -o CHIA.3df CHIA.tga
3df2wms CHIA.3df

texus -t argb4444 -mn -o COLH.3df COLH.tga
3df2wms COLH.3df
texus -t argb4444 -mn -o COLA.3df COLA.tga
3df2wms COLA.3df

texus -t argb4444 -mn -o COWH.3df COWH.tga
3df2wms COWH.3df
texus -t argb4444 -mn -o COWA.3df COWA.tga
3df2wms COWA.3df

texus -t argb4444 -mn -o DOLH.3df DOLH.tga
3df2wms DOLH.3df
texus -t argb4444 -mn -o DOLA.3df DOLA.tga
3df2wms DOLA.3df

texus -t argb4444 -mn -o EAGH.3df EAGH.tga
3df2wms EAGH.3df
texus -t argb4444 -mn -o EAGA.3df EAGA.tga
3df2wms EAGA.3df

texus -t argb4444 -mn -o FALH.3df FALH.tga
3df2wms FALH.3df
texus -t argb4444 -mn -o FALA.3df FALA.tga
3df2wms FALA.3df

texus -t argb4444 -mn -o FORH.3df FORH.tga
3df2wms FORH.3df
texus -t argb4444 -mn -o FORA.3df FORA.tga
3df2wms FORA.3df

texus -t argb4444 -mn -o GIAH.3df GIAH.tga
3df2wms GIAH.3df
texus -t argb4444 -mn -o GIAA.3df GIAA.tga
3df2wms GIAA.3df

texus -t argb4444 -mn -o JAGH.3df JAGH.tga
3df2wms JAGH.3df
texus -t argb4444 -mn -o JAGA.3df JAGA.tga
3df2wms JAGA.3df

texus -t argb4444 -mn -o JETH.3df JETH.tga
3df2wms JETH.3df
texus -t argb4444 -mn -o JETA.3df JETA.tga
3df2wms JETA.3df

texus -t argb4444 -mn -o LIOH.3df LIOH.tga
3df2wms LIOH.3df
texus -t argb4444 -mn -o LIOA.3df LIOA.tga
3df2wms LIOA.3df

texus -t argb4444 -mn -o TITH.3df TITH.tga
3df2wms TITH.3df
texus -t argb4444 -mn -o TITA.3df TITA.tga
3df2wms TITA.3df

texus -t argb4444 -mn -o PACH.3df PACH.tga
3df2wms PACH.3df
texus -t argb4444 -mn -o PACA.3df PACA.tga
3df2wms PACA.3df

texus -t argb4444 -mn -o PANH.3df PANH.tga
3df2wms PANH.3df
texus -t argb4444 -mn -o PANA.3df PANA.tga
3df2wms PANA.3df

texus -t argb4444 -mn -o PATH.3df PATH.tga
3df2wms PATH.3df
texus -t argb4444 -mn -o PATA.3df PATA.tga
3df2wms PATA.3df

texus -t argb4444 -mn -o RAIH.3df RAIH.tga
3df2wms RAIH.3df
texus -t argb4444 -mn -o RAIA.3df RAIA.tga
3df2wms RAIA.3df

texus -t argb4444 -mn -o RAMH.3df RAMH.tga
3df2wms RAMH.3df
texus -t argb4444 -mn -o RAMA.3df RAMA.tga
3df2wms RAMA.3df

texus -t argb4444 -mn -o RAVH.3df RAVH.tga
3df2wms RAVH.3df
texus -t argb4444 -mn -o RAVA.3df RAVA.tga
3df2wms RAVA.3df

texus -t argb4444 -mn -o REDH.3df REDH.tga
3df2wms REDH.3df
texus -t argb4444 -mn -o REDA.3df REDA.tga
3df2wms REDA.3df

texus -t argb4444 -mn -o SAIH.3df SAIH.tga
3df2wms SAIH.3df
texus -t argb4444 -mn -o SAIA.3df SAIA.tga
3df2wms SAIA.3df

texus -t argb4444 -mn -o SEAH.3df SEAH.tga
3df2wms SEAH.3df
texus -t argb4444 -mn -o SEAA.3df SEAA.tga
3df2wms SEAA.3df

texus -t argb4444 -mn -o STEH.3df STEH.tga
3df2wms STEH.3df
texus -t argb4444 -mn -o STEA.3df STEA.tga
3df2wms STEA.3df

texus -t argb4444 -mn -o VIKH.3df VIKH.tga
3df2wms VIKH.3df
texus -t argb4444 -mn -o VIKA.3df VIKA.tga
3df2wms VIKA.3df

texus -t argb4444 -mn -o BRWH.3df BRWH.tga
3df2wms BRWH.3df
texus -t argb4444 -mn -o BRWA.3df BRWA.tga
3df2wms BRWA.3df

REM ---------------
REM		STAGE 2
REM ---------------

copy BEAH.tga BEAHLOD.tga
copy BEAA.tga BEAALOD.tga

copy BENH.tga BENHLOD.tga
copy BENA.tga BENALOD.tga

copy BILH.tga BILHLOD.tga
copy BILA.tga BILALOD.tga

copy BROH.tga BROHLOD.tga
copy BROA.tga BROALOD.tga

copy BUCH.tga BUCHLOD.tga
copy BUCA.tga BUCALOD.tga

copy CARH.tga CARHLOD.tga
copy CARA.tga CARALOD.tga

copy CHAH.tga CHAHLOD.tga
copy CHAA.tga CHAALOD.tga

copy CHIH.tga CHIHLOD.tga
copy CHIA.tga CHIALOD.tga

copy COLH.tga COLHLOD.tga
copy COLA.tga COLALOD.tga

copy COWH.tga COWHLOD.tga
copy COWA.tga COWALOD.tga

copy DOLH.tga DOLHLOD.tga
copy DOLA.tga DOLALOD.tga

copy EAGH.tga EAGHLOD.tga
copy EAGA.tga EAGALOD.tga

copy FALH.tga FALHLOD.tga
copy FALA.tga FALALOD.tga

copy FORH.tga FORHLOD.tga
copy FORA.tga FORALOD.tga

copy GIAH.tga GIAHLOD.tga
copy GIAA.tga GIAALOD.tga

copy JAGH.tga JAGHLOD.tga
copy JAGA.tga JAGALOD.tga

copy JETH.tga JETHLOD.tga
copy JETA.tga JETALOD.tga

copy LIOH.tga LIOHLOD.tga
copy LIOA.tga LIOALOD.tga

copy TITH.tga TITHLOD.tga
copy TITA.tga TITALOD.tga

copy PACH.tga PACHLOD.tga
copy PACA.tga PACALOD.tga

copy PANH.tga PANHLOD.tga
copy PANA.tga PANALOD.tga

copy PATH.tga PATHLOD.tga
copy PATA.tga PATALOD.tga

copy RAIH.tga RAIHLOD.tga
copy RAIA.tga RAIALOD.tga

copy RAMH.tga RAMHLOD.tga
copy RAMA.tga RAMALOD.tga

copy RAVH.tga RAVHLOD.tga
copy RAVA.tga RAVALOD.tga

copy REDH.tga REDHLOD.tga
copy REDA.tga REDALOD.tga

copy SAIH.tga SAIHLOD.tga
copy SAIA.tga SAIALOD.tga

copy SEAH.tga SEAHLOD.tga
copy SEAA.tga SEAALOD.tga

copy STEH.tga STEHLOD.tga
copy STEA.tga STEALOD.tga

copy VIKH.tga VIKHLOD.tga
copy VIKA.tga VIKALOD.tga

copy BRWH.tga BRWHLOD.tga
copy BRWA.tga BRWALOD.tga

REM ---------------
REM		STAGE 3
REM ---------------

texus -t argb1555 -o BEAHLOD.3df BEAHLOD.tga
3df2wms BEAHLOD.3df
texus -t argb1555 -o BEAALOD.3df BEAALOD.tga
3df2wms BEAALOD.3df

texus -t argb1555 -o BENHLOD.3df BENHLOD.tga
3df2wms BENHLOD.3df
texus -t argb1555 -o BENALOD.3df BENALOD.tga
3df2wms BENALOD.3df

texus -t argb1555 -o BILHLOD.3df BILHLOD.tga
3df2wms BILHLOD.3df
texus -t argb1555 -o BILALOD.3df BILALOD.tga
3df2wms BILALOD.3df

texus -t argb1555 -o BROHLOD.3df BROHLOD.tga
3df2wms BROHLOD.3df
texus -t argb1555 -o BROALOD.3df BROALOD.tga
3df2wms BROALOD.3df

texus -t argb1555 -o BUCHLOD.3df BUCHLOD.tga
3df2wms BUCHLOD.3df
texus -t argb1555 -o BUCALOD.3df BUCALOD.tga
3df2wms BUCALOD.3df

texus -t argb1555 -o CARHLOD.3df CARHLOD.tga
3df2wms CARHLOD.3df
texus -t argb1555 -o CARALOD.3df CARALOD.tga
3df2wms CARALOD.3df

texus -t argb1555 -o CHAHLOD.3df CHAHLOD.tga
3df2wms CHAHLOD.3df
texus -t argb1555 -o CHAALOD.3df CHAALOD.tga
3df2wms CHAALOD.3df

texus -t argb1555 -o CHIHLOD.3df CHIHLOD.tga
3df2wms CHIHLOD.3df
texus -t argb1555 -o CHIALOD.3df CHIALOD.tga
3df2wms CHIALOD.3df

texus -t argb1555 -o COLHLOD.3df COLHLOD.tga
3df2wms COLHLOD.3df
texus -t argb1555 -o COLALOD.3df COLALOD.tga
3df2wms COLALOD.3df

texus -t argb1555 -o COWHLOD.3df COWHLOD.tga
3df2wms COWHLOD.3df
texus -t argb1555 -o COWALOD.3df COWALOD.tga
3df2wms COWALOD.3df

texus -t argb1555 -o DOLHLOD.3df DOLHLOD.tga
3df2wms DOLHLOD.3df
texus -t argb1555 -o DOLALOD.3df DOLALOD.tga
3df2wms DOLALOD.3df

texus -t argb1555 -o EAGHLOD.3df EAGHLOD.tga
3df2wms EAGHLOD.3df
texus -t argb1555 -o EAGALOD.3df EAGALOD.tga
3df2wms EAGALOD.3df

texus -t argb1555 -o FALHLOD.3df FALHLOD.tga
3df2wms FALHLOD.3df
texus -t argb1555 -o FALALOD.3df FALALOD.tga
3df2wms FALALOD.3df

texus -t argb1555 -o FORHLOD.3df FORHLOD.tga
3df2wms FORHLOD.3df
texus -t argb1555 -o FORALOD.3df FORALOD.tga
3df2wms FORALOD.3df

texus -t argb1555 -o GIAHLOD.3df GIAHLOD.tga
3df2wms GIAHLOD.3df
texus -t argb1555 -o GIAALOD.3df GIAALOD.tga
3df2wms GIAALOD.3df

texus -t argb1555 -o JAGHLOD.3df JAGHLOD.tga
3df2wms JAGHLOD.3df
texus -t argb1555 -o JAGALOD.3df JAGALOD.tga
3df2wms JAGALOD.3df

texus -t argb1555 -o JETHLOD.3df JETHLOD.tga
3df2wms JETHLOD.3df
texus -t argb1555 -o JETALOD.3df JETALOD.tga
3df2wms JETALOD.3df

texus -t argb1555 -o LIOHLOD.3df LIOHLOD.tga
3df2wms LIOHLOD.3df
texus -t argb1555 -o LIOALOD.3df LIOALOD.tga
3df2wms LIOALOD.3df

texus -t argb1555 -o TITHLOD.3df TITHLOD.tga
3df2wms TITHLOD.3df
texus -t argb1555 -o TITALOD.3df TITALOD.tga
3df2wms TITALOD.3df

texus -t argb1555 -o PACHLOD.3df PACHLOD.tga
3df2wms PACHLOD.3df
texus -t argb1555 -o PACALOD.3df PACALOD.tga
3df2wms PACALOD.3df

texus -t argb1555 -o PANHLOD.3df PANHLOD.tga
3df2wms PANHLOD.3df
texus -t argb1555 -o PANALOD.3df PANALOD.tga
3df2wms PANALOD.3df

texus -t argb1555 -o PATHLOD.3df PATHLOD.tga
3df2wms PATHLOD.3df
texus -t argb1555 -o PATALOD.3df PATALOD.tga
3df2wms PATALOD.3df

texus -t argb1555 -o RAIHLOD.3df RAIHLOD.tga
3df2wms RAIHLOD.3df
texus -t argb1555 -o RAIALOD.3df RAIALOD.tga
3df2wms RAIALOD.3df

texus -t argb1555 -o RAMHLOD.3df RAMHLOD.tga
3df2wms RAMHLOD.3df
texus -t argb1555 -o RAMALOD.3df RAMALOD.tga
3df2wms RAMALOD.3df

texus -t argb1555 -o RAVHLOD.3df RAVHLOD.tga
3df2wms RAVHLOD.3df
texus -t argb1555 -o RAVALOD.3df RAVALOD.tga
3df2wms RAVALOD.3df

texus -t argb1555 -o REDHLOD.3df REDHLOD.tga
3df2wms REDHLOD.3df
texus -t argb1555 -o REDALOD.3df REDALOD.tga
3df2wms REDALOD.3df

texus -t argb1555 -o SAIHLOD.3df SAIHLOD.tga
3df2wms SAIHLOD.3df
texus -t argb1555 -o SAIALOD.3df SAIALOD.tga
3df2wms SAIALOD.3df

texus -t argb1555 -o SEAHLOD.3df SEAHLOD.tga
3df2wms SEAHLOD.3df
texus -t argb1555 -o SEAALOD.3df SEAALOD.tga
3df2wms SEAALOD.3df

texus -t argb1555 -o STEHLOD.3df STEHLOD.tga
3df2wms STEHLOD.3df
texus -t argb1555 -o STEALOD.3df STEALOD.tga
3df2wms STEALOD.3df

texus -t argb1555 -o VIKHLOD.3df VIKHLOD.tga
3df2wms VIKHLOD.3df
texus -t argb1555 -o VIKALOD.3df VIKALOD.tga
3df2wms VIKALOD.3df

texus -t argb1555 -o BRWHLOD.3df BRWHLOD.tga
3df2wms BRWHLOD.3df
texus -t argb1555 -o BRWALOD.3df BRWALOD.tga
3df2wms BRWALOD.3df
