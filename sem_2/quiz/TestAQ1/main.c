
#include <avr/io.h>

int state=0;

void pwm_init()
{
		TCCR0A = (1 << COM0A1) | (0 << COM0A0) |
		(1 << COM0B1) | (0 << COM0B0) |
		(1 << WGM01) | (1 << WGM00);	
		TCCR0B = (1 << WGM02) |
		(1 << CS02) | (0 << CS01) | (0 << CS00);
		OCR0A = 124;
		OCR0B = 0;
}

void delay_1s()
{
	OCR2A=0x7d;
	TCCR2A=0x02;
	TCCR2B=0x05;
	while((TIFR2&(1<<OCF2A))==0);
	TCCR2A=0x00;
	TCCR2B=0x00;
	TIFR2=0x02;
}



int main(void)
{
    /* Replace with your application code */

	DDRD=0b00100000;
	pwm_init();
	
   while (1) 
    {

		if (state==0)
		{
			OCR0B = 99;
			state=1;
		
		}else
		{
			OCR0B = 24;
			state=0;
		}
	for(int i=0; i<=1250; i++){delay_1s();}
    }
}

