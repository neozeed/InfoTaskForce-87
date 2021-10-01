#NO_APP
gcc_compiled.:
.text
	.align 1
.globl _start
_start:
	enter [],0
#APP
#       setting the stack 256k under 8MB
        lprd sp,0x7c0000
	jsr _main
#NO_APP
L1:
	exit []
#       setting the stack 256k under 8MB
        lprd sp,0x7c0000
        bpt
	.align 1

#does nothing
.globl ___main
___main:
	ret 0

.globl _exit
_exit:
	bpt
	ret 0


