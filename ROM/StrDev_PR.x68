
    ORG    $400000
    DC.L 0
    DC.L START
    

START:  
	move.l #$800000, a1
	move.b #1, d1
	move.b d1, (a1)
	move.b (a1), d2                
    END    START 



*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
