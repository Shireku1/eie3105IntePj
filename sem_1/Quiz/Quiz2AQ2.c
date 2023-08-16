

#include <avr/io.h>
#include <avr/interrupt.h>
int s=0;
int c=0;
int i=0;

int main(void)
{
	/* Replace with your application code */
	
	DDRD=0b11000000;
	
OCR0A=0x7d;
TCCR0A=0x02;
TCCR0B=0x05;

	TIMSK0=(1<<OCIE0A);
	
	
	EIMSK=(1<<INT0);
	EICRA=0x02;				//TEST 03
	
	
	sei();
	while (1)
	{
	}
}

ISR(INT0_vect)
{
	if (s==0)
	{
		s=1;
	}
	else
	{
		s=0;
	}
}

ISR(TIMER0_COMPA_vect)
{
	if (i<=125)
	{i++;}
	else
	{i=0;
		if(c==0){c=1;}
		else{c=0;}
	}
	
	if (s==0)
	{
		if(c==0){PORTD=0b11000000;}
		if(c==1){PORTD=0x00;}
	}
	
	if(s==1)
	{
		if(c==0){PORTD=0b10000000;}
		if(c==1){PORTD=0b01000000;}
		
	}
}
