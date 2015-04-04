
; Setup program for the 68HC11A1 microcontroller.
; version: 0.1
;
; 	description:
; The MCU must be placed into bootstrap mode. Then this program is downloaded
; to the MCU and placed at address $0000-$00FF. It *must not* be larger than
; 256 bytes.

PORTD		equ		$0008
TMSK2		equ		$0024
BAUD		equ		$002B
SCCR1		equ		$002C
SCCR2		equ		$002D
SCSR		equ		$002E
SCDR		equ		$002F
OPTION		equ		$0039
PPROG		equ		$003B
HPRIO		equ		$003C
INIT		equ		$003D
CONFIG		equ		$003F

talker_s	equ		$0100		; start offset of talker in page
talker_sz	equ		$1EFF		; talker size
talker_e	equ		$1FFF		; end offset of talker


;		opt l,c

		org $0000
start:	lds		#$00F0
		ldx		#$1000
		
		; enable/disable on-chip ROM and EEPROM
		; <<< this code is disabled until a way to reboot the MCU is found >>>
;		jsr		dly10			; allow charge pump to stabilize
;		ldaa	CONFIG,X
;		ora		#$03
;		ldab	options3
;		cba
;		beq		next0
;		ldaa	#$06		; erase CONFIG register
;		staa	PPROG,X
;		staa	CONFIG,X
;		inca
;		staa	PPROG,X
;		jsr		dly10
;		ldaa	#$02		; program CONFIG register
;		staa	PPROG,X
;		stab	CONFIG,X
;		inca
;		staa	PPROG,X
;		jsr		dly10
;		clr		PPROG,X		; turn off charge pump
		; <<< reboot somehow >>>

		; enable/disable bootstrap ROM
next0:	brset	options3 #$02 next1
		bclr	HPRIO,X #$80
		
		; set IRQ to edge or level sensitivity
next1:	brclr	options2 #$01 next2
		bset	OPTION,X #$20
		
		; enable/disable oscillator startup delay
next2:	brset	options2 #$02 next3
		bclr	OPTION,X #$10
		
		; set the COP timer rate
next3:	ldaa	OPTION,X
		ora		options0
		staa	OPTION,X
		
		; set prescaler rate for free-running timer
		ldaa	TMSK2,X
		ora		options1
		staa	TMSK2,X
		
		; Note this ugly hack! We must have the register-remap code at address
		; $40 or greater, so that we can remap regs to the same page as RAM.
		; Best thing would be to insert some useful code in here instead.
		; Also, we *must* remap registers (and everyting else remappable)
		; before changing MCU mode. MCU mode should be the last environmental
		; change, after that the upload of int vectors and monitor, and after
		; that a jump to monitor. Do *not* alter this flow of execution.
		jmp		$40
		org	$0040
		; remap RAM and MCU registers
		; do it when area at 0-$40 is no longer used and monitor is
		; not yet downloaded, just in case
		ldaa	options4
		staa	INIT,X
		; point X to registers
		ldaa	options4
		lsla
		lsla
		lsla
		lsla
		clrb
		xgdx

		; put the MCU into desired mode. Do it befor uploading anything more.
		brclr	options5 #$01 s_chip
		bset	HPRIO,X #$20
s_chip:	brset	options5 #$02 smod
		bclr	HPRIO,X #$40
smod:

		; change baud rate to 9600bps
		ldaa	#$30
		staa	BAUD,X
		
		; set up interrupt vectors at $FFC0-$FFFF
		; MUST SWITCH TO EXPANDED MODE FIRST, ST00PID!
		ldy		#$FFC0
		bset	SCCR2,X #$01	; send break
		brset	PORTD,X #$01 *	; wait for start bit
		bclr	SCCR2,X #$01	; clear break
i_recv:	brclr	SCSR,X #$20 *	; wait for byte to arrive
		ldaa	SCDR,X			; read byte
		staa	0,Y				; store byte
		staa	SCDR,X			; echo byte
		iny
		bne		i_recv

		; download talker
		ldaa	options5		; Y = talker addr
		anda	#$F0
		clrb
		addd	#talker_e
		std		talker_end
		subd	#talker_sz
		xgdy

		bset	SCCR2,X #$01	; send break
		brset	PORTD,X #$01 *
		bclr	SCCR2,X #$01	; clear break
t_recv:	brclr	SCSR,X #$20 *
		ldaa	SCDR,X			; read byte
		staa	0,Y
		staa	SCDR,X			; echo byte
		iny
		cpy		talker_end
		bne		t_recv

		; pass control to the talker
		jmp		talker

; Subroutine to delay for 10ms (if E=2Mhz)
dly10:	pshx
		ldx		#$0D06			; 3334*6*500ns = 10ms
dloop:	dex
		bne		dloop
		pulx
		rts

; Debug routine. Outputs ones on PORTA. Connect a LED to pin 7 of PORTA and
; call this routine. If the LED doesn't lit up, the code is never executed,
; which means it crashes somewhere before. Place the call further and further
; back to pinpoint the point of failure.
PORTA	equ		#$0000
PACTL	equ		#$0026
;PORTC	equ		#$0003
;DDRC	equ		#$0007

debug:	bset	PACTL,X #$80
		bset	PORTA,X #$80
		bra		*
		

talker_end:		fcb 0,0


; User selected options. Theese are setup by the Control program before this
; program is downloaded to the MCU. Use several option bytes to avoid shifting
; and ANDing operations in the setup code. Saves time and memory.
		org $FA

; b0	MDA bit (0=single chip mode, 1=expanded mode)
; b1	SMOD bit (0=normal mode, 1=special mode)
; b2:3	reserved
; b4:7	talker page
options5:	fcb 0

; b0:3	register page
; b4:7	on-chip RAM page
options4:	fcb 0

; b0:	on-chip EEPROM at $B600-$B7FF (0=disable, 1=enable)
; b1:	on-chip bootstrap ROM at $E000-$FFFF ??? (0=disable, 1=enable)
; b2:7	reserved
options3:	fcb 0

; b0:	IRQ pin operation (0=level sensitive, 1=edge sensitive)
; b1:	oscillator delay (0=disable, 1=4000 E-clock delay after STOP start-up)
; b2:7	reserved
options2:	fcb 0

; b0:	timer prescaler select (PR0)
; b1:	timer prescaler select (PR1)
; b2:7	reserved
options1:	fcb 0

; b0:	COP timer rate select (CR0)
; b1:	COP timer rate select (CR1)
; b2:7	reserved
options0:	fcb 0



; Downloaded talker code starts here
		org $0100
talker:
