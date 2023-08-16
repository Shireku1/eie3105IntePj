#include "stm32f10x.h"                  // Device header
#include "stdbool.h"

#define BUTTON_RCC_GPIO RCC_APB2Periph_GPIOC
#define BUTTON_GPIO  GPIOC
#define BUTTON_GPIO_PIN GPIO_Pin_13

#define LED_RCC_GPIO RCC_APB2Periph_GPIOA
#define LED_GPIO GPIOA
#define LED_GPIO_PIN GPIO_Pin_5

bool s=false;

int main(void){
RCC_APB2PeriphClockCmd(BUTTON_RCC_GPIO, ENABLE);
RCC_APB2PeriphClockCmd(LED_RCC_GPIO, ENABLE);

GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.GPIO_Pin=LED_GPIO_PIN;
GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
GPIO_Init(GPIOA, &GPIO_InitStruct);

while(1){
s=GPIO_ReadInputDataBit(BUTTON_GPIO,BUTTON_GPIO_PIN);
if (s)
{GPIO_ResetBits(LED_GPIO,LED_GPIO_PIN);}
else
{GPIO_SetBits(LED_GPIO,LED_GPIO_PIN);}
}
}
