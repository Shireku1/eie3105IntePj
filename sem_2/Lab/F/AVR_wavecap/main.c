/*
 * AVR_wavecap.c
 *
 * Created: 10/2/2023 15:15:52
 * Author : user
 */ 

#include <stdio.h>
#include <avr/io.h>

void usart_init(void)
{
	UCSR0B=(1<<TXEN0)|(1<<RXEN0);//enable transmitter and receiver
	UCSR0C=(1<<UCSZ01)|(1<<UCSZ00);//8-bit
	UBRR0L=0x67;
}

void transmit_str(char str[])
{
	int i=0;
	while(str[i]!='\0')
	{
		while (!(UCSR0A&(1<<UDRE0)));
		UDR0=str[i];
		i++;
	}
}

int main(void)
{
	char buffer[50]={'\0'};
	usart_init();
	while(1)
	{
		//measure the pulse width of a pulse
		unsigned char t1;
		unsigned char pw;//measured pulse width
		TCCR1A = 0; //Timer Mode = Normal
		TCCR1B = (1 <<ICES1) | (1 << CS12) | (0 << CS11) | (0 << CS10); //rising edge, prescaler = 256, no noise canceller
		TIFR1 = (1<<ICF1); //clear ICF1 (The Input Capture Flag)
		while ((TIFR1&(1<<ICF1)) == 0); //wait while ICF1 is clear
		t1 = ICR1L; //first edge value (Input Capture Register, low byte)
		TIFR1 = (1<<ICF1); //clear ICF1
		TCCR1B = (0 <<ICES1) | (1 << CS12) | (0 << CS11) | (0 << CS10); //falling edge
		while ((TIFR1&(1<<ICF1)) == 0); //wait while ICF1 is clear
		ICR1H=0;
		pw = ICR1L - t1; //pulse width = falling - rising
		TIFR1 = (1<<ICF1); //clear ICF1
		sprintf(buffer, "%d\r\n", pw);
		transmit_str(buffer);
	}

}