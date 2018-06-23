@include
;-------------------------------------------------
; GFX Index condition changer
;
;   XY pos      [$57] ... cond option 1 (X pos)
;
;   XY size     [$59] ... Not used
;
;   Ext value   [$58] ... GFX Index
;                           (%ffffssss)
;                           ... ffff = FG/BG
;                           ... ssss = Sprite
;-------------------------------------------------

; RPG Hacker: This should really be an automatic
; include, but UNKO doesn't support this yet
incsrc ../../libraries/sa1def.asm

GFXIndexCondChanger:
	jsr	GFXIndexCondChk
	bcc	+
	lda.b	$58		;\
	and.b	#$0f		; | Set sprite tile set
	sta.w	$192b|!Base2		;/
	lda.b	$58		;\
	lsr	#4		; | Set FG/BG tile set
	sta.w	$1931|!Base2		; |
	sta.w	$1932|!Base2		;/
	phk			;\
	per	$0006		; | Call graphic load routine
	pea	$8124		; |
	jml	$0581fb		;/
+	rts

GFXIndexCondChk:
	tya				;\  Get block X pos from Y-register.
	tax				;/
	jsl	SMW_ExecutePtr

	dw	isMidwayPassed		; - *0
	dw	isMidwayPassedInv	; - *1
	dw	isLifeLastOne		; - *2
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


