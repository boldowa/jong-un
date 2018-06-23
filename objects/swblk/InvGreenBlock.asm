;-------------------------------------------------
; Invert Green Block
;-------------------------------------------------

; RPG Hacker: This should really be an automatic
; include, but UNKO doesn't support this yet
incsrc ../../libraries/sa1def.asm

main:
	ldx.b	#$00

; label export
; It'll export "InvSwitch" label.
print "export InvSwitch = $", pc

	ldy.b	#00
	lda.w	$1f27|!Base2,x
	bne	+
	iny
+	tya
	xba
	lda.l	.SwitchTable,x
	ldy.b	$57
	sta.b	[$6b],y
	xba
	sta.b	[$6e],y
	rtl

.SwitchTable
	db	$6a, $6b, $6c, $6d

