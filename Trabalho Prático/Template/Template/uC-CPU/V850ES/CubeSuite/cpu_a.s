#********************************************************************************************************
#                                                uC/CPU
#                                    CPU CONFIGURATION & PORT LAYER
#
#                          (c) Copyright 2004-2011; Micrium, Inc.; Weston, FL
#
#               All rights reserved.  Protected by international copyright laws.
#
#               uC/CPU is provided in source form to registered licensees ONLY.  It is 
#               illegal to distribute this source code to any third party unless you receive 
#               written permission by an authorized Micrium representative.  Knowledge of 
#               the source code may NOT be used to develop a similar product.
#
#               Please help us continue to provide the Embedded community with the finest 
#               software available.  Your honesty is greatly appreciated.
#
#               You can contact us at www.micrium.com.
#********************************************************************************************************

#********************************************************************************************************
#
#                                            CPU PORT FILE
#
#                                               V850ES
#                                            Renesas CA850
#
# Filename      : cpu_a.asm            
# Version       : V1.29.00.00                
# Programmer(s) : ITJ                  
#                 FF
#********************************************************************************************************



#********************************************************************************************************
#                                           PUBLIC FUNCTIONS
#********************************************************************************************************

          
    .extern  _CPU_SR_Save
    .extern  _CPU_SR_Restore
    .extern  _CPU_IntDis
    .extern  _CPU_IntEn
    .extern  _CPU_ECR_Rd


#********************************************************************************************************
#                                                EQUATES
#********************************************************************************************************

    .set    ECR, 4
    .set    PSW, 5                             

#********************************************************************************************************
#                                      CODE GENERATION DIRECTIVES
#********************************************************************************************************

    .text
    .align  4                                

#$PAGE
#********************************************************************************************************
#                                  SAVE/RESTORE CPU STATUS REGISTER
#
# Description : Save/Restore the state of CPU interrupts, if possible.
#
#               (1) (c) For CPU_CRITICAL_METHOD_STATUS_LOCAL, the state of the interrupt status flag is
#                       stored in the local variable 'cpu_sr' & interrupts are then disabled ('cpu_sr' is
#                       allocated in all functions that need to disable interrupts).  The previous interrupt
#                       status state is restored by copying 'cpu_sr' into the CPU's status register.
#
#
# Prototypes  : CPU_SR  CPU_SR_Save   (void);
#               void    CPU_SR_Restore(CPU_SR  cpu_sr);
#
# Note(s)     : (1) These functions are used in general like this :
#
#                       void  Task (void  *p_arg)
#                       {
#                           CPU_SR_ALLOC();                     /* Allocate storage for CPU status register */
#                               :
#                               :
#                           CPU_CRITICAL_ENTER();               /* cpu_sr = CPU_SR_Save();                  */
#                               :
#                               :
#                           CPU_CRITICAL_EXIT();                /* CPU_SR_Restore(cpu_sr);                  */
#                               :
#                       }
#********************************************************************************************************

_CPU_SR_Save:
    stsr  PSW, r10				-- Store PSW
    di
    jmp   [lp]
            
_CPU_SR_Restore:
    ldsr  r6  , PSW
    jmp   [lp]	 

#********************************************************************************************************
#                                    DISABLE and ENABLE INTERRUPTS
#
# Description: Disable/Enable interrupts.
#
# Prototypes : void  CPU_IntDis(void);
#              void  CPU_IntEn (void);
#********************************************************************************************************

_CPU_IntDis:
    di
    jmp [lp]

_CPU_IntEn:
    ei
    jmp [lp]

#********************************************************************************************************
#                                  READS CPU EXCEPTION CAUSE REGISTER
#
# Description : Reads CPU exception code register(ECR), which identifies each interrupt source according to   
#               its exception code.
#
# Prototypes  : CPU_DATA CPU_ECR_Rd   (void);
#
# Note(s)     : None.
#
#********************************************************************************************************

_CPU_ECR_Rd:
    stsr ECR, r10
    jmp [lp]

#$PAGE
#********************************************************************************************************
#                                     CPU ASSEMBLY PORT FILE END
#********************************************************************************************************
