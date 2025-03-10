#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_r = 0;
volatile int flag_g = 0;

volatile bool timer_tick_flag_r = false;  
volatile bool timer_tick_flag_g = false;  


repeating_timer_t timerr;
repeating_timer_t timerg;

bool timer_callback_red(repeating_timer_t *t) {
    timer_tick_flag_r = true;
    return true;  
}

bool timer_callback_green(struct repeating_timer_t *t) {
    timer_tick_flag_g = true;
    return true;  
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {  
        if (gpio == BTN_PIN_R)
            flag_r = 1;
        else if (gpio == BTN_PIN_G)
            flag_g = 1;
    }
}

int main() {
    stdio_init_all();
    
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);
    
    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);
    gpio_put(LED_PIN_G, 0);
    
    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);
    
    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);
    
    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    bool timer_active_r = false;
    bool timer_active_g = false;
    
    while (true) {
        if (flag_r) {
            flag_r = 0;
            if (!timer_active_r) {
                add_repeating_timer_ms(500, timer_callback_red, NULL, &timerr);
                timer_active_r = true;
            } else {
                cancel_repeating_timer(&timerr);
                timer_active_r = false;
                gpio_put(LED_PIN_R, 0);
            }
        }
        
        if (flag_g) {
            flag_g = 0;
            if (!timer_active_g) {
                add_repeating_timer_ms(250, timer_callback_green, NULL, &timerg);
                timer_active_g = true;
            } else {
                cancel_repeating_timer(&timerg);
                timer_active_g = false;
                gpio_put(LED_PIN_G, 0);
            }
        }
        
        if (timer_tick_flag_r) {
            timer_tick_flag_r = false;
            int state = gpio_get(LED_PIN_R);
            gpio_put(LED_PIN_R, !state);
        }
        
        if (timer_tick_flag_g) {
            timer_tick_flag_g = false;
            int state = gpio_get(LED_PIN_G);
            gpio_put(LED_PIN_G, !state);
        }
    }
    
    return 0;
}
