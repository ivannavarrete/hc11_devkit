
;	opt l,c

; The X index register must always point at register base address. Do not
; change it.
; The monitor must start at offset 100 from a page, so we can have monitor and
; registers (and RAM, when RAM is less than 256 bytes) at the same page.


;*******************************************************************************
;*** equates *******************************************************************
;*******************************************************************************
; MCU registers (offsets from a page)
PORTD				equ	$0008
DDRD				equ	$0009
BAUD				equ $002B
SCCR1				equ	$002C
SCCR2				equ	$002D
SCSR				equ	$002E
SCDR				equ	$002F
CONFIG				equ	$003F

; packet variables (must match packet struct in monitor.h)
packet_hdr_size:	equ	8
_packet_id:			equ 0 
_packet_cmd:		equ	3
_packet_size:		equ	4
_packet_csum:		equ	6

; packet commands (must be same as in monitor.h)
cmd_hw_ok:			equ	98
cmd_hw_err:			equ	99
; command modifiers (must be sabe as in command.h)
cmd_bp_set:			equ	3
cmd_bp_clear:		equ	4


;*******************************************************************************
;*** monitor code **************************************************************
;*******************************************************************************
; this is needed to properly parse the file
	org $E000
	zmb	$FF


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

		; don't change the jump table order; it follows command.h
max_cmd			equ	7
jmp_table:
		fdb		cmd_nop
		fdb		cmd_get_data
		fdb		cmd_set_data
		fdb		cmd_get_state
		fdb		cmd_set_state
		fdb		cmd_exec
		fdb		cmd_bp
		fdb		cmd_set_code
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
_recv1:	brclr	SCSR,X #$20 *			; get packet header
		ldaa	SCDR,X
		staa	0,Y
		iny
		cpy		#m_packet+#packet_hdr_size
		bne		_recv1

		ldd		m_packet+_packet_size	; get packet data
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

_csum:	ldaa	#cmd_hw_ok				; determine if packet is ok
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
;	Y					packet header pointer
;	packet_data_ptr		packet data pointer
;===============================================================================
send_packet:
		psha
		pshb
		pshy

		; insert csum into packet
		jsr		csum
		std		_packet_csum,Y

		pshy
		pula
		pulb
		addd	#packet_hdr_size
		std		packet_hdr_e		; D points to packet header end
		sty		packet_hdr_s		; Y points to packet header start

		; send packet header
_send0:	ldaa	0,Y
		brclr	SCSR,X #$80 *
		staa	SCDR,X
		iny
		cpy		packet_hdr_e
		bne		_send0

		; send packet data
		ldy		packet_hdr_s
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

		; save and zero old csum
		ldx		_packet_csum,Y
		pshx
		ldd		#$0000
		std		_packet_csum,Y

		; Y=last byte-2, D=0
		dey
		sty		packet_hdr_s
		ldd		packet_hdr_s
		addd	#packet_hdr_size-1
		addd	_packet_size+1,Y
		xgdy
		clra
		clrb

		; calculate csum
_csum0:	addd	0,Y
		dey
		cpy		packet_hdr_s
		bne		_csum0
		iny
		
		; restore old csum
		pulx
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

		; create packet
		ldd		packet_data+4			; calculate packet size
		bne		_l10
		ldd		#$20
		bra		_l20
_l10:	subd	packet_data+2
_l20:	std		m_packet+_packet_size	; put size into main packet
		ldd		packet_data+2
		std		packet_data_ptr			; set up data pointer

		; send packet
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


;=== cmd_get_state =============================================================
cmd_get_state:
		; not implemented
		ldaa	#$03
		jsr		debug_ac
		rts


;=== cmd_set_state =============================================================
cmd_set_state:
		; not implemented
		ldaa	#$04
		jsr		debug_ac
		rts


;=== cmd_exec ==================================================================
cmd_exec:
		ldaa	#$05
		jsr		debug_ac

		ldd		packet_data+2			; again ntohs() should be at PC
		stab	packet_data+2			; get start addr
		staa	packet_data+3
		ldy		packet_data+2

		jmp		0,Y						; jump to code
		
		rts


;=== cmd_bp ====================================================================
cmd_bp:
		ldaa	#$06
		jsr		debug_ac

		ldaa	packet_data+1			; get subcommand (set/clear)
		cmpa	cmd_bp_set
		beq		_bp_set

										; clear breakpoint

_bp_set:								; set breakpoint

		rts


;=== cmd_set_code ==============================================================
cmd_set_code:
		; not implemented
		; isn't even necessary since it's done by cmd_set_data and logic
		; at PC side
		ldaa	#$07
		jsr		debug_ac

		rts


;=== cmd_err ===================================================================
cmd_err:
		ldaa	#$FE
		jsr		debug_ac
		rts


;*******************************************************************************
;*** breakpoint code ***********************************************************
;*******************************************************************************
breakpoint:
		jmp		init


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


;=== def_int ===================================================================
def_int:
		rti


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

m_packet:			fcb	'M,'C,'P, 0, 0,0, 0,0	; main packet
c_packet:			fcb	'M,'C,'P, 0, 0,0, 0,0	; confirmation packet

packet_data:		rmb	256			; packet data buffer
packet_data_ptr:	rmb 2			; pointer to packet data

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
res1:				fdb	def_int
res2:				fdb	def_int
res3:				fdb	def_int
res4:				fdb	def_int
res5:				fdb	def_int
res6:				fdb	def_int
res7:				fdb	def_int
res8:				fdb def_int
res9:				fdb def_int
res10:				fdb def_int
res11:				fdb def_int
v_sci:				fdb	def_int
v_spi_done:			fdb	def_int
v_pa_inedge:		fdb	def_int
v_pa_owrflow:		fdb	def_int
v_t_ovrflow:		fdb	def_int
v_tic4_tok5:		fdb	def_int
v_tok4:				fdb	def_int
v_tok3:				fdb	def_int
v_tok2:				fdb	def_int
v_tok1:				fdb	def_int
v_tic3:				fdb	def_int
v_tic2:				fdb	def_int
v_tic1:				fdb	def_int
v_rti:				fdb	def_int
v_irq:				fdb	def_int
v_xirq:				fdb	def_int
v_softint:			fdb	breakpoint
v_ill_opcode:		fdb	def_int
v_cop_fail:			fdb	def_int
v_clk_fail:			fdb	def_int
v_reset:			fdb	def_int

end:
