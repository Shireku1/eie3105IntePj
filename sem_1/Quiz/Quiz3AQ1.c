/*
 * Quiz3AQ1_20063241d.c
 *
 * Created: 7/11/2022 17:01:58
 * Author : user
 */ 

//Chan Sze Man 20063241d


#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char temp='E';
int s=0;
int b=0;

void usart_init()
{
	UBRR0=0x67;
	UCSR0B=((1<<TXEN0)|(1<<RXEN0));			//|(1<<RXCIE0)|(1<<UDRIE0)
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
	if (s<=125)
	{
		s++;
	}
	if(s==125)
	{
		transmit(temp);
		s=0;
	}
}
