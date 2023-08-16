/*
 * lab 2a.c
 *
 * Created: 3/10/2022 3:50:46 pm
 * Author : 20063241d
 */ 

#include <avr/io.h>

void t0delay_normal();
void t0delay_ctc();

int main(void)
{
    DDRB=0xff;
	DDRC=0xff;

    while (1) 
    {
	PORTB=0x01;
	for(int i=0; i<=125; i++)
	{t0delay_ctc();}				// t0delay_normal():normal mode		t0delay_ctc():CTC mode

	PORTB=0x00;
	for(int i=0; i<=125; i++)
	{t0delay_ctc();}				// t0delay_normal():normal mode		t0delay_ctc():CTC mode
	}
}


void t0delay_normal()		//prescale 1024, cnt=125, 8ms (1/125s)
{
TCNT0=0x83;

TCCR0A=0x00;
TCCR0B=0x05;

while((TIFR0&(1<<TOV0))==0);

TCCR0A=0x00;
TCCR0B=0x00;
TIFR0=(1<<TOV0);
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