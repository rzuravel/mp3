#ifndef _EXCEPTION_H
#define _EXCEPTION_H

#include "types.h"
#include "lib.h"
#include "system_calls.h"

/*
 *	These C functions handle exceptions 0-19 (except #1 and #15) as defined by Intel.
 *  These functions are called from assembly context.
 *
 *
*/


extern void divide_by_zero_exception(void);

extern void non_maskable_int_exception(void);

extern void breakpoint_exception(void);

extern void overflow_exception(void);

extern void bounds_exceeded_exception(void);

extern void invalid_opcode_exception(void);

extern void device_not_available_exception(void);

extern void double_fault_exception(void);

extern void co_cpu_segment_overrun_exception(void);

extern void invalid_TSS_exception(void);

extern void segment_not_present_exception(void);

extern void stack_segment_fault_exception(void);

extern void general_protection_exception(void);

extern void page_fault_exception(void);

extern void x87_FPU_error_exception(void);

extern void alignment_check_exception(void);

extern void machine_check_exception(void);

extern void SIMD_FPU_exception(void);




#endif

