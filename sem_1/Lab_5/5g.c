#include "stm32f10x.h"                  // Device header
#include "stdbool.h"

//TI2 PA1 Tim2 Ch2
#define BUTTON_RCC_GPIO   RCC_APB2Periph_GPIOA
#define BUTTON_GPIO	  GPIOA
#define BUTTON_GPIO_PIN   GPIO_Pin_1

#define L3_RCC_GPIO  RCC_APB2Periph_GPIOB
#define L3_GPIO      GPIOB
#define L3_R_PIN     GPIO_Pin_8

int state = 0;

int main(void) {
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// GPIO clock for I/O
	RCC_APB2PeriphClockCmd(BUTTON_RCC_GPIO, ENABLE);
        RCC_APB2PeriphClockCmd(L3_RCC_GPIO, ENABLE);
	
	// Configure I/O for L3
	GPIO_InitStruct.GPIO_Pin = L3_R_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(L3_GPIO, &GPIO_InitStruct);
 
	// Configure I/O for BUTTON
	GPIO_InitStruct.GPIO_Pin = BUTTON_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BUTTON_GPIO, &GPIO_InitStruct);
	
	//Timer 2 set up 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
 
  	TIM_TimeBaseInitTypeDef timerInitStructure;
  	timerInitStructure.TIM_Prescaler = 0;
  	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  	timerInitStructure.TIM_Period = 2;
  	timerInitStructure.TIM_ClockDivision = 0;
  	timerInitStructure.TIM_RepetitionCounter = 0;
  	TIM_TimeBaseInit(TIM2, &timerInitStructure);
  	TIM_Cmd(TIM2, ENABLE);
	
	TIM_TIxExternalClockConfig(TIM2, TIM_TIxExternalCLK1Source_TI2, TIM_ICPolarity_Rising, 0);
	
	//Enable update event for Timer2
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	NVIC_EnableIRQ(TIM2_IRQn);
		
GPIO_ResetBits(L3_GPIO, L3_R_PIN);		
	while(1) {}
}

void TIM2_IRQHandler(void) {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		if(state) {
			GPIO_SetBits(L3_GPIO, L3_R_PIN);
			state = 0;
		} else {
			GPIO_ResetBits(L3_GPIO, L3_R_PIN);
			state = 1;
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}          
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);                                       
}

