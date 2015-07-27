*-----------------------------------------------------------
* Title      :
* Written by :
* Date       :
* Description:
*-----------------------------------------------------------
    ORG    $400000
    DC.L    0
    DC.L    START

vBase EQU $EFE1FE 
vBufB EQU 512*0
vDirB EQU 512*2 

rTCData EQU 0
rTCClk EQU 1
rTCEnb EQU 2

 START: 
     move.l #$610000, sp
     move.b #%10000001, d0
     jsr SEND

     move.b #%10101010, d0
     jsr SEND

     RECEIVE_END:
     
     END_LABEL:
     bra END_LABEL
   
 SEND:
     bset #rTCEnb, vBase+vBufB
     move.b #7, d2
     ITER_SEND:
     move.b d0, d1
     lsr.b d2, d1    
     and.b #$01, d1
     bclr #rTCClk, vBase+vBufB
     and.b #$FE, vBase+vBufB
     or.b d1, vBase+vBufB
     bset #rTCClk, vBase+vBufB
     cmp.b #0, d2
     sub.b #1, d2
     bge ITER_SEND
     bclr #rTCClk, vBase+vBufB
     bclr #rTCEnb, vBase+vBufB 
     rts

 RECEIVE: *not work
     move.b #0, d0 
     bset #rTCEnb, vBase+vBufB
     bclr #rTCData, vBase+vBufB
     move.b #7, d2
     ITER_RECEIVE:
     bclr #rTCClk, vBase+vBufB
     move.b (vBase+vDirB), d1
     bset #rTCClk, vBase+vBufB
     and.b #$01, d1
     add.b d1, d0
     lsl.b #1, d0
     cmp.b #0, d2
     sub.b #1, d2
     bge ITER_RECEIVE
     bclr #rTCEnb, vBase+vBufB 
     bra RECEIVE_END

        
 END    START





















*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
