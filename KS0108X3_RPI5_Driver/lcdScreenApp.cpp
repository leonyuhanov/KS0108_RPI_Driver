//g++ -o lcdScreenApp lcdScreenApp.cpp KS0108X3_RPI_Driver.cpp -lgpiod
#include "KS0108X3_RPI_Driver.h"
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <cmath>
#include <bcm2835.h>
#include <cstdio>

KS0108X3_RPI_Driver lcdScreen;

int main(int argc, char **argv)
{
	unsigned short int yIndex=0, xIndex=0;
	uint8_t WIDTH = 192, HEIGHT = 64;
	
	if(lcdScreen.online()!=0)
	{
		printf("\r\n\t\tError!\r\n");
		return 1;
	}
	
	while(true)
	{
		lcdScreen.drawHLine(0, yIndex, WIDTH, 1);
		lcdScreen.drawHLine(0, HEIGHT-yIndex, WIDTH, 1);
		lcdScreen.drawVLine(xIndex,0, HEIGHT, 1);
		lcdScreen.drawVLine(WIDTH-xIndex,0, HEIGHT, 1);
		lcdScreen.drawCircle(WIDTH/2,HEIGHT/2,yIndex, 1);
		yIndex++;
		xIndex++;
		if(yIndex==HEIGHT)
		{
			yIndex=0;
		}
		if(xIndex==WIDTH)
		{
			xIndex=0;
		}
		lcdScreen.writeToDisplay();
		usleep(50000);
		lcdScreen.clearDisplay();
	}
	return 0;
}