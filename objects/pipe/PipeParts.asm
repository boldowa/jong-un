;---------------------------------------------------------------------
; Scroll pipe parts
; -----------------
;
;   === TYPE ===
;     ---------- Green pipe ----------
;     --- 2x2 (corner) ---
;     00 : Green pipe's upper left corner (outer highlight)
;     01 : Green pipe's upper left corner (inner highlight)
;     02 : Green pipe's upper right corner (outer highlight)
;     03 : Green pipe's upper right corner (inner highlight)
;     04 : Green pipe's lower left corner (outer highlight)
;     05 : Green pipe's lower left corner (inner highlight)
;     06 : Green pipe's lower right corner (outer highlight)
;     07 : Green pipe's lower right corner (inner highlight)
;
;     --- 2x2 (injection 1) ---
;     08 : Green pipe's injection (upper left highlight)
;     09 : Green pipe's injection (upper right highlight)
;     0A : Green pipe's injection (lower left highlight)
;     0B : Green pipe's injection (lower right highlight)
;
;     --- 2x2 (injection 2) ---
;     0C : Green pipe's vertical pipe injection (left highlight)
;     0D : Green pipe's vertical pipe injection (right highlight)
;     0E : Green pipe's horizontal pipe injection (upper highlight)
;     0F : Green pipe's horizontal pipe injection (lower highlight)
;
;     --- 2x2 (Vertical exit) ---
;     10 : Vertical green pipe's upper side exit (left highlight)
;     11 : Vertical green pipe's upper side exit (right highlight)
;     12 : Vertical green pipe's lower side exit (left highlight)
;     13 : Vertical green pipe's lower side exit (right highlight)
;
;     --- 2x2 (Horizontal exit) ---
;     14 : Horizontal green pipe's left side exit (upper highlight)
;     15 : Horizontal green pipe's left side exit (lower highlight)
;     16 : Horizontal green pipe's right side exit (upper highlight)
;     17 : Horizontal green pipe's right side exit (lower highlight)
;
;     --- 2x2 or 2x1 (entrance) ---
;     18 : Vertical green pipe's upper side entrance (left highlight)
;     19 : Vertical green pipe's upper side entrance (right highlight)
;     1A : Vertical green pipe's lower side entrance (left highlight *2x1)
;     1B : Vertical green pipe's lower side entrance (right highlight *2x1)
;
;     --- 1x2 (entrance) ---
;     1C : Horizontal green pipe's left side entrance (upper highlight)
;     1D : Horizontal green pipe's left side entrance (lower highlight)
;     1E : Horizontal green pipe's right side entrance (upper highlight)
;     1F : Horizontal green pipe's right side entrance (lower highlight)
;
;     ---------- Blue pipe ----------
;     --- 2x2 (corner) ---
;     20 : Blue pipe's upper left corner (outer highlight)
;     21 : Blue pipe's upper left corner (inner highlight)
;     22 : Blue pipe's upper right corner (outer highlight)
;     23 : Blue pipe's upper right corner (inner highlight)
;     24 : Blue pipe's lower left corner (outer highlight)
;     25 : Blue pipe's lower left corner (inner highlight)
;     26 : Blue pipe's lower right corner (outer highlight)
;     27 : Blue pipe's lower right corner (inner highlight)
;
;     --- 2x2 (injection 1) ---
;     28 : Blue pipe's injection (upper left highlight)
;     29 : Blue pipe's injection (upper right highlight)
;     2A : Blue pipe's injection (lower left highlight)
;     2B : Blue pipe's injection (lower right highlight)
;
;     --- 2x2 (injection 2) ---
;     2C : Blue pipe's vertical pipe injection (left highlight)
;     2D : Blue pipe's vertical pipe injection (right highlight)
;     2E : Blue pipe's horizontal pipe injection (upper highlight)
;     2F : Blue pipe's horizontal pipe injection (lower highlight)
;
;     --- 2x2 (Vertical entrance/exit) ---
;     30 : Vertical green pipe's upper side exit (left highlight)
;     31 : Vertical green pipe's upper side exit (right highlight)
;     32 : Vertical green pipe's lower side exit (left highlight)
;     33 : Vertical green pipe's lower side exit (right highlight)
;
;     --- 2x2 (Horizontal entrance/exit) ---
;     34 : Horizontal green pipe's left side exit (upper highlight)
;     35 : Horizontal green pipe's left side exit (lower highlight)
;     36 : Horizontal green pipe's right side exit (upper highlight)
;     37 : Horizontal green pipe's right side exit (lower highlight)
;---------------------------------------------------------------------

;=== Configs =====================================

!Map16Page		= $04

;***************************************
; Object type ram config
; if you insert to Normal object, 
; set it to $59.
;***************************************
!TypeRAM		= $58

!XSizeRAM		= $06
!YSizeRAM		= !XSizeRAM+1

;=== Macro =======================================
incsrc	"../macros.asm"

;=== Asserts =====================================
; It checks that FuSoYa's SMB3 Scrolling pipes.

; Movement code check
!Insadr = read3($00cd8c)
assert	!Insadr != $00ceb1
assert	read3(!Insadr+1) == $00ceb1



;=== Main code ===================================

main:
	ldy.b #$00
	; Get Map16Page
	lda.b	#!Map16Page
	xba

	; Get object type
	ldx.b	!TypeRAM
	; check valid
	cpx.b	#$38
	bcc	+
	%OnError()

	; GreenPipe
+	cpx.b	#$20
	bcs	.bmap
	cpx.b	#$1a
	bcs	.gmap2
	%LoadPointer($00, GreenPipeTileMap)
	txa
	bra	.putmodebranch
.gmap2
	%LoadPointer($00, GreenPipeTileMap2)
	txa
	bra	.putmodebranch

	; Blue pipe
.bmap
	%LoadPointer($00, BluePipeTileMap)
	txa
	sec
	sbc	#$20
	tax

.putmodebranch
	cmp.b	#$1a
	bcc	Mode2x2
	jmp	Mode2Tiles

Mode2x2:
	asl
	asl
	clc
	adc.b	$00
	sta.b	$00
	bcc	+
	inc	$01

	; Set XY size
+	lda.b	#2
	sta.b	!XSizeRAM
	sta.b	!YSizeRAM
	jml	PresettedPutTileMaps

Mode2Tiles:
	sec
	sbc	#$1a
	asl
	clc
	adc.b	$00
	sta.b	$00
	bcc	+
	inc	$01

	; Set XY size
+	cpx.b	#$1c
	bcc	+
	; 1x2
	lda.b	#1
	sta.b	!XSizeRAM
	lda.b	#2
	sta.b	!YSizeRAM
	bra	++

	; 2x1
+	lda.b	#2
	sta.b	!XSizeRAM
	lda.b	#1
	sta.b	!YSizeRAM
++	jml	PresettedPutTileMaps


;=== TileMap Datas ===============================
GreenPipeTileMap:	; 00 - 15
	; upper left corner
	db	$04, $ed, $8e, $08	; 00
	db	$26, $64, $ac, $28	; 01
	; upper right corner
	db	$ed, $05, $09, $ad	; 02
	db	$64, $27, $29, $8f	; 03
	; lower left corner
	db	$8e, $18, $14, $74	; 04
	db	$ac, $38, $36, $fd	; 05
	; lower right corner
	db	$19, $ad, $74, $15	; 06
	db	$39, $8f, $fd, $37	; 07
	; injection 1
	db	$39, $d9, $aa, $08	; 08
	db	$da, $38, $09, $a9	; 09
	db	$8a, $18, $29, $c9	; 0A
	db	$19, $b9, $ca, $28	; 0B
	; injection 2
	db	$0a, $0b, $0a, $0b	; 0C
	db	$1a, $1b, $1a, $1b	; 0D
	db	$2a, $2a, $3a, $3a	; 0E
	db	$2b, $2b, $3b, $3b	; 0F
	; vertical exit
	db	$6e, $6e, $7e, $7f	; 10
	db	$6e, $6e, $6c, $6d	; 11
	db	$ce, $cf, $de, $de	; 12
	db	$bc, $bd, $de, $de	; 13
	; horizontal exit
	db	$e8, $e9, $e9, $f9	; 14
	db	$e8, $63, $e8, $73	; 15
	db	$ee, $ef, $fe, $ef	; 16
	db	$85, $ef, $95, $ef	; 17
	; vertical entrance
	db	$1e, $1e, $ae, $af	; 18
	db	$1e, $1e, $9c, $9d	; 19

GreenPipeTileMap2:	; 1A - 1F
	; vertical entrance
	db	$9e, $9f	; 1A
	db	$8c, $8d	; 1B
	; horizontal entrance
	db	$ec, $fc 	; 1C
	db	$83, $93 	; 1D
	db	$eb, $fb 	; 1E
	db	$65, $75	; 1F

BluePipeTileMap:	; 20 - 37
	; upper left corner
	db	$00, $a3, $d3, $02	; 20
	db	$20, $c5, $b5, $22	; 21
	; upper right corner
	db	$a3, $01, $03, $a6	; 22
	db	$c5, $21, $23, $c4	; 23
	; lower left corner
	db	$d3, $12, $10, $d6	; 24
	db	$b5, $32, $30, $b4	; 25
	; lower right corner
	db	$a5, $32, $30, $f5	; 26
	db	$33, $c4, $b4, $31	; 27
	; injection 1
	db	$33, $d7, $a8, $02	; 28
	db	$d8, $32, $03, $a7	; 29
	db	$b8, $12, $23, $c7	; 2A
	db	$13, $b7, $c8, $22	; 2B
	; injection 2
	db	$0c, $0d, $0c, $0d	; 2C
	db	$0e, $0f, $0e, $0f	; 2D
	db	$1c, $1c, $2c, $2c	; 2E
	db	$1d, $1d, $2d, $2d	; 2F
	; vertical exit
	db	$2e, $2f, $3e, $3f	; 30
	db	$3c, $3d, $b5, $b6	; 31
	db	$3e, $3f, $4e, $4f	; 32
	db	$4c, $4d, $5c, $5d	; 33
	; horizontal exit
	db	$e4, $e5, $f4, $f5	; 34
	db	$43, $44, $53, $54	; 35
	db	$a3, $e6, $b3, $f6	; 36
	db	$44, $45, $54, $55	; 37

