    ORG $400000
* Contron sum?
    DC.L 0
* Address of the first instruction
    DC.L START

START:
    ;move.l #1, d0
    move #10000, sp
    move.l #0, d1
    move.l #0, d2
    move.l #0, d3
    ;move.l #$1BFD9, a1
    move.l #$1A700, a0
    ;move.b #%10101010, $1A702
PR1:
    move.l #$110102B3, $800000
    move.l $800000, d2
    move.l d2, d1
    bra PR2
    
    jsr CHECK
    move.l d2, d1
    lsl.b #4, d1
    lsr.b #4, d1
    jsr CHECK
    bra QU

PR2:
    move.l d2, d1
   ANDi.l #$F0000000, d1
    lsr.l #28, d1
    ANDi.l #$0FFFFFFF, d2
    cmp #0, d2
    beq QU
    jsr CHECK
    bra PR2

CHECK:
    cmp #0, d1
    beq F0
    cmp #1, d1
    beq F1
    cmp #2, d1
    beq F2
    cmp #3, d1
    beq F3
    cmp #4, d1
    beq F4
    cmp #5, d1
    beq F5
    cmp #6, d1
    beq F6
    cmp #7, d1
    beq F7
    cmp #8, d1
    beq F8
    cmp #9, d1
    beq F9
    cmp #10, d1
    beq FA
    cmp #11, d1
    beq FB
    cmp #12, d1
    beq FC
    cmp #13, d1
    beq FD
    cmp #14, d1
    beq FE
    cmp #15, d1
    beq FF
F0:
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
    move.b #%00111100, (a0)
    add.l #64, a0
    move.b #%00000000, (a0)
    sub.l #511, a0
    rts   
F1:
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%00001000, (a0)
    add.l #64, a0
    move.b #%00011000, (a0)
    add.l #64, a0
    move.b #%00101000, (a0)
    add.l #64, a0
    move.b #%00001000, (a0)
    add.l #64, a0
    move.b #%00001000, (a0)
    add.l #64, a0
    move.b #%00001000, (a0)
    add.l #64, a0
    move.b #%00001000, (a0)
    add.l #64, a0
    move.b #%00000000, (a0)
    sub.l #511, a0
    rts
    
F2:
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%00111000, (a0)
    add.l #64, a0
    move.b #%01000100, (a0)
    add.l #64, a0
    move.b #%00000100, (a0)
    add.l #64, a0
    move.b #%00001000, (a0)
    add.l #64, a0
    move.b #%00010000, (a0)
    add.l #64, a0
    move.b #%00100000, (a0)
    add.l #64, a0
    move.b #%01111100, (a0)
    add.l #64, a0
    move.b #%00000000, (a0)
    sub.l #511, a0
    rts
    
F3:
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%00111100, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00011100, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%00111100, (a0)
    add.l #64, a0
    move.b #%00000000, (a0)
    sub.l #511, a0
    rts 
F4:
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
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%00000100, (a0)
    add.l #64, a0
    move.b #%00000100, (a0)
    add.l #64, a0
    move.b #%00000000, (a0)
    sub.l #511, a0
    rts
    
F5:
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%00111100, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%00111100, (a0)
    add.l #64, a0
    move.b #%00000000, (a0)
    sub.l #511, a0
    rts
    
F6:
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%00011110, (a0)
    add.l #64, a0
    move.b #%00100000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01111100, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%00111100, (a0)
    add.l #64, a0
    move.b #%00000000, (a0)
    sub.l #511, a0
    rts
 
F7:
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000100, (a0)
    add.l #64, a0
    move.b #%00001000, (a0)
    add.l #64, a0
    move.b #%00010000, (a0)
    add.l #64, a0
    move.b #%00010000, (a0)
    add.l #64, a0
    move.b #%00010000, (a0)
    add.l #64, a0
    move.b #%00000000, (a0)
    sub.l #511, a0
    rts
    
F8:
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%00111100, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%00111100, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%00111100, (a0)
    add.l #64, a0
    move.b #%00000000, (a0)
    sub.l #511, a0
    rts
    
F9:
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%00111100, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%00111110, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00000010, (a0)
    add.l #64, a0
    move.b #%00111100, (a0)
    add.l #64, a0
    move.b #%00000000, (a0)
    sub.l #511, a0
    rts
    
FA:
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%00011000, (a0)
    add.l #64, a0
    move.b #%00011000, (a0)
    add.l #64, a0
    move.b #%00100100, (a0)
    add.l #64, a0
    move.b #%00100100, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%00000000, (a0)
    sub.l #511, a0
    rts
 
FB:
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%01111100, (a0)
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
    move.b #%01111100, (a0)
    add.l #64, a0
    move.b #%00000000, (a0)
    sub.l #511, a0
    rts

FC:
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%00111100, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%00111100, (a0)
    add.l #64, a0
    move.b #%00000000, (a0)
    sub.l #511, a0
    rts

FD:
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
    move.b #%01000010, (a0)
    add.l #64, a0
    move.b #%01111100, (a0)
    add.l #64, a0
    move.b #%00000000, (a0)
    sub.l #511, a0
    rts

FE:
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01111100, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%00000000, (a0)
    sub.l #511, a0
    rts
FF:
    move.b #%00000000, (a0)
    add.l #64, a0
    move.b #%01111110, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01111100, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%01000000, (a0)
    add.l #64, a0
    move.b #%00000000, (a0)
    sub.l #511, a0
    rts

QU:
    move.l #0, d5 
    bra QU
    END START








*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
