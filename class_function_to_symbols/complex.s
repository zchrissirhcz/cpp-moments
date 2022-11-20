
./a.out:	file format mach-o arm64

Disassembly of section __TEXT,__text:

0000000100003f1c <_main>:
100003f1c: ff 83 00 d1 	sub	sp, sp, #32
100003f20: fd 7b 01 a9 	stp	x29, x30, [sp, #16]
100003f24: fd 43 00 91 	add	x29, sp, #16
100003f28: e0 03 00 91 	mov	x0, sp
100003f2c: 00 10 60 1e 	fmov	d0, #2.00000000
100003f30: 01 10 6e 1e 	fmov	d1, #1.00000000
100003f34: 0d 00 00 94 	bl	0x100003f68 <__ZN7complexC1Edd>
100003f38: e0 03 00 91 	mov	x0, sp
100003f3c: 00 10 61 1e 	fmov	d0, #3.00000000
100003f40: 0f 00 00 94 	bl	0x100003f7c <__ZN7complex4realEd>
100003f44: e0 03 00 91 	mov	x0, sp
100003f48: 00 90 62 1e 	fmov	d0, #5.00000000
100003f4c: 0c 00 00 94 	bl	0x100003f7c <__ZN7complex4realEd>
100003f50: e0 03 00 91 	mov	x0, sp
100003f54: 0c 00 00 94 	bl	0x100003f84 <__ZN7complexD1Ev>
100003f58: 00 00 80 52 	mov	w0, #0
100003f5c: fd 7b 41 a9 	ldp	x29, x30, [sp, #16]
100003f60: ff 83 00 91 	add	sp, sp, #32
100003f64: c0 03 5f d6 	ret

0000000100003f68 <__ZN7complexC1Edd>:
100003f68: fd 7b bf a9 	stp	x29, x30, [sp, #-16]!
100003f6c: fd 03 00 91 	mov	x29, sp
100003f70: 0a 00 00 94 	bl	0x100003f98 <__ZN7complexC2Edd>
100003f74: fd 7b c1 a8 	ldp	x29, x30, [sp], #16
100003f78: c0 03 5f d6 	ret

0000000100003f7c <__ZN7complex4realEd>:
100003f7c: 00 00 00 fd 	str	d0, [x0]
100003f80: c0 03 5f d6 	ret

0000000100003f84 <__ZN7complexD1Ev>:
100003f84: fd 7b bf a9 	stp	x29, x30, [sp, #-16]!
100003f88: fd 03 00 91 	mov	x29, sp
100003f8c: 06 00 00 94 	bl	0x100003fa4 <__ZN7complexD2Ev>
100003f90: fd 7b c1 a8 	ldp	x29, x30, [sp], #16
100003f94: c0 03 5f d6 	ret

0000000100003f98 <__ZN7complexC2Edd>:
100003f98: 00 00 00 fd 	str	d0, [x0]
100003f9c: 01 04 00 fd 	str	d1, [x0, #8]
100003fa0: c0 03 5f d6 	ret

0000000100003fa4 <__ZN7complexD2Ev>:
100003fa4: c0 03 5f d6 	ret
