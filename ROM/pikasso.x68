    ORG $400000
    DC.L 0
    DC.L START
    
START:
    move.l #$01A700,a0
    move.l #64,a1    

    move.l a1,d7
    mulu #341,d7

    move.l d7,a2 
    
    move #15, d3
    BOTTOM_LINE:
        move.l #$ffffffff,(0,a0,a2.l)
        add #4,a2
        dbf d3,BOTTOM_LINE
    
    move.b #$80,d4
    move.l d7,a2
    sub #64,a2
    move #31,d3
    DIAGONAL_LEFT:
        move #7,d2
        CYCLE_8_L:
            move.b d4,(0,a0,a2.l)
            sub #64,a2
	        ror.b #1,d4
	        dbf d2,CYCLE_8_L
    	add #1,a2
        dbf d3,DIAGONAL_LEFT
        
    move.b #1,d4
    move.l d7,a2
    sub #1,a2
    move #31,d3
    DIAGONAL_RIGHT:
        move #7,d2
        CYCLE_8_R:
            move.b d4,(0,a0,a2.l)
            sub #64,a2
	        rol.b #1,d4
	        dbf d2,CYCLE_8_R
	    sub #1,a2
        dbf d3,DIAGONAL_RIGHT
    
    
    stop #0
    END START


*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
