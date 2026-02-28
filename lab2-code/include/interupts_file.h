#ifndef INTERUPTS_FILE_H
#define INTERUPTS_FILE_H


#include "stm32l4xx_hal.h"

extern volatile int previous_state_enc1;
extern volatile int enc_ticks1;
// emcpder 2
extern volatile int previous_state_enc2;
extern volatile int enc_ticks2;
// emcpder 3
extern volatile int previous_state_enc3;
extern volatile int enc_ticks3;
// encoder 4
extern volatile int previous_state_enc4;
extern volatile int enc_ticks4;


// enable the input pins and the interupts
int enable_input_and_interrupts();
// enable the input pins for gpio.
int enable_input_pin(GPIO_TypeDef *GPIOX,int pin);
// interupt handlers. These handle the interupts to deal with the encoder rotation. 
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI3_IRQHandler(void);

void EXTI4_IRQHandler(void);

void EXTI9_5_IRQHandler(void);

void EXTI15_10_IRQHandler(void);






#endif