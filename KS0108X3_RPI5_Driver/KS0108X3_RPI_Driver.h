#ifndef KS0108X3_RPI_Driver_h
#define KS0108X3_RPI_Driver_h

#include <stdint.h>
#include <sys/time.h>

#define KS0108_CMD_SET_DISP_START_LINE  0xC0   // start line address, 0x40
#define KS0108_CMD_SET_PAGE_ADDRESS     0xB8   // page address
#define KS0108_CMD_SET_COL_ADDRESS      0x40   // column address
#define KS0108_CMD_DISPLAY_OFF   0x3E  			// lcd off
#define KS0108_CMD_DISPLAY_ON    0x3F  			// lcd on

class KS0108X3_RPI_Driver
{
  public:
	KS0108X3_RPI_Driver();
	uint8_t online();
	
	void clearDisplay(void);
	void fillDisplay(void);
	void chipSelect(uint8_t chipID);
	void writeCommand(uint8_t chipID, uint8_t data);
	void write_byte_SPI(uint8_t _byte);
	void startTimer(unsigned long duration);
	uint8_t hasTimedOut();
	void writeToDisplay(void);
	void enablePulse(void);
	void resetScreen(void);
	void drawPixel(int16_t x, int16_t y, uint16_t color);
	void drawHLine(int16_t x, int16_t y, int16_t length, uint16_t color);
	void drawVLine(int16_t x, int16_t y, int16_t length, uint16_t color);
	void getCircularPoints(unsigned short int circleX, unsigned short int circleY, unsigned short int circleR, unsigned short int angleFromTopLeftoRight, short int *points);
	void drawCircle(unsigned short int x0,unsigned short int y0,unsigned short int radius, int16_t pixelColour);
	
  private:
	const char *chipname = "gpiochip4";
	struct gpiod_chip *chip;
	struct gpiod_line *GPIO_DI;
	struct gpiod_line *GPIO_RW;
	struct gpiod_line *GPIO_E;
	struct gpiod_line *GPIO_CS1;
	struct gpiod_line *GPIO_CS2;
	struct gpiod_line *GPIO_RES;
	
	int SPI0;
	char *SPI0Device = "/dev/spidev0.0";
	uint8_t spiMode;
	uint8_t bits = 8;
	uint32_t speed = 3000000;
	
	uint8_t PIN_DI;
	uint8_t PIN_RW;
	uint8_t PIN_E;
	uint8_t PIN_CS1;
	uint8_t PIN_CS2;
	uint8_t PIN_RES;

	const uint8_t numberOfChips = 3;
	const uint8_t WIDTH=192;
	const uint8_t HEIGHT=64;
	unsigned long timeData[3];
	struct timeval tp;
	uint8_t *buffer;

};

#endif