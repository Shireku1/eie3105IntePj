/*
 * GccApplication1.c
 *
 * Created: 18/10/2022 23:17:44
 * Author : user
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
void t0delay_ctc();
int s=0;
int cnt=0;

int main(void)
{
	DDRB=0xff;
	DDRC=0xff;
	DDRD=0x80;
	EIMSK=(1<<INT0);
	EICRA=0x03;
	
			OCR0A=0x7d;
			TCCR0A=0x02;
			TCCR0B=0x05;
			TIMSK0=(1<<OCIE0A);
	
	sei();

	while (1)
	{	
	}
}

ISR (INT0_vect)
{
	if (cnt<=3)
	{cnt++;}
	else
	{cnt=0;
	PORTD^=0x80;}
	
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