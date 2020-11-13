	.text
	.code 32

@ wait for interrupt
#define C7_CRM_WFI	 c0
#define C7_OP2_WFI	 4
@ clean/inval operations
#define C7_CRM_I_INVAL	 c5		@ invalidate instruction cache
#define C7_CRM_D_INVAL	 c6		@ invalidate data cache
#define C7_CRM_INVAL	 c7		@ invalidate both (reg = 0)
#define C7_OP2_INVAL	 0
#define C7_CRM_D_CLEAN	 c10		@ clean data cache
#define C7_CRM_D_CLEAN_INVAL	 c14	@ clean and invalidate data cache
@ OP2 for cache clean/inval operations
#define C7_OP2_ALL	 0	@ reg = 0
#define C7_OP2_ADDR	 1
#define C7_OP2_IDX	 2
@ data synchronization barrier
#define C7_CRM_DSB	 c10
#define C7_OP2_DSB	 4
@ data memory barrier
#define C7_CRM_DMB	 c10
#define C7_OP2_DMB	 5

@ vi:set filetype=armasm:
