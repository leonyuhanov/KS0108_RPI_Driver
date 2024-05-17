#include "KS0108X3_RPI_Driver.h"
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <cmath>
#include <bcm2835.h>
#include <cstdio>

KS0108X3_RPI_Driver::KS0108X3_RPI_Driver()
{
	// set default pins, can be changed via INIT
	PIN_DI = 14;
	PIN_RW = 15;
	PIN_E = 18;
	PIN_CS1 = 23;
	PIN_CS2 = 24;
	PIN_RES = 1;
	//Init the image buffer
	buffer = (uint8_t *)malloc((WIDTH * HEIGHT) / 8);
}

uint8_t KS0108X3_RPI_Driver::online()
{  
	uint8_t chipCounter=0;
	
	//Bring online the BCM driver
	if (!bcm2835_init())
	{
		  printf("\r\n\t bcm2835 did not init!");
		  return 1;
	}
	//	----	init SPI	----
	if (!bcm2835_spi_begin())
    {
      printf("bcm2835_spi_begin failedg. Are you running as root??\n");
      return 2;
    }
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);     
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                 
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64);
	bcm2835_spi_chipSelect(7);                     
    bcm2835_spi_setChipSelectPolarity(7, LOW);
	
	bcm2835_gpio_fsel(PIN_DI, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(PIN_RW, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(PIN_E, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(PIN_CS1, BCM2835_GPIO_FSEL_OUTP);	
	bcm2835_gpio_fsel(PIN_CS2, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(PIN_RES, BCM2835_GPIO_FSEL_OUTP);
	chipSelect(0);
	resetScreen();	
	//Init each KS0108 chip
	for(chipCounter=1; chipCounter<numberOfChips+1; chipCounter++)
	{
		writeCommand(chipCounter, KS0108_CMD_SET_DISP_START_LINE | 0);
		writeCommand(chipCounter, KS0108_CMD_SET_PAGE_ADDRESS | 0);
		writeCommand(chipCounter, KS0108_CMD_SET_COL_ADDRESS | 0);
		writeCommand(chipCounter, KS0108_CMD_DISPLAY_ON);
	}
	return 0;
}

void KS0108X3_RPI_Driver::writeToDisplay(void)
{
	uint8_t ch = 0, col=0;
	uint16_t p = 0;
  
	writeCommand(0, KS0108_CMD_SET_COL_ADDRESS | 0);
	for(ch=1; ch<numberOfChips+1; ch++) 
	{
		for(p=0; p<8; p++) 
		{
		  writeCommand(ch, KS0108_CMD_SET_PAGE_ADDRESS | p );  
		  bcm2835_gpio_write(PIN_DI, 1);
		  chipSelect(ch);
		  for(col=0; col<64; col++) 
		  {
			write_byte_SPI( buffer[ (p * WIDTH) + (ch-1)*64 + col ] );
			enablePulse();			
		  }
		}
	}
}

void KS0108X3_RPI_Driver::enablePulse(void)
{
	bcm2835_gpio_write(PIN_E, 1);	
	bcm2835_delayMicroseconds(10);	
	bcm2835_gpio_write(PIN_E, 0);
}

void KS0108X3_RPI_Driver::resetScreen(void)
{
	bcm2835_gpio_write(PIN_RES, 0);
	bcm2835_delay(100);
	bcm2835_gpio_write(PIN_RES, 1);
	bcm2835_delay(10);
}

void KS0108X3_RPI_Driver::clearDisplay(void)
{
	memset(buffer, 0, WIDTH*HEIGHT/8);
}

void KS0108X3_RPI_Driver::fillDisplay(void)
{
  memset(buffer, 0xFF, WIDTH*HEIGHT/8);
}

void KS0108X3_RPI_Driver::chipSelect(uint8_t chipID)
{
	switch(chipID)
	{
		case 0:		bcm2835_gpio_write(PIN_CS1, 1);
					bcm2835_gpio_write(PIN_CS2, 1);	
					break;
		case 1:		bcm2835_gpio_write(PIN_CS1, 0);
					bcm2835_gpio_write(PIN_CS2, 0);	
					break;
		case 2:		bcm2835_gpio_write(PIN_CS1, 1);
					bcm2835_gpio_write(PIN_CS2, 0);	
					break;
		case 3:		bcm2835_gpio_write(PIN_CS1, 0);
					bcm2835_gpio_write(PIN_CS2, 1);	
					break;
	}
}
void KS0108X3_RPI_Driver::writeCommand(uint8_t chipID, uint8_t data)
{
	
  bcm2835_gpio_write(PIN_DI, 0);
  bcm2835_gpio_write(PIN_RW, 0);
  chipSelect(chipID);
  write_byte_SPI(data);
  enablePulse();
  chipSelect(chipID); 
  bcm2835_gpio_write(PIN_DI, 1);
  bcm2835_gpio_write(PIN_RW, 0);
 }

void KS0108X3_RPI_Driver::write_byte_SPI(uint8_t _byte)
{
  char dataToSend[2] = {_byte, 0};
  bcm2835_spi_writenb(dataToSend, 1);
}

////---------------	Timers	---------------
void KS0108X3_RPI_Driver::startTimer(unsigned long duration)
{
	gettimeofday(&tp, NULL);
	timeData[0] = tp.tv_sec * 1000 + tp.tv_usec / 1000;
	timeData[2] = duration;
}
uint8_t KS0108X3_RPI_Driver::hasTimedOut()
{
	gettimeofday(&tp, NULL);
	timeData[1] = tp.tv_sec * 1000 + tp.tv_usec / 1000;
	if(timeData[1]-timeData[0]>timeData[2])
	{
		return 1;
	}
	return 0;
}
//---------------	Helper Draw Methods	---------------
void KS0108X3_RPI_Driver::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  if ((x >= 0) && (x < WIDTH) && (y >= 0) && (y < HEIGHT)) 
  {
   if(color>0)
   {
	   buffer[x + (y / 8) * WIDTH] |= (1 << (y & 7));
   }
   else
   {
	   buffer[x + (y / 8) * WIDTH] &= ~(1 << (y & 7));
   }
  }
}
void KS0108X3_RPI_Driver::drawHLine(int16_t x, int16_t y, int16_t length, uint16_t color)
{
	unsigned short int pCounter=0;
	for(pCounter=0; pCounter<length; pCounter++)
	{
		drawPixel(x+pCounter, y, color);
	}
}
void KS0108X3_RPI_Driver::drawVLine(int16_t x, int16_t y, int16_t length, uint16_t color)
{
	unsigned short int pCounter=0;
	for(pCounter=0; pCounter<length; pCounter++)
	{
		drawPixel(x, y+pCounter, color);
	}
}

void KS0108X3_RPI_Driver::getCircularPoints(unsigned short int circleX,unsigned short int circleY,unsigned short int circleR,unsigned short int angleFromTopLeftoRight, short int *points)
{
    points[0] = round( circleX + (sin(angleFromTopLeftoRight)*circleR) );
    points[1] = round( circleY - (cos(angleFromTopLeftoRight)*circleR) );
}

void KS0108X3_RPI_Driver::drawCircle(unsigned short int x0,unsigned short int y0,unsigned short int radius, int16_t pixelColour)
{
  short int degCounter = 0;
  short int tempCPoints[2];
  
  for(degCounter=0; degCounter<360; degCounter++)
  {
    getCircularPoints(x0, y0, radius, degCounter, tempCPoints);
    drawPixel(tempCPoints[0], tempCPoints[1], pixelColour);
  }
}
//---------------	Helper Draw Methods	---------------
