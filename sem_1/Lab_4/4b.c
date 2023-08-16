/*
 * Lab 4b.c
 *
 * Created: 7/11/2022 2:59:55
 * Author : user
 */ 

#include <avr/io.h>
void usart_init();
unsigned char receice();
void transmit(unsigned char ch);
void transmit_str(unsigned char str[]);


int main(void)
{
	usart_init();
	unsigned char str_s[]="we are ready!";
	unsigned char str_b[]="bye!";
	
	transmit_str(str_s);
	
    while (1) 
    {
		if (receice()=='h')
		{
			if (receice()=='i')
			{
				transmit_str(str_b);
			}
		}
    }
}

void usart_init()
{
   UCSR0B=((1<<TXEN0)|(1<<RXEN0));			//enable transmitter & receiver of usart0
   UCSR0C=((1<<UCSZ00)|(1<<UCSZ01));		//choose 8bit
   UBRR0=0XCF;								//X=[16000000/(16*4800)]-1
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

void transmit_str(unsigned char str[])				//send data to udr0 to serial port to pc (in str)
{
	int i=0;
	while(str[i]!='\0')
	{
		while (!(UCSR0A&(1<<UDRE0)));
		UDR0=str[i];
		i++;
	}
}

