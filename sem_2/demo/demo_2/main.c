/*log
Tested USART3 and successfully send out character
SPI2.c change the char c in unsigned
SPI2.c change all PB13 to PA5,PB15 to PA7
SPI2.c change SPI2 to SPI1 
SPI2.c change RCC_APB1ENR_SPI2EN to RCC_APB2ENR_SPI1EN



*/


//0:black line; 1:white space
#include "stm32f10x.h"

unsigned int state=0;
unsigned int cnt=0;
unsigned char c;
static __IO uint32_t msTicks;
unsigned int mode=0;
unsigned int cps=0;
unsigned long rt=0;

char line='o';

void DelayMs(uint32_t ms);
void readFloor();
void linetrack();

void pwm_forward(int L_pwm, int R_pwm, int T);
void pwm_backward(int L_pwm, int R_pwm, int T);
void pwm_cw(int turning_pwm, int T);
void pwm_atcw(int turning_pwm, int T);

void init_clk()
{
		// Setup PA5 and PA7
	// PA5 = SPI1_SCK, PA7 = IR LED / MODE
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
	
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_SPI1EN, ENABLE);

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
	
	// Enable USART3 and GPIOB clocks
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	// Configure PB10 and PB11 as USART3 pins
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  // Initialize USART3
	USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx;
  USART_Init(USART3, &USART_InitStructure);
	
	// Enable USART2 and GPIOB clocks
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// Configure PA2 and PA3 as USART3 pins
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);

  // Enable USART2,USART23
  USART_Cmd(USART3, ENABLE);
	USART_Cmd(USART2, ENABLE);
	
	
	
	
	//clk_pwm init
	
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
  timerInitStructure.TIM_Period = 2000-1;//0.00001s*200=0.002s=1/500Hz
  timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  timerInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM1, &timerInitStructure);
  TIM_Cmd(TIM1, ENABLE);
	
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Enable);
	
  TIM_CtrlPWMOutputs(TIM1,ENABLE);
}


int main(void)
{
	// Update SystemCoreClock value
	SystemCoreClockUpdate();
	
	// Configure the SysTick timer to overflow every 1 ms
	SysTick_Config(SystemCoreClock / 1000);
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

  init_clk();	

	while(1)
	{
		readFloor();
		DelayMs(3);//can not too fast, maybe need to clear some flag
					


		if (   ((~(c&(1<<3))|~(c&(1<<4))))&((c&(1<<1))|(c&(1<<2)))&((c&(1<<5))|(c&(1<<6)))
			|(c==0xff)|((c==0xfc)&(line=='o'))|((c==0xfe)&(line=='o'))) {				//(cps==1)|
			cnt++; rt=0; cps=0;			
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
			

			
			switch (cnt){
			case 1:
				for (int i=0; i<8; i++){pwm_forward(425,425, 100);}
				DelayMs(200);
				break;
				
			case 2:

				for (int i=0; i<6; i++){pwm_forward(250,755, 100);}
				DelayMs(200);
				break;
				
			case 3:
				for (int i=0; i<8; i++){pwm_forward(425,425, 100);}
				DelayMs(200);
				break;
			
			case 4:					//switch to inner line
				for (int i=0; i<18; i++){pwm_forward(215,775, 70);}
				line='i';
				DelayMs(200);

				break;
				
			case 5:					//switch to outer line
				for (int i=0; i<19; i++){pwm_forward(250,815, 70);}
			  line='o';
			 DelayMs(200);
				break;
				
			case 6:
				for (int i=0; i<8; i++){pwm_forward(425,425, 100);}
				DelayMs(200);
				break;
				
			case 7:
				for (int i=0; i<7; i++){pwm_forward(250,745, 100);}
				DelayMs(200);
				break;

      case 8:
				for (int i=0; i<8; i++){pwm_forward(425,425, 100);}
				DelayMs(200);
				break;

			case 9:					//switch to inner line
				for (int i=0; i<6; i++){pwm_atcw(665, 100);}
				for (int i=0; i<11; i++){pwm_forward(470,470, 100);}
				DelayMs(200);
				while(c==0x00){pwm_forward(260,260, 70);}
				line='i';
				break;

				
				
			default:
				break;}
			rt=0;
			
			
		}	
		
				if (c==0x00) {mode=mode;}
		else if ((c&(1<<3))|(c&(1<<4))){mode=3;}
		else if ((c&(1<<0))|(c&(1<<1))){mode=1;}
		else if ((c&(1<<6))|(c&(1<<7))){mode=2;}				//(c&(1<<7))|(c&(1<<6))



		
		if(line=='o'){
					switch (mode){
			case 1:
				pwm_forward(705,220, 80);
				break;
			case 2:
				pwm_forward(225,730, 80);
				break;
			case 3:
				pwm_forward(475,485, 80);
				break;
			default:
				break;}}
			
			if(line=='i'){

			switch (mode){
			case 1:
				pwm_forward(765,205, 60);
				break;
			case 2:
				pwm_forward(205,765, 60);
				break;
			case 3:
				pwm_forward(395,395, 60);
				break;

			default:
				break;}
			
			
			}
		
	}
}



void pwm_forward(int L_pwm, int R_pwm, int T)
{
		//Enable Tim1 CH1 PWM PA8 PA13
			TIM_OCInitTypeDef CH1;
			CH1.TIM_OCMode = TIM_OCMode_PWM1;
			CH1.TIM_Pulse =L_pwm;//
			CH1.TIM_OutputState = TIM_OutputState_Enable;
			CH1.TIM_OCPolarity = TIM_OCPolarity_High;
			//CH1.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH1.TIM_OutputNState = TIM_OutputNState_Disable;//N
			CH1.TIM_OCNPolarity = TIM_OCNPolarity_Low;//N
			//CH1.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC1Init(TIM1, &CH1);//PA8 
			
			TIM_OCInitTypeDef CH2;
			CH2.TIM_OCMode = TIM_OCMode_PWM1;
			CH2.TIM_Pulse =R_pwm;//
			CH2.TIM_OutputState = TIM_OutputState_Disable;
			CH2.TIM_OCPolarity = TIM_OCPolarity_Low;
			//CH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH2.TIM_OutputNState = TIM_OutputNState_Enable;//N
			CH2.TIM_OCNPolarity = TIM_OCNPolarity_High;//N
			//CH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC2Init(TIM1, &CH2);//PA9
			
			
			DelayMs(T);
			
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
}
	
	
void pwm_atcw(int turning_pwm,  int T)
{
			//Enable Tim1 CH1 PWM PA8 PA13
			TIM_OCInitTypeDef DCH1;
			DCH1.TIM_OCMode = TIM_OCMode_PWM1;
			DCH1.TIM_Pulse =turning_pwm;//
			DCH1.TIM_OutputState = TIM_OutputState_Disable;
			DCH1.TIM_OCPolarity = TIM_OCPolarity_Low;
			//DCH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			DCH1.TIM_OutputNState = TIM_OutputNState_Enable;//N
			DCH1.TIM_OCNPolarity = TIM_OCNPolarity_High;//N
			//DCH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC1Init(TIM1, &DCH1);//PB13 CH1N
	
			TIM_OCInitTypeDef CH2;
			CH2.TIM_OCMode = TIM_OCMode_PWM1;
			CH2.TIM_Pulse =turning_pwm;//
			CH2.TIM_OutputState = TIM_OutputState_Disable;
			CH2.TIM_OCPolarity = TIM_OCPolarity_Low;
			//CH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH2.TIM_OutputNState = TIM_OutputNState_Enable;//N
			CH2.TIM_OCNPolarity = TIM_OCNPolarity_High;//N
			//CH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC2Init(TIM1, &CH2);//PA9
			
			
			DelayMs(T);
			
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
			
			CH2.TIM_OCMode = TIM_OCMode_PWM1;
			CH2.TIM_Pulse =0;//
			CH2.TIM_OutputState = TIM_OutputState_Disable;
			CH2.TIM_OCPolarity = TIM_OCPolarity_Low;
			//CH2.TIM_OCIdleState=TIM_OCIdleState_Reset;
			CH2.TIM_OutputNState = TIM_OutputNState_Enable;//N
			CH2.TIM_OCNPolarity = TIM_OCNPolarity_High;//N
			//CH2.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//N
			TIM_OC2Init(TIM1, &CH2);//PA9
}


void readFloor() 
{
	// Set PA7 to 1
	GPIO_SetBits(GPIOA, GPIO_Pin_7);
	// New code: it can turn on Infra-red LED in 3 ms so that
	// the photoresistors can get the reading properly
	DelayMs(3);
	// Initialize the data transmission from the master to the slave
	SPI_I2S_SendData(SPI1, 0);
	// Enable the interrupt to receive data by using the ISR handler
	NVIC_EnableIRQ(SPI1_IRQn);
}
// put the readings to the variable c
void SPI1_IRQHandler() 
{

	// the received character has all the readings
	 c = (char) SPI_I2S_ReceiveData(SPI1) & 0xff;
	
	if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_7) == 1) 
	{
	  // Set PA7 to 0 to trigger the shift register
	  GPIO_ResetBits(GPIOA, GPIO_Pin_7);
	  // Go to get the next reading
	  SPI_I2S_SendData(SPI1, 0);
		
	} 
	else 
	{
	//debug and test code
	{
	if(c&(1<<0))
	{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	
	USART_SendData(USART2,'0');
	}
	else
	{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	
	USART_SendData(USART2,'1');
	}
	if(c&(1<<1))
	{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	
	USART_SendData(USART2,'0');
	}
	else
	{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	
	USART_SendData(USART2,'1');
	}	
	if(c&(1<<2))
	{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	
	USART_SendData(USART2,'0');
	}
	else
	{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	
	USART_SendData(USART2,'1');
	}	
	if(c&(1<<3))
	{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	
	USART_SendData(USART2,'0');
	}
	else
	{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	
	USART_SendData(USART2,'1');
	}	
	if(c&(1<<4))
	{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	
	USART_SendData(USART2,'0');
	}
	else
	{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	
	USART_SendData(USART2,'1');
	}	
	if(c&(1<<5))
	{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	
	USART_SendData(USART2,'0');
	}
	else
	{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	
	USART_SendData(USART2,'1');
	}	
	if(c&(1<<6))
	{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	
	USART_SendData(USART2,'0');
	}
	else
	{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	
	USART_SendData(USART2,'1');
	}	
	if(c&(1<<7))
	{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	
	USART_SendData(USART2,'0');
	}
	else
	{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	
	USART_SendData(USART2,'1');
	}	
	
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	USART_SendData(USART2, c);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	USART_SendData(USART2, '\r');
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	USART_SendData(USART2, '\n');
	}
		
	
	// Check PA7. If it is 1, it means the data is ready
	
	  // disable the interrupt because it is not ready
	  NVIC_DisableIRQ(SPI1_IRQn);
	
	}

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
	rt++;
	if ((rt==6000)&(line=='o')){cps=1;}
	
}

