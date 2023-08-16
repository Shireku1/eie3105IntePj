#include "stm32f10x.h"                  // Device header

#define LED_RCC_GPIOA RCC_APB2Periph_GPIOA
#define LED_RCC_GPIOB RCC_APB2Periph_GPIOB
#define LED_GPIOA GPIOA
#define LED_GPIOB GPIOB


#define L1G_PIN GPIO_Pin_8
#define L1R_PIN GPIO_Pin_9

#define L2G_PIN GPIO_Pin_5
#define L2Y_PIN GPIO_Pin_6
#define L2R_PIN GPIO_Pin_7

#define L3G_PIN GPIO_Pin_6
#define L3Y_PIN GPIO_Pin_10
#define L3R_PIN GPIO_Pin_0


int state=0;
int cnt=0;
int b=0;
void led(void);

int main (void){
RCC_APB2PeriphClockCmd(LED_RCC_GPIOA, ENABLE);
RCC_APB2PeriphClockCmd(LED_RCC_GPIOB, ENABLE);
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

//timer 2
TIM_TimeBaseInitTypeDef tmInitStruct;
tmInitStruct.TIM_Prescaler=18000-1;
tmInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
tmInitStruct.TIM_Period=800-1;
tmInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
tmInitStruct.TIM_RepetitionCounter=0;
TIM_TimeBaseInit(TIM2,&tmInitStruct);
TIM_Cmd(TIM2, ENABLE);

TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
NVIC_EnableIRQ(TIM2_IRQn);

//IO
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.GPIO_Pin =L1G_PIN|L1R_PIN|L2G_PIN|L2Y_PIN|L2R_PIN|L3G_PIN|L3Y_PIN|L3R_PIN;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
GPIO_Init(LED_GPIOA, &GPIO_InitStruct);
GPIO_Init(LED_GPIOB, &GPIO_InitStruct);

GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
GPIO_Init(GPIOC, &GPIO_InitStruct);

//external int
GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0);
	
EXTI_InitTypeDef EXTI_InitStr;
EXTI_InitStr.EXTI_Line = EXTI_Line1;
EXTI_InitStr.EXTI_Mode = EXTI_Mode_Interrupt;
EXTI_InitStr.EXTI_LineCmd = ENABLE;
EXTI_InitStr.EXTI_Trigger = EXTI_Trigger_Rising;
EXTI_Init(&EXTI_InitStr);
	
NVIC_InitTypeDef NVIC_InitStr;
NVIC_InitStr.NVIC_IRQChannel = EXTI1_IRQn;
NVIC_InitStr.NVIC_IRQChannelCmd = ENABLE;
NVIC_InitStr.NVIC_IRQChannelPreemptionPriority = 0x02;
NVIC_Init(&NVIC_InitStr);

while(1){}
}

void EXTI1_IRQHandler()
{
	if (EXTI_GetITStatus(EXTI_Line1) != RESET)
	{
		if(state==0){state=1;}else{state=0;}
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

void TIM2_IRQHandler(){
if (TIM_GetITStatus(TIM2, TIM_IT_Update)!=RESET){

if(cnt<80){cnt++;}else{cnt=0;}
led();
TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
}
}


void led()
{
	
if(state==1){
//1
if (cnt<25){
		GPIO_WriteBit(LED_GPIOB,L1R_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOB,L1G_PIN,Bit_RESET);
		
		GPIO_WriteBit(LED_GPIOA,L2R_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOA,L2Y_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOA,L2G_PIN,Bit_SET);
		
		GPIO_WriteBit(LED_GPIOB,L3R_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOB,L3Y_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOB,L3G_PIN,Bit_RESET);
	return;}

//2
if (cnt>=25){if(cnt<30){
		GPIO_WriteBit(LED_GPIOB,L1R_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOB,L1G_PIN,Bit_RESET);
		
		GPIO_WriteBit(LED_GPIOA,L2R_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOA,L2Y_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOA,L2G_PIN,Bit_RESET);
		
		GPIO_WriteBit(LED_GPIOB,L3R_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOB,L3Y_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOB,L3G_PIN,Bit_RESET);
return;}}

//3
if (cnt>=30){if(cnt<35){
		GPIO_WriteBit(LED_GPIOB,L1R_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOB,L1G_PIN,Bit_RESET);
		
		GPIO_WriteBit(LED_GPIOA,L2R_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOA,L2Y_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOA,L2G_PIN,Bit_RESET);
		
		GPIO_WriteBit(LED_GPIOB,L3R_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOB,L3Y_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOB,L3G_PIN,Bit_RESET);
return;}}

//4
if (cnt>=35){if(cnt<40){
		GPIO_WriteBit(LED_GPIOB,L1R_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOB,L1G_PIN,Bit_RESET);
		
		GPIO_WriteBit(LED_GPIOA,L2R_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOA,L2Y_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOA,L2G_PIN,Bit_RESET);
		
		GPIO_WriteBit(LED_GPIOB,L3R_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOB,L3Y_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOB,L3G_PIN,Bit_RESET);
return;}}
		
//5
if (cnt>=40){if(cnt<65){
		GPIO_WriteBit(LED_GPIOB,L1R_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOB,L1G_PIN,Bit_SET);
		
		GPIO_WriteBit(LED_GPIOA,L2R_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOA,L2Y_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOA,L2G_PIN,Bit_RESET);
		
		GPIO_WriteBit(LED_GPIOB,L3R_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOB,L3Y_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOB,L3G_PIN,Bit_SET);
return;}}

//6
if (cnt>=65){if(cnt<70){
		b++;
		GPIO_WriteBit(LED_GPIOB,L1R_PIN,Bit_RESET);
		if(b%2==1){
		GPIO_WriteBit(LED_GPIOB,L1G_PIN,Bit_SET);}else{
		GPIO_WriteBit(LED_GPIOB,L1G_PIN,Bit_RESET);}	//TO BLINK
		
		GPIO_WriteBit(LED_GPIOA,L2R_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOA,L2Y_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOA,L2G_PIN,Bit_RESET);
		
		GPIO_WriteBit(LED_GPIOB,L3R_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOB,L3Y_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOB,L3G_PIN,Bit_RESET);
return;}}

//7
if (cnt>=70){if(cnt<75){
		GPIO_WriteBit(LED_GPIOB,L1R_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOB,L1G_PIN,Bit_RESET);
		
		GPIO_WriteBit(LED_GPIOA,L2R_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOA,L2Y_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOA,L2G_PIN,Bit_RESET);
		
		GPIO_WriteBit(LED_GPIOB,L3R_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOB,L3Y_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOB,L3G_PIN,Bit_RESET);
return;}}

//8
if (cnt>=75){if(cnt<80){
		GPIO_WriteBit(LED_GPIOB,L1R_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOB,L1G_PIN,Bit_RESET);
		
		GPIO_WriteBit(LED_GPIOA,L2R_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOA,L2Y_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOA,L2G_PIN,Bit_RESET);
		
		GPIO_WriteBit(LED_GPIOB,L3R_PIN,Bit_SET);
		GPIO_WriteBit(LED_GPIOB,L3Y_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOB,L3G_PIN,Bit_RESET);
return;}}
}else{
		GPIO_WriteBit(LED_GPIOB,L1R_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOB,L1G_PIN,Bit_RESET);
		
		GPIO_WriteBit(LED_GPIOA,L2R_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOA,L2Y_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOA,L2G_PIN,Bit_RESET);
		
		GPIO_WriteBit(LED_GPIOB,L3R_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOB,L3Y_PIN,Bit_RESET);
		GPIO_WriteBit(LED_GPIOB,L3G_PIN,Bit_RESET);
}


}

