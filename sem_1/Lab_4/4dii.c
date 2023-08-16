/*
 * Lab 4dii.c
 *
 * Created: 7/11/2022 4:24:53
 * Author : user
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

int s=0;
unsigned char temp;

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
    UCSR0B=((1<<TXEN0)|(1<<RXEN0)|(1<<RXCIE0)|(1<<UDRIE0));			//INT: RXCIE0 UDRIE0
    UCSR0C=((1<<UCSZ00)|(1<<UCSZ01));
    UBRR0=0X67;
    sei();

    while (1);
}

ISR(USART_RX_vect)
{
	if (s==0)
	{	
		temp=UDR0;
		for(int i=0;i<10;i++)
		{transmit(temp);}
			s=1;
	}
	
	if(s==1)
	{
		if (UDR0==temp)
		{
			s=0;
		}

	}
		
}

ISR(USART_UDRE_vect)
{
	if (s==0)
	{
		transmit('a');
	}

}

