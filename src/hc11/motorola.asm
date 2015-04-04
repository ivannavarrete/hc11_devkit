
; motorola.asm
;
; MCU: 68HC11A1
; mode: single chip
;
; This program is used to brute force the security code on a motorola 3210
; phone. This is done by receiving a keyspace block from the PC and then
; stepping through each key, generating the corresponding pulses on the
; appropriate ports.
;
; The security code on the 3210 is up to 10 digits. The total number of
; keys is then:
;
;	10 + 10^2 + 10^3 + ... 10^10 = 1.111111111E10
;
; This is about 11 billion. By testing 10 keys per second it would take
; roughly 35 years to test every key. This makes it important to choose
; appropriate keyspace blocks.
;
; There is nothing sacred with the 3210. It could be replaced by any phone,
; or pretty much anything else for that matter. All this program does is
; generates pulses on ports. Got nothin to do with phones whatsoever.
;
; Btw: Don't bother telling me that you could build a cable and run wintesla
; to remove the code. I already know. (Though I admit I didn't knew this when
; I first started this project.)

	opt	l

PORTA		equ	$0000
PACTL		equ	$0026
PORTB		equ	$0004

; Since we have 11 buttons (and one diode), we use two ports.
; PORTB
BTN_0		equ	$01
BTN_1		equ	$02
BTN_2		equ	$04
BTN_3		equ	$08
BTN_4		equ	$10
BTN_5		equ	$20
BTN_6		equ	$40
BTN_7		equ	$80
; PORTA
BTN_8		equ	$08
BTN_9		equ $10
BTN_ENTER	equ	$20
DIODE		equ	$40



	org $0000
start:
	jsr		init
	jsr		get_key_block
	jsr		brute_force
	jsr		end


;=== init ======================================================================
; Init SCI and ports.
;===============================================================================
init:
	;ldx	#$0000			; X points to registers
	
	rts


;=== get_key_block =============================================================
; Get a keyspace block from the PC.
;===============================================================================
get_key_block:

	rts


;=== brute_force ===============================================================
; Test all keys in the keyspace block.
;===============================================================================
brute_force:
	jsr		next_key
	jsr		send_key

	rts


;=== next_key ==================================================================
; 
;===============================================================================
next_key:

	rts


;=== send_key ==================================================================
;
;===============================================================================
send_key:
	ldaa	key_size
	ldy		#key

_next_digit:
	; get a digit from key
	ldab	0,Y
	; convert to appropriate pulse
	; depending on digit, we send a pulse to either PORTA or PORTB
	pshx
	ldx		#btn_1
	abx							; if AccB is > 9 then we're screwed
	cmpb	7
	bgt		_porta
_portb:
	ldab	0,X
	pulx
	stab	PORTB,X				; press button
	bra		_dly
_porta:
	ldab	0,X
	pulx
	stab	PORTA,X				; press button
_dly:
	jsr		delay
	clrb
	stab	PORTA,X				; release button
	stab	PORTB,X

	iny							; point to next digit
	deca						; one more digit done
	bne		_next_digit
	
	; press the 'enter' button
	ldaa	BTN_ENTER
	staa	PORTA,X
	jsr		delay
	clr		PORTA,X
	
	rts


;=== delay =====================================================================
; Delay for a moment before releasing a button.
;===============================================================================
delay:
	pshx
	ldx		#$FFFF
_delay0:
	dex
	bne		_delay0
	pulx
	rts


;=== end =======================================================================
; Turn on a diode and hang.
;===============================================================================
end:
	clra
	staa	PORTA,X
	staa	PORTB,X
	bset	PORTA,X DIODE
	bra		*


;===============================================================================
key_size:		fcb	1
key:			fcb 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 

btn_1:			fcb	BTN_1
btn_2:			fcb	BTN_2
btn_3:			fcb	BTN_3
btn_4:			fcb	BTN_4
btn_5:			fcb	BTN_5
btn_6:			fcb	BTN_6
btn_7:			fcb	BTN_7
btn_8:			fcb	BTN_8
btn_9:			fcb	BTN_9
btn_0:			fcb	BTN_0 
