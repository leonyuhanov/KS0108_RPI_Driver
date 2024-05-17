# KS0108_RPI_Driver
LCD Screen driver for the 192x64px KS0108(3 chip device) for the Raspberry Pi

1st, these screens are terrible, dont use them! But if you have to please follow along :)

This is the screen I have tested it with
https://www.aliexpress.com/item/1005003484467776.html


# Prereq Setup Instructions for RPI4

This lib uses the BCM2835 fast gpio and spi lib for all the hardware interfacing.


```
sudo apt-get update
sudo apt-get upgrade
```

//Install BCM C++ driver LIB from http://www.airspayce.com/mikem/bcm2835/index.html
```
wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.71.tar.gz
tar zxvf bcm2835-1.50.tar.gz
cd bcm2835-1.xx
./configure
make
sudo make check
sudo make install
