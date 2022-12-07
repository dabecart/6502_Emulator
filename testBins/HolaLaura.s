; To compile:	.\vasm6502_oldstyle.exe -Fbin -dotdir -c02 HolaLaura.s
; To send:		java -jar EEPROM_Programmer.jar

PORTB = $c000		; Write direction for port B
DD_PORTB = $c002	; Data direction register for port B	
	
	.org $e000
program_start:	
	ldx #$ff
	txs

	lda DD_PORTB	; Lets find out if the board has been reset or not
	and #%0000111	
	bne start_lcd_4bit	; If 3 last bits are 1, LCD has been started

	; Init the LCD Module
	lda #%0010000	; Start configuration for 4 bit mode (this will get in 8bit mode)
	jsr lcd_instruction_nowait ; Send directly, as it is 8 bit mode.

start_lcd_4bit:
	lda #%00101000	; 4 bit mode, display is 8x5 pixels
	jsr send_lcd_instruction

	lda #%00001110	; Display ON, cursor ON and blinking cursor OFF
	jsr send_lcd_instruction

	lda #%00000110	; Increment register by one and shift cursor to the right
	jsr send_lcd_instruction

	lda #%00000001	; Increment register by one and shift cursor to the right
	jsr send_lcd_instruction

	ldx #0
print:
	lda message,x
	beq loop		; If message[x] is 0 (end of asciiz), jump to loop
	jsr send_lcd
	inx
	jmp print

loop:
	jmp loop

message: .asciiz "Hola Laura!"

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

	;lda #%1111111	; Set bottom 7 pins as output
	;sta DD_PORTB	; Initialize Data direction register B
	
	plx
	pla
	rts

	.org $fffc			; Entry direction of the W65C02
	.word program_start	; Fetch this direction to the start of the program.
	.word 0