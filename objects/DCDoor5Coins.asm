;-------------------------------------------------
; Dragon coin door (5 coins)
;-------------------------------------------------

main:
	jsl	GetCheck
	and.w	$1f2f,y
	beq	+
	print	"export SetDoor = $", pc
	ldy.b	$57
	lda.b	#$1f
	jsl	Store0to6E
	jsl	DownSet
	lda.b	#$20
	jml	Store0to6E
+	rtl

