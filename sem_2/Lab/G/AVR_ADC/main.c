/*
 * AVR_ADC.c
 *
 * Created: 13/2/2023 15:49:26
 * Author : user
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>

void pwm_init();
uint16_t thre=100;

int main(void)
{
    pwm_init();
	DDRB=0xff;		//output
	DDRD=0xff;		//output
	DDRC=0;					//Analog input port
	sei();
	ADCSRA=0x8F;
	ADMUX=0xC0;
	
	ADCSRA|=(1<<ADSC);
	
    while (1) 
    {
    }
	return 0;
}

// OC0B(PD5), OC1B(PB2), OC2B(PD3)
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
		TCCR1A = (1<<WGM11) | (1<<WGM10) |
		(1<<COM1A1) | (0<<COM1A0) |
		(1<<COM1B1) | (0<<COM1B0);
		//	Pre = 256, Non-inverting
		TCCR1B = (1<<WGM13) |(1<<WGM12) | 
		(1<<CS12) | (0<<CS11) | (0<<CS10);
		//	Freq = 500Hz
		OCR1AH = 0;
		OCR1AL = 124;
		//	DC
		OCR1BH = 0;
		OCR1BL = 0;	
		
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
	
	switch (ADMUX)
	{
		case 0xC0:
		if (result>thre){OCR0B = 100;}else{OCR0B = 0;}
		ADMUX=0xC1;
		break;
		
		case 0xC1:
		if (result>thre){OCR1BL = 100;}else{OCR1BL = 0;}
		ADMUX=0xC2;
		break;
		
		case 0xC2:
		if (result>thre){OCR2B = 100;}else{OCR2B = 0;}
		ADMUX=0xC0;
		break;
		
		default:
		break;
	}
		ADCSRA|=(1<<ADSC);
}