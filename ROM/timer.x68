    ORG $400000
    DC.L 0
    DC.L START

P0 DC.b $00, $3C, $42, $42, $42, $42, $42, $3C
P1 DC.b $00, $08, $18, $28, $08, $08, $08, $08
P2 DC.b $00, $38, $44, $04, $08, $10, $20, $7C
P3 DC.b $00, $3C, $42, $02, $1C, $02, $42, $3C
P4 DC.b $00, $04, $0C, $14, $24, $7E, $04, $04
P5 DC.b $00, $7E, $40, $40, $3C, $02, $42, $3C
P6 DC.b $00, $1E, $20, $40, $7C, $42, $42, $3C
P7 DC.b $00, $7E, $02, $04, $08, $10, $10, $10
P8 DC.b $00, $3C, $42, $42, $3C, $42, $42, $3C
P9 DC.b $00, $3C, $42, $42, $3E, $02, $02, $3C
PA DC.b $00, $18, $18, $24, $24, $7E, $42, $42
PB DC.b $00, $7C, $42, $42, $7C, $42, $42, $7C
PC DC.b $00, $3C, $42, $40, $40, $40, $42, $3C
PD DC.b $00, $7C, $42, $42, $42, $42, $42, $7C
PE DC.b $00, $7E, $40, $40, $7C, $40, $40, $7E
PF DC.b $00, $7E, $40, $40, $7C, $40, $40, $40

vBase   EQU $EFE1FE
vBufB   EQU 512*0
vDirB   EQU 512*2
vDirA   EQU 512*3
rTCData EQU 0
rTCClk  EQU 1
rTCEnb  EQU 2

START:
    move.l #$610000, sp
    move.b #%10000101, d0
    move.b #0, d2
    bclr #rTCEnb ,(vBase+vBufB)
    jsr write
    ;move.b #%00001001, d0
    move.b #0, d2
    move.b #0, d0
    jsr read

;print
    move.l #$1A700, a0
    move.l d0, d2
    move.l #0, d4
PR2: 
    add.l #1, d4       
    move.l d2, d1
    ANDi.l #$F0000000, d1
    rol.l #4, d1
    ANDi.l #$0FFFFFFF, d2
    lsl.l #4, d2
    jsr PRINT
    cmp.l #8, d4
    bne PR2
    bra qu

PRINT:
    muls #8, d1
    move.l d1, a1
    move.l #P0, a2
    add.l a1, a2
    move.b (a2)+,(a0)
    add.l #64, a0
    move.b (a2)+,(a0)
    add.l #64, a0
    move.b (a2)+,(a0)
    add.l #64, a0
    move.b (a2)+,(a0)
    add.l #64, a0
    move.b (a2)+,(a0)
    add.l #64, a0
    move.b (a2)+,(a0)
    add.l #64, a0
    move.b (a2)+,(a0)
    add.l #64, a0
    move.b (a2)+,(a0)
    sub.l #447, a0
    rts 

write:
    move.b d0, d1
    and.b #1, d1
    ;and.b #%11111110, d0
    lsr.b #1, d0
    and.b #$FE, (vBase+vBufB)
    or.b d1, (vBase+vBufB)
    bset #rTCClk, (vBase+vBufB)
    bclr #rTCClk, (vBase+vBufB)
    add.b #1, d2
    cmp #7, d2
    ble write
    rts
read:
    bclr #rTCData, (vBase+vBufB)
    bset #rTCClk, (vBase+vBufB)
    bclr #rTCClk, (vBase+vBufB)
    move.b (vBase+vBufB), d1
    and.b #1, d1
    lsl.b d2, d1
    bclr d2, d0
    or.b d1, d0
    add.b #1, d2
    cmp #7, d2
    ble read
    rts

QU:
    bra qu
    END START






*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
