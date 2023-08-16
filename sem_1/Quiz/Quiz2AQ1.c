

#include <avr/io.h>
#include <avr/interrupt.h>


int main(void)
{
    /* Replace with your application code */
	
	DDRD=0B11000000;
	EIMSK=(1<<INT0);
	EICRA=0x02;				//TEST 03
	sei();
	PORTD=0x80;
    while (1) 
    {
    }
}

ISR(INT0_vect)
{
	PORTD^=0x80;
}
