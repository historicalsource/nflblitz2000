	sectionw	.data

	opt	w-
	cnop	0,8
	opt	w+

	xdef	anim_data
	xdef	anim_data_end

anim_data
	incbin	"/video/nfl/anim/genseq.ani"
anim_data_end
