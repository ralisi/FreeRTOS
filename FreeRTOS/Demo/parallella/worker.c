#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "c2c.h"

QueueHandle_t xRecvQueue;

void sqTask (void* pvParameters)
{               
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        vTaskDelayUntil( &xLastWakeTime, 20 );
        c2c_payload message;
        
        if( xQueueReceive( xRecvQueue, &message, ( TickType_t ) portMAX_DELAY ) ) {
            int x = message.msg;
            int y = x*x;

        
            sendMsgTo(y, 0, 0, 0); // send result to core at (0,0)
        }
    }
}


void doubleTask (void* pvParameters)
{               
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        vTaskDelayUntil( &xLastWakeTime, 20 );
        c2c_payload message;
        
        if( xQueueReceive( xRecvQueue, &message, ( TickType_t ) portMAX_DELAY ) ) {
            int x = message.msg;
            int y = 2*x;

        
            sendMsgTo(y, 0, 0, 0); // send result to core at (0,0)
        }
    }
}



int main( void )
{  
    xTaskCreate( sqTask, ( signed char * ) "square", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, ( xTaskHandle * ) NULL );
    xTaskCreate( doubleTask, ( signed char * ) "double", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, ( xTaskHandle * ) NULL );
    
    
    createReceiver();
    xRecvQueue = xQueueCreate( 5, sizeof( c2c_payload ) );
    registerForSlot(0, xRecvQueue);

    vTaskStartScheduler();
    // Should never return
    
    return 0;
}

