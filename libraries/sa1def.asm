;SA-1 defines

if read1($00FFD5) == $23		; check if the rom is sa-1
	sa1rom
	!SA1 = 1
	!SA_1 = 1
	!Base1 = $3000
	!Base2 = $6000
	
	!BankA = $400000
	!BankB = $000000
else
	lorom
	!SA1 = 0
	!SA_1 = 0
	!Base1 = $0000
	!Base2 = $0000
	
	!BankA = $7E0000
	!BankB = $800000
endif