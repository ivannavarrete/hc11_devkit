
;	opt l,c

PORTC		equ	$0003
DDRC		equ	$0007

	org $0000
vec1:		fcb	$11,$11
vec2:		fcb	00,00
vec3:		fcb	00,00
vec4:		fcb	00,00
vec5:		fcb	00,00
vec6:		fcb	00,00
vec7:		fcb	00,00
vec8:		fcb	00,00
vec9:		fcb	00,00
vec10:		fcb	00,00
vec11:		fcb	00,00
vec12:		fcb	00,00
vec13:		fcb	00,00
vec14:		fcb	00,00
vec15:		fcb	00,00
vec16:		fcb	00,00
vec17:		fcb	00,00
vec18:		fcb	00,00
vec19:		fcb	00,00
vec20:		fcb	00,00
vec21:		fcb	00,00
vec22:		fcb	00,00
vec23:		fcb	00,00
vec24:		fcb	00,00
vec25:		fcb	00,00
vec26:		fcb	00,00
vec27:		fcb	00,00
vec28:		fcb	00,00
vec29:		fcb	00,00
vec30:		fcb	00,00
vec31:		fcb	00,00
vec32:		fcb	$FF,$FF


	org $040
monitor:
	ldx		regaddr
	bset	DDRC,X #$FF
	bset	PORTC,X #$FF
	bra		*


regaddr:	fcb $10, $00

	org $1F3E
	fcb	$55,$AA
	org $1F40
end:
