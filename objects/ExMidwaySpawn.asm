;-------------------------------------------------
; Extend Midway Spawn Point
;-------------------------------------------------

; RPG Hacker: This should really be an automatic
; include, but UNKO doesn't support this yet
incsrc ../libraries/sa1def.asm

main:	
	ldx.w	$13bf|!Base2
	lda.w	$1ea2|!Base2,x
	and.b	#$40
	beq	+

	lda.b	$57
	and.b	#$0f		;\
	asl			; | Set X low
	asl			; |
	asl			; |
	asl			; |
	sta.b	$94		;/
	lda.w	$1928|!Base2		;\  Set X high
	sta.b	$95		;/
	lda.b	$57		;\
	and.b	$f0		; | Set Y low
	sta.b	$96		;/
	lda.b	$0a		;\
	and.b	#$10		; | Set Y high
	lsr			; |
	lsr			; |
	lsr			; |
	lsr			; |
	sta.b	$97		;/
+	rtl

