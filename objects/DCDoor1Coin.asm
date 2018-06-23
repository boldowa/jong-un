;-------------------------------------------------
; Dragon coin door (1 coins)
;-------------------------------------------------

; RPG Hacker: This should really be an automatic
; include, but UNKO doesn't support this yet
incsrc ../libraries/sa1def.asm

main:
	lda.w	$1420|!Base2		; * Dragon coin nums
	bne	.set
	jsl	GetCheck
	and.w	$1f2f|!Base2,y
	bne	.set
	rtl
.set
	jml	SetDoor

