//macro to generate the different ISR starter, that just write their number
.macro ISR name, number
_\name:
  .global _\name
  str r0, [sp, #-1]
  str r1, [sp, #-2]
  mov  r0,%low(_interrupt_mode)
  movt r0,%high(_interrupt_mode)
  mov r1, \number
  str r1, [r0]
  ldr r1, [sp, -#2]
  ldr r0, [sp, -#1]
  b _common_interrupt_handler
.endm

ISR sw_ex_handler,    1
ISR memfault_handler, 2
ISR timer0_handler,   3
ISR timer1_handler,   4
ISR message_handler,  5
ISR dma0_handler,     6
ISR dma1_handler,     7
ISR wand_handler,     8
ISR userint_handler,  9

_handlers:
  .global _handlers
  .word 0, _sw_ex_handler, _memfault_handler, _timer0_handler, _timer1_handler, _message_handler, _dma0_handler, _dma1_handler, _wand_handler, _userint_handler

_common_interrupt_handler:
  .global _common_interrupt_handler
  str r63, [sp, #-1]
  ldr r63, [sp], #-2 // r63 is already save and just move sp without touching the flags
  str r62, [sp], #-1
  str r61, [sp], #-1
  str r60, [sp], #-1

  str r59, [sp], #-1
  str r58, [sp], #-1
  str r57, [sp], #-1
  str r56, [sp], #-1
  str r55, [sp], #-1
  str r54, [sp], #-1
  str r53, [sp], #-1
  str r52, [sp], #-1
  str r51, [sp], #-1
  str r50, [sp], #-1

  str r49, [sp], #-1
  str r48, [sp], #-1
  str r47, [sp], #-1
  str r46, [sp], #-1
  str r45, [sp], #-1
  str r44, [sp], #-1
  str r43, [sp], #-1
  str r42, [sp], #-1
  str r41, [sp], #-1
  str r40, [sp], #-1

  str r39, [sp], #-1
  str r38, [sp], #-1
  str r37, [sp], #-1
  str r36, [sp], #-1
  str r35, [sp], #-1
  str r34, [sp], #-1
  str r33, [sp], #-1
  str r32, [sp], #-1
  str r31, [sp], #-1
  str r30, [sp], #-1

  str r29, [sp], #-1
  str r28, [sp], #-1
  str r27, [sp], #-1
  str r26, [sp], #-1
  str r25, [sp], #-1
  str r24, [sp], #-1
  str r23, [sp], #-1
  str r22, [sp], #-1
  str r21, [sp], #-1
  str r20, [sp], #-1

  str r19, [sp], #-1
  str r18, [sp], #-1
  str r17, [sp], #-1
  str r16, [sp], #-1
  str r15, [sp], #-1
  str r14, [sp], #-1
  str r13, [sp], #-1
  str r12, [sp], #-1
  str r11, [sp], #-1
  str r10, [sp], #-1

  str r9, [sp], #-1
  str r8, [sp], #-1
  str r7, [sp], #-1
  str r6, [sp], #-1
  str r5, [sp], #-1
  str r4, [sp], #-1
  str r3, [sp], #-1
  str r2, [sp], #-1
  str r1, [sp], #-1
  str r0, [sp], #-1


  movfs r1, iret
  str r1, [sp], #-1

  movfs r0, status
  str r0, [sp]
  
  mov  fp,%low(_pxCurrentTCB)
  movt fp,%high(_pxCurrentTCB)
  ldr  fp, [fp]
  str  sp, [fp]

  // use stack from start to avoid stress on task's stack
  mov sp, %low(__stack_start_)
  movt sp, %high(__stack_start_)
  
  // switch the task (calls our c-function which will do that, and some more things)
  mov  r0,%low(_vInterruptCentral)
  movt r0,%high(_vInterruptCentral)
  jalr r0
  
  
  //now continue to restore it, might be called stand-alone
  

_portRESTORE_CONTEXT:
  .global _portRESTORE_CONTEXT
  mov  fp,%low(_pxCurrentTCB)
  movt fp,%high(_pxCurrentTCB)
  ldr  fp, [fp]
  ldr  sp, [fp]

  ldr r0, [sp], #1
  ldr r1, [sp], #1

  //load PC
  movts iret, r1

  //load status
  movts status, r0

  //start loading gp regs
  ldr  r0, [sp], #1
  ldr  r1, [sp], #1
  ldr  r2, [sp], #1
  ldr  r3, [sp], #1
  ldr  r4, [sp], #1
  ldr  r5, [sp], #1
  ldr  r6, [sp], #1
  ldr  r7, [sp], #1
  ldr  r8, [sp], #1
  ldr  r9, [sp], #1

  ldr  r10, [sp], #1
  ldr  r11, [sp], #1
  ldr  r12, [sp], #1
  ldr  r14, [sp], #1 //Stack pointer restoration is useless, but must increment sp without touching the flags
  ldr  r14, [sp], #1
  ldr  r15, [sp], #1
  ldr  r16, [sp], #1
  ldr  r17, [sp], #1
  ldr  r18, [sp], #1
  ldr  r19, [sp], #1

  ldr  r20, [sp], #1
  ldr  r21, [sp], #1
  ldr  r22, [sp], #1
  ldr  r23, [sp], #1
  ldr  r24, [sp], #1
  ldr  r25, [sp], #1
  ldr  r26, [sp], #1
  ldr  r27, [sp], #1
  ldr  r28, [sp], #1
  ldr  r29, [sp], #1

  ldr  r30, [sp], #1
  ldr  r31, [sp], #1
  ldr  r32, [sp], #1
  ldr  r33, [sp], #1
  ldr  r34, [sp], #1
  ldr  r35, [sp], #1
  ldr  r36, [sp], #1
  ldr  r37, [sp], #1
  ldr  r38, [sp], #1
  ldr  r39, [sp], #1

  ldr  r40, [sp], #1
  ldr  r41, [sp], #1
  ldr  r42, [sp], #1
  ldr  r43, [sp], #1
  ldr  r44, [sp], #1
  ldr  r45, [sp], #1
  ldr  r46, [sp], #1
  ldr  r47, [sp], #1
  ldr  r48, [sp], #1
  ldr  r49, [sp], #1

  ldr  r50, [sp], #1
  ldr  r51, [sp], #1
  ldr  r52, [sp], #1
  ldr  r53, [sp], #1
  ldr  r54, [sp], #1
  ldr  r55, [sp], #1
  ldr  r56, [sp], #1
  ldr  r57, [sp], #1
  ldr  r58, [sp], #1
  ldr  r59, [sp], #1

  ldr  r60, [sp], #1
  ldr  r61, [sp], #1
  ldr  r62, [sp], #1
  ldr  r63, [sp], #1

  rti
