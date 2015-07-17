    ORG $400000
    DC.L 0
    DC.L MAIN
    
MAIN:
    move.l #$010000,sp
    move.l #$01A700,a0    
    lea N0,a1

    move.b #0,d0
    move.b #0,d1
    move.b #0,d2

.PRINT_ALL_NUMBERS:
    bsr PRINT
    add #1,d0
    add #1,d2
    cmp #16,d2
    bne .PRINT_ALL_NUMBERS
 
    move.l #$800000,a6
    *move.l #$50000000,d4    
    *move.l d4,4(a6)
    LALAL:
    move.l (a6),d4
    
    bsr PRINT_REGISTER
    bra LALAL

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
    move.l #9,d1
    move.l #0,d0
    move.l #7,d7
.LOOP_PR_REGISTER:
    rol.l #4,d5
    move.l d5,d6
    and #$f,d6

    move.b d6,d2
    bsr PRINT
    add #1,d0
    
    cmp.l d4,d5
    dbf d7,.LOOP_PR_REGISTER
    rts


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
