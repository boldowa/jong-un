;-------------------------------------------------
; Any Condition Executer
; ----------------------
;
; !!! Attention !!!
;   This object doesn't work except for 2DObject.
;   So, you should to insert it 2DObject.
;
;   XY position    [$57] ... cond type / cond option 1
;   XY size        [$59] ... cond option 2
;   Ext value(*)   [$58] ... cond option 3
;   * $58 is enabled in 2DObject only.
;-------------------------------------------------

!DragonCoinCollects	= $1f2f
!3upMoonObteinded	= $1fee

main:
;--------
	phb
	phk
	plb
	jsr	AnyCondExecMain
	plb
	rtl
;--------

AnyCondExecMain:
	lda.b	$57		;\  Get cond option 1 from block X
	and.b	#$0f		; |
	tay			;/
	lda.b	$57		;\  Get cond type from block Y
	lsr	a		; |
	lsr	a		; |
	lsr	a		; |
	lsr	a		; |
	tax			;/
	jsl	SMW_ExecutePtr	;*  branch

	dw	MusicCondChanger	; - 0*
	dw	EmptyHandler		;dw	PaletteCondChanger	; - 1*
	dw	GFXIndexCondChanger	; - 2*
	dw	EmptyHandler		; - 3*
	dw	EmptyHandler		; - 4*
	dw	EmptyHandler		; - 5*
	dw	EmptyHandler		; - 6*
	dw	EmptyHandler		; - 7*
	dw	EmptyHandler		; - 8*
	dw	EmptyHandler		; - 9*
	dw	EmptyHandler		; - A*
	dw	EmptyHandler		; - B*
	dw	EmptyHandler		; - C*
	dw	EmptyHandler		; - D*
	dw	EmptyHandler		; - E*
	dw	EmptyHandler		; - F*

;-----------------------------
isCollectDC:
	jsl	GetCheck
	clc
	and.w	!DragonCoinCollects,y
	beq	+
	sec
+	rts

;-----------------------------
isCollect3up:
	jsl	GetCheck
	clc
	and.w	!3upMoonObteinded,y
	beq	+
	sec
+	rts

;-----------------------------
isLifeLastOne:
	clc
	lda.w	$0dbe
	bne	+
	sec
+	rts

;-----------------------------
isMidwayPassed:
	clc
	ldx.w	$13bf
	lda.w	$1ea2,x
	and.b	#$40
	bne	+
	lda.w	$13ce
	beq	++
+	sec
++	rts

;-----------------------------
isMidwayPassedInv:
	sec
	ldx.w	$13bf
	lda.w	$1ea2,x
	and.b	#$40
	bne	+
	lda.w	$13ce
	beq	++
+	clc
++	rts

;-----------------------------
EmptyHandler:
	clc
	rts


	incsrc	"objects/cond/MusicCondChanger.asm"
	;incsrc	"objects/cond/PaletteCondChanger.asm"		; RPG Hacker: So where exactly is this file?
	incsrc	"objects/cond/GFXIndexCondChanger.asm"
	;incsrc "objects/cond/foo.asm"
	;incsrc "objects/cond/bar.asm"
	;            :



