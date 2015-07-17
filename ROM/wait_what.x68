    ORG $400000
    DC.L 0
    DC.L START
    
START:
    move.l #$800000,a0
    move.l #10,d1
    move.b d1,(a0)
    move.b (a0),d0
 
    DC.W $ffff
    END START
*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
