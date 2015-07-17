
    ORG    $400000
    DC.L 0
    DC.L START

START:  
    add.l #0, d0 
    add.l #1, d1  
   
    add.l #8, d3
    move.l #2, d4
       GO:
          
        add.l d1, d0
        add.l d0, d1                       
        
        add.l #2, d4        
        cmp d3, d4        
        ble GO
        
        sub d3, d4
        cmp #0, d4
        bne F
        move d1, d7
        
        
        bra  C              
        F:        
        move d0, d7       
        C:
        END    START       






*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
