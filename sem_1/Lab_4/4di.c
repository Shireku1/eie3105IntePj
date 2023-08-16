/*
 * Lab 4di.c
 *
 * Created: 7/11/2022 4:24:11
 * Author : user
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
int s=0;
unsigned char temp;
unsigned char str_s[]="we are ready!";
unsigned char str_b[]="bye!";

void transmit_str(unsigned char str[])
{
	int i=0;
	while(str[i]!='\0')
	{
		while (!(UCSR0A&(1<<UDRE0)));
		UDR0=str[i];
		i++;
	}
}


void transmit(unsigned char ch)
{
	while (!(UCSR0A&(1<<UDRE0)));
	UDR0=ch;
}


int main(void)
{
    UCSR0B=((1<<TXEN0)|(1<<RXEN0)|(1<<RXCIE0));			//INT: RXCIE0 UDRIE0
    UCSR0C=((1<<UCSZ00)|(1<<UCSZ01));		
    UBRR0=0XCF;
	sei();
	
	transmit_str(str_s);
	
    while (1);
}

ISR(USART_RX_vect)
{
	temp=UDR0;
	if (s==1)
	{
		if (temp=='i')
		{
			transmit_str(str_b);
		}
		s=0;
	}
	
	
		if (temp=='h')
	{
		s=1;
	}
	
	
}

