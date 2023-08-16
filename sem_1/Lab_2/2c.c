/*
 * Lab 2c.c
 *
 * Created: 10/10/2022 11:42:05
 * Author : user
 */ 

#include <avr/io.h>

void counter();

int main(void)
{
    /* Replace with your application code */
	DDRC=0xff;

	int s=0;
		
    while (1) 
    {

	if (s==1)						//output according state
	{
		PORTC=0x01;
	}

	if (s==0)
	{
		PORTC=0x00;
	}
	
		if (s==1)
		{s=0;}
		else
		{s=1;}
			
	counter();	
		
    }
}


void counter()
{
		TCCR1A=0x00;
		TCCR1B=0x0E;
		
		OCR1AL=0x02;
		OCR1AH=0X00;
		
		while((TIFR1&(1<<OCF1A))==0);
		
		TCCR1A=0x00;
		TCCR1B=0x00;
		TIFR1=0x02;
	
}
