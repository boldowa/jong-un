;-------------------------------------------------
; 2x4 cement object
;-------------------------------------------------
	print	"+---- Wall Maria ----+"
	print	"|                    |"
	print	"|                    |"
	print	"|                    |"
	print	"|         c          |"
	print	"|                    |"
	print	"|                    |"
	print	"|                    |"
	print	"+--------------------+"

;--- Set object property for LM
	print	"Visible = true"		; not invisible
	print	"XSize = 2"			; 2 blocks
	print	"YSize = 5"			; 5 blocks
	print	"HorzElongationAmount = 0"	; 0 blocks (can't expand in X)
	print	"VertElongationAmount = 0"	; 0 blocks (can't expand in Y)

main:
	jsl	PutTileMaps	; this code perfome like jml
; TilemapSize
	db	$02	; X size
	db	$05	; Y size
; Map16x page
	db	$01
; TileMaps ---
	db	$30,$30
	db	$30,$30
	db	$30,$30
	db	$30,$30
	db	$00,$00

