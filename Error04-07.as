.entry X
.entry Y
.extern W;

MAIN: add r1,Y
sub X, #4
inc #9584
mov r1, r7
lea r3, X

add X, Y sub Y,X
mov ;r1, r0
rts hlt

jsr: .data 8
r5: .string "a"
X: .data 7
Y: .string "abcd"
