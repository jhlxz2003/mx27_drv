	.file	"asm-offsets.c"
@ GNU C version 4.1.1 (arm-926ejs-linux)
@	compiled by GNU C version 3.4.3.
@ GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
@ options passed:  -nostdinc -Iinclude -iprefix -D__KERNEL__
@ -D__LINUX_ARM_ARCH__=5 -Uarm -DKBUILD_STR(s)=#s
@ -DKBUILD_BASENAME=KBUILD_STR(asm_offsets)
@ -DKBUILD_MODNAME=KBUILD_STR(asm_offsets) -isystem -include -MD
@ -mlittle-endian -mapcs -mno-sched-prolog -mabi=apcs-gnu
@ -mno-thumb-interwork -march=armv5te -mtune=arm9tdmi -msoft-float
@ -auxbase-strip -Os -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs
@ -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-aliasing
@ -fno-common -fno-omit-frame-pointer -fno-omit-frame-pointer
@ -fno-optimize-sibling-calls -fno-stack-protector -fverbose-asm
@ options enabled:  -falign-loops -fargument-alias -fbranch-count-reg
@ -fcaller-saves -fcprop-registers -fcrossjumping -fcse-follow-jumps
@ -fcse-skip-blocks -fdefer-pop -fdelete-null-pointer-checks
@ -fearly-inlining -feliminate-unused-debug-types -fexpensive-optimizations
@ -ffunction-cse -fgcse -fgcse-lm -fguess-branch-probability -fident
@ -fif-conversion -fif-conversion2 -finline-functions
@ -finline-functions-called-once -fipa-pure-const -fipa-reference
@ -fipa-type-escape -fivopts -fkeep-static-consts -fleading-underscore
@ -floop-optimize -floop-optimize2 -fmath-errno -fmerge-constants
@ -foptimize-register-move -fpeephole -fpeephole2 -freg-struct-return
@ -fregmove -freorder-functions -frerun-cse-after-loop -frerun-loop-opt
@ -fsched-interblock -fsched-spec -fsched-stalled-insns-dep
@ -fschedule-insns -fschedule-insns2 -fshow-column -fsplit-ivs-in-unroller
@ -fstrength-reduce -fthread-jumps -ftrapping-math -ftree-ccp
@ -ftree-copy-prop -ftree-copyrename -ftree-dce -ftree-dominator-opts
@ -ftree-dse -ftree-fre -ftree-loop-im -ftree-loop-ivcanon
@ -ftree-loop-optimize -ftree-lrs -ftree-salias -ftree-sink -ftree-sra
@ -ftree-store-ccp -ftree-store-copy-prop -ftree-ter
@ -ftree-vect-loop-version -ftree-vrp -funit-at-a-time -fverbose-asm
@ -fzero-initialized-in-bss -mapcs-frame -mlittle-endian

@ Compiler executable checksum: 0b1c0bf26f1cfe55dc46b45e93c82099

	.text
	.align	2
	.global	main
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp	@,
	stmfd	sp!, {fp, ip, lr, pc}	@,
	sub	fp, ip, #4	@,,
#APP
	
->TSK_ACTIVE_MM #132 offsetof(struct task_struct, active_mm)	@
	
->
	
->TI_FLAGS #0 offsetof(struct thread_info, flags)	@
	
->TI_PREEMPT #4 offsetof(struct thread_info, preempt_count)	@
	
->TI_ADDR_LIMIT #8 offsetof(struct thread_info, addr_limit)	@
	
->TI_TASK #12 offsetof(struct thread_info, task)	@
	
->TI_EXEC_DOMAIN #16 offsetof(struct thread_info, exec_domain)	@
	
->TI_CPU #20 offsetof(struct thread_info, cpu)	@
	
->TI_CPU_DOMAIN #24 offsetof(struct thread_info, cpu_domain)	@
	
->TI_CPU_SAVE #28 offsetof(struct thread_info, cpu_context)	@
	
->TI_USED_CP #76 offsetof(struct thread_info, used_cp)	@
	
->TI_TP_VALUE #92 offsetof(struct thread_info, tp_value)	@
	
->TI_FPSTATE #280 offsetof(struct thread_info, fpstate)	@
	
->TI_VFPSTATE #420 offsetof(struct thread_info, vfpstate)	@
	
->
	
->S_R0 #0 offsetof(struct pt_regs, ARM_r0)	@
	
->S_R1 #4 offsetof(struct pt_regs, ARM_r1)	@
	
->S_R2 #8 offsetof(struct pt_regs, ARM_r2)	@
	
->S_R3 #12 offsetof(struct pt_regs, ARM_r3)	@
	
->S_R4 #16 offsetof(struct pt_regs, ARM_r4)	@
	
->S_R5 #20 offsetof(struct pt_regs, ARM_r5)	@
	
->S_R6 #24 offsetof(struct pt_regs, ARM_r6)	@
	
->S_R7 #28 offsetof(struct pt_regs, ARM_r7)	@
	
->S_R8 #32 offsetof(struct pt_regs, ARM_r8)	@
	
->S_R9 #36 offsetof(struct pt_regs, ARM_r9)	@
	
->S_R10 #40 offsetof(struct pt_regs, ARM_r10)	@
	
->S_FP #44 offsetof(struct pt_regs, ARM_fp)	@
	
->S_IP #48 offsetof(struct pt_regs, ARM_ip)	@
	
->S_SP #52 offsetof(struct pt_regs, ARM_sp)	@
	
->S_LR #56 offsetof(struct pt_regs, ARM_lr)	@
	
->S_PC #60 offsetof(struct pt_regs, ARM_pc)	@
	
->S_PSR #64 offsetof(struct pt_regs, ARM_cpsr)	@
	
->S_OLD_R0 #68 offsetof(struct pt_regs, ARM_ORIG_r0)	@
	
->S_FRAME_SIZE #72 sizeof(struct pt_regs)	@
	
->
	
->VMA_VM_MM #0 offsetof(struct vm_area_struct, vm_mm)	@
	
->VMA_VM_FLAGS #20 offsetof(struct vm_area_struct, vm_flags)	@
	
->
	
->VM_EXEC #4 VM_EXEC	@
	
->
	
->PAGE_SZ #4096 PAGE_SIZE	@
	
->
	
->SYS_ERROR0 #10420224 0x9f0000	@
	
->
	
->SIZEOF_MACHINE_DESC #52 sizeof(struct machine_desc)	@
	
->MACHINFO_TYPE #0 offsetof(struct machine_desc, nr)	@
	
->MACHINFO_NAME #12 offsetof(struct machine_desc, name)	@
	
->MACHINFO_PHYSIO #4 offsetof(struct machine_desc, phys_io)	@
	
->MACHINFO_PGOFFIO #8 offsetof(struct machine_desc, io_pg_offst)	@
	
->
	
->PROC_INFO_SZ #52 sizeof(struct proc_info_list)	@
	
->PROCINFO_INITFUNC #16 offsetof(struct proc_info_list, __cpu_flush)	@
	
->PROCINFO_MM_MMUFLAGS #8 offsetof(struct proc_info_list, __cpu_mm_mmu_flags)	@
	
->PROCINFO_IO_MMUFLAGS #12 offsetof(struct proc_info_list, __cpu_io_mmu_flags)	@
	mov	r0, #0	@ <result>,
	ldmfd	sp, {fp, sp, pc}	@
	.size	main, .-main
	.ident	"GCC: (GNU) 4.1.1"
