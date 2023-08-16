#include "stm32f10x.h"                  // Device header

int main(void){

RCC->APB2ENR |= RCC_APB2Periph_GPIOA; //Enable APB2 peripheral clock
GPIOA->CRL &= ~0x00F00000; //clear the setting
GPIOA->CRL |= 0 << 22 | 2 << 20; //GPIO_Mode_Out_PP, GPIO_Speed_2MHz
GPIOA->BSRR |= 0x20;
}
