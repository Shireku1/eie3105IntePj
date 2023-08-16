/*
 * lab 1a.c
 *
 * Created: 26/9/2022 3:39:28 pm
 * Author :  
 */ 
#include <avr/io.h>

int main(void)
{

	DDRD=0xff;
	while(1)

	{
		PORTD=0x55;
		for (int i=0; i<1000; i++)
		{
			for (int j=0; j<1000; j++);
		}
		
		PORTD=0xaa;
		for (int i=0; i<1000; i++)
		{
			for (int j=0; j<1000; j++);
		}

	}

}

