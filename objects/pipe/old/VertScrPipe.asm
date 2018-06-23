;-------------------------------------------------
; Vertical Scroll Pipe
; --------------------
;
; !!! Attention !!!
;   This object can't work in ExtendObjext.
;   So, you should to insert it as the normal object
;   or 2DObject.
;
; ***** Size/Type/Ext branch *****
;
;   * Y is vertical length of pipe.
;     if you set it to 0, it'll generate too long pipe.
;
;     --- Y0 ~ Y6 ... Green pipe ---
;
;        Y0
;       |   |
;       |   |   
;       |   |   
;       |   |   
;
;        Y1          Y2
;      [     ]  |  [-----]  
;       |   |   |   |   |   
;       |   |   |   |   |   
;       |   |   |   |   |   
;    
;        Y3          Y4
;      [     ]  |  [-----]  
;       |   |   |   |   |      
;       |   |   |   |   |       
;      [-----]  |  [     ]
;    
;        Y5          Y6
;       |   |   |   |   |   
;       |   |   |   |   |   
;       |   |   |   |   |   
;      [-----]  |  [     ]  
;    
;     --- Y7 ~ Ya ... Blue pipe ---
;    
;        Y7
;       |   |
;       |   |   
;       |   |   
;       |   |   
;
;        Y8          Y9          Ya
;      [     ]  |  [     ]  |   |   | 
;       |   |   |   |   |   |   |   | 
;       |   |   |   |   |   |   |   | 
;       |   |   |  [     ]  |  [     ]
;
;    --- Yb ~ Yf ... not asigned
;        (Invalid setting)
;
;-------------------------------------------------

!TilePage		= $04

;----- Green pipe tile set
!GrTopTileL		= $7e
!GrTopTileR		= $7f
!GrTopTileLE		= $ae
!GrTopTileRE		= $af

!GrBodyTileL		= $8e
!GrBodyTileR		= $8f
;!GrBodyTileL		= $8e
;!GrBodyTileR		= $8f

!GrBottomTileL		= $9e
!GrBottomTileR		= $9f
!GrBottomTileLE		= $6d
!GrBottomTileRE		= $6d


;----- Blue pipe tile set
!BlTopTileL		= $2e
!BlTopTileR		= $2f

!BlBodyTileL		= $3e
!BlBodyTileR		= $3f

!BlBottomTileL		= $4e
!BlBottomTileR		= $4f

;--- system define
!NULL			= $00

;--- if you enabled it, Mario will die when you set invalid parameter.
!NotPlayableInInvalid = 1


;----- macro
macro SetTop()
	lda.w	.TopLeft,x
	jsl	SetTileXAndMoveR
	lda.w	.TopRight,x
	jsl	StoreXto6E
endmacro

macro SetBody(left, right)
	lda.b	#<left>
	jsl	SetTileXAndMoveR
	lda.b	#<right>
	jsl	StoreXto6E
endmacro

macro SetBottom()
	lda.w	.BottomLeft,x
	jsl	SetTileXAndMoveR
	lda.w	.BottomRight,x
	jsl	StoreXto6E
endmacro


;----- Call the main code
main:
	phb
	phk
	plb
	jsr	VerScrPipeMain
	plb
	rtl


VerScrPipeMain:
;--- Initial routine -----------------------------
	; Get position index
	ldy.b	$57

	; Set 16x tile page
	lda.b	#!TilePage
	xba

	; Get length
	lda.b	$59
	lsr
	lsr
	lsr
	lsr
	;inc	a
	sta.b	$00

	; Get object type
	lda.b	$59
	and.b	#$0f
	tax

	jsl	PreservePointer

if !NotPlayableInInvalid == 1
	cpx.b	#$0b
	bcc		+
	;--- Invalid settings
	jsl	SMW_KillMario
	lda.b	#$16
	sta.w	$1dfc
	rts
+
endif

	cpx.b	#$07
	bpl	BluePipe

;--- Green pipe routine --------------------------
GreenPipe:
.top
	txa
	beq	.body
	dex
	cpx.b	#$04
	bpl	.body
	%SetTop()
	bra	.nextRow

.body
	%SetBody(!GrBodyTileL, !GrBodyTileR)

.nextRow
	jsl	ReloadPointer
	jsl	DownSet
	cpx.b	#$02
	bpl	.bottom
	dec	$00
	bpl	.body
	rts

.bottom
	dec	$00
	bne	.body
	%SetBottom()
	rts


;----- Data
;                   Down          Up            Down            Up               Down             Up
.TopLeft
	db	!GrTopTileLE, !GrTopTileL, !GrTopTileLE,   !GrTopTileL,     !NULL,          !NULL
.TopRight
	db	!GrTopTileRE, !GrTopTileR, !GrTopTileRE,   !GrTopTileR,     !NULL,          !NULL
.BottomLeft
	db	!NULL,        !NULL,       !GrBottomTileL, !GrBottomTileLE, !GrBottomTileL, !GrBottomTileLE
.BottomRight
	db	!NULL,        !NULL,       !GrBottomTileR, !GrBottomTileRE, !GrBottomTileR, !GrBottomTileRE



;--- Blue pipe routine ---------------------------
BluePipe:
	txa
	sec
	sbc.b	#$07
	tax
.top
	; set top tile
	txa
	beq	.body
	dex
	cpx.b	#$02
	bpl	.body
	%SetTop()
	bra	.nextRow

.body
	%SetBody(!BlBodyTileL, !BlBodyTileR)

.nextRow
	jsl	ReloadPointer
	jsl	DownSet
	cpx.b	#$01
	bpl	.bottom
	dec	$00
	bpl	.body
	rts

.bottom
	dec	$00
	bne	.body
	%SetBottom()
	rts


;----- Data
.TopLeft
	db	!BlTopTileL, !BlTopTileL,    !NULL
.TopRight
	db	!BlTopTileR, !BlTopTileR,    !NULL
.BottomLeft
	db	!NULL,       !BlBottomTileL, !BlBottomTileL
.BottomRight
	db	!NULL,       !BlBottomTileR, !BlBottomTileR

