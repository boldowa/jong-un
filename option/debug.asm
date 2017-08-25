;-------------------------------------------------
; debug patch
; -----------
;
; Skip [File select] and
;      [1 Player / 2 Player select]
;
;-------------------------------------------------

;=== Configs =====================================

; If you enable it, this patch works uninstall mode.
!Uninstall		= 0

;=== Main code ===================================

if !Uninstall == 0
	print	"[Install mode]"

	org $9cb8
		autoclean jml Hijack

	freecode

	Hijack:
		ldx.b	#$00

		phk
		per	$0006
		pea	$9c1e-1
		jml	$009cef

		stz.w	$0012
		ldx.b	#$00
		jml	$009e0d
	
else
	print	"[Uninstall mode]"

	;-------------------
	; Clean main code
	;-------------------
	autoclean	read3($9cb9)

	;-------------------
	; Restore hijacks
	;-------------------
	org $9cb8
		jsr	$9d38
		jmp.w	$9417
endif

