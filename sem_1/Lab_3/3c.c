/*
 * GccApplication1.c
 *
 * Created: 18/10/2022 23:17:44
 * Author : user
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
void t0delay_ctc();
void trafficled();
int s=0;

int main(void)
{
	DDRB=0xff;
	DDRC=0xff;
	EIMSK=(1<<INT1);
	EICRA=0b00001000;		//08 in int1
	sei();

	while (1)
	{	

		if (s==1)
		{trafficled();}
		else
		{
			PORTB=0x00;
			PORTC=0x00;
		}

	}
}

ISR (INT1_vect)
{	
	if (s==0)
	{s=1;} 
	else{s=0;
	PORTB=0x00;
	PORTC=0x00;}	

}

void t0delay_ctc()		////prescale 1024, cnt=125, 8ms (1/125s)
{
	OCR0A=0x7d;

	TCCR0A=0x02;
	TCCR0B=0x05;

	while((TIFR0&(1<<OCF0A))==0);

	TCCR0A=0x00;
	TCCR0B=0x00;
	TIFR0=0x02;
}

void trafficled()
{
if (s==0){return;}
	PORTB=0x21;
	PORTC=0x10;
	for(int i=0; i<=625; i++)
	{t0delay_ctc();}
if (s==0){return;}
	PORTB=0x11;
	PORTC=0x10;
	for(int i=0; i<=125; i++)
	{t0delay_ctc();}
if (s==0){return;}
	PORTB=0x09;
	PORTC=0x10;
	for(int i=0; i<=125; i++)
	{t0delay_ctc();}
if (s==0){return;}
	PORTB=0x0b;
	PORTC=0x10;
	for(int i=0; i<=125; i++)
	{t0delay_ctc();}
if (s==0){return;}
	PORTB=0x0c;
	PORTC=0x20;
	for(int i=0; i<=625; i++)
	{t0delay_ctc();}
if (s==0){return;}
	PORTB=0x0a;
if (s==0){return;}
	for (int i=0;i<=5;i++)
	{

		PORTC=0x20;
		for(int j=0; j<=15; j++)
		{t0delay_ctc();}

		PORTC=0x00;
		for(int j=0; j<=10; j++)
		{t0delay_ctc();}
	}
if (s==0){return;}
	PORTB=0x09;
	PORTC=0x10;
	for(int i=0; i<=125; i++)
	{t0delay_ctc();}
if (s==0){return;}
	PORTB=0x19;
	PORTC=0x10;
	for(int i=0; i<=125; i++)
	{t0delay_ctc();}
}