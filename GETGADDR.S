bss	group	bss

	section	.text

	xdef	get_text_addr
	xdef	get_text_end_addr
	xref	start
	xref	__tend

get_text_addr:
	la	r2,start
	jr	r31
	nop

get_text_end_addr:
	la	r2,__tend
	jr	r31
	nop

