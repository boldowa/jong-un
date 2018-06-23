;-------------------------------------------------
; SMW object
;
;   XY pos      [$57] ... object pos
;
;   XY size     [$59] ... SMW object size
;
;   Ext value   [$58] ... SMW object number
;-------------------------------------------------

; RPG Hacker: This should really be an automatic
; include, but UNKO doesn't support this yet
incsrc ../libraries/sa1def.asm

main:
	ldx.w	$13bf|!Base2
	lda.w	$1ea2|!Base2,x
	and.b	#$40
	bne	+
	lda.w	$13ce|!Base2
	beq	++
+	rtl
++	jml	PutSmwObject

