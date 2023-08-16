//car
#include "stm32f10x.h"                  // Device header
#include "string.h"
#include <stdio.h>
#include "stdbool.h"


#define PWM 0x6870
#define CAPTURE 0x6801
#define IDLE 0x6803

char msg[] = " ";
static uint8_t msg_i=0;
static __IO uint32_t msTicks;
void DelayMs(uint32_t ms);
unsigned char character, state=0;
unsigned char led8bit=1;
static int PWMmode=0;
void PWMODE(void);

char buffer[50] = {'\0'};
__IO bool pulseHigh = false;
__IO bool generate_pulse = true;
__IO u32 pulseWidth = 0;
__IO u32 captureValue = 0;
__IO u32 distance = 0;

void TIM1_CH3_CH3N_init()
{
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  
  	// config TIM1 CH3 I/O
  	// echo inpute capture
  	GPIO_InitTypeDef init_gpio;
  	init_gpio.GPIO_Mode = GPIO_Mode_IPD; // the mode is not AF_PP
  	init_gpio.GPIO_Pin = GPIO_Pin_10; // TIM1 CH3 pin
  	init_gpio.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOA, &init_gpio);
  
  	//set TIM1 in 1000Hz
  	//pulse range: 0 to 1000
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  
  	// 1ms per cycle, 1000Hz
  	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	
  	timerInitStructure.TIM_Prescaler = 72-1; // 1(72MHz/72) = 1us, 72
	timerInitStructure.TIM_Period = 10000-1; // 1us*10000 = 10ms, 10000
	timerInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &timerInitStructure);
	TIM_Cmd(TIM1, ENABLE);
 
  	//enable TIM1 CH3 Input Capture
  	TIM_ICInitTypeDef inputCapture_init;
	inputCapture_init.TIM_Channel = TIM_Channel_3; //seclect IC1
	inputCapture_init.TIM_ICPolarity = TIM_ICPolarity_Rising; //capture rising edge
	inputCapture_init.TIM_ICSelection = TIM_ICSelection_DirectTI; //map to TI3
	inputCapture_init.TIM_ICPrescaler = TIM_ICPSC_DIV1; //configure input frequency
	inputCapture_init.TIM_ICFilter = 0; //no filter
	TIM_ICInit(TIM1, &inputCapture_init);
  
	//enable input capture interrput
	NVIC_InitTypeDef nvic_init;
	nvic_init.NVIC_IRQChannel = TIM1_CC_IRQn; //TIM1 capture compare interrput
	//preemptive priority level 2
	nvic_init.NVIC_IRQChannelPreemptionPriority = 2;
	//from the priority level 0
	nvic_init.NVIC_IRQChannelSubPriority = 0;
	//the IRQ channel is enabled
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init);
	//allow update to interrput, allows the CC3IE to capture interrput
	TIM_ITConfig(TIM1, TIM_IT_Update|TIM_IT_CC3, ENABLE);
}

void USART_init(void)
{
	//USART2 TX RX
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	//USART2 ST-LINK USB
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	USART_InitTypeDef USART_InitStructure;
	//USART_ClockInitTypeDef USART_ClockInitStructure; 
	
	USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
 	USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(USART2, &USART_InitStructure);
	USART_Cmd(USART2, ENABLE);
	
	
	NVIC_InitTypeDef NVIC_InitStructure;
	// Enable the USART2 TX Interrupt 
	USART_ITConfig(USART2, USART_IT_TC, ENABLE );
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	// Enable the USART2 RX Interrupt
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE );
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	msg_i=0;
	
	// Enable USART3 and GPIOB clocks
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	
	// Configure PB10 and PB11 as USART3 pins
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
  // Initialize USART3
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
  USART_Init(USART3, &USART_InitStructure);
	  	USART_Cmd(USART3,ENABLE);
}

void USARTSend(char *pucBuffer, unsigned long ulCount)
{
  	//loop until the whole string is sent
  	while(ulCount--)
	{
    		USART_SendData(USART3, *pucBuffer++);
    		/* loop until the end of transmission */
    		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
		{
    		}
  	}
}


//void TIM3_delay_us(int us){
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
//	TIM_TimeBaseInitTypeDef timerInitStructure;
//	timerInitStructure.TIM_ClockDivision =0;
//	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	
//  timerInitStructure.TIM_Prescaler = 72-1; // 1(72MHz/72) = 1us, 72
//	timerInitStructure.TIM_Period = 1; // 1us*10000 = 10ms, 10000
//	timerInitStructure.TIM_RepetitionCounter = 0;
//	TIM_TimeBaseInit(TIM3, &timerInitStructure);
//	TIM_Cmd(TIM3, ENABLE);
//	TIM_ITConfig(TIM3, TIM_IT_Update,ENABLE);
//	while(us){
//		if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) 
//		{
//			us--;
//		}
//		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
//	}
//	TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
//	TIM_Cmd(TIM3, DISABLE);
//		

//}

int main(void) {
	
	// Update SystemCoreClock value
	SystemCoreClockUpdate();
	
	// Configure the SysTick timer to overflow every 1 ms
	SysTick_Config(SystemCoreClock / 1000);
	
	USART_init();

	
	// Setup PA5 and PA4
	// PA5 = SPI2_SCK
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_SPI1EN, ENABLE);
	
	// SPI initialization
  SPI_InitTypeDef   SPI_InitStructure;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  // 36 MHz / 256 = 140.625 kHz
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_Init(SPI1, &SPI_InitStructure);
	
	// Enable the receive interrupt
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
  // Enable SPI1
  SPI_Cmd(SPI1, ENABLE);
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
	//GPIO set up for PA8(green) PA9(blue) PA10(red) 
	//GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8|GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//GPIO set up for PB13 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13|GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//PB6 LED
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	//Timer 1 set up 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);

  TIM_TimeBaseInitTypeDef timerInitStructure; 
  timerInitStructure.TIM_Prescaler = 720-1;//1/(72Mhz/720)=0.01ms=0.00001s
  timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  timerInitStructure.TIM_Period = 200-1;//0.00001s*200=0.002s=1/500Hz
  timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  timerInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM1, &timerInitStructure);
  TIM_Cmd(TIM1, ENABLE);
	
	

	
	
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Enable);
	
  TIM_CtrlPWMOutputs(TIM1,ENABLE);
	//http://news.eeworld.com.cn/mcu/ic471919.html
	
	
	
	  	//TIM1_CH3_CH3N_init();
		// config TIM1 CH3N I/O (PB15) BUT do not enable TIM1 CH3N
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  	GPIO_InitTypeDef init_gpio;
  	init_gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  	init_gpio.GPIO_Pin = GPIO_Pin_15;
  	init_gpio.GPIO_Speed = GPIO_Speed_2MHz;
  	GPIO_Init(GPIOB, &init_gpio);
		
		
		GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
	while(1) 
	{
		
		//generate a pulse to PB15 (TIM1_CH3N)
		//write your program code here
		//you can use a timer and GPIO to generate the pulse
		

//		GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
//		TIM3_delay_us(10);
//		GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);		
//		
		SPI_I2S_SendData(SPI1,led8bit);
		led8bit=(led8bit<<1);
		if(led8bit==0)
		{
			led8bit=1;
		}
		
		
		GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
		DelayMs(8);
		GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);
		DelayMs(8);
		if(PWMmode==2)//_____________________________________________________________forward
		{
			//Enable Tim1 CH1 PWM PA8 PA13
			TIM_OCInitTypeDef CH1;
			CH1.TIM_OCMode = TIM_OCMode_PWM1;
			CH1.TIM_Pulse =183;//
			CH1.TIM_OutputState = TIM_OutputState_Enable;
			CH1.TIM_OCPolarity = TIM_OCPolarity_High;
			//CH1.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH1.TIM_OutputNState = TIM_OutputNState_Disable;//N
			CH1.TIM_OCNPolarity = TIM_OCNPolarity_Low;//N
			//CH1.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC1Init(TIM1, &CH1);//PA8 
			
			TIM_OCInitTypeDef CH2;
			CH2.TIM_OCMode = TIM_OCMode_PWM1;
			CH2.TIM_Pulse =188;//
			CH2.TIM_OutputState = TIM_OutputState_Disable;
			CH2.TIM_OCPolarity = TIM_OCPolarity_Low;
			//CH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH2.TIM_OutputNState = TIM_OutputNState_Enable;//N
			CH2.TIM_OCNPolarity = TIM_OCNPolarity_High;//N
			//CH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC2Init(TIM1, &CH2);//PA9
			
			
			DelayMs(200);
			
			//Enable Tim1 CH1 PWM PA8 PA13
			CH1.TIM_OCMode = TIM_OCMode_PWM1;
			CH1.TIM_Pulse =0;//
			CH1.TIM_OutputState = TIM_OutputState_Enable;
			CH1.TIM_OCPolarity = TIM_OCPolarity_High;
			//CH1.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH1.TIM_OutputNState = TIM_OutputNState_Disable;//N
			CH1.TIM_OCNPolarity = TIM_OCNPolarity_Low;//N
			//CH1.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC1Init(TIM1, &CH1);//PA8 
			
			CH2.TIM_OCMode = TIM_OCMode_PWM1;
			CH2.TIM_Pulse =0;//
			CH2.TIM_OutputState = TIM_OutputState_Disable;
			CH2.TIM_OCPolarity = TIM_OCPolarity_Low;
			//CH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH2.TIM_OutputNState = TIM_OutputNState_Enable;//N
			CH2.TIM_OCNPolarity = TIM_OCNPolarity_High;//N
			//CH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC2Init(TIM1, &CH2);//PA9
		
			
			PWMmode=-1;
		}
		if(PWMmode==3)//_____________________________________________________________backward
		{
			//Enable Tim1 CH1 PWM PA8 PA13
			TIM_OCInitTypeDef DCH1;
			DCH1.TIM_OCMode = TIM_OCMode_PWM1;
			DCH1.TIM_Pulse =160;//
			DCH1.TIM_OutputState = TIM_OutputState_Disable;
			DCH1.TIM_OCPolarity = TIM_OCPolarity_Low;
			//DCH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			DCH1.TIM_OutputNState = TIM_OutputNState_Enable;//N
			DCH1.TIM_OCNPolarity = TIM_OCNPolarity_High;//N
			//DCH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC1Init(TIM1, &DCH1);//PB13 CH1N
			
			TIM_OCInitTypeDef DCH2;
			DCH2.TIM_OCMode = TIM_OCMode_PWM1;
			DCH2.TIM_Pulse =170;//
			DCH2.TIM_OutputState = TIM_OutputState_Enable;
			DCH2.TIM_OCPolarity = TIM_OCPolarity_High;
			//DCH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			DCH2.TIM_OutputNState = TIM_OutputNState_Disable;//N
			DCH2.TIM_OCNPolarity = TIM_OCNPolarity_Low;//N
			//DCH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC2Init(TIM1, &DCH2);//PA9 CH2
		
			DelayMs(200);
			
			//Enable Tim1 CH1 PWM PA8 PA13
			DCH1.TIM_OCMode = TIM_OCMode_PWM1;
			DCH1.TIM_Pulse =0;//
			DCH1.TIM_OutputState = TIM_OutputState_Enable;
			DCH1.TIM_OCPolarity = TIM_OCPolarity_High;
			//DCH1.TIM_OCIdleState=TIM_OCIdleState_Reset;
			DCH1.TIM_OutputNState = TIM_OutputNState_Disable;//N
			DCH1.TIM_OCNPolarity = TIM_OCNPolarity_Low;//N
			//DCH1.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC1Init(TIM1, &DCH1);//PA8 
			
			DCH2.TIM_OCMode = TIM_OCMode_PWM1;
			DCH2.TIM_Pulse =0;//
			DCH2.TIM_OutputState = TIM_OutputState_Disable;
			DCH2.TIM_OCPolarity = TIM_OCPolarity_Low;
			//DCH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			DCH2.TIM_OutputNState = TIM_OutputNState_Enable;//N
			DCH2.TIM_OCNPolarity = TIM_OCNPolarity_High;//N
			//DCH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC2Init(TIM1, &DCH2);//PA9
		
		
			PWMmode=-1;
		}
		if(PWMmode==4)//_____________________________________________________________LEFT
		{
			// try by yourself
						//Enable Tim1 CH1 PWM PA8 PA13
			TIM_OCInitTypeDef CH1;
			CH1.TIM_OCMode = TIM_OCMode_PWM1;
			CH1.TIM_Pulse =94;//
			CH1.TIM_OutputState = TIM_OutputState_Enable;
			CH1.TIM_OCPolarity = TIM_OCPolarity_High;
			//CH1.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH1.TIM_OutputNState = TIM_OutputNState_Disable;//N
			CH1.TIM_OCNPolarity = TIM_OCNPolarity_Low;//N
			//CH1.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC1Init(TIM1, &CH1);//PA8 
			
			TIM_OCInitTypeDef CH2;
			CH2.TIM_OCMode = TIM_OCMode_PWM1;
			CH2.TIM_Pulse =188;//
			CH2.TIM_OutputState = TIM_OutputState_Disable;
			CH2.TIM_OCPolarity = TIM_OCPolarity_Low;
			//CH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH2.TIM_OutputNState = TIM_OutputNState_Enable;//N
			CH2.TIM_OCNPolarity = TIM_OCNPolarity_High;//N
			//CH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC2Init(TIM1, &CH2);//PA9
			
			
			DelayMs(200);
			
			//Enable Tim1 CH1 PWM PA8 PA13
			CH1.TIM_OCMode = TIM_OCMode_PWM1;
			CH1.TIM_Pulse =0;//
			CH1.TIM_OutputState = TIM_OutputState_Enable;
			CH1.TIM_OCPolarity = TIM_OCPolarity_High;
			//CH1.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH1.TIM_OutputNState = TIM_OutputNState_Disable;//N
			CH1.TIM_OCNPolarity = TIM_OCNPolarity_Low;//N
			//CH1.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC1Init(TIM1, &CH1);//PA8 
			
			CH2.TIM_OCMode = TIM_OCMode_PWM1;
			CH2.TIM_Pulse =0;//
			CH2.TIM_OutputState = TIM_OutputState_Disable;
			CH2.TIM_OCPolarity = TIM_OCPolarity_Low;
			//CH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH2.TIM_OutputNState = TIM_OutputNState_Enable;//N
			CH2.TIM_OCNPolarity = TIM_OCNPolarity_High;//N
			//CH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC2Init(TIM1, &CH2);//PA9
		
			
			PWMmode=-1;
			

		}
		if(PWMmode==5)//_____________________________________________________________RIGHT
		{
			// try by yourself
						//Enable Tim1 CH1 PWM PA8 PA13
			TIM_OCInitTypeDef CH1;
			CH1.TIM_OCMode = TIM_OCMode_PWM1;
			CH1.TIM_Pulse =188;//
			CH1.TIM_OutputState = TIM_OutputState_Enable;
			CH1.TIM_OCPolarity = TIM_OCPolarity_High;
			//CH1.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH1.TIM_OutputNState = TIM_OutputNState_Disable;//N
			CH1.TIM_OCNPolarity = TIM_OCNPolarity_Low;//N
			//CH1.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC1Init(TIM1, &CH1);//PA8 
			
			TIM_OCInitTypeDef CH2;
			CH2.TIM_OCMode = TIM_OCMode_PWM1;
			CH2.TIM_Pulse =94;//
			CH2.TIM_OutputState = TIM_OutputState_Disable;
			CH2.TIM_OCPolarity = TIM_OCPolarity_Low;
			//CH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH2.TIM_OutputNState = TIM_OutputNState_Enable;//N
			CH2.TIM_OCNPolarity = TIM_OCNPolarity_High;//N
			//CH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC2Init(TIM1, &CH2);//PA9
			
			
			DelayMs(200);
			
			//Enable Tim1 CH1 PWM PA8 PA13
			CH1.TIM_OCMode = TIM_OCMode_PWM1;
			CH1.TIM_Pulse =0;//
			CH1.TIM_OutputState = TIM_OutputState_Enable;
			CH1.TIM_OCPolarity = TIM_OCPolarity_High;
			//CH1.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH1.TIM_OutputNState = TIM_OutputNState_Disable;//N
			CH1.TIM_OCNPolarity = TIM_OCNPolarity_Low;//N
			//CH1.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC1Init(TIM1, &CH1);//PA8 
			
			CH2.TIM_OCMode = TIM_OCMode_PWM1;
			CH2.TIM_Pulse =0;//
			CH2.TIM_OutputState = TIM_OutputState_Disable;
			CH2.TIM_OCPolarity = TIM_OCPolarity_Low;
			//CH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH2.TIM_OutputNState = TIM_OutputNState_Enable;//N
			CH2.TIM_OCNPolarity = TIM_OCNPolarity_High;//N
			//CH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC2Init(TIM1, &CH2);//PA9
		
			
			PWMmode=-1;
		}
		if(PWMmode==6)//_____________________________________________________________forward
		{
			//Enable Tim1 CH1 PWM PA8 PA13
			TIM_OCInitTypeDef CH1;
			CH1.TIM_OCMode = TIM_OCMode_PWM1;
			CH1.TIM_Pulse =150;//
			CH1.TIM_OutputState = TIM_OutputState_Enable;
			CH1.TIM_OCPolarity = TIM_OCPolarity_High;
			//CH1.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH1.TIM_OutputNState = TIM_OutputNState_Disable;//N
			CH1.TIM_OCNPolarity = TIM_OCNPolarity_Low;//N
			//CH1.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC1Init(TIM1, &CH1);//PA8 
			
			TIM_OCInitTypeDef CH2;
			CH2.TIM_OCMode = TIM_OCMode_PWM1;
			CH2.TIM_Pulse =150;//
			CH2.TIM_OutputState = TIM_OutputState_Disable;
			CH2.TIM_OCPolarity = TIM_OCPolarity_Low;
			//CH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH2.TIM_OutputNState = TIM_OutputNState_Enable;//N
			CH2.TIM_OCNPolarity = TIM_OCNPolarity_High;//N
			//CH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC2Init(TIM1, &CH2);//PA9
			
			
			DelayMs(200);
			
			//Enable Tim1 CH1 PWM PA8 PA13
			CH1.TIM_OCMode = TIM_OCMode_PWM1;
			CH1.TIM_Pulse =0;//
			CH1.TIM_OutputState = TIM_OutputState_Enable;
			CH1.TIM_OCPolarity = TIM_OCPolarity_High;
			//CH1.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH1.TIM_OutputNState = TIM_OutputNState_Disable;//N
			CH1.TIM_OCNPolarity = TIM_OCNPolarity_Low;//N
			//CH1.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC1Init(TIM1, &CH1);//PA8 
			
			CH2.TIM_OCMode = TIM_OCMode_PWM1;
			CH2.TIM_Pulse =0;//
			CH2.TIM_OutputState = TIM_OutputState_Disable;
			CH2.TIM_OCPolarity = TIM_OCPolarity_Low;
			//CH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH2.TIM_OutputNState = TIM_OutputNState_Enable;//N
			CH2.TIM_OCNPolarity = TIM_OCNPolarity_High;//N
			//CH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC2Init(TIM1, &CH2);//PA9
		
			
			PWMmode=-1;
		}
		if(PWMmode==8)//_____________________________________________________________forward
		{
			//Enable Tim1 CH1 PWM PA8 PA13
			TIM_OCInitTypeDef CH1;
			CH1.TIM_OCMode = TIM_OCMode_PWM1;
			CH1.TIM_Pulse =130;//
			CH1.TIM_OutputState = TIM_OutputState_Enable;
			CH1.TIM_OCPolarity = TIM_OCPolarity_High;
			//CH1.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH1.TIM_OutputNState = TIM_OutputNState_Disable;//N
			CH1.TIM_OCNPolarity = TIM_OCNPolarity_Low;//N
			//CH1.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC1Init(TIM1, &CH1);//PA8 
			
			TIM_OCInitTypeDef CH2;
			CH2.TIM_OCMode = TIM_OCMode_PWM1;
			CH2.TIM_Pulse =130;//
			CH2.TIM_OutputState = TIM_OutputState_Disable;
			CH2.TIM_OCPolarity = TIM_OCPolarity_Low;
			//CH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH2.TIM_OutputNState = TIM_OutputNState_Enable;//N
			CH2.TIM_OCNPolarity = TIM_OCNPolarity_High;//N
			//CH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC2Init(TIM1, &CH2);//PA9
			
			
			DelayMs(200);
			
			//Enable Tim1 CH1 PWM PA8 PA13
			CH1.TIM_OCMode = TIM_OCMode_PWM1;
			CH1.TIM_Pulse =0;//
			CH1.TIM_OutputState = TIM_OutputState_Enable;
			CH1.TIM_OCPolarity = TIM_OCPolarity_High;
			//CH1.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH1.TIM_OutputNState = TIM_OutputNState_Disable;//N
			CH1.TIM_OCNPolarity = TIM_OCNPolarity_Low;//N
			//CH1.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC1Init(TIM1, &CH1);//PA8 
			
			CH2.TIM_OCMode = TIM_OCMode_PWM1;
			CH2.TIM_Pulse =0;//
			CH2.TIM_OutputState = TIM_OutputState_Disable;
			CH2.TIM_OCPolarity = TIM_OCPolarity_Low;
			//CH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH2.TIM_OutputNState = TIM_OutputNState_Enable;//N
			CH2.TIM_OCNPolarity = TIM_OCNPolarity_High;//N
			//CH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC2Init(TIM1, &CH2);//PA9
		
			
			PWMmode=-1;
		}
		if(PWMmode==9)//_____________________________________________________________forward
		{
			//Enable Tim1 CH1 PWM PA8 PA13
			TIM_OCInitTypeDef CH1;
			CH1.TIM_OCMode = TIM_OCMode_PWM1;
			CH1.TIM_Pulse =130;//
			CH1.TIM_OutputState = TIM_OutputState_Enable;
			CH1.TIM_OCPolarity = TIM_OCPolarity_High;
			//CH1.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH1.TIM_OutputNState = TIM_OutputNState_Disable;//N
			CH1.TIM_OCNPolarity = TIM_OCNPolarity_Low;//N
			//CH1.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC1Init(TIM1, &CH1);//PA8 
			
			TIM_OCInitTypeDef CH2;
			CH2.TIM_OCMode = TIM_OCMode_PWM1;
			CH2.TIM_Pulse =0;//
			CH2.TIM_OutputState = TIM_OutputState_Disable;
			CH2.TIM_OCPolarity = TIM_OCPolarity_Low;
			//CH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH2.TIM_OutputNState = TIM_OutputNState_Enable;//N
			CH2.TIM_OCNPolarity = TIM_OCNPolarity_High;//N
			//CH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC2Init(TIM1, &CH2);//PA9
			
			
			DelayMs(200);
			
			//Enable Tim1 CH1 PWM PA8 PA13
			CH1.TIM_OCMode = TIM_OCMode_PWM1;
			CH1.TIM_Pulse =0;//
			CH1.TIM_OutputState = TIM_OutputState_Enable;
			CH1.TIM_OCPolarity = TIM_OCPolarity_High;
			//CH1.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH1.TIM_OutputNState = TIM_OutputNState_Disable;//N
			CH1.TIM_OCNPolarity = TIM_OCNPolarity_Low;//N
			//CH1.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC1Init(TIM1, &CH1);//PA8 
			
			CH2.TIM_OCMode = TIM_OCMode_PWM1;
			CH2.TIM_Pulse =0;//
			CH2.TIM_OutputState = TIM_OutputState_Disable;
			CH2.TIM_OCPolarity = TIM_OCPolarity_Low;
			//CH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH2.TIM_OutputNState = TIM_OutputNState_Enable;//N
			CH2.TIM_OCNPolarity = TIM_OCNPolarity_High;//N
			//CH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC2Init(TIM1, &CH2);//PA9
		
			
			PWMmode=-1;
		}
		if(PWMmode==7)//_____________________________________________________________forward
		{
			//Enable Tim1 CH1 PWM PA8 PA13
			TIM_OCInitTypeDef CH1;
			CH1.TIM_OCMode = TIM_OCMode_PWM1;
			CH1.TIM_Pulse =0;//
			CH1.TIM_OutputState = TIM_OutputState_Enable;
			CH1.TIM_OCPolarity = TIM_OCPolarity_High;
			//CH1.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH1.TIM_OutputNState = TIM_OutputNState_Disable;//N
			CH1.TIM_OCNPolarity = TIM_OCNPolarity_Low;//N
			//CH1.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC1Init(TIM1, &CH1);//PA8 
			
			TIM_OCInitTypeDef CH2;
			CH2.TIM_OCMode = TIM_OCMode_PWM1;
			CH2.TIM_Pulse =130;//
			CH2.TIM_OutputState = TIM_OutputState_Disable;
			CH2.TIM_OCPolarity = TIM_OCPolarity_Low;
			//CH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH2.TIM_OutputNState = TIM_OutputNState_Enable;//N
			CH2.TIM_OCNPolarity = TIM_OCNPolarity_High;//N
			//CH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC2Init(TIM1, &CH2);//PA9
			
			
			DelayMs(200);
			
			//Enable Tim1 CH1 PWM PA8 PA13
			CH1.TIM_OCMode = TIM_OCMode_PWM1;
			CH1.TIM_Pulse =0;//
			CH1.TIM_OutputState = TIM_OutputState_Enable;
			CH1.TIM_OCPolarity = TIM_OCPolarity_High;
			//CH1.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH1.TIM_OutputNState = TIM_OutputNState_Disable;//N
			CH1.TIM_OCNPolarity = TIM_OCNPolarity_Low;//N
			//CH1.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC1Init(TIM1, &CH1);//PA8 
			
			CH2.TIM_OCMode = TIM_OCMode_PWM1;
			CH2.TIM_Pulse =0;//
			CH2.TIM_OutputState = TIM_OutputState_Disable;
			CH2.TIM_OCPolarity = TIM_OCPolarity_Low;
			//CH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH2.TIM_OutputNState = TIM_OutputNState_Enable;//N
			CH2.TIM_OCNPolarity = TIM_OCNPolarity_High;//N
			//CH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC2Init(TIM1, &CH2);//PA9
		
			
			PWMmode=-1;
		}
		
	}
}



void USART2_IRQHandler() {
	if(USART_GetITStatus(USART2, USART_IT_TC) != RESET) {
		
		
		USART_ClearITPendingBit(USART2, USART_IT_TC);
	}
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 
	{
		character = (unsigned char) USART_ReceiveData(USART2); 
		USART_SendData(USART3, character);
		if(character=='o')
		{
			if(state==0)
			{
				
				GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_SET);
				state=1;
			}
			else
			{
				GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET);
				state=0;
			}
		}
		if(character=='u')
		{
			PWMmode=2;
		}
		if(character=='d')
		{
			PWMmode=3;
		}	
		if(character=='l')
		{
			PWMmode=4;
		}	
		if(character=='r')
		{
			PWMmode=5;
		}
				if(character=='1'){PWMmode=6;}
				if(character=='2'){PWMmode=7;}
				if(character=='3'){PWMmode=8;}
				if(character=='4'){PWMmode=9;}
	
	}
	}


void SPI1_IRQHandler() 
{
//  if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_TXE) == SET) 
//	{
//  
//    SPI_I2S_SendData(SPI1,'5');
//  }
//  if(SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE) == SET) 
//	{
//
//  }
	
}
void DelayMs(uint32_t ms)
{
	// Reload ms value
	msTicks = ms;
	// Wait until msTick reach zero
	while (msTicks);
}

// SysTick_Handler function will be called every 1 ms
void SysTick_Handler()
{
	if (msTicks != 0)
	{
		msTicks--;
	}
}

void USART3_IRQHandler()
{
	if(USART_GetITStatus(USART3, USART_IT_TC) != RESET) 
	{
		
	USART_ClearITPendingBit(USART3, USART_IT_TC);
	}
	//if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) 
	//{
	//character = (unsigned char) USART_ReceiveData(USART3); 
	//USART_SendData(USART3, character);
	//}	
}

void TIM1_CC_IRQHandler(void)
{
    	if(TIM_GetITStatus(TIM1, TIM_IT_CC3) != RESET)
	{
      		if(!pulseHigh)
		{
        		pulseHigh = true;   //pulse starts
        		TIM_SetCounter(TIM1, 0);
		        //change to detect the falling edge
        		TIM_OC3PolarityConfig(TIM1, TIM_ICPolarity_Falling);
	      	}
		else
		{
        		pulseWidth += TIM_GetCounter(TIM1);
        		captureValue = pulseWidth;
			// output an integer only not a floating point number
			// very similar to captureValue/2*330/1000000
        		distance = (captureValue/2)/30; // in cm
        		sprintf(buffer, "\rdistance = %d  ", distance);
        		USARTSend(buffer, sizeof(buffer));
        
        		//change to detect the rising edge
	        	TIM_OC3PolarityConfig(TIM1, TIM_ICPolarity_Rising);
		        pulseHigh = false;
		        pulseWidth = 0;
	      	}
    	}
	//clear interrupt flag
  	TIM_ClearITPendingBit(TIM1, TIM_IT_Update|TIM_IT_CC3);
}
