# system_linkage.S - links C system calls and file descriptor by file operations pointer

.text

.globl sys_call_handler

# Call number is placed in EAX, the first argument in EBX, then
# ECX, and finally EDX
sys_call_handler:
	
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %ecx
	pushl %ebx

	# check if system call index is in range [1,10]
	cmpl $11, %eax
	jge sys_fail
	cmpl $0, %eax
	jle sys_fail 

	# push all parameters
	pushl %edx
	pushl %ecx
	pushl %ebx

	call *jump_n_fun(,%eax,4)
	# pop parameters
	addl $12, %esp

	# return here after C function
	jmp sys_done


	# not found, error return -1
sys_fail:
	movl $-1, %eax

sys_done:
	popl %ebx
	popl %ecx
	popl %esi
	popl %edi
	popl %ebp

	iret


# system calls jump table to C functions
jump_n_fun:
.long dummy_offset, sys_halt, sys_execute, sys_read, sys_write, sys_open, sys_close, sys_getargs, sys_vidmap, sys_set_handler, sys_sigreturn

dummy_offset:
	jmp sys_fail
