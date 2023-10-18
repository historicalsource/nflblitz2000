texus -mn -o grass1a.3df grass1a.tga
rem texus -mn -o grass1e.3df grass1e.tga
texus -mn -o grass1c.3df grass1c.tga
texus -mn -o grass1d.3df grass1d.tga
texus -mn -o grass1b.3df grass1b.tga
texus -mn -o grass2a.3df grass2a.tga
texus -mn -o grass2e.3df grass2e.tga
texus -mn -o grass2d.3df grass2d.tga
texus -mn -o grass2c.3df grass2c.tga
texus -mn -o grass2b.3df grass2b.tga
texus -mn -o grass3a.3df grass3a.tga
texus -mn -o grass3e.3df grass3e.tga
texus -mn -o grass3d.3df grass3d.tga
texus -mn -o grass3c.3df grass3c.tga
texus -mn -o grass3b.3df grass3b.tga
3df2wms -f GR_MIPMAP_NEAREST_DITHER grass??.3df
mwms2wms grass1?.wms grass1.wms
rem mwms2wms grass2?.wms grass2.wms
rem mwms2wms grass3?.wms grass3.wms
pcopy grass1.wms phx:
