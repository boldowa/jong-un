;-------------------------------------------------
; RomMap Detect test
;-------------------------------------------------

; make sure to fail
assert 0

    if 1 == !map
	print	"This rom is LoROM"
elseif 2 == !map
	print	"This rom is HiROM"
elseif 3 == !map
	print	"This rom is ExLoROM"
elseif 4 == !map
	print	"This rom is ExHiROM"
elseif 5 == !map
	print	"This rom is SA-1ROM"
else
	print	"Unknown ROM map"
endif

main:

