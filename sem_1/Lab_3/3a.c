/*
 * Lab 3a.c
 *
 * Created: 10/10/2022 15:49:46
 * Author : user
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
int s=0;

int main(void)
{
	DDRB=0xff;
	DDRC=0xff;
	DDRD=0x80;
	
	TCCR1A=0x00;
	TCCR1B=0b00001111;	
	OCR1AL=0x02;
	OCR1AH=0x00;
	
		OCR0A=0x7d;
		TCCR0A=0x02;
		TCCR0B=0x05;
	
	TIMSK1=(1<<OCIE1A);
	TIMSK0=(1<<OCIE0A);	
	
	sei();

	while (1)
	{	

	}
}

ISR (TIMER1_COMPA_vect)
{
	PORTD ^=0x80;
}

ISR(TIMER0_COMPA_vect)
{
	if (s<=2000){s++;}
	else{s=0;}
		
	if (s<=625)
	{
		PORTB=0x21;
		PORTC=0x10;
	}
	
	if (s>625)
	{
		if (s<=750)
		{
			PORTB=0x11;
			PORTC=0x10;
		}
	}
	
	if (s>750)
	{
		if (s<=875)
		{
			PORTB=0x09;
			PORTC=0x10;

		}
	}
	
	if (s>875)
	{
		if (s<=1000)
		{
			PORTB=0x0b;
			PORTC=0x10;

		}
	}
	
	if (s>1000)
	{
		if (s<=1625)
		{
			PORTB=0x0c;
			PORTC=0x20;

		}
	}
	
	if (s>1625)
	{
		if (s<=1750)
		{
			PORTB=0x0a;
			PORTC^=0x20;

		}
	}
	
	if (s>1750)
	{
		if (s<=1875)
		{
			PORTB=0x09;
			PORTC=0x10;

		}
	}
	
	if (s>1875)
	{
		if (s<=2000)
		{
			PORTB=0x09;
			PORTC=0x10;

		}
	}
	
	
	
}





