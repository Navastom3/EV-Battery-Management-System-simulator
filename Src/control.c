#include "control.h"
#include "main.h"

static volatile uint8_t waiting_for_double = 0;
static volatile uint8_t double_click = 0;
static volatile uint8_t single_click = 0;

static volatile uint32_t first_click_time = 0;
static volatile uint32_t last_press_time = 0;

static uint8_t led_enabled = 0;
static PowerMode power_mode = FULL_POWER;


PowerMode get_power_mode(void)
{
    return power_mode;
}


uint8_t update_control(void)
{
    uint32_t now = HAL_GetTick();

    if (waiting_for_double && (now - first_click_time > 300))
    {
        waiting_for_double = 0;
        single_click = 1;
    }

    if (double_click)
    {
        double_click = 0;
        power_mode = (power_mode == FULL_POWER) ? LOW_POWER : FULL_POWER;
    }

    if (single_click)
    {
        single_click = 0;
        led_enabled = !led_enabled;
    }

    return led_enabled;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin != Button_1_Pin)
        return;

    uint32_t now = HAL_GetTick();

    // Ignore button bounce
    if (now - last_press_time < 50)
        return;

    last_press_time = now;

    if (!waiting_for_double)
    {
        first_click_time = now;
        waiting_for_double = 1;
    }
    else if (now - first_click_time <= 300)
    {
        waiting_for_double = 0;
        double_click = 1;
    }
    else
    {
        single_click = 1;
        first_click_time = now;
        waiting_for_double = 1;
    }
}
