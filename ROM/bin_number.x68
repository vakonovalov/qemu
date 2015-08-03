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
    move.l #1,  d2
    move.l #24, d3
    move.l #31, d4
    
cmd:
    move.l #8, d5
    move.l d0, d1
    lsl.l d3, d1
    lsr.l d4, d1
    sub.l d2, d5
    lsl.l d5, d1
    cmp #0, d1
    beq clr
    
    bset #rTCData, (vBase+vBufB)
    bra clk
clr:
    bclr #rTCData, (vBase+vBufB)
    bra clk
clk:
    bset #rTCClk, (vBase+vBufB)
    bclr #rTCClk, (vBase+vBufB)

    add.l #1, d3
    cmp #8, d2
    beq init
    add.l #1, d2
    bra cmd

init:
    move.l #0,  d1
    move.l #1,  d2
    move.l #7,  a1
    move.l #24, d3
    move.l #31, d4
    move.l d0,  d7
    move.l #8,  (count)
    move.l #7,  (bits)
    lsr.l #7,   d7
    cmp #1,     d7
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
    bset #rTCData, (vBase+vBufB)
    bra clk1
clr1:
    bclr #rTCData, (vBase+vBufB)
    bra clk1
clk1:
    bset #rTCClk, (vBase+vBufB)
    bclr #rTCClk, (vBase+vBufB)
    add.l #1, d3
    cmp #8, d2
    beq exit
    add.l #1, d2
    bra write

read:
    
    move.l (count), d5
    move.l d6, d1
    lsl.l d3, d1
    lsr.l d4, d1
    sub.l d2, d5
    lsl.l d5, d1
    cmp #0, d1
    beq clr2
    bset #rTCData, (vBase+vBufB)   
    bra clk2
clr2:
    bclr #rTCData, (vBase+vBufB)
    bra clk2
clk2:
    bset #rTCClk, (vBase+vBufB)
    bclr #rTCClk, (vBase+vBufB)
    move.l (bits), d5
    move.l (vBase+vBufB), d7
    and.l #$1, d7
    lsr.l d5, d7    
    *or.b (vBase+vBufB), d7
    lsl.l d5, d7
    sub.l #1, (bits)
    add.l #1, d3
    cmp #8, d2
    beq init2
    add.l #1, d2
    bra read

init2:
    move.l d7, (a1)
    move.l #28, d3
    move.l #28, d5
    move.l #0, d6

shift:
    move.l #$1A700, a0
    move.l (a1),d2
    move.l d3, d4
    lsl.l d3, d2
    lsr.l d5, d2
    sub #4, d3 
    add #1, d6
    cmp #8, d6
    beq exit
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
    move.b #%00000000, (a0)
    add.l #65, a0
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
    add.l #1, a0    
    bra shift
draw1:
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
    add.l #64, a0
    bra shift
draw2:
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
    add.l #64, a0
    bra shift
draw3:
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
    add.l #64, a0    
    bra shift
draw4:
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
    add.l #64, a0
    bra shift
draw5:
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
    add.l #64, a0
    bra shift
draw6:
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
    add.l #64, a0
    bra shift
draw7:
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
    add.l #64, a0
    bra shift
draw8:
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
    add.l #64, a0    
    bra shift
draw9:
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
    add.l #64, a0    
    bra shift
drawA:
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
    add.l #64, a0
    bra shift
drawB:
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
    add.l #64, a0
    bra shift
drawC:
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
    add.l #64, a0
    bra shift
drawD:
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
    add.l #64, a0    
    bra shift
drawE:
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
    add.l #64, a0
    bra shift
drawF:
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
    add.l #64, a0
    bra shift

exit:
    bra exit
    END    START      










*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
