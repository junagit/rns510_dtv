.section .text

.global ssl_memcpy

ssl_memcpy:
	stmdb	sp!, {r0, r4, lr}
	subs	r2, r2, #4
	blt		l_lt4
	ands	ip, r0, #3
	pld		[r1]
	bne		l_nalign4
	ands	ip, r1, #3
	bne		l_nalign

l_align4:
	subs	r2, r2, #28
	stmdb	sp!, {r5, r6, r7, r8}
	blt		l_lt32
	pld		[r1]
2:
	subs	r2, r2, #96
	pld		[r1, #28]
	blt		l_lt128
	pld		[r1, #60]
	pld		[r1, #92]

l_ge32:
	pld	[r1, #124]

l_lt128:
	ldmia	r1!, {r3, r4, r5, r6, r7, r8, ip, lr}
	subs	r2, r2, #32
	stmia	r0!, {r3, r4, r5, r6, r7, r8, ip, lr}
	bge		l_ge32
	cmn		r2, #96
	bge		l_lt128

l_lt32:
	ands	ip, r2, #28
	rsb		ip, ip, #32
	addne	pc, pc, ip
	b		7f

6:
	nop
	ldr		r3, [r1], #4
	ldr		r4, [r1], #4
	ldr		r5, [r1], #4
	ldr		r6, [r1], #4
	ldr		r7, [r1], #4
	ldr		r8, [r1], #4
	ldr		lr, [r1], #4
	add		pc, pc, ip
	nop
	nop
	str		r3, [r0], #4
	str		r4, [r0], #4
	str		r5, [r0], #4
	str		r6, [r0], #4
	str		r7, [r0], #4
	str		r8, [r0], #4
	str		lr, [r0], #4

7:
	ldmia	sp!, {r5, r6, r7, r8}

l_lt4:
	movs	r2, r2, lsl #31
	ldrneb	r3, [r1], #1
	ldrcsb	r4, [r1], #1
	ldrcsb	ip, [r1], #1
	strneb	r3, [r0], #1
	strcsb	r4, [r0], #1
	strcsb	ip, [r0], #1
	ldmia	sp!, {r0, r4, pc}

l_nalign4:
	rsb		ip, ip, #4
	cmp		ip, #2
	ldrgtb	r3, [r1], #1
	ldrgeb	r4, [r1], #1
	ldrb	lr, [r1], #1
	strgtb	r3, [r0], #1
	strgeb	r4, [r0], #1
	subs	r2, r2, ip
	strb	lr, [r0], #1
	blt		l_lt4
	ands	ip, r1, #3
	beq		l_align4

l_nalign:
	bic		r1, r1, #3
	cmp		ip, #2
	ldr		lr, [r1], #4
	beq		17f
	bgt		18f


;@ 8/24
	subs	r2, r2, #28
	blt		14f
11:
	stmdb	sp!, {r5, r6, r7, r8, r9}
	pld		[r1]
	subs	r2, r2, #96
	pld		[r1, #28]
	blt		13f
	pld		[r1, #60]
	pld		[r1, #92]
12:
	pld		[r1, #124]
13:
	ldmia	r1!, {r4, r5, r6, r7}
	mov		r3, lr, lsr #8
	subs	r2, r2, #32
	ldmia	r1!, {r8, r9, ip, lr}
	orr		r3, r3, r4, lsl #24
	mov		r4, r4, lsr #8
	orr		r4, r4, r5, lsl #24
	mov		r5, r5, lsr #8
	orr		r5, r5, r6, lsl #24
	mov		r6, r6, lsr #8
	orr		r6, r6, r7, lsl #24
	mov		r7, r7, lsr #8
	orr		r7, r7, r8, lsl #24
	mov		r8, r8, lsr #8
	orr		r8, r8, r9, lsl #24
	mov		r9, r9, lsr #8
	orr		r9, r9, ip, lsl #24
	mov		ip, ip, lsr #8
	orr		ip, ip, lr, lsl #24
	stmia	r0!, {r3, r4, r5, r6, r7, r8, r9, ip}
	bge		12b
	cmn		r2, #96
	bge		13b
	ldmia	sp!, {r5, r6, r7, r8, r9}
14:
	ands	ip, r2, #28
	beq		16f
15:
	mov		r3, lr, lsr #8
	ldr		lr, [r1], #4
	subs	ip, ip, #4
	orr		r3, r3, lr, lsl #24
	str		r3, [r0], #4
	bgt		15b
16:
	sub		r1, r1, #3
	b		l_lt4


;@ 16/16
17:
	subs	r2, r2, #28
	blt		14f
11:
	stmdb	sp!, {r5, r6, r7, r8, r9}
	pld		[r1]
	subs	r2, r2, #96
	pld		[r1, #28]
	blt		13f
	pld		[r1, #60]
	pld		[r1, #92]
12:
	pld		[r1, #124]
13:
	ldmia	r1!, {r4, r5, r6, r7}
	mov		r3, lr, lsr #16
	subs	r2, r2, #32
	ldmia	r1!, {r8, r9, ip, lr}
	orr		r3, r3, r4, lsl #16
	mov		r4, r4, lsr #16
	orr		r4, r4, r5, lsl #16
	mov		r5, r5, lsr #16
	orr		r5, r5, r6, lsl #16
	mov		r6, r6, lsr #16
	orr		r6, r6, r7, lsl #16
	mov		r7, r7, lsr #16
	orr		r7, r7, r8, lsl #16
	mov		r8, r8, lsr #16
	orr		r8, r8, r9, lsl #16
	mov		r9, r9, lsr #16
	orr		r9, r9, ip, lsl #16
	mov		ip, ip, lsr #16
	orr		ip, ip, lr, lsl #16
	stmia	r0!, {r3, r4, r5, r6, r7, r8, r9, ip}
	bge		12b
	cmn		r2, #96
	bge		13b

	ldmia	sp!, {r5, r6, r7, r8, r9}
14:
	ands	ip, r2, #28
	beq		16f
15:
	mov		r3, lr, lsr #16
	ldr		lr, [r1], #4
	subs	ip, ip, #4
	orr		r3, r3, lr, lsl #16
	str		r3, [r0], #4
	bgt		15b
16:
	sub		r1, r1, #2
	b		l_lt4


;@ 24/8
18:
	subs	r2, r2, #28
	blt		14f
11:
	stmdb	sp!, {r5, r6, r7, r8, r9}
	pld		[r1]
	subs	r2, r2, #96
	pld		[r1, #28]
	blt		13f
	pld		[r1, #60]
	pld		[r1, #92]
12:
	pld		[r1, #124]
13:
	ldmia	r1!, {r4, r5, r6, r7}
	mov		r3, lr, lsr #24
	subs	r2, r2, #32
	ldmia	r1!, {r8, r9, ip, lr}
	orr		r3, r3, r4, lsl #8
	mov		r4, r4, lsr #24
	orr		r4, r4, r5, lsl #8
	mov		r5, r5, lsr #24
	orr		r5, r5, r6, lsl #8
	mov		r6, r6, lsr #24
	orr		r6, r6, r7, lsl #8
	mov		r7, r7, lsr #24
	orr		r7, r7, r8, lsl #8
	mov		r8, r8, lsr #24
	orr		r8, r8, r9, lsl #8
	mov		r9, r9, lsr #24
	orr		r9, r9, ip, lsl #8
	mov		ip, ip, lsr #24
	orr		ip, ip, lr, lsl #8
	stmia	r0!, {r3, r4, r5, r6, r7, r8, r9, ip}
	bge		12b
	cmn		r2, #96
	bge		13b
	ldmia	sp!, {r5, r6, r7, r8, r9}
14:
	ands	ip, r2, #28
	beq		16f
15:
	mov		r3, lr, lsr #24
	ldr		lr, [r1], #4
	subs	ip, ip, #4
	orr	r3, r3, lr, lsl #8
	str		r3, [r0], #4
	bgt		15b
16:
	sub		r1, r1, #1
	b		l_lt4
