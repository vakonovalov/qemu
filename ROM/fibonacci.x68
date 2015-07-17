    ORG $400000
    DC.L 0
    DC.L START
    
START:
    move #7,d3
    move #0, d0
    move #1, d1
    LABEL:
        move d1,d2
        add d0,d1
        move d2,d0
        dbf d3,LABEL
    move d2,d4
    END START

*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
