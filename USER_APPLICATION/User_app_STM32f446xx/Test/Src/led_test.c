#include "led_test.h"

void LED_Test()
{
    LED_Init();

    while(1)
    {
        GPIO_ToggleOutputPin(LED_PORT, LED_PIN);

        Delay1();
    }

}

