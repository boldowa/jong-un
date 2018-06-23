;-------------------------------------------------
; This is library code for module test.
;   : : : :
;   LunarMagician: lda.b #$00
;What: db $00
;-------------------------------------------------

!ItemIndex	= $13be
!TransLevNum	= $13bf

; 16x tile store library -------------------------
Store0to6E:
	sta.b	[$6b],y
	lda.b	#$00
	sta.b	[$6e],y
	rtl

Store1to6E:
	sta.b	[$6b],y
	lda.b	#$01
	sta.b	[$6e],y
	rtl

StoreXto6E:
	sta.b	[$6b],y
	xba
	sta.b	[$6e],y
	xba
	rtl

; Tile position library -------------------------
PreservePointer:
	lda.b	$6b
	sta.b	$04
	lda.b	$6c
	sta.b	$05
	rtl

ReloadPointer:
	lda.b	$04
	sta.b	$6b
	sta.b	$6e
	lda.b	$05
	sta.b	$6c
	sta.b	$6f
	lda.w	$1928
	sta.w	$1ba1
	rtl

SetTileXAndMoveR:
	sta.b	[$6b],y
	xba
	sta.b	[$6e],y
	xba
RightSet:
	iny
	tya
	and.b	#$0f
	bne	+
	lda.b	$6b
	clc
	adc.b	#$b0
	sta.b	$6b
	sta.b	$6e
	lda.b	$6c
	adc.b	#$01
	sta.b	$6c
	sta.b	$6f
	inc	$1ba1
	lda.b	$57
	and.b	#$f0
	tay
+	rtl

DownSet:
	lda.b	$57
	clc
	adc.b	#$10
	sta.b	$57
	tay
	bcc	+
	lda.b	$6c
	adc.b	#$00
	sta.b	$6c
	sta.b	$6f
	sta.b	$05
+	rtl

; Put tilemap library ---------------------------

; --- Put Tilemaps lib
;  This is the function for puts out 16x tile.
;  (this function likes ExecutePtr)
;
;  Usage:
;    jsl  PutTileMaps
;    db   <xsize>
;    db   <ysize>
;    db   <16x page number>
;    db   ... 16x tiles ...
;
;  Example: 2x2 cement block
;        jsl  .sub_put
;        rtl
;    .sub_put
;        jsl  PutTileMaps
;        db   $02		; x = 2
;        db   $02		; y = 2
;        db   $01		; dp = 0
;        db   $30, $30
;        db   $30, $30
;
;  and also refer "objects/CementWall.asm"
;

!XSize		= $06
!YSize		= !XSize+1
!OffsetStack	= $08
PutTileMaps:
	; Get tile address from stack
	pla
	sta.b	$00
	rep	#$20
	pla
	sta.b	$01

	; Get size
	ldy.b	#$01
	lda.b	[$00],y
	sta.b	!XSize
	iny
	iny

	; Get 16x bank
	sep	#$20
	lda.b	[$00],y
	xba
	iny
	tya
	ldy.b	#$00
	clc
	adc.b	$00
	sta.b	$00
	bcc	Put16x
	inc	$01

PresettedPutTileMaps:
	lda.b	$57
	sta.b	!OffsetStack

	; Puts tilemap data
Put16x:
	ldx.b	!XSize
	lda.b	$57
	sta.b	!OffsetStack
-	lda.b	[$00],y
	iny
	phy
	ldy.b	!OffsetStack
	jsl	StoreXto6E
	jsl	RightSet
	sty.b	!OffsetStack
	ply
	dex
	bne	-

	phy
	jsl	DownSet
	ply
	dec	!YSize
	bne	Put16x
	rtl


; Item index library ----------------------------
IndexCheck:
	ldx.w	!ItemIndex
	lda.b	#$f8
	clc
	adc.l	$0da8ae,x	; 00 80 00
	sta.b	$08
	lda.b	#$19
	adc.l	$0da8b1,x	; 00 00 01
	sta.b	$09
	lda.w	$1ba1
	asl
	asl
	sta.b	$0e
	lda.b	$0a
	and.b	#$10
	beq	+
	lda.b	$0e
	ora.b	#$02
	sta.b	$0e

+	ldy.b	$57
	tya
	and.b	#$08
	beq	+
	lda.b	$0e
	ora.b	#$01
	sta.b	$0e

+	tya
	and.b	#$07
	tax
	ldy.b	$0e
	lda.b	[$08],y
	and.l	SMW_IndexToBit,x	; 80 40 20 10 08 04 02 01
	rtl

GetCheck:
	phx
	lda.w	!TransLevNum
	pha
	and.b	#$07
	tax
	pla
	lsr
	lsr
	lsr
	tay
	lda.l	SMW_IndexToBit,x
	plx
	rtl

