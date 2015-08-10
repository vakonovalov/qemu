    ORG    $400000
    DC.L 0
    DC.L START
    
START:
     
vBase EQU $EFE1FE
vBufB EQU 512*0
vDirB EQU 512*2
vDirA EQU 512*3

rTCData EQU 0
rTCClk  EQU 1
rTCEnb  EQU 2

count EQU $605000
bits  EQU $606000
data  EQU $607000

    move.l #133,  d0
    move.l #10, d6
    move.l #0,  d1
    move.l #8,  d2
    move.l #7, d3
    move.l #31, d4
   *move.l #0, d2
cmd:
    move.l d0, d1
    lsr.l d3, d1
    and.l #$1, d1
    cmp #0, d1
    beq clr
    bset.b #rTCData, (vBase+vBufB)
    bra clk
clr:
    bclr.b #rTCData, (vBase+vBufB)
    bra clk
clk:
    bset.b #rTCClk, (vBase+vBufB)
    bclr.b #rTCClk, (vBase+vBufB)
    sub.l #1, d2
    sub.l #1, d3
    cmp #0, d2
    beq init
    bra cmd
init:
    move.l #0,  d1
    move.l #1,  d2
    move.l #7,  a1
    move.l #24, d3
    move.l #31, d4
    move.l d0,  d7
    move.l #8,  (count)
    move.l #7,  (bits)
    lsr.l  #7,   d7
    cmp #1,     d7
    move.l #0, d7
    beq read

write:
    move.l #8, d5
    move.l d6, d1
    lsl.l d3, d1
    lsr.l d4, d1
    sub.l d2, d5
    lsl.l d5, d1
    cmp #0, d1
    beq clr1
    bset.b #rTCData, (vBase+vBufB)
    bra clk1
clr1:
    bclr.b #rTCData, (vBase+vBufB)
    bra clk1
clk1:
    bset.b #rTCClk, (vBase+vBufB)
    bclr.b #rTCClk, (vBase+vBufB)
    add.l #1, d3
    cmp #8, d2
    beq exit
    add.l #1, d2
    bra write

read: 
    bset.b #rTCClk, (vBase+vBufB)
    bclr.b #rTCClk, (vBase+vBufB)
    move.b (vBase+vBufB), d0
    and.l #$1, d0
    lsl.l #1, d7
    or.b d0, d7
    add.l #1, d2
    cmp #8, d2
    beq init2
    bra read

init2:
    move.l #28, d3
    move.l #28, d5
    move.l #$1A700, a0
    move.l #0, d6
    move.l a1, d1    
    move.l #$1A700, a2 
    move.l #0, d4
*ls:
 *    add.l #1, d4
  *   cmp.l #10000, d4
   *  ble ls

shift:
    move.l d7, d2
    add.l #1, d6
    lsr.l d3, d2
    lsl.l d5, d2
    lsr.l d5, d2
    cmp #8, d6
    bgt exit
    sub #4, d3 
    bra check 

check:
    cmp #0, d2
    beq draw0
    cmp #1, d2
    beq draw1
    cmp #2, d2
    beq draw2
    cmp #3, d2
    beq draw3
    cmp #4, d2
    beq draw4
    cmp #5, d2
    beq draw5
    cmp #6, d2
    beq draw6
    cmp #7, d2
    beq draw7
    cmp #8, d2
    beq draw8
    cmp #9, d2
    beq draw9
    cmp #10, d2
    beq drawA
    cmp #11, d2
    beq drawB
    cmp #12, d2
    beq drawC
    cmp #13, d2
    beq drawD
    cmp #14, d2
    beq drawE
    cmp #15, d2
    beq drawF

draw0:   
    add.l #1, a2
    move.l a2,a0
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%10000001, (a0)
    add.l #64, a0
    move.b #%10000001, (a0)
    add.l #64, a0
    move.b #%10000001, (a0)
    add.l #64, a0
    move.b #%10000001, (a0)
    add.l #64, a0
    move.b #%10000001, (a0)
    add.l #64, a0
    move.b #%10000001, (a0)
    add.l #64, a0
    move.b #%10000001, (a0)
    add.l #64, a0
    move.b #%10000001, (a0)
    add.l #64, a0
    move.b #%10000001, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    bra shift

draw1:
    add.l #1, a2
    move.l a2,a0
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%00000100, (a0)
    add.l #64, a0
    move.b #%00001100, (a0)
    add.l #64, a0
    move.b #%00010100, (a0)
    add.l #64, a0
    move.b #%00100100, (a0)
    add.l #64, a0
    move.b #%01000100, (a0)
    add.l #64, a0
    move.b #%00000100, (a0)
    add.l #64, a0
    move.b #%00000100, (a0)
    add.l #64, a0
    move.b #%00000100, (a0)
    add.l #64, a0
    move.b #%00000100, (a0)
    add.l #64, a0
    move.b #%00000100, (a0)
    add.l #64, a0
    move.b #%00000100, (a0)
    bra shift

draw2:
    add.l #1, a2
    move.l a2,a0
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    bra shift

draw3:
    add.l #1, a2
    move.l a2,a0
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    bra shift

draw4:
    add.l #1, a2
    move.l a2,a0
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    bra shift

draw5:
    add.l #1, a2
    move.l a2,a0
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    bra shift

draw6:
    add.l #1, a2
    move.l a2,a0
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0    
    move.b #%01000000, (a0)   
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    bra shift

draw7:
    add.l #1, a2
    move.l a2,a0
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%011111100, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    bra shift

draw8:
    add.l #1, a2
    move.l a2,a0
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    bra shift

draw9:
    add.l #1, a2
    move.l a2,a0
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    bra shift

drawA:
    add.l #1, a2
    move.l a2,a0
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%00111100, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    bra shift

drawB:
    add.l #1, a2
    move.l a2,a0
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%01111100, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01111100, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01111100, (a0)
    bra shift

drawC:
    add.l #1, a2
    move.l a2,a0
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%00011100, (a0)
    add.l #64, a0
    move.b #%00100000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%00100000, (a0)
    add.l #64, a0
    move.b #%00011100, (a0)
    bra shift

drawD:
    add.l #1, a2
    move.l a2,a0
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%01111000, (a0)
    add.l #64, a0
    move.b #%01000100, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000100, (a0)
    add.l #64, a0
    move.b #%01111111, (a0)
    bra shift

drawE:
    add.l #1, a2
    move.l a2,a0
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01111100, (a0)
    bra shift

drawF:
    add.l #1, a2
    move.l a2,a0
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    bra shift
exit:
    bra exit
    END    START      


















*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
