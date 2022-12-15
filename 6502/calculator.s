; To compile: .\vasm6502_oldstyle.exe -Fbin -dotdir -c02 calculator.s
; To send:		java -jar EEPROM_Programmer.jar

; VIA
PORTB = $c000		; Read/Write Direction for port B
PORTA = $c001		; Read/Write Direction for port A
DD_PORTB = $c002	; Data Direction Register for Port B	
DD_PORTA = $c003	; Data Direction Register for Port A	
PCR = $c00c			; Peripheral Control Register
IFR = $c00d			; Interrupt Flag Register
INT_EN_REG = $c00e	; Interrupt enable register VIA

; Circular buffer for the keyboard
kb_w = $00	; Keyboard write pointer
kb_r = $01	; Keyboard read pointer
kb_flags = $02	; Keyboard flag

; Stores the values of the operands and result (64 bits)
op1 = $03
op2 = op1 + 4
ans = op1 + 8

KB_RELEASE 	= %00000001
KB_SHIFT 	= %00000010

kb_buffer = $0200	; Position of the buffer (256 bytes, 0200 - 02ff)

	.org $e000
program_start:	
	sei
	ldx #$ff
	txs

	lda #0			; Start the keyboard pointers to 0
	sta kb_w
	sta kb_r
	sta kb_flags

	; Set port A as input
	sta DD_PORTA	; Initialize Data direction register A

	lda #1			; Set CA1 to work with positive edge
	sta PCR

	lda #$82		; Set CA1 as interrupt source. Top bit is to enable
	sta INT_EN_REG

	lda DD_PORTB	; Lets find out if the board has been reset or not
	and #%0000111	
	bne start_lcd_4bit	; If 3 last bits are 1, LCD has been started

	; Init the LCD Module
	lda #%0010000	; Start configuration for 4 bit mode (this will get in 8bit mode)
	jsr lcd_instruction_nowait ; Send directly, as it is 8 bit mode.

start_lcd_4bit:
	lda #%00101000	; 4 bit mode, display is 8x5 pixels
	jsr send_lcd_instruction

	lda #%00001111	; Display ON, cursor ON and blinking cursor ON
	jsr send_lcd_instruction

	lda #%00000110	; Increment register by one and shift cursor to the right
	jsr send_lcd_instruction

	lda #%00000001	; Home
	jsr send_lcd_instruction

    lda #$B9
    sta op1
	lda #$4A
	sta op1+1

    lda #$0A
    sta op2
	lda #$41
	sta op2+1
    
    jsr u8_mult
    lda ans
    lda ans+1
	lda ans+2
	lda ans+3

    nop

loop:
	jmp loop

send_lcd_instruction:
	pha
	; Send the first 4 bits of the letter
	and #$F0		; Fetch the first 4 bits
	ror				; Rotate A one to the right

	jsr lcd_instruction

	pla
	; Send the next 4 bits of the letter
	and #$0F		; Fetch the last 4 bits
	rol				; Rotate A three to the left
	rol
	rol

	jsr lcd_instruction_nowait
	rts

send_lcd:
	pha
	; Send the first 4 bits of the letter
	and #$F0		; Fetch the first 4 bits
	ror				; Rotate A one to the right

	ora #$01		; Add the 1 from RS signal
	jsr lcd_instruction

	pla
	; Send the next 4 bits of the letter
	and #$0F		; Fetch the last 4 bits
	rol				; Rotate A three to the left
	rol
	rol

	ora #$01		; Add the 1 from RS signal
	jsr lcd_instruction_nowait
	rts

lcd_instruction:
	jsr lcd_wait
lcd_instruction_nowait:
	pha
	lda #%1111111	; Set bottom 7 pins as output
	sta DD_PORTB	; Initialize Data direction register B
	pla
	sta PORTB
	ora #%00000100	; Set ENABLE high
	sta PORTB
	and #%11111011	; Set ENABLE low
	sta PORTB
	rts

lcd_wait:	;  Wait for busy flag = 0
	pha
	phx
	lda #%0000111	; Set 4 data pins as inputs, but maintain RS,RW,E as outputs
	sta DD_PORTB	; Initialize Data direction register B
lcd_read_flag:
	lda #%0000010	; Fetch the busy flag (ENABLE low)
	sta PORTB
	lda #%0000110	; Set ENABLE high
	sta PORTB
	ldx PORTB		; BF x x x x x x -> X
	lda #%0000010	;  Set ENABLE low
	sta PORTB

	lda #%0000010	; Second mandatory read (because 4 bit mode)
	sta PORTB
	lda #%0000110	; Set ENABLE high
	sta PORTB
	lda #%0000010	;  Set ENABLE low
	sta PORTB

	txa
	and #%1000000
	bne lcd_read_flag ; BF is 1, so AND instruction makes Z flag = 0.

	plx
	pla
	rts

print_number:
    and #$0F
    cmp #10
    clc
    bpl num_over10

    adc #'0'
    jmp continue_key_down

num_over10:
    adc #55 ; A is 65 in ASCII, minus 10...

continue_key_down:
	jsr send_lcd
	rts

u16_mult:
	jsr clear_ans
	lda #$80
	sta ans+1
	asl A
	dec op1
u16_l1:
	lsr op2+1	; Get low bit of op2
	ror op2
	bcc u16_l2	; Low bit = 0?
	lda ans+2	
	adc op1
	sta ans+2
	
	lda ans+3
	adc op1+1
	sta ans+3	; A is loaded with the highest byte of ans
u16_l2:
	ror a
	ror ans+2
	ror ans+1
	ror ans
	bcc u16_l1
	sta ans+3
	rts

u8_mult:
	jsr clear_ans
    lda #$80	; This bit will end up in the carry on the last iteration.
	sta ans
	asl A		; Clear the high bit (A=0)
	dec op1		; This is so no CLC has to be called in ADC op1
u8_l1: 
	lsr op2		; Get low bit of op2 to carry
    bcc u8_l2	; If carry = 1:
    adc op1		; Add op1 to A
u8_l2:
	ror A		; Shift A one place to the right, low bit gets copied to carry.
    ror ans		; And here, the carry gets copied to ans. Last bit of ans gets to carry.
    bcc u8_l1	; If carry = 1, the first bit loaded at LDA #80 has been reached, and so, 8 iterations have been done.
	sta ans+1
    rts

u16_sum:
	jsr clear_ans
    ldx #0
    jsr u8_sum
    inx
    jsr sum_byte
    rts

u16_sub:
	jsr clear_ans
    ldx #0
    jsr u8_sub
    inx
    jsr sub_byte
    rts

u8_sum:
	jsr clear_ans
    ldx #0
    clc
    jsr sum_byte
    rts

u8_sub:
	jsr clear_ans
    ldx #0
    sec
    jsr sub_byte
    rts

sum_byte:
	lda op1,x
    adc op2,x
    sta ans,x
    rts

sub_byte:
	lda op1,x
    sbc op2,x
    sta ans,x
    rts

clear_ans:
	lda #0
	sta ans
	sta ans+1
	sta ans+2
	sta ans+3
	rts

; ********************* IRQ *********************
irq:
	pha
	phx
	lda kb_flags
	and #KB_RELEASE	; check if key is being released
	beq read_key	; if not, read key

	lda kb_flags	; Key is being released, so it has sent F0 and now the key released.
	eor #KB_RELEASE	; flip only the bit of the RELEASE
	sta kb_flags
    lda PORTA    ; Only to read the flag and release the interrupt
	jmp exit_irq

read_key:
	lda PORTA
	cmp #$F0	; If releasing a key, it will receive a F0
	beq set_release_flag

push_key_to_buffer:
	ldx kb_w
	sta kb_buffer, x
	inc kb_w
	jmp exit_irq

set_release_flag:
	lda kb_flags
	ora #KB_RELEASE
	sta kb_flags

exit_irq:
	plx
	pla
	rti

nmi:
	rti

	.org $fffa			; Entry direction of the W65C02
	.word nmi
	.word program_start	; Fetch this direction to the start of the program.
	.word irq