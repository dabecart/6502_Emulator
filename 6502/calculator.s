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

; Stores the values of the operands and result
OP_SIZE = 4
op1 = $03
op2 = op1 + OP_SIZE
ans = op1 + OP_SIZE*2
rem = op1 + OP_SIZE*3	; Remainder (used in division)

KB_RELEASE 	= %00000001

; CALCULATOR
CALCULATOR_MEMSPACE = $0300 

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

	ldx #0
test_input:
	lda input_expression,x
	sta kb_buffer,x
	beq test_2
	inx
	jmp test_input

test_2:
	stx kb_r
	jsr calculator_bootup

loop:
	sei
	lda kb_r
	cmp kb_w
	cli
	bne key_down
	jmp loop

key_down:
	lda #%00000001	; Clear screen
	jsr send_lcd_instruction

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
	
	ldx kb_r
	stz kb_buffer,x	; Clear the enter sign as it could confound the calculator.
	dec kb_r

	jsr calculator_bootup

esc_pressed:
	lda #%00000001	; clear display
	jsr send_lcd_instruction
	
	; Restart whole calculator
clear_calculator_buffer:
	ldx #0
	stz kb_buffer,x
	inx
	beq clear_calculator_buffer

	stz kb_w
	stz kb_r
	stz kb_flags
	jmp loop

;************************ CALCULATOR ************************
; Stores the level in the program we're in. First level 1 means the program is on the first call, 
; if the same method is later called because there's an inner parenthesis, this value will be different.
; Think of it as a memory bank indicator.
; Each level will get a total of 16 different places of memory, that's 16 byte size variables. 
; There are only 15 levels. Level 0 is reserved for global variables.
input_expression: .asciiz "5+6"

; GLOBAL VARIABLES
currentLevel = CALCULATOR_MEMSPACE
return = CALCULATOR_MEMSPACE+15

inLength = kb_r	; We can save this one as the length has been already calculated.

; Memory location of the subsequent variables will be: 
; CALCULATOR_MEMSPACE ($0400) + CurrentLevel + VARIABLE NAME = Variable direction.
VARIABLE_MEMSPACE = CALCULATOR_MEMSPACE+$100
TOTAL_VARIABLES = 4	; To make everything easier, all functions will have the same number of variables.
from = 0
to = 1
s1 = 2
s2 = 3

calculator_bootup:
	lda #$00
	sta currentLevel

	ldx #from
	lda #0
	jsr pass_param

	ldx #to
	lda inLength
	jsr pass_param

checkExpression:
	pha
	phx
	phy
	jsr increase_currentLevel
	lda inLength	; Previous length

; while_check_parenthesis:
; 	jsr checkParenthesis
; 	jsr updateToParam
; 	ldx return
; 	bne while_check_parenthesis

; while_searchForMultDiv:
; 	jsr searchForSymbol
; 	jsr updateToParam
; 	ldx return
; 	bne while_searchForMultDiv

while_searchForSumSub:
	ldx #from
	jsr lda_var
	jsr pass_param

	ldx #to
	jsr lda_var
	jsr pass_param

	ldx #s1
	lda #'+'
	jsr pass_param

	ldx #s2
	lda #'-'
	jsr pass_param

	jsr searchForSymbol
	jsr updateToParam
	ldx return
	bne while_searchForSumSub

	ply
	plx
	pla
	rts

updateToParam:
	; Prev already stored in A
	; To = To - Prev - InLenght -> To = To - (Prev + InLength)
	clc
	adc inLength
	ldx to
	sta to	; Prev + Inlength saved to TO
	txa
	sec
	sbc to	
	sta to
	lda inLength
	rts

checkParenthesis:
	pha
	phx
	phy
	jsr increase_currentLevel

	ldx #from
	jsr lda_var
	tax		; From value -> X

checkParenthesis_whileFromLoop:
	lda kb_buffer, x
	cmp #'('
	beq find_closing_parenthesis
	cmp #')'
	jsr error_handling	; SYNTAX ERROR: Missing closing parenthesis
	jmp checkParenthesis_whileFromLoop_continue

find_closing_parenthesis:
	txa
	ldx #from
	jsr sta_var	;Save "from" value for later parenthesis removal
	tax

	ldy #1	; Parenthesis recount -> Y
	inx		; Now, j variable of for loop -> X
find_closing_parenthesis_loop:
	lda kb_buffer, x
	cmp #'('
	bne find_closing_parenthesis_checkClose
	iny
find_closing_parenthesis_checkClose:
	cmp #')'
	bne find_closing_parenthesis_checkZeroRecount
	dey
find_closing_parenthesis_checkZeroRecount:
	bne find_closing_parenthesis_loopContinue

	; Call check expression again. Current level must be increased.
	phx

	ldx #from
	jsr lda_var
	inc A
	jsr pass_param

	plx
	txa
	ldx #to
	jsr pass_param

	jsr checkExpression

	ldx #from
	jsr lda_var
	tax
	inx

remove_parenthesis_loop:
	lda kb_buffer, x
	cmp #'('
	bne remove_parenthesis_loopcontinue

	ldy #' '
	tya
	sta kb_buffer, x

	ldx #from
	jsr lda_var
	tax
	tya
	sta kb_buffer, x

	jsr clearWhitespaces

remove_parenthesis_loopcontinue:
	inx
	txa
	cmp inLength
	bne remove_parenthesis_loop
	
	jsr error_handling ; How did I get here?

find_closing_parenthesis_loopContinue:
	inx
	txa
	cmp to
	bne find_closing_parenthesis_loop

	jsr error_handling	; SYNTAX ERROR: Misplaced opening parenthesis

checkParenthesis_whileFromLoop_continue:
	inx
	txa
	cmp to
	bne checkParenthesis_whileFromLoop

	jsr return_false
	pla
	rts

clearWhitespaces:
	rts

searchForSymbol:
	pha
	phx
	phy

	

	ply
	plx
	pla
	rts

error_message: .asciiz "Error"

error_handling:
	ldx #0
error_handling_loop:
	lda error_message,x
	beq error_handling_exit	
	jsr send_lcd
	inx
	jmp error_handling_loop
error_handling_exit:
	; Restart the stack pointer
	ldx #$ff
	txs
	jmp loop

return_true:
	sec
	rol return
	rts

return_false:
	stz return
	rts

; Stores value A in variable "named" X for the next stage
pass_param:
	jsr increase_currentLevel
	jsr sta_var
	jsr decrease_currentLevel
	rts

increase_currentLevel:
	pha
	lda #TOTAL_VARIABLES
	clc
	adc currentLevel
	sta currentLevel
	pla
	rts

decrease_currentLevel:
	pha
	lda #TOTAL_VARIABLES
	sec
	sbc currentLevel
	sta currentLevel
	pla
	rts

; Loads value stored in variable "named" X to register A
lda_var:
	jsr create_variable_loc
	lda VARIABLE_MEMSPACE, Y
	rts

; Stores value A into variable "named" (whose location is) X
sta_var:
	jsr create_variable_loc
	sta VARIABLE_MEMSPACE, Y
	rts  

; Variable named (whose location is) X will be located in currentLevel + Y.
create_variable_loc:
	pha
	txa
	clc
	adc currentLevel
	tay
	pla
	rts

;************************ MATH CORE ************************

u16_mod:
	jsr u16_division
	lda rem
	sta ans
	lda rem+1
	sta ans+1
	rts

u8_mod:
	jsr u8_division
	lda rem
	sta ans
	rts

u16_division:
	jsr clear_ans
	phx
	inc ans
u16_divl1:
	asl op1
	rol op1+1
	rol rem
	rol rem+1
	
	lda rem
	sec
	sbc op2
	tax
	lda rem+1
	sbc op2+1
	bcc u16_divl2	; Branch if subtraction is negative
	sta rem+1
	txa
	sta rem
u16_divl2:
	rol ans
	rol ans+1
	bcc u16_divl1
	plx
	rts

u8_division:
	jsr clear_ans
	inc ans		; Load with trigger bit. When the answer ROL gets this bit, 8 cycles have been done.
u8_divl1:
	asl op1		; Pass the next bit to the remainder
	rol rem		
	lda rem
	sec			; Subtract the divisor from the current remainder
	sbc op2		
	bcc u8_divl2 ; Branch if subtraction is negative
	sta rem
u8_divl2:
	rol ans
	bcc u8_divl1
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
	ror ans+3
	ror ans+2
	ror ans+1
	ror ans
	bcc u16_l1
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

clear_ans:	; Clear ans and remainder
	ldx #OP_SIZE*2
clear_ans_loop:
	dex
	stz ans,x
	bne clear_ans_loop
	rts

; ********************* LCD *********************

print_number:
    and #$0F
    cmp #10
    clc
    bpl num_over10

    adc #'0'
    jmp continue_print_number
num_over10:
    adc #55 ; A is 65 in ASCII, minus 10...
continue_print_number:
	jsr send_lcd
	rts

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
	cmp #$0A	; ENTER key is down, so skip comprobation of 255 characters written in input.
	beq push_key_to_buffer

	; If the buffer is at 254 chars no more chars will be loaded.
	tax
	lda kb_w
	eor #$FE
	beq exit_irq

	; Fetch the real key from the scancode
	lda keymap, x

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

  .org $fd00
keymap:.byte "????????????? `?" ; 00-0F
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

	.org $fffa			; Entry direction of the W65C02
	.word nmi
	.word program_start	; Fetch this direction to the start of the program.
	.word irq