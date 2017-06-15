;-------------------------------------------------
; Horizontal scroll pipe body extention
; -------------------------------------
;
; This is a patch that adds a horizontal scroll pipe
; body to object 10.
;
; This object requires FuSoYa's SMB3 Screen
; Scrolling Pipes patch.
;
;
; ***** Size/Type/Ext branch *****
;
;   * X is horizontal length of pipe.
;     if you set it to 0, it'll generate too long pipe.
;
;     0X - 3X     ... SMW original pipes.
;
;     4X          ... Green right scroll pipe body
;                     with upper highlight.
;
;     5X          ... Green right scroll pipe body
;                     with lower highlight.
;
;     6X          ... Green left scroll pipe body
;                     with upper highlight.
;
;     7X          ... Green left scroll pipe body
;                     with lower highlight.
;
;     8X          ... Blue scroll pipe body
;                     with upper highlight.
;
;     9X          ... Blue scroll pipe body
;                     with lower highlight.
;
;     AX - FX     ... Not allocated.
;
;-------------------------------------------------

;=== Configs =====================================

;***************************************
; The installed Map16 page number
; of scroll pipe.
;***************************************
!Map16Page		= $04

;***************************************
; Green pipe body tile map
;***************************************

;--- Right scroll ----------------------
;--- Upper highlight ----
!GrRiUHU		= $ed
!GrRiUHL		= $fd
;--- Lower highlight ----
!GrRiLHU		= $84
!GrRiLHL		= $94

;--- Left scroll -----------------------
;--- Upper highlight ----
!GrLeUHU		= $ea
!GrLeUHL		= $fa
;--- Lower highlight ----
!GrLeLHU		= $64
!GrLeLHL		= $74

;***************************************
; Blue pipe body tile map
;***************************************
;--- Upper highlight ----
!BlUHU			= $e5
!BlUHL			= $f5
;--- Lower highlight ----
!BlLHU			= $44
!BlLHL			= $54


;=== Macros ======================================
incsrc	"../macros.asm"


;=== Asserts =====================================
; It checks that FuSoYa's SMB3 Scrolling pipes.

; Movement code check
!Insadr = read3($00cd8c)
assert	!Insadr != $00ceb1
assert	read3(!Insadr+1) == $00ceb1



;=== Main code ===================================

main:
;--- Initial routine -----------------------------
	; Get XY position index
	ldy.b	$57

	; Get length
	lda.b	$59
	and.b	#$0f
	sta.b	$00
	sta.b	$01

	; Checks smw oroginal pipes.
	lda.b	$59
	cmp.b	#$40
	bcs	+
	%JumpToSmwObj(aac2)

+	cmp.b	#$a0
	bcc		+
	%OnError()

	; Get object type
+	sec
	sbc	#$40
	lsr
	lsr
	lsr
	tax

	; Set 16x tile page
	lda.b	#!Map16Page
	xba

	; Change db register to current bank number.
	phb
	phk
	plb

	jsl	PreservePointer

.SetTile
	lda.w	.TileSets,x
	jsl	SetTileXAndMoveR
	dec	$01
	bpl	.SetTile

.NextRow
	lda.b	$00
	sta.b	$01
	jsl	ReloadPointer
	jsl	DownSet
	inx
	txa
	and.b	#$01
	bne	.SetTile

	; Restore db register
	plb
	rtl

.TileSets
	db	!GrRiUHU, !GrRiUHL
	db	!GrRiLHU, !GrRiLHL
	db	!GrLeUHU, !GrLeUHL
	db	!GrLeLHU, !GrLeLHL
	db	!BlUHU,   !BlUHL
	db	!BlLHU,   !BlLHL

