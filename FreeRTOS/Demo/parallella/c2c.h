#ifndef C2C_H
#define C2C_H

#ifndef C2C_MSG_TYPE
  typedef int c2c_msg_t;
#else
  typedef C2C_MSG_TYPE c2c_msg_t;
#endif

typedef struct{
  c2c_msg_t msg;
  int src;
} c2c_payload;

void sendMsgTo(c2c_msg_t msg, unsigned row, unsigned col, unsigned slot);
void registerForSlot(unsigned slot, QueueHandle_t handler);
void createReceiver();

#endif