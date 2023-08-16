/*
 * Lab2b.c
 *
 * Created: 3/10/2022 5:05:12 pm
 * Author :  
 */ 

#include <avr/io.h>

void t0delay_normal();
void t0delay_ctc();

void s1();
void s2();
void s3();
void s4();
void s5();
void s6();
void s7();
void s8();

int main(void)
{
	DDRB=0xff;
	DDRC=0xff;

	while (1)
	{
	s1();
	s2();
	s3();
	s4();
	s5();
	s6();
	s7();
	s8();
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

void s1()
{
PORTB=0x21;
PORTC=0x10;
for(int i=0; i<=625; i++)
{t0delay_normal();}
}

void s2()
{
PORTB=0x11;
PORTC=0x10;
for(int i=0; i<=125; i++)
{t0delay_normal();}
}

void s3()
{
PORTB=0x09;
PORTC=0x10;
for(int i=0; i<=125; i++)
{t0delay_normal();}
}

void s4()
{
PORTB=0x0b;
PORTC=0x10;
for(int i=0; i<=125; i++)
{t0delay_normal();}
}

void s5()
{
PORTB=0x0c;
PORTC=0x20;
for(int i=0; i<=625; i++)
{t0delay_normal();}
}

void s6()
{
PORTB=0x0a;

for (int i=0;i<=5;i++)
{

PORTC=0x20;
for(int j=0; j<=15; j++)
{t0delay_normal();}

PORTC=0x00;
for(int j=0; j<=10; j++)
{t0delay_normal();}
}
}

void s7()
{
PORTB=0x09;
PORTC=0x10;
for(int i=0; i<=125; i++)
{t0delay_normal();}
}

void s8()
{
PORTB=0x19;
PORTC=0x10;
for(int i=0; i<=125; i++)
{t0delay_normal();}
}

