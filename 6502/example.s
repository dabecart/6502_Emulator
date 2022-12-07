; To compile: .\vasm6502_oldstyle.exe -Fbin -dotdir -c02 example.s
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
kb_w = $0000	; Keyboard write pointer
kb_r = $0001	; Keyboard read pointer
kb_flags = $0002	; Keyboard flag

KB_RELEASE 	= %00000001
KB_SHIFT 	= %00000010

kb_buffer = $0200	; Position of the buffer (256 bytes, 0200 - 02ff)

	.org $e000
program_start:	
	ldx #$ff
	txs

	ldx #0			; Start the keyboard pointers to 0
	stx kb_w
	stx kb_r
	stx kb_flags

	lda #%1111111	; Set bottom 7 pins as output
	sta DD_PORTB	; Initialize Data direction register B

	lda #0			; Set port A as input
	sta DD_PORTA	; Initialize Data direction register A

	lda #1			; Set CA1 to work with positive edge
	sta PCR

	lda #$82		; Set CA1 as interrupt source. Top bit is to enable
	sta INT_EN_REG

	; Init the LCD Module
	lda #%0010000	; Start configuration for 4 bit mode (this will get in 8bit mode)
	jsr lcd_instruction_nowait ; Send directly, as it is 8 bit mode.

	lda #%00101000	; 4 bit mode, display is 8x5 pixels
	jsr send_lcd_instruction

	lda #%00001110	; Display ON, cursor ON and blinking cursor OFF
	jsr send_lcd_instruction

	lda #%00000110	; Increment register by one and shift cursor to the right
	jsr send_lcd_instruction

	jmp loop

loop:
	sei
	lda kb_r
	cmp kb_w
	cli
	bne key_down
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
	jsr lcd_instruction_nowait

	txa
	and #%1000000
	bne lcd_read_flag ; BF is 1, so AND instruction makes Z flag = 0.

	lda #%1111111	; Set bottom 7 pins as output
	sta DD_PORTB	; Initialize Data direction register B
	
	plx
	pla
	rts

key_down:
	ldx kb_r
	lda kb_buffer, X
; CHECK FOR IMPORTANT KEY PRESSES
	cmp #$0a		; enter key down
	beq enter_pressed
	cmp #$1b		; escape pressed
	beq esc_pressed 

	jsr send_lcd
key_down_end:
	inc kb_r
	jmp loop

enter_pressed:
	lda #%10101000	; put cursor at position 40
	jsr send_lcd_instruction
	jmp key_down_end

esc_pressed:
	lda #%00000001	; clear display
	jsr send_lcd_instruction
	jmp key_down_end

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
; CHECK FOR IMPORTANT KEY RELEASES	
	lda PORTA
	cmp #$12 		; left shift
	beq reset_shift_flag
	cmp #$59		; right shift
	beq reset_shift_flag

	jmp exit_irq

read_key:
	lda PORTA
	cmp #$F0	; If releasing a key, it will receive a F0
	beq set_release_flag
	cmp #$12 		; left shift
	beq set_shift_flag
	cmp #$59		; right shift
	beq set_shift_flag

	tax			; Keyscan transfer A -> X
	lda kb_flags
	and #KB_SHIFT
	bne shifted_key	; If its not zero, then keys are shifted

	lda keymap, x	; Normal keys
	jmp	push_key_to_buffer

shifted_key:
	lda keymap_shifted, x

push_key_to_buffer:
	ldx kb_w
	sta kb_buffer, x
	inc kb_w
	jmp exit_irq

set_release_flag:
	lda kb_flags
	ora #KB_RELEASE
	sta kb_flags
	jmp exit_irq

set_shift_flag:
	lda kb_flags
	ora #KB_SHIFT
	sta kb_flags
	jmp exit_irq

reset_shift_flag:
	lda kb_flags
	eor KB_SHIFT
	sta kb_flags

exit_irq:
	plx
	pla
	rti

nmi:
	rti

  .org $fd00
keymap:
  .byte "????????????? `?" ; 00-0F
  .byte "?????q1???zsaw2?" ; 10-1F
  .byte "?cxde43?? vftr5?" ; 20-2F
  .byte "?nbhgy6???mju78?" ; 30-3F
  .byte "?,kio09??./l;p-?" ; 40-4F
  .byte "??'?[=????",$0a,"]?\??" ; 50-5F
  .byte "?????????1?47???" ; 60-6F
  .byte "0.2568",$1b,"??+3-*9??" ; 70-7F
  .byte "????????????????" ; 80-8F
  .byte "????????????????" ; 90-9F
  .byte "????????????????" ; A0-AF
  .byte "????????????????" ; B0-BF
  .byte "????????????????" ; C0-CF
  .byte "????????????????" ; D0-DF
  .byte "????????????????" ; E0-EF
  .byte "????????????????" ; F0-FF
keymap_shifted:
  .byte "????????????? ~?" ; 00-0F
  .byte "?????Q!???ZSAW@?" ; 10-1F
  .byte "?CXDE#$?? VFTR%?" ; 20-2F
  .byte "?NBHGY^???MJU&*?" ; 30-3F
  .byte "?<KIO)(??>?L:P_?" ; 40-4F
  .byte '??"?{+?????}?|??' ; 50-5F
  .byte "?????????1?47???" ; 60-6F
  .byte "0.2568???+3-*9??" ; 70-7F
  .byte "????????????????" ; 80-8F
  .byte "????????????????" ; 90-9F
  .byte "????????????????" ; A0-AF
  .byte "????????????????" ; B0-BF
  .byte "????????????????" ; C0-CF
  .byte "????????????????" ; D0-DF
  .byte "????????????????" ; E0-EF
  .byte "????????????????" ; F0-FF

	.org $fffa			; Entry direction of the W65C02
	.word nmi
	.word program_start	; Fetch this direction to the start of the program.
	.word irq