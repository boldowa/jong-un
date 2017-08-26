;-----------------------------------------------------------
; Unko
;  - object insert tool main code
;-----------------------------------------------------------

!VER_MAJ = $01
!VER_MIN = $10

!ExecutePtrL		= $0086fa
!EmptyObjectPtr		= $0db3e3
!GenericObject		= $0da8c3

macro ExecutePtrHijack(addr)
	assert read3(<addr>) == !ExecutePtrL
	org <addr>
		dl	ExecutePtrLC
endmacro

macro Object2DHijack(addr)
	assert read3(<addr>) == !EmptyObjectPtr
	org <addr>
		dl	Obj2DCode
endmacro

; --- Hijacks
; Group 1 (Normal/Cloud)
	%ExecutePtrHijack($0da452)
	%Object2DHijack($0da4d9)

; Group 2 (Castle)
	%ExecutePtrHijack($0dc197)
	%Object2DHijack($0dc21e)

; Group 3 (Rope)
	%ExecutePtrHijack($0dcd97)
	%Object2DHijack($0dce1e)

; Group 4 (Underground)
	%ExecutePtrHijack($0dd997)
	%Object2DHijack($0dda1e)

; Group 5 (Ghost House/Switch Palace)
	%ExecutePtrHijack($0de897)
	%Object2DHijack($0de91e)

; Group 6 (ExObj)
	%ExecutePtrHijack($0da10c)




freecode
;--- Unko codes

ExecutePtrLC:
	sty.b	$05
	ply
	sty.b	$02
	rep	#$30
	and.w	#$00ff
	sta.b	$03
	asl

	adc.b	$03
	tay
	pla
	sta.b	$03
	iny
	lda.b	[$02],y
	sta.b	$00
	iny
	lda.b	[$02],y
	sta.b	$01
	sep	#$30
	ldy.b	$05

	; isn't smw original code
	lda.b	$02		;\   Check code bank.
	cmp.b	#$10		; |  It's custom object that bank $10 or higher.
	bcc	+		;/
	pla			;\   Remove JSR return address from stack.
	pla			;/
+	jmp	[$0000]		

Obj2DCode:
	sep	#$30
	ldy.b	#0
	lda.b	[$65],y
	sta.b	$5a
	iny
	lda.b	[$65],y
	sta.b	$58
	iny
	tya
	clc
	adc.b	$65
	sta.b	$65
	bcc	+
	inc	$66
+	ldx	$5a
	txa
	jsl.l	!ExecutePtrL
Obj2DTable:
	rep 256 : dl !GenericObject


;--- set code version.
;    it's important for detect code version in Unko.

	db	!VER_MIN, !VER_MAJ

