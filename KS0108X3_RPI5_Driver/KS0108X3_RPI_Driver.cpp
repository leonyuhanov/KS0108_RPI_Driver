#include "KS0108X3_RPI_Driver.h"
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <cmath>
#include <gpiod.h>
#include <cstdio>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>


KS0108X3_RPI_Driver::KS0108X3_RPI_Driver()
{
	// set default pins, can be changed via INIT
	PIN_DI = 14;
	PIN_RW = 15;
	PIN_E = 25;			//18;
	PIN_CS1 = 23;
	PIN_CS2 = 24;
	PIN_RES = 1;
	
	//Init the image buffer
	buffer = (uint8_t *)malloc((WIDTH * HEIGHT) / 8);
}

uint8_t KS0108X3_RPI_Driver::online()
{  
	uint8_t chipCounter=0;
	
	//Bring online the GPIOB driver
	chip = gpiod_chip_open_by_name(chipname);
	
	//	----	init SPIDEV	----
	SPI0 = open(SPI0Device, O_RDWR);
	if(SPI0<0){printf("\r\nSPI DEVICE FAILED TO OPEN\r\n");}
	spiMode = SPI_MODE_0;
	//Set SPI Mode 
	ioctl(SPI0, SPI_IOC_WR_MODE, &spiMode);
	if( ioctl(SPI0, SPI_IOC_RD_MODE, &spiMode)==-1 ){printf("\r\nMODE NOT SET\r\n");}
	//set Bits Per Word
	ioctl(SPI0, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if( ioctl(SPI0, SPI_IOC_RD_BITS_PER_WORD, &bits)==-1 ){printf("\r\nBITS NOT SET\r\n");}
	//Set SPI Speed
	ioctl(SPI0, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if( ioctl(SPI0, SPI_IOC_RD_MAX_SPEED_HZ, &speed)==-1 ){printf("\r\nSPEED NOT SET\r\n");}
	//	----	init SPIDEV	----
	
	//Set up GPIOB Pins for OUTPUT
	GPIO_DI = gpiod_chip_get_line(chip, PIN_DI);
	gpiod_line_request_output(GPIO_DI, "example1", 0);
	GPIO_RW = gpiod_chip_get_line(chip, PIN_RW);
	gpiod_line_request_output(GPIO_RW, "example1", 0);
	GPIO_E = gpiod_chip_get_line(chip, PIN_E);
	gpiod_line_request_output(GPIO_E, "example1", 0);
	GPIO_CS1 = gpiod_chip_get_line(chip, PIN_CS1);
	gpiod_line_request_output(GPIO_CS1, "example1", 0);
	GPIO_CS2 = gpiod_chip_get_line(chip, PIN_CS2);
	gpiod_line_request_output(GPIO_CS2, "example1", 0);
	GPIO_RES = gpiod_chip_get_line(chip, PIN_RES);
	gpiod_line_request_output(GPIO_RES, "example1", 0);

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
		  gpiod_line_set_value(GPIO_DI, 1);
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
	unsigned short int numberOfWritestoAvoidDelay = 5, delayCnt=0;
	
	gpiod_line_set_value(GPIO_E, 1);
	for(delayCnt=0; delayCnt<numberOfWritestoAvoidDelay; delayCnt++)
	{
		gpiod_line_set_value(GPIO_E, 1);
	}
	//usleep(1);
	gpiod_line_set_value(GPIO_E, 0);	
}

void KS0108X3_RPI_Driver::resetScreen(void)
{
	gpiod_line_set_value(GPIO_RES, 0);
	usleep(100000);
	gpiod_line_set_value(GPIO_RES, 1);
	usleep(10000);
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
		case 0:		gpiod_line_set_value(GPIO_CS1, 1);
					gpiod_line_set_value(GPIO_CS2, 1);	
					break;
		case 1:		gpiod_line_set_value(GPIO_CS1, 0);
					gpiod_line_set_value(GPIO_CS2, 0);	
					break;
		case 2:		gpiod_line_set_value(GPIO_CS1, 1);
					gpiod_line_set_value(GPIO_CS2, 0);
					break;
		case 3:		gpiod_line_set_value(GPIO_CS1, 0);
					gpiod_line_set_value(GPIO_CS2, 1);
					break;
	}
}
void KS0108X3_RPI_Driver::writeCommand(uint8_t chipID, uint8_t data)
{
	gpiod_line_set_value(GPIO_DI, 0);
	gpiod_line_set_value(GPIO_RW, 0);
	chipSelect(chipID);
	write_byte_SPI(data);
	enablePulse();
	chipSelect(chipID); 
	gpiod_line_set_value(GPIO_DI, 1);
	gpiod_line_set_value(GPIO_RW, 0);
 }

void KS0108X3_RPI_Driver::write_byte_SPI(uint8_t _byte)
{
	struct spi_ioc_transfer spiDataTransferBlock;
	uint8_t returnBuffer;
	uint8_t dataBlock[2] = {_byte, 0};

	spiDataTransferBlock.tx_buf = (unsigned long)dataBlock;
	spiDataTransferBlock.rx_buf = (unsigned long)returnBuffer;
	spiDataTransferBlock.len = 1;
	spiDataTransferBlock.delay_usecs = 0;
	spiDataTransferBlock.speed_hz = speed;
	spiDataTransferBlock.bits_per_word = bits;
	ioctl(SPI0, SPI_IOC_MESSAGE(1), &spiDataTransferBlock);
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
