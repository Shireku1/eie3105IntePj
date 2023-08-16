

#include <avr/io.h>
#include <avr/interrupt.h>

void pwm_init();
uint16_t thre=500;	

int main(void)
{
    /* Replace with your application code */
		DDRD=0xff;		//output
		DDRC=0;			//Analog input port
		pwm_init();

		sei();
		ADCSRA=0x8F;
		ADMUX=0xC0;
		
		ADCSRA|=(1<<ADSC);		

    while (1) 
    {
    }
}

// OC0B(PD5)GREEN, OC2B(PD3)RED
void pwm_init()
{
	//	Fast PWM, Non-inverting
	TCCR0A = (1<<WGM01) | (1<<WGM00) |
	(1<<COM0A1) | (0<<COM0A0) |
	(1<<COM0B1) | (0<<COM0B0);
	//	Pre = 256, Non-inverting
	TCCR0B = (1<<WGM02) | (1<<CS02) | (0<<CS01) | (0<<CS00);
	//	Freq = 500Hz
	OCR0A = 124;
	//	DC
	OCR0B = 0;

	//	Fast PWM, Non-inverting
	TCCR2A = (1<<WGM21) | (1<<WGM20) |
	(1<<COM2A1) | (0<<COM2A0) |
	(1<<COM2B1) | (0<<COM2B0);
	//	Pre = 256, Non-inverting
	TCCR2B = (1<<WGM22) | (1<<CS22) | (1<<CS21) | (0<<CS20);
	//	Freq = 500Hz
	OCR2A = 124;
	//	DC
	OCR2B = 0;
}

ISR(ADC_vect)
{
	
	uint8_t low=ADCL;
	uint16_t result=ADCH<<8|low;
	
	if(result>thre)
	{
	OCR2B=100;
	OCR0B=0;
	}else if(result<thre)
	{
	OCR0B=100;
	OCR2B=0;
	}
	ADCSRA|=(1<<ADSC);
}
