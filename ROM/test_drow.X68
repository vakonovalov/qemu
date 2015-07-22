     ORG    $400000
 * Control sum?
     DC.L    0
 * Address of the first instruction
     DC.L    START
 START:                  ; first instruction of program
 * Put program code here
     move.l  #$1A700, a0
     add.l #5460, a0
LABEL:
     cmp #16, d0
     beq L      
     move.b  #%11111111, (a0)     
     add.l #1, a0
     add.l #1, d0
     bra LABEL
 L:

     add.l  #47, a0  
     move.l #1, d0

 LABEL1:
     cmp #128, d0
     beq L1      
     move.b  #%00000001, (a0)     
     add.l #64, a0
     add.l #1, d0
     bra LABEL1
 L1:

     add.l  #1, a0 
     move.l #1, d0
     move.l #128, d2

 LABEL2:
     cmp #128, d0
     beq L2 
     move.b  d2, (a0) 

     ror.b #1, d2  
     cmp #128, d2
     beq K1
     bne K2 
 K1:
     sub.l  #63, a0
     bra K3
 K2:
     sub.l  #64, a0
     bra K3
 K3:     
     add.l #1, d0
     bra LABEL2
 L2:
     bra L2
     END    START        ; last line of source

























*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
