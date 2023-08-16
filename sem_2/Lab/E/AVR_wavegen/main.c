/*
 * AVR_wavegen.c
 *
 * Created: 3/2/2023 1:48:50 pm
 * Author : 20063241d
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

int d=0;
unsigned char a1;
unsigned char a2;

void usart_init ();
void pwm_init();
void wave_capture();

unsigned char receice()						//copy from udr0 & return data
{
	unsigned char ch=0;
	while(!(UCSR0A&(1<<RXC0)));
	ch=UDR0;
	return ch;
}

void transmit(unsigned char ch)
{
	while (!(UCSR0A&(1<<UDRE0)));
	UDR0=ch;
}


int main(void)
{
    /* Replace with your application code */
	usart_init();
	pwm_init();
	sei();
	DDRD=0b00100000;
	
	/*capture*/	
	TCCR1A = 0; //Timer Mode = Normal
	TCCR1B = (1 <<ICES1) | (1 << CS12) | (0 << CS11) | (0 << CS10);
	//rising edge, prescaler = 256, no noise canceller

	
    while (1) 
    {
		OCR0B = ((a1-'0')*10+(a2-'0')-1);
	}
}

void usart_init (void)
{
	UBRR0=0x67;		//X=[16000000/(16*9600)]-1
	UCSR0B=((1<<TXEN0)|(1<<RXEN0)|(1<<RXCIE0));			//enable
	UCSR0C=((1<<UCSZ00)|(1<<UCSZ01));	//8bit
}

void pwm_init()
{
	// fast pwm, pre 64
	TCCR0A = (1 << COM0A1) | (0 << COM0A0) |
	(1 << COM0B1) | (0 << COM0B0) |
	(1 << WGM01) | (1 << WGM00);
	
	TCCR0B = (1 << WGM02) |
	(1 << CS02) | (0 << CS01) | (0 << CS00);
	
		//	Freq=500Hz OCR0A=16MHz/(500*256-1)
		OCR0A = 124;

		//	duty cycle
		OCR0B = 0;
}

ISR(USART_RX_vect)
{
	if (d==0)
	{
		a1=UDR0;
		transmit(a1);
		d=1;
	}else
	{
		a2=UDR0;
		transmit(a2);
		transmit('\r');
		transmit('\n');
		d=0;
	}
}