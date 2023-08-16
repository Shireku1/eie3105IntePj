

#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char F='F';
unsigned char E='E';

int s=0;
int b=0;
int cnt=0;

void usart_init()
{
	UBRR0=0x67;
	UCSR0B=((1<<TXEN0)|(1<<RXEN0)|(1<<RXCIE0));
	UCSR0C=((1<<UCSZ00)|(1<<UCSZ01));
}

void transmit(unsigned char ch)
{
	while (!(UCSR0A&(1<<UDRE0)));
	UDR0=ch;
}


int main(void)
{
	usart_init();
	OCR0A=0x7d;
	TCCR0A=0x02;
	TCCR0B=0x05;
	TIMSK0=(1<<OCIE0A);		
	sei();
	
    while (1);
}

ISR(TIMER0_COMPA_vect)
{
	if (s==0)
	{
		if (b<=125)
		{
			b++;
		}
		if (b>=125)
		{
			b=0;
			transmit(E);
		}
	}
	
	if(s==1)
	{
		if(b<=62)
		{
			b++;
		}
		if (b>=62)
		{
			b=0;
			transmit(F);
		}
	}
}

ISR(USART_RX_vect)
{
	unsigned char temp=0;
	temp=UDR0;

	
	
	if (temp=='X')
	{
		if (cnt==0)
		{
			cnt=1;
			return;
		}
		
	}
		if (cnt==1)
	{
		if (temp=='X')
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
		cnt=0;
	}
	
	
}

