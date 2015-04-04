
; Test program that sends pulses to PORTA. Assumes that X points to register
; base and that PORTA pins are all outputs.


PORTA		equ		0


	org $0040
start:	ldaa	#$55
		staa	PORTA,X
		jsr		delay
		
		ldaa	#$AA
		staa	PORTA,X
		jsr		delay

		bra		start


delay:	ldy		#$FFFF
_dly0:	dey
		bne		_dly0
		rts
