;-------------------------------------------------
; Cement gate
;-------------------------------------------------
!ConditionRAM	= $0dbe

main:
	lda	!ConditionRAM
	beq	+
	ldy.b	$57
	lda.b	#$30
	jml	Store1to6E
+	rtl

