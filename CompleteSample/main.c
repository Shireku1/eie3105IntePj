#include "stm32f10x.h"                  // Device header
#include "string.h"
#include "stdbool.h"
#include "stdio.h"

//static bool up =false;//B5
//static bool down =false;//B9
//static bool left =false;//B8
//static bool right =false;//B6
char buffer[50] = {'0'};
volatile uint16_t ADC_values[3];
volatile uint32_t status = 0;
static __IO uint32_t msTicks;
void DelayMs(uint32_t ms);

void LED(uint32_t ms);


void USARTSend(char *pucBuffer, unsigned long ulCount)
{

//
// Loop while there are more characters to send.
//
while(ulCount--)
{
USART_SendData(USART1, *pucBuffer++);
/* Loop until the end of transmission */
while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
{
}
}
}
int main(void)
{
	//PCLK2 is the APB2 clock 
	//ADCCLK = PCLK2/6 = 72/6 = 12MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	//Configure I/O for  ADC
	//no need to set default is input floating 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE); 
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* Enable ADC1 clock so that we can talk to it */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	/* Put everything back to power-on defaults */
	ADC_DeInit(ADC1);
	
	/* ADC1 Configuration */
	ADC_InitTypeDef  ADC_InitStructure;
	/* ADC1 and ADC2 operate independently */ 
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	/* Disable the scan conversion so we do one at a time */ 
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; 
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	/* Start conversion by software, not an external trigger */ 
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	/* 12-bit conversions: put them in the lower 12 bits of the result */
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	/* Say how many channels would be used by the sequencer */
	ADC_InitStructure.ADC_NbrOfChannel = 3;

	// define regular conversion configuration
	ADC_RegularChannelConfig(ADC1,ADC_Channel_7,1,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_8,2,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_9,3,ADC_SampleTime_239Cycles5);
	/* Now do the setup */
	ADC_Init(ADC1, &ADC_InitStructure);
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	//enable DMA for ADC
	ADC_DMACmd(ADC1,ENABLE);


	/* Enable ADC1 reset calibration register */
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));
	/* Start ADC1 calibration */
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));
	
	//enable DMA1 clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	//create DMA structure
	DMA_InitTypeDef  DMA_InitStructure;
	//reset DMA1 channe1 to default values; 
	DMA_DeInit(DMA1_Channel1);
	//channel will not be used for memory to memory transfer
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	//setting circular mode
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	//medium priority
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	//source and destination data size word=32bit
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	//automatic memory destination increment enable.
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//source address increment disable
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	//Location assigned to peripheral register will be source
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	//chunk of data to be transfered
	DMA_InitStructure.DMA_BufferSize = 3;
	//source and destination start addresses
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	//(uint32_t)ADC1_DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_values;
	//send values to DMA registers
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	// Enable DMA1 Channel Transfer Complete interrupt 
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE); 
	DMA_Cmd(DMA1_Channel1, ENABLE); //Enable the DMA1 - Channel1
	NVIC_InitTypeDef NVIC_InitStructure;
	//Enable DMA1 channel IRQ Channel */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	// Update SystemCoreClock value
	SystemCoreClockUpdate();
	
	// Configure the SysTick timer to overflow every 1 ms
	SysTick_Config(SystemCoreClock / 1000);
	
	//USART3 TX RX
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	// Configure I/O for BUTTON
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//internal pull-up resistor
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// Configure I/O for LED
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	
	//USART2 ST-LINK USB
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	USART_InitTypeDef USART_InitStructure;
	//USART_ClockInitTypeDef USART_ClockInitStructure; 
	
	USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
 	USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(USART3, &USART_InitStructure);
	USART_Cmd(USART3, ENABLE);
	
	// Enable the USART3 TX Interrupt 
	USART_ITConfig(USART3, USART_IT_TC, ENABLE );
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	// Enable the USART3 RX Interrupt
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE );
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//USART1 TX RX
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	//USART1 ST-LINK USB
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	//USART_ClockInitTypeDef USART_ClockInitStructure; 
	
	USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
 	USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);

	GPIO_WriteBit(GPIOA, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6, Bit_SET);
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
	DelayMs(100);	
	GPIO_WriteBit(GPIOA, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6, Bit_RESET);
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);	

	// start conversion (will be endless as we are in continuous mode)
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
	while(1)
	{
	if(((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5))&(0x01))==1)
	{
	}
	else
	{
	USART_SendData(USART3, '1');
		USART_SendData(USART1, '1');
	LED(200);	
	}
	if(((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9))&(0x01))==1)
	{
	}
	else
	{
	USART_SendData(USART3, '3');
		USART_SendData(USART1, '3');
	LED(200);	
	}
	if(((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8))&(0x01))==1)
	{
	}
	else
	{
	USART_SendData(USART3, '2');
		USART_SendData(USART1, '2');
	LED(200);
	}
	if(((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6))&(0x01))==1)
	{
	}
	else
	{
	USART_SendData(USART3, '4');
	USART_SendData(USART1, '4');
		LED(200);
	}
	if(((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13))&(0x01))==1)
	{
	}
	else
	{
	USART_SendData(USART3, 'o');
		USART_SendData(USART1, 'o');
	LED(200);
	}
	
	if ((ADC_values[1]>=4000))
	{
	USART_SendData(USART3, 'u');
		USART_SendData(USART1, 'u');
	LED(200);	
	}
	
	if ((ADC_values[1]<=800))
	{
	USART_SendData(USART3, 'd');
		USART_SendData(USART1, 'd');
	LED(200);
	}
	
	if ((ADC_values[0]>=4000))
	{
	USART_SendData(USART3, 'r');
		USART_SendData(USART1, 'r');
	LED(200);
	}
	
	if ((ADC_values[0]<=800))
	{
	USART_SendData(USART3, 'l');
		USART_SendData(USART1, 'l');
	LED(200);
	}
	
	//while(!status);
	//	sprintf(buffer, "ch0=%d ch1=%d ch4=%d\r\n", ADC_values[0],ADC_values[1], ADC_values[2]);
	//	USARTSend(buffer, sizeof(buffer)); 
	//	//TIM_SetCompare1(TIM3, ADC_values[0]); 
	//	//TIM_SetCompare2(TIM3,ADC_values[1]); 
	//	//TIM_SetCompare3(TIM3, ADC_values[2]);
	//	status = 0;
	}
}
void DMA1_Channel1_IRQHandler(void)
{
// Test on DMA1 Channel1 Transfer Complete interrupt
if(DMA_GetITStatus(DMA1_IT_TC1))
{
status=1;
//Clear DMA1 interrupt pending bits
DMA_ClearITPendingBit(DMA1_IT_GL1);
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
void LED(uint32_t ms)//8 LED on PC13 PA0-6
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6, Bit_SET);
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
	// Reload ms value
	msTicks = ms/8;
	// Wait until msTick reach zero
	while (msTicks);
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
	// Reload ms value
	msTicks = ms/8;
	// Wait until msTick reach zero
	while (msTicks);
	GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);
	// Reload ms value
	msTicks = ms/8;
	// Wait until msTick reach zero
	while (msTicks);
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET);
	// Reload ms value
	msTicks = ms/8;
	// Wait until msTick reach zero
	while (msTicks);
	GPIO_WriteBit(GPIOA, GPIO_Pin_2, Bit_RESET);
	// Reload ms value
	msTicks = ms/8;
	// Wait until msTick reach zero
	while (msTicks);
	GPIO_WriteBit(GPIOA, GPIO_Pin_3, Bit_RESET);
	// Reload ms value
	msTicks = ms/8;
	// Wait until msTick reach zero
	while (msTicks);
	GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);
	// Reload ms value
	msTicks = ms/8;
	// Wait until msTick reach zero
	while (msTicks);
	GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
	// Reload ms value
	msTicks = ms/8;
	// Wait until msTick reach zero
	while (msTicks);
	GPIO_WriteBit(GPIOA, GPIO_Pin_6, Bit_RESET);
}
