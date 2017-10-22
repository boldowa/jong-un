@include
;-------------------------------------------------
; macros.asm
;-------------------------------------------------

; RPG Hacker: This should really be an automatic
; include, but UNKO doesn't support this yet
incsrc ../libraries/sa1def.asm

;--- if you enabled it, Mario will die when you set invalid parameter.
!NotPlayableOnError = 1

macro JumpToSmwObj(ptr)
	pea	$a414-1		; "rts" resturn dest in bank $0d
	jml	$0d<ptr>
endmacro

macro OnError()
	if !NotPlayableOnError != 0
		lda.b	#$16
		sta.w	$1dfc|!Base2
		jml	SMW_KillMario
	else
		lda.b	#$2a
		sta.w	$1dfc|!Base2
		ldy.b	$57
		lda.b	#$2f		; put muncher
		jml	Store0to6E
	endif
endmacro

macro LoadPointer(dp, adr)
	lda.b	#<adr>>>16
	sta.b	<dp>+2
	lda.b	#<adr>>>8
	sta.b	<dp>+1
	lda.b	#<adr>
	sta.b	<dp>
endmacro

