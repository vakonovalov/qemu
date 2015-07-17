
 ORG    $400000
 * Control sum?
     DC.L    0
 * Address of the first instruction
     DC.L    START
 START:                  ; first instruction of program
     S:
     move.l #0, d1
     move.l #28, d2  

     move.l  #$1A700, a0
     *move.l  #0, $800000 
    * move.l  #15, $800004 
     move.l  $800000, d0 
     bra PRINT
     Q:

     bra S
     bra L
     
PRINT:
     move.l d0, d3
     lsl.l d1, d3 
     lsr.l d1, d3 
     lsr.l d2, d3 
     bra CHECK 
     E:
     sub.l #4, d2
     add.l #4, d1
     cmp #0, d2
     bge PRINT
     bra Q

CHECK:
     cmp #$0, d3
     beq L0
     cmp #$1, d3
     beq L1
     cmp #$2, d3
     beq L2
     cmp #$3, d3
     beq L3
     cmp #$4, d3
     beq L4
     cmp #$5, d3
     beq L5
     cmp #$6, d3
     beq L6
     cmp #$7, d3
     beq L7
     cmp #$8, d3
     beq L8
     cmp #$9, d3
     beq L9
     cmp #$A, d3
     beq LA
     cmp #$B, d3
     beq LB
     cmp #$C, d3
     beq LC
     cmp #$D, d3
     beq LD
     cmp #$E, d3
     beq LE
     cmp #$F, d3
     beq LF
L0:      
     move.b  #%00000000, (a0)     
     add.l #64, a0
     move.b  #%00011000, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)      
     add.l #64, a0
     move.b  #%00100100, (a0)       
     add.l #64, a0
     move.b  #%00100100, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)       
     add.l #64, a0
     move.b  #%00011000, (a0)   
     add.l #64, a0
     move.b  #%00000000, (a0)  
     sub.l  #511, a0
     bra E
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
     move.b  #%00001000, (a0)       
     add.l #64, a0
     move.b  #%00011100, (a0)   
     add.l #64, a0
     move.b  #%00000000, (a0)  
     sub.l  #511, a0
     bra E
L2:      
     move.b  #%00000000, (a0)     
     add.l #64, a0
     move.b  #%00011000, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)      
     add.l #64, a0
     move.b  #%00000100, (a0)       
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
     sub.l  #511, a0
     bra E
L3:      
     move.b  #%00000000, (a0)     
     add.l #64, a0
     move.b  #%00111100, (a0)     
     add.l #64, a0
     move.b  #%00001000, (a0)      
     add.l #64, a0
     move.b  #%00010000, (a0)       
     add.l #64, a0
     move.b  #%00111000, (a0)     
     add.l #64, a0
     move.b  #%00000100, (a0)     
     add.l #64, a0
     move.b  #%00000100, (a0)       
     add.l #64, a0
     move.b  #%00111000, (a0)   
     add.l #64, a0
     move.b  #%00000000, (a0)  
     sub.l  #511, a0
     bra E
L4:      
     move.b  #%00000000, (a0)     
     add.l #64, a0
     move.b  #%00000100, (a0)     
     add.l #64, a0
     move.b  #%00001100, (a0)      
     add.l #64, a0
     move.b  #%00010100, (a0)       
     add.l #64, a0
     move.b  #%00100100, (a0)     
     add.l #64, a0
     move.b  #%00111110, (a0)     
     add.l #64, a0
     move.b  #%00000100, (a0)       
     add.l #64, a0
     move.b  #%00000100, (a0)   
     add.l #64, a0
     move.b  #%00000000, (a0)  
     sub.l  #511, a0
     bra E
L5:      
     move.b  #%00000000, (a0)     
     add.l #64, a0
     move.b  #%00111100, (a0)     
     add.l #64, a0
     move.b  #%00100000, (a0)      
     add.l #64, a0
     move.b  #%00100000, (a0)       
     add.l #64, a0
     move.b  #%00111000, (a0)     
     add.l #64, a0
     move.b  #%00000100, (a0)     
     add.l #64, a0
     move.b  #%00000100, (a0)       
     add.l #64, a0
     move.b  #%00111000, (a0)   
     add.l #64, a0
     move.b  #%00000000, (a0)  
     sub.l  #511, a0
     bra E
L6:      
     move.b  #%00000000, (a0)     
     add.l #64, a0
     move.b  #%00011100, (a0)     
     add.l #64, a0
     move.b  #%00100000, (a0)      
     add.l #64, a0
     move.b  #%00100000, (a0)       
     add.l #64, a0
     move.b  #%00111000, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)       
     add.l #64, a0
     move.b  #%00011000, (a0)   
     add.l #64, a0
     move.b  #%00000000, (a0)  
     sub.l  #511, a0
     bra E
L7:      
     move.b  #%00000000, (a0)     
     add.l #64, a0
     move.b  #%00111100, (a0)     
     add.l #64, a0
     move.b  #%00000100, (a0)      
     add.l #64, a0
     move.b  #%00001000, (a0)       
     add.l #64, a0
     move.b  #%00010000, (a0)     
     add.l #64, a0
     move.b  #%00100000, (a0)     
     add.l #64, a0
     move.b  #%00100000, (a0)       
     add.l #64, a0
     move.b  #%00100000, (a0)   
     add.l #64, a0
     move.b  #%00000000, (a0)  
     sub.l  #511, a0
     bra E
L8:      
     move.b  #%00000000, (a0)     
     add.l #64, a0
     move.b  #%00011000, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)      
     add.l #64, a0
     move.b  #%00100100, (a0)       
     add.l #64, a0
     move.b  #%00011000, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)       
     add.l #64, a0
     move.b  #%00011000, (a0)   
     add.l #64, a0
     move.b  #%00000000, (a0)  
     sub.l  #511, a0
     bra E
L9:      
     move.b  #%00000000, (a0)     
     add.l #64, a0
     move.b  #%00011000, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)      
     add.l #64, a0
     move.b  #%00100100, (a0)       
     add.l #64, a0
     move.b  #%00011100, (a0)     
     add.l #64, a0
     move.b  #%00000100, (a0)     
     add.l #64, a0
     move.b  #%00000100, (a0)       
     add.l #64, a0
     move.b  #%00111000, (a0)   
     add.l #64, a0
     move.b  #%00000000, (a0)  
     sub.l  #511, a0
     bra E
LA:      
     move.b  #%00000000, (a0)     
     add.l #64, a0
     move.b  #%00001000, (a0)     
     add.l #64, a0
     move.b  #%00010100, (a0)      
     add.l #64, a0
     move.b  #%00010100, (a0)       
     add.l #64, a0
     move.b  #%00100010, (a0)     
     add.l #64, a0
     move.b  #%00111110, (a0)     
     add.l #64, a0
     move.b  #%00100010, (a0)       
     add.l #64, a0
     move.b  #%00100010, (a0)   
     add.l #64, a0
     move.b  #%00000000, (a0)  
     sub.l  #511, a0
     bra E
LB:      
     move.b  #%00000000, (a0)     
     add.l #64, a0
     move.b  #%00111000, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)      
     add.l #64, a0
     move.b  #%00100100, (a0)       
     add.l #64, a0
     move.b  #%00111000, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)       
     add.l #64, a0
     move.b  #%00111000, (a0)   
     add.l #64, a0
     move.b  #%00000000, (a0)  
     sub.l  #511, a0
     bra E
LC:      
     move.b  #%00000000, (a0)     
     add.l #64, a0
     move.b  #%00011100, (a0)     
     add.l #64, a0
     move.b  #%00100000, (a0)      
     add.l #64, a0
     move.b  #%00100000, (a0)       
     add.l #64, a0
     move.b  #%00100000, (a0)     
     add.l #64, a0
     move.b  #%00100000, (a0)     
     add.l #64, a0
     move.b  #%00100000, (a0)       
     add.l #64, a0
     move.b  #%00011100, (a0)   
     add.l #64, a0
     move.b  #%00000000, (a0)  
     sub.l  #511, a0
     bra E
LD:      
     move.b  #%00000000, (a0)     
     add.l #64, a0
     move.b  #%00111000, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)      
     add.l #64, a0
     move.b  #%00100100, (a0)       
     add.l #64, a0
     move.b  #%00100100, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)     
     add.l #64, a0
     move.b  #%00100100, (a0)       
     add.l #64, a0
     move.b  #%00111000, (a0)   
     add.l #64, a0
     move.b  #%00000000, (a0)  
     sub.l  #511, a0
     bra E
LE:      
     move.b  #%00000000, (a0)     
     add.l #64, a0
     move.b  #%00111100, (a0)     
     add.l #64, a0
     move.b  #%00100000, (a0)      
     add.l #64, a0
     move.b  #%00100000, (a0)       
     add.l #64, a0
     move.b  #%00111000, (a0)     
     add.l #64, a0
     move.b  #%00100000, (a0)     
     add.l #64, a0
     move.b  #%00100000, (a0)       
     add.l #64, a0
     move.b  #%00111100, (a0)   
     add.l #64, a0
     move.b  #%00000000, (a0)  
     sub.l  #511, a0
     bra E
LF:      
     move.b  #%00000000, (a0)     
     add.l #64, a0
     move.b  #%00111100, (a0)     
     add.l #64, a0
     move.b  #%00100000, (a0)      
     add.l #64, a0
     move.b  #%00100000, (a0)       
     add.l #64, a0
     move.b  #%00111000, (a0)     
     add.l #64, a0
     move.b  #%00100000, (a0)     
     add.l #64, a0
     move.b  #%00100000, (a0)       
     add.l #64, a0
     move.b  #%00100000, (a0)   
     add.l #64, a0
     move.b  #%00000000, (a0)  
     sub.l  #511, a0
     bra E

     L:
     bra L
   END    START        ; last line of source



     




























*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
