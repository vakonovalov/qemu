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
rTCEnb  EQU 0
count EQU $605000
  
    move.l #5, d0
    move.l #138, d6
    move.l #0,  d1
    move.l #1, d2
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
    move.l #1, d2
    move.l #24, d3
    move.l #31, d4
    move.l d6, d7
    move.l #7, (count)
    lsr.l #7, d7
    cmp #1, d7
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
    move.l #8, d5
    move.l d6, d1
    lsl.l d3, d1
    lsr.l d4, d1
    sub.l d2, d5
    lsl.l d5, d1
    cmp #0, d1
    beq clr2
    bset #rTCData, (vBase+vBufB)
    lsr.l (count), d7
    or.l (vBase+vBufB), d7
    lsl.l (count), d7
    sub #1, (count)
    bra clk2
clr2:
    bclr #rTCData, (vBase+vBufB)
    bra clk2
clk2:
    bset #rTCClk, (vBase+vBufB)
    bclr #rTCClk, (vBase+vBufB)
    add.l #1, d3
    cmp #8, d2
    beq exit
    add.l #1, d2
    bra read

exit:
    bra exit
    END    START      



*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
