
		opt l

PACTL		equ $0026
PORTA		equ $0000
PORTC		equ $0003
DDRC		equ $0007

		org 0
start:	ldx		#$1000
		bset	DDRC,X #$FF		; configure PORTC as output
		bset	PACTL,X #$80	; configure b7 of PORTA to output

strobe:	ldy		#$FFFF
		bset	PORTC,X #$FF	; strobe PORTC
lit1:	dey
		bne		lit1
		bclr	PORTC,X #$FF
		
		ldy		#$FFFF
		bset	PORTA,X #$80	; strobe b7 of PORTA
lit2	dey
		bne		lit2
		bclr	PORTA,X #$80
		
		bra		strobe

;		org 0
;		fcb $11,$22,$33,$44,$55,$66,$77,$88,$99,$00,$AA,$BB,$CC,$DD,$EE,$FF 
		
;		org $20
;		fcb $11,$22,$33,$44,$55,$66,$77,$88,$99,$00 ;,$AA,$BB,$CC,$DD,$EE,$FF
		
