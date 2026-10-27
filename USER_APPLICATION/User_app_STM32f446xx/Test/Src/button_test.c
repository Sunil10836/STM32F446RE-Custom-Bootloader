
#include "button_test.h"
#include "led_test.h"

void Button_Test()
{
    Button_Init();
    
    LED_Init();

    while(1)
    {
        if(GPIO_ReadFromInputPin(BUTTON_PORT, BUTTON_PIN) == GPIO_PIN_RESET)
        {
            GPIO_WriteToOutputPin(LED_PORT, LED_PIN, GPIO_PIN_SET);
        }
        else
        {
            GPIO_WriteToOutputPin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
        }
    }
}
