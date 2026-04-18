
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "lib_ee152.h"
#include "stm32l4xx_hal.h"
#include "interupts_file.h"
#include "motor_control_file.h"
#include <stdlib.h>

volatile uint32_t system_ticks = 0;

// config the system time 
void SysTick_Init(uint32_t tick_hz)
{
    uint32_t reload = (SystemCoreClock / tick_hz) - 1;
    SysTick->LOAD  = reload;
    SysTick->VAL   = 0;
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}
// increment the amount of ticks to contubue incrementing time
void SysTick_Handler(void)
{
    system_ticks++;
}
// get system ticks to be changed
uint64_t cur_time_ms(void){
    return system_ticks;
}

/*
 * control_motors
 * the MSB (sign) of each parameter indicates the direction in which a wheel
 * will turn in
 * - 0 = clockwise (positive numbers), 1 = counterclockwise (negative numbers)
 * - clockwise versus counterclockwise is determined by looking at the wheel 
 * face on from from outside the bot's circular perimeter
 * the magnitude of each parameter indicates the speed at which a wheel will 
 * turn in
 */
int control_motors(int16_t front, int16_t right, int16_t back, int16_t left) {
    char buffer[100]; // Declare a character array to store the formatted string
    //positive = clockwise
    bool dir[4] = {}; //1 = counterclockwise, 0 = clockwise
    dir[0] = (front <= 0); //will be 1 if negative, meaning counterclockwise
    dir[1] = (right <= 0);
    dir[2] = (back <= 0);
    dir[3] = (left <= 0);

    int spd[4] = {}; //our change_duty function can only take in values 0-1000
    spd[0] = (abs(front) * 1000) / 32767;
    spd[1] = (abs(right) * 1000) / 32767;
    spd[2] = (abs(back) * 1000) / 32767;
    spd[3] = (abs(left) * 1000) / 32767;

    //FRONT motor direction
    set_pin(GPIOB, 4, dir[0]); //J3 top backward, 4 = top/front motor, 
    //0 = counterclockwise for this motor
    //front motor speed
    change_duty(1, spd[0]); //top

    //RIGHT motor direction
    set_pin(GPIOB, 5, !dir[1]); //J4 right
    //0 = clockwise for this motor
    //right motor speed
    change_duty(4, spd[1]); //right

    //BACK motor direction
    set_pin(GPIOB, 1, dir[2]); //J2 bottom
    //0 = clockwise for this motor
    //back motor speed
    change_duty(3, spd[2]); //bottom

    //LEFT motor direction
    set_pin(GPIOB, 0, dir[3]); //J1 left 
    //left motor speed
    change_duty(2, spd[3]); //left
}

int main() {
    //set the clock to 80 mhz
    clock_setup_80MHz(); 
    //set the uart to start
    serial_begin(USART2); //for debugging 
    serial_begin(USART1); //for raspberry pi
    //enable pwm9
    enable_pwm();

    // init the gpio for the motors
    enable_gpio_output_motor(GPIOB,4);
    enable_gpio_output_motor(GPIOB,5);
    enable_gpio_output_motor(GPIOB,0);
    enable_gpio_output_motor(GPIOB,1);
    //enable the clock so can check how long actions have taken
    SysTick_Init(1000);
    //enable interupts
    enable_input_and_interrupts();

    // set up prompt
    char prompt[] = "TEST \n\0";
    
    for(;;){
        // Temporary test - remove after debugging
        serial_write(USART2, "Waiting for USART1 byte...\n");
        while (!(USART1->ISR & USART_ISR_RXNE));  // spin until a byte arrives

        int16_t spd_f = (RXBUFFER[0] << 8) | RXBUFFER[1]; //front
        int16_t spd_r = (RXBUFFER[2] << 8) | RXBUFFER[3]; //right
        int16_t spd_b = (RXBUFFER[4] << 8) | RXBUFFER[5]; //back
        int16_t spd_l = (RXBUFFER[6] << 8) | RXBUFFER[7]; //left

        // control_motors(direction, speed, 0);
        control_motors(spd_f, spd_r, spd_b, spd_l);
        
        // Debug: print all received motor commands from raspberry pi
        char debug[150];
        // sprintf(debug, "raw: %02X %02X %02X %02X %02X %02X %02X %02X\n",
        // RXBUFFER[0], RXBUFFER[1], RXBUFFER[2], RXBUFFER[3],
        // RXBUFFER[4], RXBUFFER[5], RXBUFFER[6], RXBUFFER[7]);
        // serial_write(USART2, debug);

        sprintf(debug, "front: %d right: %d back: %d left: %d", spd_f, spd_r, spd_b, spd_l);
        sprintf(debug, "front: %d right: %d back: %d left: %d", spd_f, spd_r, spd_b, spd_l);
        serial_write(USART2, debug);

        for(int i=0; i<100000;i++){
            int t=0;
        }
}
return 0;
}

//OLD STUFF!!!! don't really need
// #include <stdbool.h>
// #include <string.h>
// #include <stdio.h>
// #include "lib_ee152.h"
// #include "stm32l4xx_hal.h"
// #include "interupts_file.h"
// #include "motor_control_file.h"

// int speed=0;
// char command=0; 

// volatile uint32_t system_ticks = 0;

// // config the system time 
// void SysTick_Init(uint32_t tick_hz)
// {
//     uint32_t reload = (SystemCoreClock / tick_hz) - 1;
//     SysTick->LOAD  = reload;
//     SysTick->VAL   = 0;
//     SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
// }
// // increment the amount of ticks to contubue incrementing time
// void SysTick_Handler(void)
// {
//     system_ticks++;
// }
// // get system ticks to be changed
// uint64_t cur_time_ms(void){
//     return system_ticks;
// }

// // default motor control function to handle the motor controls.
// // TODO: FIGURE OUT HOW TO DRIVE THE MOTORS
// // IMPLEMENT PID CONTROL FOR TURNING x amount of Degrees with the bot so slows down to rotate correctly, 
// //(in case need to rotate the bot a persise amount)
// //Needs to also be able to drive forward, backward, sideways and capible of rotating a certain amount of degrees.
// // should be able to do this with the econders themselves. 
// // extern volatile int previous_state_enc1;
// // extern volatile int enc_ticks1;
// // // emcpder 2
// // extern volatile int previous_state_enc2;
// // extern volatile int enc_ticks2;
// // // emcpder 3
// // extern volatile int previous_state_enc3;
// // extern volatile int enc_ticks3;
// // // encoder 4
// // extern volatile int previous_state_enc4;
// // extern volatile int enc_ticks4;
// int control_motors(char command, int speed, int ticks){
//     char buffer[100]; // Declare a character array to store the formatted string
//     sprintf(buffer,"current command was %c \n", command);
//     serial_write(USART2, buffer);
//     if(command == 'f'){ //forward
//         //enable speed
//         change_duty(1, 0); //top
//         change_duty(2, speed); //left
//         change_duty(3, 0); //bottom
//         change_duty(4, speed); //right

//     //phase directionq1 //bottom
//         change_duty(4, speed); //right
//         //directions
//         set_pin(GPIOB, 0, 1); //J1 left backward
//         set_pin(GPIOB, 1, 0); //J2 bottom not moving
//         set_pin(GPIOB, 4, 0); //J3 top not moving
//         set_pin(GPIOB, 5, 0); //J4 right backward
//     }else if(command == 'l'){ //left
//         //speed
//         change_duty(1, speed); //top
//         change_duty(2, 0); //left
//         change_duty(3, speed); //bottom
//         change_duty(4, 0); //right
//         //direction
//         set_pin(GPIOB, 0, 1); //J1 left not moving
//         set_pin(GPIOB, 1, 1); //J2 bottom forward
//         set_pin(GPIOB, 4, 1); //J3 top forward
//         set_pin(GPIOB, 5, 1); //J4 right not moving
//     }else if(command == 'r'){ //right
//         //speed
//         change_duty(1, speed); //top
//         change_duty(2, 0); //left
//         change_duty(3, speed); //bottom
//         change_duty(4, 0); //right
//         //direction
//         set_pin(GPIOB, 0, 0); // J1 left not moving
//         set_pin(GPIOB, 1, 0); //J2 bottom backward
//         set_pin(GPIOB, 4, 0); //J3 top backward
//        set_pin(GPIOB, 5, 0); //J4 right not moving

//     }else if(command == 'b'){ //back 
//         //speed
//         change_duty(1, 0); //top
//         change_duty(2, speed); //left
//         change_duty(3, 0); //bottom
//         change_duty(4, speed); //right
//         //direction
//         set_pin(GPIOB, 0, 0); // J1 left not moving
//         set_pin(GPIOB, 1, 0); //J2 bottom backward
//         set_pin(GPIOB, 4, 0); //J3 top backward
//         set_pin(GPIOB, 5, 1); //J4 right not moving
//     }
//     else if (command == 's'){
//         change_duty(1, 0); //top
//         change_duty(2, 0); //left
//         change_duty(3, 0); //bottom
//         change_duty(4, 0); //right
//         //direction
//         set_pin(GPIOB, 0, 1); // J1 left not moving
//         set_pin(GPIOB, 1, 0); //J2 bottom backward
//         set_pin(GPIOB, 4, 0); //J3 top backward
//         set_pin(GPIOB, 5, 1); //J4 right not moving
//     }
// }

// int main()
// {

//     //set the clock to 80 mhz
//     clock_setup_80MHz(); 
//     //set the uart to start
//     serial_begin(USART2);
//     //enable pwm
//     enable_pwm();

//     // init the gpio for the motors
//     enable_gpio_output_motor(GPIOB,4);
//     enable_gpio_output_motor(GPIOB,5);
//     enable_gpio_output_motor(GPIOB,0);
//     enable_gpio_output_motor(GPIOB,1);
//     //enable the clock so can check how long actions have taken
//     SysTick_Init(1000);
//     //enable interupts
//     enable_input_and_intetupts();

//     // set up prompt
//     char prompt[] = "TEST \n\0";
    

//     // serial_write(USART2, prompt);
//     // change_duty(2,500);
//     //main loop

//     for(;;){
//         speed=RXBUFFER2[1];
//         command=RXBUFFER2[0];
//         // serial_write(USART2, prompt);s
//         serial_read(USART2);
//         // TODO: CALL THIS FUNCTION WITH HOWEVER YOU WANT TO MODIFY IT
//         //call control motors here based on the input from the buffer.
//         control_motors(command, 1000,0);

//         // serial_write(USART2,prompt);
//         for(int i=0; i<100000;i++){
//             int t=0;
//         // }
//         // memcpy(prompt,(int*)(TIM1->CNT),strlen(prompt));
//         // serial_write(USART2, prompt);
        
//     }
// }
// return 0;
// }
