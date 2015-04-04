
;	opt l,c


;=== equates =================================================
; MCU registers
PORTD		equ	$0008
DDRD		equ	$0009
BAUD		equ $002B
SCCR1		equ	$002C
SCCR2		equ	$002D
SCSR		equ	$002E
SCDR		equ	$002F


;=== Interrupt vectors =======================================
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


;=== monitor initialization =============================
	org $0100
init:
		stx		regbase
		lds		#monitor_stack


;=== main routine =======================================
main:
		; get command from PC
		jsr		recv_packet
		; jump to command routine
		ldy		#jmp_table
		ldab	_packet_cmd
		lslb
		aby
		ldy		0,Y
		jsr		0,Y
		; get next command
		jmp		main				

max_cmd			equ	6
jmp_table:
		fdb		cmd_nop
		fdb		cmd_get_data
		fdb		cmd_set_data
		fdb		cmd_block_fill
		fdb		cmd_get_state
		fdb		cmd_set_state
		fdb		cmd_exec_code
		fdb		cmd_err


;=== get packet routine ================================
recv_packet:
		psha
		pshb
		pshx
		pshy
		
		; get packet header
		bset	SCCR2,X #$01		; send break and wait for start bit
		brset	PORTD,X #$01 *
		bclr	SCCR2,X #$01 
_recv0:	ldy		#_packet_id			; first find 'MCP' in incoming byte stream
_recv1:	brclr	SCSR,X #$20 *
		ldaa	SCDR,X
		staa	SCDR,X
		cmpa	0,Y
		bne		_recv0
		iny
		cpy		#_packet_id+3
		bne		_recv1

_recv2:	brclr	SCSR,X #$20 *		; then get rest of packet header
		ldaa	SCDR,X
		staa	0,Y
		iny
		cpy		#_packet_data
		bne		_recv2

		; get packet data
		ldd		#_packet_data
		addd	_packet_size
		std		tmp1
		ldy		#_packet_data
_recv3:	cpy		tmp1
		beq		_recvd
		brclr	SCSR,X #$20 *
		ldaa	SCDR,X
		staa	0,Y
		iny
		bra		_recv3

		; check packet for errors
_recvd:	jsr		do_csum
		;cpy		packet_hdr+6

		; exit with a valid command
		ldaa	#max_cmd
		cmpa	_packet_cmd
		bgt		_exit1
		staa	_packet_cmd
		
_exit1:	puly
		pulx
		pulb
		pula
		rts


;=== send packet routine =====================================
send_packet:
		psha
		pshb
		pshx
		pshy
		
		; send packet header
		ldy		#packet_hdr
_send0:	ldaa	0,Y
		cpy		#packet_hdr+8
		beq		_send1
		brclr	SCSR,X #$80 *
		staa	SCDR,X
		iny
		bra		_send0

		; send packet data
_send1:	ldd		#$100
		addd	_packet_size
		std		tmp1
		ldy		#$100
_send2:	ldaa	0,Y
		cpy		tmp1
		beq		_exit2
		brclr	SCSR,X #$80 *
		staa	SCDR,X
		iny
		bra		_send2

_exit2:	puly
		pulx
		pulb
		pula
		rts


;=== checksum routine =========================================
; Calculate checksum on packet in packet buffer.
; Return checksum in Y.
do_csum:
		rts


;=== reset routine ============================================
reset:
		ldx		regbase
		lds		#monitor_stack
		jmp		main


;=== SPI initialization =======================================
spi_init:
		bclr	SCCR2,X #$FF		; disable everything
		ldaa	#$30				; 9600bps
		staa	BAUD,X
		bclr	SCCR1,X #$18		; 1 start, 8 data, 1 stop,
		bset	SCCR2,X #$0C		; enable receiver and transmitter
		rts


;=== command routines =========================================
;=== cmd_nop ===
cmd_nop:
		ldaa	#$FF
		jsr		debug_ac
		rts


;=== cmd_get_data ===
cmd_get_data:
		ldaa	#$01
		jsr		debug_ac

		; send packet
		ldd		#$100
		std		_packet_size
		jsr		send_packet

		rts


;=== cmd_set_data ===
cmd_set_data:
		ldaa	#$02
		jsr		debug_ac
		rts


;=== cmd_block_fill ===
cmd_block_fill:
		ldaa	#$03
		jsr		debug_ac
		rts


;=== cmd_get_state ===
cmd_get_state:
		ldaa	#$04
		jsr		debug_ac
		rts


;=== cmd_set_state ===
cmd_set_state:
		ldaa	#$05
		jsr		debug_ac
		rts


;=== cmd_exec_code ===
cmd_exec_code:
		ldaa	#$06
		jsr		debug_ac
		rts


;=== cmd_err ===
cmd_err:
		ldaa	#$FE
		jsr		debug_ac
		rts


;=== debug routines ===========================================
; Output one to bit 7 on PORTA and hang.
PORTA		equ	$0000
PACTL		equ	$0026
SPCR		equ	$0028
debug_t:
		bset	PACTL,X #$80
		bset	PORTA,X #$80
		bra		*
; Output one to bit 6 on PORTA and hang.
debug_f:
		bset	PORTA,X #$40
		bra		*
; Output AccA onto PORTA[7:3] and PORTD[5:3] and hang.
debug_ah:
		jsr		debug_a
		bra		*
; Output AccA onto PORTA[7:3] and PORTD[5:3] and return.
debug_ac:
		jsr		debug_a
		rts
; Output AccA onto PORTA[7:3] and PORTD[5:3].
debug_a:
		bset	PACTL,X #$80
		staa	PORTA,X
		bclr	SPCR,X #$60
		bset	DDRD,X #$3C
		lsla
		lsla
		lsla
		anda	#$38
		staa	PORTD,X
		rts


;=== monitor data area ========================================
regbase:		rmb	2				; register base
rambase:		rmb 2				; internal RAM base

token:			rmb 1				; communication token

; saved MCU state
mcu_state:
_mcu_AccA:		rmb 1
_mcu_AccB:		rmb	1
_mcu_X:			rmb	2
_mcu_Y:			rmb	2
_mcu_SP:		rmb	2
_mcu_PC:		rmb	2
_mcu_flags:		rmb	1

; packet buffer
packet_hdr:
_packet_id:		fcb	'M,'C,'P		; packet header (8 bytes)
_packet_cmd:	rmb	1
_packet_size:	rmb	2
_packet_csum:	rmb	2
_packet_data:	fcb 'H','A','H','A'
				rmb	256				; packet data (0-256 bytes)

; temp variables
tmp1:			rmb 2

; start of monitor stack.
	org $1FFC
monitor_stack:


;=== end-of-monitor mark ======================================
	org $1FFE
	fcb	$55,$AA
	org $1FFF
end:
