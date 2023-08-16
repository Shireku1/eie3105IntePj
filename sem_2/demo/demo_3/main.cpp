#include <stdio.h>
#include <stdbool.h>
char SPI_data = 0;

void wait(unsigned int);
unsigned int getADC(void);
char getPath(void);
unsigned int getLeft(void);
unsigned int getRight(void);
void pwmLeft(unsigned int);
void pwmRight(unsigned int);
void setLeft(bool);
void setRight(bool);
int getDistance(void);

bool uart2_print(const char*);
bool uart3_print(const char*);
int uart2_read(void);          //return -1 for none
int uart3_read(void);

char buffer[30];
bool pwm_s=1;

void car_pwm(unsigned int leftpwm,unsigned int rightpwm){
	setLeft(true);
	setRight(true);
	pwmLeft(leftpwm); 
	pwmRight(rightpwm);

}


int main(void) {
  while (1) {
    wait(40);
		char c = getPath();
    sprintf(buffer, "\rdistance= %d   ", getDistance());
    uart2_print(buffer);
		
			if (getDistance() < 50){car_pwm(0,0);}
			
				else if(getDistance() < 200){
				
				if(pwm_s){car_pwm(10000,10000);}
				else{car_pwm(8500,8500);}

			}
			
			else if(getDistance() < 400){
				car_pwm(9600,9600);
			}
			
			else if(getDistance() < 1600){
				
				if(pwm_s){car_pwm(14000,14000);}
				else{car_pwm(9500,9500);}

			}
			else if(getDistance() < 1660){
				car_pwm(0,0);
			}
			else{
				
				if(pwm_s){car_pwm(20000,20000);}
				else{car_pwm(13000,13000);}

			}

			pwm_s=!pwm_s;
			
  }
}