dnl  Intel Atom mpn_rshift -- mpn right shift.

dnl  Copyright 2011 Free Software Foundation, Inc.

dnl  Converted from AMD64 by Marco Bodrato.

dnl  This file is part of the GNU MP Library.
dnl
dnl  The GNU MP Library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public License as
dnl  published by the Free Software Foundation; either version 3 of the
dnl  License, or (at your option) any later version.
dnl
dnl  The GNU MP Library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.

include(`../config.m4')

defframe(PARAM_CNT, 16)
defframe(PARAM_SIZE,12)
defframe(PARAM_SRC,  8)
defframe(PARAM_DST,  4)

dnl  re-use parameter space
define(SAVE_UP,`PARAM_CNT')
define(VAR_COUNT,`PARAM_SIZE')
define(SAVE_EBX,`PARAM_SRC')
define(SAVE_EBP,`PARAM_DST')

define(`rp',  `%edi')
define(`up',  `%esi')
define(`cnt',  `%ecx')

ASM_START()
	TEXT
	ALIGN(8)
deflit(`FRAME',0)
PROLOGUE(mpn_rshift)
	movl	PARAM_CNT, cnt
	movl	PARAM_SIZE, %edx
	movl	up, SAVE_UP
	movl	PARAM_SRC, up
	push	rp			FRAME_pushl()
	movl	PARAM_DST, rp
	movl	%ebx, SAVE_EBX

	shr	%edx
	mov	(up), %eax
	movl	%edx, VAR_COUNT
	jnc	L(evn)

	mov	%eax, %ebx
	shr	%cl, %ebx
	neg	cnt
	shl	%cl, %eax
	test	%edx, %edx
	jnz	L(gt1)
	mov	%ebx, (rp)
	jmp	L(quit)

L(gt1):	movl	%ebp, SAVE_EBP
	push	%eax
	mov	4(up), %eax
	mov	%eax, %ebp
	shl	%cl, %eax
	jmp	L(lo1)

L(evn):	movl	%ebp, SAVE_EBP
	neg	cnt
	mov	%eax, %ebp
	mov	4(up), %edx
	shl	%cl, %eax
	mov	%edx, %ebx
	shl	%cl, %edx
	neg	cnt
	decl	VAR_COUNT
	lea	-4(rp), rp
	lea	4(up), up
	jz	L(end)
	push	%eax			FRAME_pushl()

	ALIGN(16)
L(top):	shr	%cl, %ebp
	or	%ebp, %edx
	shr	%cl, %ebx
	neg	cnt
	mov	4(up), %eax
	mov	%eax, %ebp
	mov	%edx, 4(rp)
	shl	%cl, %eax
	lea	8(rp), rp
L(lo1):	mov	8(up), %edx
	or	%ebx, %eax
	mov	%edx, %ebx
	shl	%cl, %edx
	lea	8(up), up
	neg	cnt
	mov	%eax, (rp)
	decl	VAR_COUNT
	jg	L(top)

	pop	%eax			FRAME_popl()
L(end):
	shr	%cl, %ebp
	shr	%cl, %ebx
	or	%ebp, %edx
	movl	SAVE_EBP, %ebp
	mov	%edx, 4(rp)
	mov	%ebx, 8(rp)

L(quit):
	movl	SAVE_UP, up
	movl	SAVE_EBX, %ebx
	pop	rp			FRAME_popl()
	ret
EPILOGUE()
ASM_END()
