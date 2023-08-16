#include "stm32f10x.h"                  // Device header
static __IO uint32_t msTicks;
void DelayMs(uint32_t ms);

int main (void){
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

//GPIO set up for PA5
GPIO_InitTypeDef GPIO_InitStructure;
GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
GPIO_Init(GPIOA, &GPIO_InitStructure);

SystemCoreClockUpdate();
SysTick_Config(SystemCoreClock/1000);

while (1){
GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET);
DelayMs(500);
GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
DelayMs(500);
}

}

void DelayMs(uint32_t ms)
{
msTicks=ms;
while(msTicks);
}

void SysTick_Handler()
{
if (msTicks!=0)
msTicks--;
}
