;-------------------------------------------------
; Spawn 3up moon if collect dragon coin
;-------------------------------------------------

; RPG Hacker: This should really be an automatic
; include, but UNKO doesn't support this yet
incsrc ../libraries/sa1def.asm

!DragonCoinCollects	= $1f2f|!Base2
!3upMoonObteined	= $1fee|!Base2

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

