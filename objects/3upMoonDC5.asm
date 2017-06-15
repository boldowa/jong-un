;-------------------------------------------------
; Spawn 3up moon if collect dragon coin
;-------------------------------------------------
!DragonCoinCollects	= $1f2f
!3upMoonObteined	= $1fee

main:
	jsl	GetCheck
	and	!DragonCoinCollects,y
	beq	+
	and	!3upMoonObteined,y
	bne	+
	ldy.b	$57
	lda.b	#$6e
	jml	Store0to6E
+	rtl

