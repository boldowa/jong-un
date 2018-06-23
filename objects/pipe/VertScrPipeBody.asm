;-------------------------------------------------
; Vertical scroll pipe body extention
; -----------------------------------
;
; This is a patch that adds a vertical scroll pipe
; body to object 0F.
;
; This object requires FuSoYa's SMB3 Screen
; Scrolling Pipes patch.
;
;
; ***** Size/Type/Ext branch *****
;
;   * Y is vertical length of pipe.
;     if you set it to 0, it'll generate too long pipe.
;
;     Y0 - Y5     ... SMW original pipes.
;
;     Y6          ... Green up scroll pipe body
;                     with left highlight.
;
;     Y7          ... Green up scroll pipe body
;                     with right highlight.
;
;     Y8          ... Green down scroll pipe body
;                     with left highlight.
;
;     Y9          ... Green down scroll pipe body
;                     with right highlight.
;
;     YA          ... Blue scroll pipe body
;                     with left highlight.
;
;     YB          ... Blue scroll pipe body
;                     with right highlight.
;
;     YC - YF     ... Not allocated.
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

;--- Up scroll -------------------------
;--- Left highlight -----
!GrUpLHL		= $8e
!GrUpLHR		= $8f
;--- Right highlight ----
!GrUpRHL		= $7c
!GrUpRHR		= $7d

;--- Down scroll -----------------------
;--- Left highlight -----
!GrDwLHL		= $be
!GrDwLHR		= $bf
;--- Right highlight ----
!GrDwRHL		= $ac
!GrDwRHR		= $ad

;***************************************
; Blue pipe body tile map
;***************************************
;--- Left highlight -----
!BlLHL			= $3e
!BlLHR			= $3f
;--- Right highlight ----
!BlRHL			= $4c
!BlRHR			= $4d


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
	lsr
	lsr
	lsr
	lsr
	sta.b	$00

	; Get object type
	lda.b	$59
	and.b	#$0f
	tax

	; Checks smw oroginal pipes.
	cpx.b	#$06
	bcs	+
	%JumpToSmwObj(aa35)

+	cpx.b	#$0c
	bcc		+
	%OnError()

+	txa
	sec
	sbc	#$06
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
	lda.w	.LeftTileSets,x
	jsl	SetTileXAndMoveR

	lda.w	.RightTileSets,x
	jsl	StoreXto6E
.NextRow
	jsl	ReloadPointer
	jsl	DownSet
	dec	$00
	bpl	.SetTile

	; Restore db register
	plb
	rtl

.LeftTileSets
	db	!GrUpLHL, !GrUpRHL, !GrDwLHL, !GrDwRHL, !BlLHL, !BlRHL
.RightTileSets
	db	!GrUpLHR, !GrUpRHR, !GrDwLHR, !GrDwRHR, !BlLHR, !BlRHR

