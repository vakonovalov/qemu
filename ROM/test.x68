    ORG $400000
* Contron sum?
    DC.L 0
* Address of the first instruction
    DC.L START

START:
    move.l #8, d2
    move.l #1, d1
    sub.l #2, d2
PR1:
    cmp #0, d2
    beq pr2
    EOR.l d0, d1
    EOR.l d1, d0
    EOR.l d0, d1
    add.l d0, d1
    sub.l #1, d2
    bra PR1 
PR2:
   END START


*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
