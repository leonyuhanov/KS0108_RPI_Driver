# KS0108_RPI_Driver
LCD Screen driver for the 192x64px KS0108(3 chip device) for the Raspberry Pi

1st, these screens are terrible, dont use them! But if you have to please follow along :)

This is the screen I have tested it with
https://www.aliexpress.com/item/1005003484467776.html


# Extra Parts required
This display uses 5VTTL, you will need to voltage level shift from the RPis 3.3VTTL to 5V via a SN74LS245N 8 port driver.
For the parallel data output, you will use SPI to shift the data to a CD4094 shift register.

# Prereq Setup Instructions for RPI4

This lib uses the BCM2835 fast gpio and spi lib for all the hardware interfacing.


```
sudo apt-get update
sudo apt-get upgrade
```

Install BCM C++ driver LIB from http://www.airspayce.com/mikem/bcm2835/index.html
```
wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.71.tar.gz
tar zxvf bcm2835-1.50.tar.gz
cd bcm2835-1.xx
./configure
make
sudo make check
sudo make install
```

# Pin configuration (you can change them in the .H file)

- SPI CS pin = GPIO-7
- SPI MOSI Pin = GPIO-10
- SPI CLK Pin = GPIO-11
- PIN_DI = GPIO-14
- PIN_RW = GPIO-15
- PIN_E = GPIO-18
- PIN_CS1 = GPIO-23
- PIN_CS2 = GPIO-24
- PIN_RES = GPIO-1

# Connection Diagram (TDB)

![Interface Board](https://github.com/leonyuhanov/KS0108_RPI_Driver/blob/main/RPI_to_KS0108_InterfaceBoard.png)
