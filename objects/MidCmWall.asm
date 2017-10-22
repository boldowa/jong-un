;-------------------------------------------------
; mid-way cement object
;-------------------------------------------------

; RPG Hacker: This should really be an automatic
; include, but UNKO doesn't support this yet
incsrc ../libraries/sa1def.asm

	print	"+---- Wall Rose  ----+"
	print	"|                    |"
	print	"|                    |"
	print	"|                    |"
	print	"|         :3         |"
	print	"|                    |"
	print	"|                    |"
	print	"|                    |"
	print	"+--------------------+"

main:
	ldx.w	$13bf|!Base2
	lda.w	$1ea2|!Base2,x
	and.b	#$40
	beq	+
	rtl
+	jsl	PutTileMaps	; this code perfome like jml
; TilemapSize
	db	$02	; X size
	db	$0F	; Y size
; Map16x page
	db	$01
; TileMaps ---
	db	$30,$30
	db	$30,$30
	db	$30,$30
	db	$30,$30
	db	$30,$30
	db	$30,$30
	db	$30,$30
	db	$30,$30
	db	$30,$30
	db	$30,$30
	db	$30,$30
	db	$30,$30
	db	$30,$30
	db	$30,$30
	db	$00,$00

