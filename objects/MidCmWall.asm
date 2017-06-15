;-------------------------------------------------
; mid-way cement object
;-------------------------------------------------
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
	ldx.w	$13bf
	lda.w	$1ea2,x
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

