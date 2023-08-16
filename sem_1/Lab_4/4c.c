/*
 * Lab  4c.c
 *
 * Created: 7/11/2022 3:36:39
 * Author : user
 */ 

#include <avr/io.h>
void usart_init();
unsigned char receice();
void transmit(unsigned char ch);


int main(void)
{
	unsigned char temp=0;
	usart_init();
	while (1)
	{
		
//initially empty in buffer, rxc reset; data transmitted set rxc once and leave while loop
		while(!(UCSR0A&(1<<RXC0)))			
		{
			transmit('a');
		}

//read data		
		temp=receice();
		
//print 10 times		
		for (int i=0;i<10;i++)
		{
			transmit(temp);
		}

//as buffer is empty again while variable in main not yet clear	
		while(receice()!=temp);	
	}
}

void usart_init()
{
	UCSR0B=((1<<TXEN0)|(1<<RXEN0));			//enable transmitter & receiver of usart0
	UCSR0C=((1<<UCSZ00)|(1<<UCSZ01));		//choose 8bit
	UBRR0=0X67;								//X=[16000000/(16*9600)]-1
}

unsigned char receice()						//copy from udr0 & return data to main
{
	unsigned char ch=0;
	while(!(UCSR0A&(1<<RXC0)));
	ch=UDR0;
	return ch;
}

void transmit(unsigned char ch)				//send data to udr0 to serial port to pc
{
	while (!(UCSR0A&(1<<UDRE0)));
	UDR0=ch;
}