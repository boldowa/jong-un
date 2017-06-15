;-------------------------------------------------
; Scroll pipe extention
; ---------------------
;
; This is a patch that adds vertical and horizontal
; scroll pipe body to object 0F and 10.
;
; This patch requires FuSoYa's SMB3 Screen
; Scrolling Pipes patch.
;
;-------------------------------------------------

;=== Configs =====================================

; If you enable it, this patch works uninstall mode.
!Uninstall		= 0

;--- if you enabled it, Mario will die when you set invalid parameter.
!NotPlayableOnError	= 1

;=== Asserts =====================================
; It checks that FuSoYa's SMB3 Scrolling pipes.

if !Uninstall == 0
	; Movement code check
	!Insadr = read3($00cd8c)
	assert	!Insadr != $00ceb1
	assert	read3(!Insadr+1) == $00ceb1
endif


;=== Macros ======================================

; Jong-un install check
!ExecutePtrL		= $0086fa
macro ExecutePtrChk(v)
	!<v> = 0
	!x = read3($0da452)
	if !x != !ExecutePtrL
		!<v> = 1
	endif
	if !x != read3($0dc197)
		!<v> = 1
	endif
	if !x != read3($0dcd97)
		!<v> = 1
	endif
	if !x != read3($0dd997)
		!<v> = 1
	endif
	if !x != read3($0de897)
		!<v> = 1
	endif
endmacro

macro JumpToSmwObj(ptr)
	pea	$a414-1		; "rts" resturn dest in bank $0d
	jml	$0d<ptr>
endmacro

macro OnError()
	if !NotPlayableOnError != 0
		lda.b	#$16
		sta.w	$1dfc
		jml	!KillMario
	else
		lda.b	#$2a
		sta.w	$1dfc
		ldy.b	$57
		lda.b	#$2f
		jsr	Store1to6E
		rtl
	endif
endmacro

macro ObjectHijack(from, to, clean)
	org <from>
	if read3(<from>) < $108000 && <clean> == 0
		dl	<to>
	else
		autoclean dl <to>
	endif
endmacro

macro RestoreAddress(from, to, clean)
	org <from>
	if <clean> == 0
		dl	<to>
	else
		autoclean read3(<from>)
		dl	<to>
	endif
endmacro

;=== Common defines ==============================
!KillMario		= $00f606

!NormalObj		= $0da452
!CastleObj		= $0dc197
!RopeObj		= $0dcd97
!UndergroundObj		= $0dd997
!GhostHouseObj		= $0de897

!VertPipeDef		= $0daa26
!HorzPipeDef		= $0daab4

;--- obj $0f
!NormalObj0F		= $0f*3+!NormalObj
!CastleObj0F		= $0f*3+!CastleObj
!RopeObj0F		= $0f*3+!RopeObj
!UndergroundObj0F	= $0f*3+!UndergroundObj
!GhostHouseObj0F	= $0f*3+!GhostHouseObj

;--- obj $10
!NormalObj10		= $10*3+!NormalObj
!CastleObj10		= $10*3+!CastleObj
!RopeObj10		= $10*3+!RopeObj
!UndergroundObj10	= $10*3+!UndergroundObj
!GhostHouseObj10	= $10*3+!GhostHouseObj


;=== Main code ===================================

if !Uninstall == 0
	print	"[Install mode]"

	%ExecutePtrChk(isJongunInstalled)

	%ObjectHijack(!NormalObj0F, VertPipeExtMain, 1)
	%ObjectHijack(!CastleObj0F, VertPipeExtMain, 0)
	%ObjectHijack(!RopeObj0F, VertPipeExtMain, 0)
	%ObjectHijack(!UndergroundObj0F, VertPipeExtMain, 0)
	%ObjectHijack(!GhostHouseObj0F, VertPipeExtMain, 0)

	%ObjectHijack(!NormalObj10, HorzPipeExtMain, 0)
	%ObjectHijack(!CastleObj10, HorzPipeExtMain, 0)
	%ObjectHijack(!RopeObj10, HorzPipeExtMain, 0)
	%ObjectHijack(!UndergroundObj10, HorzPipeExtMain, 0)
	%ObjectHijack(!GhostHouseObj10, HorzPipeExtMain, 0)

	freecode

	if !isJongunInstalled == 0
		; Jong-un isn't installed
		pla		;\   Remove JSR return address from stack
		pla		;/
	endif

	incsrc	"pipes/VertScrPipeBody.asm"
	incsrc	"pipes/HorzScrPipeBody.asm"

	;-------------------------------------------------
	; Local libraries
	;-------------------------------------------------
	PreservePointer:
		lda.b	$6b
		sta.b	$04
		lda.b	$6c
		sta.b	$05
		rts
	
	SetTileXAndMoveR:
		sta.b	[$6b],y
		xba
		sta.b	[$6e],y
		xba
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
	+	rts

	Store1to6E:
		sta.b	[$6b],y
		lda.b	#$01
		sta.b	[$6e],y
		rts
	
	StoreXto6E:
		sta.b	[$6b],y
		xba
		sta.b	[$6e],y
		xba
		rts
	
	NextRow:
		;----------
		; ReloadPointer
		lda.b	$04
		sta.b	$6b
		sta.b	$6e
		lda.b	$05
		sta.b	$6c
		sta.b	$6f
		lda.w	$1928
		sta.w	$1ba1
		;----------
		; DownSet
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
	+	rts
	
else
	print	"[Uninstall mode]"

	%RestoreAddress(!NormalObj0F, !VertPipeDef, 1)
	%RestoreAddress(!CastleObj0F, !VertPipeDef, 0)
	%RestoreAddress(!RopeObj0F, !VertPipeDef, 0)
	%RestoreAddress(!UndergroundObj0F, !VertPipeDef, 0)
	%RestoreAddress(!GhostHouseObj0F, !VertPipeDef, 0)

	%RestoreAddress(!NormalObj10, !HorzPipeDef, 0)
	%RestoreAddress(!CastleObj10, !HorzPipeDef, 0)
	%RestoreAddress(!RopeObj10, !HorzPipeDef, 0)
	%RestoreAddress(!UndergroundObj10, !HorzPipeDef, 0)
	%RestoreAddress(!GhostHouseObj10, !HorzPipeDef, 0)

endif
