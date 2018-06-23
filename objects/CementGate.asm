;-------------------------------------------------
; Cement gate
;-------------------------------------------------

; RPG Hacker: This should really be an automatic
; include, but UNKO doesn't support this yet
incsrc ../libraries/sa1def.asm

!ConditionRAM	= $0dbe|!Base1

main:
	lda	!ConditionRAM
	beq	+
	ldy.b	$57
	lda.b	#$30
	jml	Store1to6E
+	rtl

