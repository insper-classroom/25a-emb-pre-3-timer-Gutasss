#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;         
volatile bool timer_tick_flag = false;  
volatile bool timer_active = false;     
repeating_timer_t timer;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {  
        flag_f_r = 1;
    }
}

bool timer_callback(repeating_timer_t *t) {
    timer_tick_flag = true;
    return true;  
}

int main() {
    stdio_init_all();
    
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);
    
    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);
    
    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    
    while (true) {
        if (flag_f_r == 1) {
            flag_f_r = 0;
            if (!timer_active) {
                add_repeating_timer_ms(500, timer_callback, NULL, &timer);
                timer_active = true;
            } else {
                cancel_repeating_timer(&timer);
                timer_active = false;
                gpio_put(LED_PIN_R, 0);
            }
        }
        
        if (timer_tick_flag) {
            timer_tick_flag = false;
            int state = gpio_get(LED_PIN_R);
            gpio_put(LED_PIN_R, !state);
        }
    }
    
    return 0;
}
