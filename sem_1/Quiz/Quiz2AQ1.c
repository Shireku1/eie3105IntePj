/*
 * Quiz2AQ1_20063241d.c
 *
 * Created: 24/10/2022 16:59:19
 * Author : user
 */ 


// chan sze man		20063241d

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