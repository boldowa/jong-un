@include
;-------------------------------------------------
; Palette condition changer
;
;   XY pos      [$57] ... cond option 1 (X pos)
;
;   XY size     [$59] ... Palette config 1
;                           (%ffffssss)
;                           ... ffff = FG
;                           ... ssss = Sprite
;
;   Ext value   [$58] ... Palette config 2
;                           (%ccccbbbb)
;                           ... cccc = Background Color
;                           ... bbbb = BG
;-------------------------------------------------

; RPG Hacker: This should really be an automatic
; include, but UNKO doesn't support this yet
incsrc ../../libraries/sa1def.asm

PaletteCondChanger:
	jsr	PaletteCondChk
	bcc	+
	lda.b	$59		;\
	lsr	a		; |
	lsr	a		; | FG
	lsr	a		; |
	lsr	a		; |
	sta.w	$192d|!Base2		;/
	lda.b	$59		;\
	and.b	#$0f		; | Sprite
	sta.w	$192e|!Base2		;/
	lda.b	$58		;\
	lsr	a		; |
	lsr	a		; | Background Color
	lsr	a		; |
	lsr	a		; |
	sta.w	$192f|!Base2		;/
	lda.b	$58		;\
	and.b	#$0f		; | BG
	sta.w	$1930|!Base2		;/
+	rts

PaletteCondChk:
	tya				;\  Get block X pos from Y-register.
	tax				;/
	jsl	SMW_ExecutePtr

	dw	isMidwayPassed		; - *0
	dw	isLifeLastOne		; - *1
	dw	EmptyHandler		; - *2
	dw	EmptyHandler		; - *3
	dw	EmptyHandler		; - *4
	dw	EmptyHandler		; - *5
	dw	EmptyHandler		; - *6
	dw	EmptyHandler		; - *7
	dw	EmptyHandler		; - *8
	dw	EmptyHandler		; - *9
	dw	EmptyHandler		; - *A
	dw	EmptyHandler		; - *B
	dw	EmptyHandler		; - *C
	dw	EmptyHandler		; - *D
	dw	EmptyHandler		; - *E
	dw	EmptyHandler		; - *F


