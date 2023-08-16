

/* PD7=switch, LED1=PB0, LED2=PB4*/
/* timer0 CTC prescaling1024 count25*j_loop25*i_loop25 */

#include <avr/io.h>
void state1();
void state2();
void t0delay();

int main(void)
{
	DDRB=0xff;
	DDRD=0x00;
	
	while (1) 
    {
		if ((PIND&(1<<7)) != 0)
		{
			state2();
		}
		else
		{
			state1();
		}
    }
}

void state1()
{
	PORTB=0x10;
	t0delay();
	PORTB=0x01;
	t0delay();
}

void state2()
{
	PORTB=0x11;
	t0delay();
	PORTB=0x00;
	t0delay();
}

void t0delay()
{
	for (int i=0; i<=25;i++)
	{
	for (int j=0; j<=25;j++)
	{TCCR0A=0x02;
	TCCR0B=0x05;
	OCR0A=0x19;
	while((TIFR0&(1<<OCF0A))==0);
	TCCR0B=0;
	TCCR0A=0;
	TIFR0=0x02;}
	}	
}
