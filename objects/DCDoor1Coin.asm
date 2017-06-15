;-------------------------------------------------
; Dragon coin door (1 coins)
;-------------------------------------------------

main:
	lda.w	$1420		; * Dragon coin nums
	bne	.set
	jsl	GetCheck
	and.w	$1f2f,y
	bne	.set
	rtl
.set
	jml	SetDoor

