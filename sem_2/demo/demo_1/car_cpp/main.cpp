#include <stdio.h>

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
int uart2_read(void);          
int uart3_read(void);

char buffer[30];

int car_char = 0;
unsigned int pwm = 0;

int main(void) {
  while (1) {
    wait(40);
    sprintf(buffer, "\rdistance=%d   ", getDistance());
    uart3_print(buffer);
  }
}