#include "stm32f10x.h"                  // Device header
#include "PinMap.h"
#include "stdbool.h"
#include "stdio.h"

int main(void) {
		
  TIM3_CH1_PWM_init();
  USART2_init();
	int a1=0, a2=0;
	
	while(1) {
		
		while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
		a1 = USART_ReceiveData(USART2) & 0xFF;
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
		USART_SendData(USART2, a1);
		
		while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
		a2 = USART_ReceiveData(USART2) & 0xFF;
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
		USART_SendData(USART2, a2);
		
	TIM_OCInitTypeDef outputChannelInit;
	outputChannelInit.TIM_OCMode = TIM_OCMode_PWM1;
	outputChannelInit.TIM_Pulse = (a1-'0')*10+(a2-'0')-1; 
	outputChannelInit.TIM_OutputState = TIM_OutputState_Enable;
	outputChannelInit.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM3, &outputChannelInit);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	}
	
}
