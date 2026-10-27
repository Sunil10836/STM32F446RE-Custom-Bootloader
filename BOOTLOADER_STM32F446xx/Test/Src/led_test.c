#include "led_test.h"

void Delay1(void)
{
    for (volatile uint32_t i = 0; i < 100000; i++);
}

void LED_Test()
{
    LED_Init();

    while(1)
    {
        GPIO_ToggleOutputPin(LED_PORT, LED_PIN);

        Delay1();
    }

}

