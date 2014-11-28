#include "FreeRTOS.h"
#include "task.h"
#include "e-lib.h"

#define CyclesForTick (configCPU_CLOCK_HZ/configTICK_RATE_HZ - 50)

unsigned int M[5] SECTION("shared_dram");

/*-----------------------------------------------------------*/

/* We require the address of the pxCurrentTCB variable, but don't want to know
any details of its type. */
typedef void TCB_t;
extern volatile TCB_t * volatile pxCurrentTCB;


extern void portRESTORE_CONTEXT();

/*-----------------------------------------------------------*/

/*
 * Perform hardware setup to enable ticks from timer 1, compare match A.
 */
void vSetupTimer( void );
/*-----------------------------------------------------------*/

/* 
 * See header file for description. 
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
    StackType_t *pxStackStart;

    
    //align to double-word boundary
    pxStackStart = (StackType_t *)(( StackType_t ) pxTopOfStack & ~(0x4));
    
    /* To ensure asserts in tasks.c don't fail, although in this case the assert
    is not really required. */
    pxTopOfStack--;

    /* for alignment */
    pxTopOfStack--;

    /* Setup the initial stack of the task.  The stack is set exactly as 
    expected by the portRESTORE_CONTEXT() macro. */

    /* First on the stack is the return address - which in this case is the
    start of the task.  The offset is added to make the return address appear
    as it would within an IRQ ISR. */
    *pxTopOfStack = ( StackType_t ) 0xdeadaffe;      
    pxTopOfStack--;
    pxTopOfStack--;
    pxTopOfStack--;

    
    int i;
    
    for(i=63;i>14;i--) {
      *pxTopOfStack = ( StackType_t ) i<<8; /* higher registers */
       pxTopOfStack--; 
    }
    
    
    *pxTopOfStack = ( StackType_t ) 0xaaaaaaaa; /* R14 */
    pxTopOfStack--; 
    *pxTopOfStack = ( StackType_t ) 0x51aac4de; /* Stack used when task starts goes in R13. */
    pxStackStart = pxTopOfStack;
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) 0x12121212; /* R12 */
    pxTopOfStack--; 
    *pxTopOfStack = ( StackType_t ) 0x11111111; /* R11 */
    //*pxTopOfStack = ( StackType_t ) pxOriginalTOS; /* frame pointer to stack address. */
    pxTopOfStack--; 
    *pxTopOfStack = ( StackType_t ) 0x10101010; /* R10 */
    pxTopOfStack--; 
    *pxTopOfStack = ( StackType_t ) 0x09090909; /* R9 */
    pxTopOfStack--; 
    *pxTopOfStack = ( StackType_t ) 0x08080808; /* R8 */
    pxTopOfStack--; 
    *pxTopOfStack = ( StackType_t ) 0x07070707; /* R7 */
    pxTopOfStack--; 
    *pxTopOfStack = ( StackType_t ) 0x06060606; /* R6 */
    pxTopOfStack--; 
    *pxTopOfStack = ( StackType_t ) 0x05050505; /* R5 */
    pxTopOfStack--; 
    *pxTopOfStack = ( StackType_t ) 0x04040404; /* R4 */
    pxTopOfStack--; 
    *pxTopOfStack = ( StackType_t ) 0x03030303; /* R3 */
    pxTopOfStack--; 
    *pxTopOfStack = ( StackType_t ) 0x02020202; /* R2 */
    pxTopOfStack--; 
    *pxTopOfStack = ( StackType_t ) 0x01010101; /* R1 */
    pxTopOfStack--; 

    /* When the task starts is will expect to find the function parameter in
    R0. */
    *pxTopOfStack-- = ( StackType_t ) pvParameters; /* R0 */
    
    // second last thing: program counter, no normal register on epiphany!
    *pxTopOfStack-- = ( StackType_t ) pxCode; /* Stack used when task starts goes in R13. */
    
    

    /* The last thing onto the stack is the status register, which is set for
    enabled, with interrupts disabled. */
    *pxTopOfStack = ( StackType_t ) 0x03;

    

    /* Some optimisation levels use the stack differently to others.  This 
    means the interrupt flags cannot always be stored on the stack and will
    instead be stored in a variable, which is then saved as part of the
    tasks context. */
    //*pxTopOfStack = 0;
    //return pxOriginalTOS;
    return pxTopOfStack;
}
/*-----------------------------------------------------------*/

void setMemprotectMask(char mask) {
    unsigned memprotectregister = e_reg_read(E_REG_MEMPROTECT);

    //clear last 8 bits
    memprotectregister &= 0xffffff00;
    //sets it to the specified value
    memprotectregister |= mask;

    e_reg_write(E_REG_MEMPROTECT, memprotectregister);
}



extern sighandler_t handlers[10];
/*
 * Sets up interrupts and starts first task
 */
BaseType_t xPortStartScheduler( void )
{
    e_irq_attach(E_USER_INT, handlers[E_USER_INT]);
    e_irq_mask(E_USER_INT, E_FALSE);

    e_irq_attach(E_TIMER0_INT, handlers[E_TIMER0_INT]);
    e_irq_mask(E_TIMER0_INT, E_FALSE);

    e_irq_attach(E_MEM_FAULT, handlers[E_MEM_FAULT]);
    e_irq_mask(E_MEM_FAULT, E_FALSE);

    e_irq_attach(E_SW_EXCEPTION, handlers[E_SW_EXCEPTION]);
    e_irq_mask(E_SW_EXCEPTION, E_FALSE);

    e_irq_attach(E_MESSAGE_INT, handlers[E_MESSAGE_INT]);
    e_irq_mask(E_MESSAGE_INT, E_FALSE);

    setMemprotectMask(0x0f);

    vSetupTimer();

	/* Restore the context of the first task that is going to run. */
	portRESTORE_CONTEXT();

	/* Should not get here. */
	return pdTRUE;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* Unmasks the timer1-interrupt, which is used for reoccurring scheduling */
    e_irq_mask(E_TIMER1_INT, E_TRUE);
}
/*-----------------------------------------------------------*/

/*
 * Manual context switch. 
 * Cause a user interrupt on this core.
 */
void vPortYield( void )
{
    // There seems to be no working software interrupt, so we send a user-interrupt to ourself
    e_coreid_t thiscore = e_get_coreid();
    unsigned row, col;
    e_coords_from_coreid(thiscore, &row, &col);
    e_irq_set(row,col,E_USER_INT);
}
/*-----------------------------------------------------------*/

/*
 * Setus up the timer as necessary.
 */
void vSetupTimer( void )
{
    unsigned int cyclesForTick = CyclesForTick;
    int delta = E_CTIMER_MAX - e_ctimer_get(E_CTIMER_1) - cyclesForTick;  //How much off are we this time?
    e_ctimer_set(E_CTIMER_1, E_CTIMER_MAX);

    //We are never to early, case is taken on startup
    if(delta < 0)
        delta=0;

    e_ctimer_set(E_CTIMER_0, cyclesForTick-delta);
    e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
}


struct layout
{
    volatile int status;
    volatile int pc;
    volatile int no[64]; //collection of all the registers as stored onto the stack
};



void injectErrors( void )
{
    TaskHandle_t runningTask = xTaskGetCurrentTaskHandle();
    TCB_t **tcb = runningTask;
    struct layout* reg = *tcb;

    int bitmask = 1 << (rand()%32);
    int failreg = rand()%64;
    reg->no[failreg] ^= bitmask;
    M[4]++;
}

extern void irqMsg();
int interrupt_mode;
void vInterruptCentral( void ) {
    switch (interrupt_mode) {
        case E_TIMER0_INT:
            vSetupTimer();
            if( xTaskIncrementTick() != pdFALSE ) {
                vTaskSwitchContext();
            }
            break;
        case E_SW_EXCEPTION: //triggered extrenally for error injection. Must be that high prority
            injectErrors();
            break;
        case E_USER_INT:
            vTaskSwitchContext();
            break;
        case E_MESSAGE_INT: //message, triggered externally for message transmission
            irqMsg();
            break;
        case E_MEM_FAULT: // written to read-only memory
            while(1);
            break;
        default:
            break;
    }
}
