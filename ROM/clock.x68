*-----------------------------------------------------------
* Title      :
* Written by :
* Date       :
* Description:
*-----------------------------------------------------------
    ORG    $400000
    DC.L    0
    DC.L    START

vBase EQU $EFE1FE 
vBufB EQU 512*0
vDirB EQU 512*2 
vBufA EQU 512*15

rTCData EQU 0
rTCClk EQU 1
rTCEnb EQU 2

D0 DC.B $00, $18, $24, $24, $24, $24, $24, $18, $00
D1 DC.B $00, $08, $18, $08, $08, $08, $08, $1C, $00  
D2 DC.B $00, $18, $24, $04, $08, $10, $20, $3C, $00
D3 DC.B $00, $3C, $08, $10, $38, $04, $04, $38, $00
D4 DC.B $00, $04, $0C, $14, $24, $3E, $04, $04, $00
D5 DC.B $00, $3C, $20, $20, $38, $04, $04, $38, $00
D6 DC.B $00, $1C, $20, $20, $38, $24, $24, $18, $00
D7 DC.B $00, $3C, $04, $08, $10, $20, $20, $20, $00
D8 DC.B $00, $18, $24, $24, $18, $24, $24, $18, $00
D9 DC.B $00, $18, $24, $24, $1C, $04, $04, $38, $00
DA DC.B $00, $08, $14, $14, $22, $3E, $22, $22, $00
DB DC.B $00, $38, $24, $24, $38, $24, $24, $38, $00
DC DC.B $00, $1C, $20, $20, $20, $20, $20, $1C, $00
DD DC.B $00, $38, $24, $24, $24, $24, $24, $38, $00
DE DC.B $00, $3C, $20, $20, $38, $20, $20, $3C, $00
DF DC.B $00, $3C, $20, $20, $38, $20, $20, $20, $00

 START: 
    bra ROM_MANIPULATION
    END_ROM_MANIPULATION:
    move.l #$10000, sp
    move.l #HANDLER, $64
    bclr #rTCEnb, vBase+vBufB

    move.b #%00110101, d0
    jsr SEND
    move.b #%00000000, d0
    jsr SEND
    move.b #%00000001, d0
    jsr SEND
    move.b #%00000000, d0
    jsr SEND
    move.b #%00110101, d0
    jsr SEND
    move.b #%10000000, d0
    jsr SEND   

    READ:
    stop #$2000
    bra READ

    bset #rTCEnb, vBase+vBufB
     
    END_LABEL:
    bra END_LABEL

 HANDLER:
	move.b #%10001101, d0
    jsr SEND
    jsr RECEIVE
    lsl.l #8, d0
    move.b #%10001001, d0
    jsr SEND
    jsr RECEIVE
    lsl.l #8, d0
    move.b #%10000101, d0
    jsr SEND
    jsr RECEIVE
    lsl.l #8, d0
    move.b #%10000001, d0
    jsr SEND
    jsr RECEIVE
    move.l #28, d2  
    move.l  #$1A700, a0
    jsr PRINT
    rte
   
 SEND:
    move.b #7, d2
    ITER_SEND:
    move.b d0, d1
    lsr.b d2, d1    
    and.b #$01, d1
    bclr #rTCClk, vBase+vBufB
    and.b #$FE, vBase+vBufB
    or.b d1, vBase+vBufB
    bset #rTCClk, vBase+vBufB
    cmp.b #0, d2
    sub.b #1, d2
    bge ITER_SEND
    rts

 RECEIVE:
    move.b #0, d0 
    move.b #7, d2
    ITER_RECEIVE:
    lsl.b #1, d0
    bclr #rTCClk, vBase+vBufB
    move.b (vBase+vBufB), d1
    bset #rTCClk, vBase+vBufB
    and.b #$01, d1
    or.b d1, d0
    cmp.b #0, d2
    sub.b #1, d2
    bge ITER_RECEIVE
    rts

 PRINT:
    move.l d0, d1
    lsr.l d2, d1 
    and.l #$0000000F, d1
    jsr CHECK 
    CHECK_END:
    sub.l #4, d2
    cmp #0, d2
    bge PRINT
    rts
 CHECK:
    move.l #D0, a1
    mulu #9, d1
    add.l d1, a1 
    move.l #0, d1
    ITER:    
    move.b (a1)+, (a0)
    add.l #64, a0
    add.l #1, d1
    cmp #8, d1
    bne ITER
    sub.l  #511, a0
    rts

 ROM_MANIPULATION:
    move.b (vBase+vBufA), d0
    and.b #$EF, d0
    move.b d0, vBase+vBufA
    bra END_ROM_MANIPULATION
        
 END    START







































*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
