global fb_font

section .data
align 8
fb_font:
    incbin "drivers/terminus.psf"
