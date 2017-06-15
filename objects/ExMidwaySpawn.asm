;-------------------------------------------------
; Extend Midway Spawn Point
;-------------------------------------------------
main:	
	ldx.w	$13bf
	lda.w	$1ea2,x
	and.b	#$40
	beq	+

	lda.b	$57
	and.b	#$0f		;\
	asl			; | Set X low
	asl			; |
	asl			; |
	asl			; |
	sta.b	$94		;/
	lda.w	$1928		;\  Set X high
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

