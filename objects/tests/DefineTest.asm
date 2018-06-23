;-------------------------------------------------
; RomMap Detect test
; ------------------
;
; Test source for use -D command from unko.
;
;-------------------------------------------------

; make sure to fail
assert 0

main:
if !Def == 0
	print "[EQU] !Def is 0."
else
	print "[NEQ] !Def is not 0."
endif

