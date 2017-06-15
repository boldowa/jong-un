;-------------------------------------------------
; SMW object
;
;   XY pos      [$57] ... object pos
;
;   XY size     [$59] ... SMW object size
;
;   Ext value   [$58] ... SMW object number
;-------------------------------------------------

main:
	ldx.w	$13bf
	lda.w	$1ea2,x
	and.b	#$40
	bne	+
	lda.w	$13ce
	beq	++
+	rtl
++	jml	PutSmwObject

