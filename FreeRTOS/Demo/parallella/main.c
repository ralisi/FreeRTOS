#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "c2c.h"



//required for following SECTION-attribute
#include "e-lib.h"
extern int M[5];

QueueHandle_t xRecvQueue;




void prvTask (void* pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    int value = 0;
    int count;
    while(1) {
        while(M[1]==0)
            ;
        count = 3000;
        while(count--) ;
        value = M[1];
        vTaskDelayUntil( &xLastWakeTime, 10 );
        sendMsgTo(value, 0, 1, 0);
        
        c2c_payload message;
        if (xQueueReceive( xRecvQueue, &message, portMAX_DELAY ) == pdTRUE) {
            M[2] = message.msg;
        }
        else {
            M[2] = -1;
        }
        
        count = 3000;
        while(count--) ;
        M[1] = 0;
        count = 3000;
        while(count--) ;
    }
}

int main( void )
{  
    M[0]=0;
    M[1]=0;
    M[2]=0;
    M[3]=0;
    M[4]=0;
    
    xTaskCreate( prvTask, ( signed char * ) "TaskA", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, ( xTaskHandle * ) NULL );
    createReceiver();
    xRecvQueue = xQueueCreate( 3, sizeof( c2c_payload ) );
    registerForSlot(0, xRecvQueue);
    
    
    
    
    vTaskStartScheduler();
    // Should never return
    
    return 0;
}

