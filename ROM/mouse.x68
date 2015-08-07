*-----------------------------------------------------------
* Title      :
* Written by :
* Date       :
* Description:
*-----------------------------------------------------------
    ORG    $400000
    DC.L    0
    DC.L    START

monitor EQU $1A700
vBase EQU $EFE1FE 
vBufB EQU 512*0
vDirB EQU 512*2 
vBufA EQU 512*15

rTCData EQU 0
rTCClk EQU 1
rTCEnb EQU 2

COURSOR DC.W $8000, $C000, $A000, $9000, $8800, $8400, $8200, $8100, $8080, $8040, $81E0

 START: 
    and.b #$EF, vBase+vBufA
    move.l #$10000, sp
    
    jsr PRINT_COURSOR
     
    END_LABEL:
    bra END_LABEL

 PRINT_COURSOR:
    move.l #COURSOR, a1
    move.l d0, a0
    add.l #monitor, a0
    move.l #0, d1
    PRINT_COURSOR_ITER:
    move.w (a1)+, (a0)
    add.l #64, a0
    add.l #1, d1
    cmp #11, d1
    bne PRINT_COURSOR_ITER
    rts
        
 END    START



*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
