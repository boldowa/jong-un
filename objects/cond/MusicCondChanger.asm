@include
;-------------------------------------------------
; Music condition changer
;
;   XY pos      [$57] ... cond option 1 (X pos)
;   Ext value   [$58] ... music number
;-------------------------------------------------

; RPG Hacker: This should really be an automatic
; include, but UNKO doesn't support this yet
incsrc ../../libraries/sa1def.asm

MusicCondChanger:
	jsr	MusicCondChk
	bcc	+
	lda.b	$58
	sta.w	$0dda|!Base1
+	rts

MusicCondChk:
	tya				;\  Get block X pos from Y-register.
	tax				;/
	jsl	SMW_ExecutePtr

	dw	isCollectDC		; - *0
	dw	isCollect3up		; - *1
	dw	isMidwayPassed		; - *2
	dw	isLifeLastOne		; - *3
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


