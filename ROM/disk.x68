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

dBase EQU $DFE1FF
phOL EQU 512*0		;CAO off (0)
phOH EQU 512*1 		;CAO on (1)
ph1L EQU 512*2 		;CA1 off (0)
ph1H EQU 512*3 		;CA1 on (1)
ph2L EQU 512*4 		;CA2 off (0)
ph2H EQU 512*5		;CA2 on (1) 
ph3L EQU 512*6 		;LSTRB off (low)
ph3H EQU 512*7 		;LSTRB on (high) 
mtrOff EQU 512*8    ;disk enable off
mtrOn EQU 512*9     ;disk enable on 
intDrive EQU 512*10 ;select internal drive
extDrive EQU 512*11 ;select external drive 
q6L EQU 512*12      ;Q6 off
q6H EQU 512*13      ;Q6 on
q7L EQU 512*14      ;Q7 off
q7H EQU 512*15      ;Q7 on 

 START: 
    and.b #$EF, vBase+vBufA
    move.l #$10000, sp

    bset #1, dBase+ph3H
    bset #0, dBase+ph2L

    END_LABEL:
    bra END_LABEL     
 END    START

*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
