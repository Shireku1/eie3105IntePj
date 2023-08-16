/*
 * lab 1b.c
 *
 */ 

#include <avr/io.h>

int main(void)
{
    DDRD=0xff;
	DDRB=0x00;

	int bs =0;					//int represent state

    while (1) 
    {
		if ((PINB&(1<<0)) != 0)			//change state
		{
		
		if (bs==1)
		{bs=0;} 

		else
		{bs=1;}

		}

		if (bs==1)						//output according state
		{
			PORTD=0xff;
		}

		if (bs==0)
		{
			PORTD=0x00;
		}


		for (int i=0; i<1000; i++)		//cool down
		{for (int j=0; j<1000; j++);}
    }
}

