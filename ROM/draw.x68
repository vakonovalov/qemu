
    ORG    $400000
    DC.L 0
    DC.L START
    

START:  

        move.l #$1A700, a0
        move.l #0, d4
        move.l #0, d5


        H:
        move.b #$FF, (a0)  
        add.l #1, a0
	    add.l #1, d4
        cmp #10, d4
        blt H        
                  
        C:      
        add.l #54, a0       
        move.b #128, (a0)        
        add.l #10, a0
        move.b #128, (a0)
        add.l #1, d5
        cmp #80, d5 
        blt C
       
        move.l #0, d4
        add.l #54, a0
        B:
        move.b #$FF, (a0)
        add.l #1, a0
        add.l #1, d4
        cmp #10, d4
        blt B
        
        move.l #$1A700, a0

        move #0, d5
        D:
        add.l #64, a0 

       move.l #128, (a0)                
        add.b #1, d5              
        cmp #6, d5              
        blt D              
        stop #0       
        END    START 





*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
