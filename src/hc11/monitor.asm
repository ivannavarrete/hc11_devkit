
;	opt l,c

; The X index register must always point at register base address. Do not
; change it.

;=== equates ===================================================================
; MCU registers
PORTD		equ	$0008
DDRD		equ	$0009
BAUD		equ $002B
SCCR1		equ	$002C
SCCR2		equ	$002D
SCSR		equ	$002E
SCDR		equ	$002F
CONFIG		equ	$003F


; this is needed to properly parse the file
	org $E000
	zmb	$FF


; The monitor must start at offset 100 from a page, so we can have monitor and
; registers (and RAM, when RAM is less than 256 bytes) at the same page.
	org $E100
;=== monitor initialization ====================================================
init:
		stx		regbase
		lds		#mon_stack
		jsr		sci_init

;=== main routine ==============================================================
main:
		; get command from PC
		ldd		#packet_data
		std		packet_data_ptr
		jsr		recv_packet

		; jump to command routine
		ldy		#jmp_table
		ldab	m_packet+_packet_cmd
		lslb
		aby
		ldy		0,Y
		jsr		0,Y
		; get next command
		jmp		main				

		; don't change the jump table order
max_cmd			equ	6
jmp_table:
		fdb		cmd_nop
		fdb		cmd_get_data
		fdb		cmd_set_data
		fdb		cmd_block_fill
		fdb		cmd_get_state
		fdb		cmd_set_state
		fdb		cmd_exec
		fdb		cmd_err


;=== recv_packet ===============================================================
; input:
;	packet_data_ptr		points to where to store packet data
; output:
;	m_packet			received packet header
;	packet_data_ptr		packet data stored at pointer location
;===============================================================================
recv_packet:
		psha
		pshb
		pshy
		
_recv0:	ldy		#m_packet
_recv1:	brclr	SCSR,X #$20 *		; get packet header
		ldaa	SCDR,X
		staa	0,Y
		iny
		cpy		#m_packet+#packet_hdr_size
		bne		_recv1

		ldd		m_packet+_packet_size		; get packet data
		beq		_csum
		ldy		packet_data_ptr
_recv2:	std		packet_size_tmp
		brclr	SCSR,X #$20 *
		ldaa	SCDR,X
		staa	0,Y
		iny
		ldd		packet_size_tmp
		subd	#$0001
		bne		_recv2

_csum:	ldaa	#cmd_hw_ok			; determine if packet is ok
		staa	c_packet+_packet_cmd
		ldy		#m_packet
		jsr		csum
		cpd		m_packet+_packet_csum
		beq		_reply
		ldaa	#cmd_hw_err
		staa	c_packet+_packet_cmd

_reply:	ldy		#c_packet
		jsr		send_packet			; send a response to PC based on csum

		cmpa	cmd_hw_err			; if error, try again
		beq		_recv0

		puly
		pulb
		pula
		rts


;=== send_packet ===============================================================
; input:
;	Y		packet pointer
;===============================================================================
send_packet:
		psha
		pshb
		pshy

		jsr		csum				; insert csum into packet
		std		_packet_csum,Y

		pshy
		pula
		pulb
		addd	#packet_hdr_size
		std		packet_hdr_e		; D points to packet header end
		sty		packet_hdr_s		; Y points to packet header start

_send0:	ldaa	0,Y					; send packet header
		brclr	SCSR,X #$80 *
		staa	SCDR,X
		iny
		cpy		packet_hdr_e
		bne		_send0

		ldy		packet_hdr_s		; send packet data
		ldd		_packet_size,Y
		beq		_done				; if data size = 0, then send nothing more
		ldy		packet_data_ptr		; command routines set up packet_data_ptr
_send1:	std		packet_size_tmp
		ldaa	0,Y
		brclr	SCSR,X #$80 *
		staa	SCDR,X				; send one byte
		iny
		ldd		packet_size_tmp
		subd	#$0001
		bne		_send1

_done:	puly
		pulb
		pula
		rts


;=== csum ======================================================================
; Calculate checksum on packet in packet buffer. Old csum is counted as zero.
;
;	short csum = 0;
;	for (i=packet_hdr+packet_hdr_size+*(packet_size)-2; i>=packet_hdr; i--)
;		csum += packet_hdr[i];
;
; input:
;	Y		packet pointer
; output:
;	D		packet checksum
;===============================================================================
csum:
		pshx

		ldx		_packet_csum,Y		; save and zero old csum
		pshx
		ldd		#$0000
		std		_packet_csum,Y

		dey
		sty		packet_hdr_s		; Y=last byte - 2, D=0
		ldd		packet_hdr_s
		addd	#packet_hdr_size-1
		addd	_packet_size+1,Y
		xgdy
		clra
		clrb

_csum0:	addd	0,Y					; calculate csum
		dey
		cpy		packet_hdr_s
		bne		_csum0
		iny
		
		pulx						; restore old csum
		stx		_packet_csum,Y

		pulx
		rts


;=== sci_init ==================================================================
sci_init:
		psha

		clr		SCCR2,X				; disable everything
		ldaa	#$30				; 9600bps
		staa	BAUD,X
		clr		SCCR1,X				; 1 start, 8 data, 1 stop,
		bset	SCCR2,X #$0C		; enable receiver and transmitter

		ldaa	SCSR,X				; clear status
		ldaa	SCDR,X
		
		pula
		rts


;=== dly =======================================================================
dly:	pshx
		ldx		#$FFFF
_dly0:	dex
		bne		_dly0
		pulx
		rts
	

;*******************************************************************************
;*** command routines **********************************************************
;*******************************************************************************
;=== cmd_nop ===================================================================
cmd_nop:
		ldaa	#$FF
		jsr		debug_ac
		rts


;=== cmd_get_data ==============================================================
cmd_get_data:
		ldaa	#$01
		jsr		debug_ac

		; parse packet data
		ldd		packet_data+2			; ntohs() should be done at PC side
		stab	packet_data+2			; addr1
		staa	packet_data+3
		
		ldd		packet_data+4
		stab	packet_data+4			; addr2
		staa	packet_data+5

		ldd		packet_data+4			; calculate packet size
		bne		_l10
		ldd		#$20
		bra		_l20
_l10:	subd	packet_data+2
_l20:	std		m_packet+_packet_size	; put size into main packet
		ldd		packet_data+2
		std		packet_data_ptr			; set up data pointer

		ldy		#m_packet
		jsr		send_packet

		rts


;=== cmd_set_data ==============================================================
cmd_set_data:
		ldaa	#$02
		jsr		debug_ac

		ldd		packet_data+2			; ntohs() should be done at PC side
		stab	packet_data+2			; addr1
		staa	packet_data+3

		ldd		packet_data+2
		std		packet_data_ptr

		jsr		recv_packet
		
		rts


;=== cmd_block_fill ============================================================
cmd_block_fill:
		ldaa	#$03
		jsr		debug_ac
		rts


;=== cmd_get_state =============================================================
cmd_get_state:
		ldaa	#$04
		jsr		debug_ac
		rts


;=== cmd_set_state =============================================================
cmd_set_state:
		ldaa	#$05
		jsr		debug_ac
		rts


;=== cmd_exec ==================================================================
cmd_exec:
		ldaa	#$06
		jsr		debug_ac

		ldd		packet_data+2
		stab	packet_data+2
		staa	packet_data+3
		ldy		packet_data+2

		jmp		0,Y
		
		rts


;=== cmd_err ===================================================================
cmd_err:
		ldaa	#$FE
		jsr		debug_ac
		rts


;*******************************************************************************
;*** debug routines ************************************************************
;*******************************************************************************
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


;*******************************************************************************
;*** monitor data area *********************************************************
;*******************************************************************************
regbase:			rmb	2			; register base
rambase:			rmb 2			; on-chip RAM base

; saved MCU state
mcu_state:
_mcu_AccA:			rmb 1
_mcu_AccB:			rmb	1
_mcu_X:				rmb	2
_mcu_Y:				rmb	2
_mcu_SP:			rmb	2
_mcu_PC:			rmb	2
_mcu_flags:			rmb	1

; packet variables
packet_hdr_size:	equ	8
_packet_id:			equ 0 
_packet_cmd:		equ	3
_packet_size:		equ	4
_packet_csum:		equ	6

m_packet:			fcb	'M,'C,'P, 0, 0,0, 0,0	; main packet
c_packet:			fcb	'M,'C,'P, 0, 0,0, 0,0	; confirmation packet

packet_data:		rmb	256			; packet data buffer
packet_data_ptr:	rmb 2			; pointer to packet data

; packet commands
cmd_hw_ok:			equ	98
cmd_hw_err:			equ	99


; temp variables
packet_size_tmp:	rmb 2
packet_csum_tmp:	rmb	2
packet_hdr_s:		rmb	2
packet_hdr_e:		rmb	2

; start of monitor stack.
	org $FFBE
mon_stack:


;*******************************************************************************
;*** interrupt vectors *********************************************************
;*******************************************************************************
	org $FFC0
res1:				fcb	$11,$11
res2:				fcb	00,00
res3:				fcb	00,00
res4:				fcb	00,00
res5:				fcb	00,00
res6:				fcb	00,00
res7:				fcb	00,00
res8:				fcb 00,00
res9:				fcb 00,00
res10:				fcb 00,00
res11:				fcb 00,00
v_sci:				fcb	00,00
v_spi_done:			fcb	00,00
v_pa_inedge:		fcb	00,00
v_pa_owrflow:		fcb	00,00
v_t_ovrflow:		fcb	00,00
v_tic4_tok5:		fcb	00,00
v_tok4:				fcb	00,00
v_tok3:				fcb	00,00
v_tok2:				fcb	00,00
v_tok1:				fcb	00,00
v_tic3:				fcb	00,00
v_tic2:				fcb	00,00
v_tic1:				fcb	00,00
v_rti:				fcb	00,00
v_irq:				fcb	00,00
v_xirq:				fcb	00,00
v_softint:			fcb	00,00
v_ill_opcode:		fcb	00,00
v_cop_fail:			fcb	00,00
v_clk_fail:			fcb	00,00
v_reset:			fcb	$FF,$FF

end:
