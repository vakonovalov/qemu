 ORG    $400000
 * Control sum?
     DC.L    0
 * Address of the first instruction
     DC.L    START
 START:                  ; first instruction of program
     move.l  #$1A700, a0
     move.l  #0, $800000
     move.l  $800000, d0  


L0:      
     move.b  #%00000000, (a0)     
     add.l #64, a0
     move.b  #%00111100, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)      
     add.l #64, a0
     move.b  #%00100100, (a0)       
     add.l #64, a0
     move.b  #%00100100, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)     
     add.l #64, a0
     move.b  #%00111100, (a0)       
     add.l #64, a0
     move.b  #%00000000, (a0)     
     sub.l  #447, a0
L1: 
     move.b  #%00000000, (a0)     
     add.l #64, a0
     move.b  #%00001000, (a0)     
     add.l #64, a0
     move.b  #%00011000, (a0)        
     add.l #64, a0
     move.b  #%00001000, (a0)       
     add.l #64, a0
     move.b  #%00001000, (a0)       
     add.l #64, a0
     move.b  #%00001000, (a0)       
     add.l #64, a0
     move.b  #%00011100, (a0)         
     add.l #64, a0
     move.b  #%00000000, (a0)     
     sub.l  #447, a0
L2: 
     move.b  #%00000000, (a0)     
     add.l #64, a0
     move.b  #%00111100, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)        
     add.l #64, a0
     move.b  #%00001000, (a0)       
     add.l #64, a0
     move.b  #%00010000, (a0)       
     add.l #64, a0
     move.b  #%00100000, (a0)       
     add.l #64, a0
     move.b  #%00111100, (a0)         
     add.l #64, a0
     move.b  #%00000000, (a0)     
     sub.l  #447, a0
L3: 
     move.b  #%00000000, (a0)     
     add.l #64, a0
     move.b  #%00111100, (a0)     
     add.l #64, a0
     move.b  #%00000100, (a0)        
     add.l #64, a0
     move.b  #%00000100, (a0)       
     add.l #64, a0
     move.b  #%00111100, (a0)       
     add.l #64, a0
     move.b  #%00000100, (a0)       
     add.l #64, a0
     move.b  #%00111100, (a0)         
     add.l #64, a0
     move.b  #%00000000, (a0)     
     sub.l  #447, a0
L4: 
     move.b  #%00000000, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)        
     add.l #64, a0
     move.b  #%00000100, (a0)       
     add.l #64, a0
     move.b  #%00111100, (a0)       
     add.l #64, a0
     move.b  #%00000100, (a0)       
     add.l #64, a0
     move.b  #%00000100, (a0)         
     add.l #64, a0
     move.b  #%00000000, (a0)     
     sub.l  #447, a0



     L:
     bra L
   END    START        ; last line of source




     




*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
