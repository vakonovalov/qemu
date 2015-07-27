    ORG $400000
    DC.L 0
    DC.L START
vBase   EQU $EFE1FE
vBufB   EQU 512*0
vDirB   EQU 512*2
vDirA   EQU 512*3
rTCData EQU 0
rTCClk  EQU 1
tTCEnb  EQU 2

START:
    move.l #$610000, sp
    move.b #%00001101, d0
    move.b #0, d2
    jsr CYCLE
    move.b #%00001001, d0
    move.b #0, d2
    jsr CYCLE
    bra qu
CYCLE:
    move.b d0, d1
    and.b #%00000001, d1
    ;and.b #%11111110, d0
    lsr.b #1, d0
    and.b #$FE, (vBase+vBufB)
    or.b d1, (vBase+vBufB)
    bset #rTCClk, (vBase+vBufB)
    bclr #rTCClk, (vBase+vBufB)
    add.b #1, d2
    cmp #7, d2
    ble CYCLE
    rts
QU:
    bra qu
    END START



*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
