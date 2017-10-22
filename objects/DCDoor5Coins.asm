;-------------------------------------------------
; Dragon coin door (5 coins)
;-------------------------------------------------

; RPG Hacker: This should really be an automatic
; include, but UNKO doesn't support this yet
incsrc ../libraries/sa1def.asm

main:
	jsl	GetCheck
	and.w	$1f2f|!Base2,y
	beq	+
	print	"export SetDoor = $", pc
	ldy.b	$57
	lda.b	#$1f
	jsl	Store0to6E
	jsl	DownSet
	lda.b	#$20
	jml	Store0to6E
+	rtl

