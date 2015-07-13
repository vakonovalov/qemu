    ORG $400000
* Contron sum?
    DC.L 0
* Address of the first instruction
    DC.L START

START:
    move.l #1, d0
    move.l #32, d1
    move.l #0, d2
    move.l #0, d3
    move.l #$1BFD9, a0
    ;move.l #$1A702, a1
    ;move.b #%10101010, $1A702
PR1:
    move.b d0, (a0)
    ROl.b #1, d0
    add.l #64, a0
    add.l #1, d2
    cmp #8, d2
    beq PR3
    bra PR1
PR3:
    sub.l #1, a0
    move.l #0, d2
    add.l #1, d3    
    cmp #20, d3
    beq PR2
    bra PR1
PR2:
    sub.l #1, a0
    bra PR4
PR4:
    move.b #%00000001, a(0)
        
    bra PR2
    END START









*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
