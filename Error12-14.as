.entry X
.extern W
.entry W

.extern END
.entry END

add r4, r5
mov r0, r7
sub #4.8596, r5
prn #1.894

END: hlt
X: .data 8, 1.00, 7.394
Y: .string "ab"
X: .data 256 
END: hlt
