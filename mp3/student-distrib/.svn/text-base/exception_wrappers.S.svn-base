# exception_wrappers.S
# These are wrappers that transition into the C exception handlers
# (calling an assembly label doesn't need the stack, but a C function does)


.text

.globl wrap_divide_by_zero_exception
.globl wrap_non_maskable_int_exception
.globl wrap_breakpoint_exception
.globl wrap_overflow_exception
.globl wrap_bounds_exceeded_exception
.globl wrap_invalid_opcode_exception
.globl wrap_device_not_available_exception
.globl wrap_double_fault_exception
.globl wrap_co_cpu_segment_overrun_exception
.globl wrap_invalid_TSS_exception
.globl wrap_segment_not_present_exception
.globl wrap_stack_segment_fault_exception
.globl wrap_general_protection_exception
.globl wrap_page_fault_exception
.globl wrap_x87_FPU_error_exception
.globl wrap_alignment_check_exception
.globl wrap_machine_check_exception
.globl wrap_SIMD_FPU_exception


wrap_divide_by_zero_exception:
	pushal
	call divide_by_zero_exception
	popal
	iret

wrap_non_maskable_int_exception:
	pushal
	call non_maskable_int_exception
	popal
	iret

wrap_breakpoint_exception:
	pushal
	call breakpoint_exception
	popal
	iret

wrap_overflow_exception:
	pushal
	call overflow_exception
	popal
	iret

wrap_bounds_exceeded_exception:
	pushal
	call bounds_exceeded_exception
	popal
	iret

wrap_invalid_opcode_exception:
	pushal
	call invalid_opcode_exception
	popal
	iret

wrap_device_not_available_exception:
	pushal
	call device_not_available_exception
	popal
	iret

wrap_double_fault_exception:
	pushal
	call double_fault_exception
	popal
	iret

wrap_co_cpu_segment_overrun_exception:
	pushal
	call co_cpu_segment_overrun_exception
	popal
	iret

wrap_invalid_TSS_exception:
	pushal
	call invalid_TSS_exception
	popal
	iret

wrap_segment_not_present_exception:
	pushal
	call segment_not_present_exception
	popal
	iret

wrap_stack_segment_fault_exception:
	pushal
	call stack_segment_fault_exception
	popal
	iret

wrap_general_protection_exception:
	pushal
	call general_protection_exception
	popal
	iret

wrap_page_fault_exception:
	pushal
	call page_fault_exception
	popal
	iret

wrap_x87_FPU_error_exception:
	pushal
	call x87_FPU_error_exception
	popal
	iret

wrap_alignment_check_exception:
	pushal
	call alignment_check_exception
	popal
	iret

wrap_machine_check_exception:
	pushal
	call machine_check_exception
	popal
	iret

wrap_SIMD_FPU_exception:
	pushal
	call SIMD_FPU_exception
	popal
	iret
