vBufB EQU $EFE1FE
vBufA EQU $EFFFFE

    ORG $400000
    DC.L 0
    DC.L MAIN
    
MAIN:
    move.b #0,(vBufA)
    move.l #INTERRUPT,$64

    move.l #$010000,sp
    move.l #$01A700,a0
    lea N0,a1

    move.l #$01,d0
    bsr SEND_VIA
    move.l #$fa,d0
    bsr SEND_VIA

    move.l #$05,d0
    bsr SEND_VIA
    move.l #$fb,d0
    bsr SEND_VIA

    move.l #$09,d0
    bsr SEND_VIA
    move.l #$fc,d0
    bsr SEND_VIA

    move.l #$0d,d0
    bsr SEND_VIA
    move.l #$fd,d0
    bsr SEND_VIA

    move.l #$35,d0
    bsr SEND_VIA
    move.l #$ff,d0
    bsr SEND_VIA

    move.l #$01,d0
    bsr SEND_VIA
    move.l #$0,d0
    bsr SEND_VIA

    move.l #$35,d0
    bsr SEND_VIA
    move.l #$00,d0
    bsr SEND_VIA

    move.l #$01,d0
    bsr SEND_VIA
    move.l #$f0,d0
    bsr SEND_VIA
    
LALAL:
    stop #$2000

    bra LALAL

SEND_VIA:
    move.l #7,d2
.LOOP_SEND_VIA:
    move.b d0,d1
    ror.b d2,d1
    and.b #1,d1
    move.b d1,(vBufB)
    or.b #2,d1
    move.b d1,(vBufB)

    dbf d2,.LOOP_SEND_VIA
    rts

RECEIVE_VIA:
    move.l #0,d0
    move.l #7,d2
.LOOP_RECEIVE_VIA:
    move.b #$02,(vBufB)
    move.b #$00,(vBufB)
    move.b (vBufB),d1
    and.b #$1,d1
    rol.b #1,d0
    or.b d1,d0

    dbf d2,.LOOP_RECEIVE_VIA
    rts

PRINT:
    move #7,d3

    move.l d1,d6
    mulu #64,d6
    add.l d0,d6
    move.l d6,a2

    move.l d2,d6
    mulu #8,d6
    move.l d6,a3
.LOOP_PRINT
    move.b (a1,a3),d6
    move.b d6,(0,a0,a2.l)
    add #1,a3
    add #64,a2
    dbf d3,.LOOP_PRINT
    rts

PRINT_REGISTER:
    move.l d4,d5
    move.l #0,d1
    move.l #0,d0
    move.l #7,d7
.LOOP_PR_REGISTER:
    rol.l #4,d5
    move.l d5,d6
    and #$f,d6

    move.l d6,d2
    bsr PRINT
    add #1,d0
    
    dbf d7,.LOOP_PR_REGISTER
    rts

INTERRUPT:
    move.l #$8d,d0
    bsr SEND_VIA
    bsr RECEIVE_VIA
    move.b d0,d4
    lsl.l #8,d4

    move.l #$89,d0
    bsr SEND_VIA
    bsr RECEIVE_VIA
    move.b d0,d4
    lsl.l #8,d4

    move.l #$85,d0
    bsr SEND_VIA
    bsr RECEIVE_VIA
    move.b d0,d4
    lsl.l #8,d4

    move.l #$81,d0
    bsr SEND_VIA
    bsr RECEIVE_VIA
    move.b d0,d4
    
    bsr PRINT_REGISTER
    rte

NUMBERS:
N0 DC.B $18,$24,$42,$42,$42,$42,$24,$18
N1 DC.B $04,$0c,$14,$24,$04,$04,$04,$04
N2 DC.B $18,$24,$44,$04,$08,$10,$20,$7e
N3 DC.B $1c,$22,$02,$0c,$02,$02,$22,$1c
N4 DC.B $42,$42,$42,$42,$7e,$02,$02,$02
N5 DC.B $3e,$20,$20,$38,$04,$02,$02,$3c
N6 DC.B $06,$08,$10,$38,$24,$22,$22,$1c
N7 DC.B $3e,$22,$02,$04,$04,$1e,$08,$08
N8 DC.B $1c,$22,$22,$1c,$1c,$22,$22,$1c
N9 DC.B $1c,$22,$22,$1e,$02,$02,$04,$38
NA DC.B $18,$24,$42,$42,$7e,$42,$42,$42
NB DC.B $78,$44,$44,$78,$44,$42,$42,$7c
NC DC.B $1c,$22,$40,$40,$40,$40,$22,$1c
ND DC.B $78,$44,$42,$42,$42,$42,$44,$78
NE DC.B $7e,$40,$40,$7c,$40,$40,$40,$7e
NF DC.B $7e,$40,$40,$78,$40,$40,$40,$40

    END MAIN

*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
