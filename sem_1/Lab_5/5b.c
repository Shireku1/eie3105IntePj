#include "stm32f10x.h"                  // Device header
void delay(int t);
int main(void){

RCC->APB2ENR |= RCC_APB2Periph_GPIOA; 	//Enable APB2 peripheral clock 
GPIOA->CRL &= ~0x00F00000; 			//clear the setting 
GPIOA->CRL |= 0 << 22 | 2 << 20; 		//GPIO_Mode_Out_PP, GPIO_Speed_2MHz 
GPIOA->BSRR |= 0x20; 

while(1){
GPIOA->BSRR|=0x20;
delay(1000000);
GPIOA->BRR|=0x20;
delay(1000000);
}
}

void delay(int t) 
{ 
int i; 
for(i = 0; i < t; i++) 
GPIOA->BSRR |= 0x01; // do something to PA0 
}

