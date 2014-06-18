#include "exception.h"

#define EXCEPTION_CODE 256

/*
 * asm volatile(".1: hlt; jmp .1;");  is equivalent to a while(1) loop
 */



/*
 * divide_by_zero_exception
 *   DESCRIPTION: Exception when attempted divide by zero
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Halts the process that caused the exception and
 *				   sends 256 to be returned by halt.
 */ 
void divide_by_zero_exception()
{
//	clear();
	printf("Divide by zero exception\n");
	sys_halt((uint8_t)EXCEPTION_CODE);
	// asm volatile(".1: hlt; jmp .1;");
}

/*
 * non_maskable_int_exception
 *   DESCRIPTION: Exception when attempting to interrupt a NMI
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Halts the process that caused the exception and
 *				   sends 256 to be returned by halt.
 */ 
void non_maskable_int_exception()
{
//	clear();
	printf("Non-maskable interrupts exception\n");
	sys_halt((uint8_t)EXCEPTION_CODE);
	// asm volatile(".2: hlt; jmp .2;");
}

/*
 * breakpoint_exception
 *   DESCRIPTION: Exception when hitting a breakpoint
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Halts the process that caused the exception and
 *				   sends 256 to be returned by halt.
 */ 
void breakpoint_exception()
{
//	clear();
	printf("Breakpoint exception\n");
	sys_halt((uint8_t)EXCEPTION_CODE);
	// asm volatile(".3: hlt; jmp .3;");
}

/*
 * overflow_exception
 *   DESCRIPTION: Exception when an overflow occurs
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Halts the process that caused the exception and
 *				   sends 256 to be returned by halt.
 */ 
void overflow_exception()
{
//	clear();
	printf("Overflow exception\n");
	sys_halt((uint8_t)EXCEPTION_CODE);
	// asm volatile(".4: hlt; jmp .4;");
}

/*
 * bounds_exceeded_exception
 *   DESCRIPTION: Exception when you have exceeded your bounds
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Halts the process that caused the exception and
 *				   sends 256 to be returned by halt.
 */ 
void bounds_exceeded_exception()
{
	//clear();
	printf("Bounds exceeded exception\n");
	sys_halt((uint8_t)EXCEPTION_CODE);
	// asm volatile(".5: hlt; jmp .5;");
}

/*
 * invalid_opcode_exception
 *   DESCRIPTION: Exception when an invalid opcode is received
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Halts the process that caused the exception and
 *				   sends 256 to be returned by halt.
 */ 
void invalid_opcode_exception()
{
//	clear();
	printf("Invalid opcode exception\n");
	sys_halt((uint8_t)EXCEPTION_CODE);
	// asm volatile(".6: hlt; jmp .6;");
}

/*
 * device_not_available_exception
 *   DESCRIPTION: Exception when the device is just not available (don't call, she's not interested)
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Halts the process that caused the exception and
 *				   sends 256 to be returned by halt.
 */ 
void device_not_available_exception()
{
	//clear();
	printf("Device not available exception\n");
	sys_halt((uint8_t)EXCEPTION_CODE);
	// asm volatile(".7: hlt; jmp .7;");
}

/*
 * double_fault_exception
 *   DESCRIPTION: Exception when the first fault faults. If the first fault faults,
 *				  another fault is likely to fault.  It's your fault.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Halts the process that caused the exception and
 *				   sends 256 to be returned by halt.
 */ 
void double_fault_exception()
{
//	clear();
	printf("Double fault exception\n");
	sys_halt((uint8_t)EXCEPTION_CODE);
	// asm volatile(".8: hlt; jmp .8;");
}

/*
 * co_cpu_segment_overrun_exception
 *   DESCRIPTION: Exception when there's a segment overrun on co cpu
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Halts the process that caused the exception and
 *				   sends 256 to be returned by halt.
 */ 
void co_cpu_segment_overrun_exception()
{
//	clear();
	printf("Co-processor segment overrun exception\n");
	sys_halt((uint8_t)EXCEPTION_CODE);
	// asm volatile(".9: hlt; jmp .9;");
}

/*
 * invalid_TSS_exception
 *   DESCRIPTION: Exception when there's an invalid Task State Segment
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Halts the process that caused the exception and
 *				   sends 256 to be returned by halt.
 */ 
void invalid_TSS_exception()
{
	//clear();
	printf("Invalid Task State Segment exception\n");
	sys_halt((uint8_t)EXCEPTION_CODE);
	// asm volatile(".10: hlt; jmp .10;");
}

/*
 * segment_not_present_exception
 *   DESCRIPTION: Exception when segment is not present
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Halts the process that caused the exception and
 *				   sends 256 to be returned by halt.
 */ 
void segment_not_present_exception()
{
	//clear();
	printf("Segment not present exception\n");
	sys_halt((uint8_t)EXCEPTION_CODE);
	// asm volatile(".11: hlt; jmp .11;");
}

/*
 * stack_segment_fault_exception
 *   DESCRIPTION: Exception when a stack segment fault occurs
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Halts the process that caused the exception and
 *				   sends 256 to be returned by halt.
 */ 
void stack_segment_fault_exception()
{
	//clear();
	printf("Stack segment fault exception\n");
	sys_halt((uint8_t)EXCEPTION_CODE);
	// asm volatile(".12: hlt; jmp .12;");
}

/*
 * general_protection_exception
 *   DESCRIPTION: Exception when there's a permissions violation (usually).
 *				  Or attempting to write to read-only.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Halts the process that caused the exception and
 *				   sends 256 to be returned by halt.
 */ 
void general_protection_exception()
{
	//clear();
	printf("General protection exception\n");
	sys_halt((uint8_t)EXCEPTION_CODE);
	// asm volatile(".13: hlt; jmp .13;");
}

/*
 * page_fault_exception
 *   DESCRIPTION: Exception when accessing memory that is not mapped by current process
 *				  Our favorite and most occurring exception!
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Halts the process that caused the exception and
 *				   sends 256 to be returned by halt.
 */ 
void page_fault_exception()
{
	//clear();
	int CR2, CR3;
	asm volatile("MOVL %%CR2, %%EAX; MOVL %%EAX, %0" : "=r"(CR2));
	asm volatile("MOVL %%CR3, %%EBX; MOVL %%EBX, %0" : "=r"(CR3));
	printf("CR3: %x     CR2: %x\n", CR3, CR2);
	printf("Page fault \n");

	sys_halt((uint8_t)EXCEPTION_CODE);

	// asm volatile(".14: hlt; jmp .14;");

}

/*
 * x87_FPU_error_exception
 *   DESCRIPTION: Exception when an x87 FPU error occurs
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Halts the process that caused the exception and
 *				   sends 256 to be returned by halt.
 */ 
void x87_FPU_error_exception()
{
	//clear();
	printf("x87 FPU error exception\n");
	sys_halt((uint8_t)EXCEPTION_CODE);
	// asm volatile(".15: hlt; jmp .15;");
}

/*
 * alignment_check_exception
 *   DESCRIPTION: Exception when things alignment check fails
 *				  (i.e. data memory size at an addr location not aligned with that size)
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Halts the process that caused the exception and
 *				   sends 256 to be returned by halt.
 */ 
void alignment_check_exception()
{
	//clear();
	printf("Alignment check exception\n");
	sys_halt((uint8_t)EXCEPTION_CODE);
	// asm volatile(".16: hlt; jmp .16;");
}

/*
 * machine_check_exception
 *   DESCRIPTION: Exception when machine check fails
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Halts the process that caused the exception and
 *				   sends 256 to be returned by halt.
 */ 
void machine_check_exception()
{
	//clear();
	printf("Machine check exception\n");
	sys_halt((uint8_t)EXCEPTION_CODE);
	// asm volatile(".17: hlt; jmp .17;");
}

/*
 * SIMD_FPU_exception
 *   DESCRIPTION: Exception when SIMD FPU occurs...
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: Halts the process that caused the exception and
 *				   sends 256 to be returned by halt.
 */ 
void SIMD_FPU_exception()
{
	//clear();
	printf("SIMD FPU exception\n");
	sys_halt((uint8_t)EXCEPTION_CODE);
	// asm volatile(".18: hlt; jmp .18;");
}

