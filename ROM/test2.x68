    ORG $400000
* Contron sum?
    DC.L 0
* Address of the first instruction
    DC.L START

START:
    move.l #$800000, a0
    add.l #1, (a0)
    move.l $800000, d0 
    ;dc.w $ffff
    END START













*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
