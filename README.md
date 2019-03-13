# SSD1289 Driver for STM32F1xx  
  
This is a driver for `SSD1289` displays for use with `STM32F103` devices.  
It also brings a ADS7843 touchscreen driver.  
  
It is very simple to use, see `src/main.c`.  
  
# Screenshots  
  
# Provided graphics and text functions  
See `src/include/ssd1289.h` for all available functions.  
See `src/ssd1289_demo.c` for examples on all those functions.  
  
If you want to use printf-family for outputting on display use `sprintf()/snprintf()`
and `ssd1289_puts()`. Regular `printf()` will output to USART1.
  
# 4 different fonts taken from linux kernel + IBM's XGA BIOS font  
I took 4 different fonts from Linux Kernel. They are really nice and  
good to read on this display.  
In another project I dissected hundreds of video BIOSes for their fonts.  
I have included IBM's XGA fonts in sizes 8x8, 8x14 and 8x16.  
  
# Touchscreen calibration
See `src/ssd1289/ads7843_calibration.c` for an example calibration routine.  
This takes up some space. If you do not intend to change you display,  
you may just run this calibration routine once and use it's output for  
compiled in calibration data.
  
# Speed measurements + Code size
These measurements were done on an overclocked (128MHz) STM32F103:  
```
- ssd1289_print_image(): printing an image from flash       = 98.3mS
- ssd1289_fill_screen(): fill screen with single color      = 11.2mS
```
The library uses 3.5k-30k of flash, depending on usage of it's functions.  
  
# Hardware connections:  
The MCU communicates via FSMC with your display, mode is 16bit 8080-bus.  
Display color format is `RGB565`. This means a 16bit color value is  
encoded like this:  
```
<Bit00-Bit04> RED  
<Bit05-Bit10> GREEN  
<Bit11-Bit15> BLUE  
```
The display will be mapped into memory at a base of `0x60000000`.  
The `RS` (register select) line used by my hardware-setup is `FSMC_A19`,  
this means we get an offset of `0x00020000`.  
All the bus-stuff is done by the chip's FSMC, so reading/writing  
to `0x60000000` means access to the registers, r/w to `0x60020000`  
means access to the RAM. We don't have to deal with `A19/RS` by ourself.  
  
```
LCD Pin	    STM32F10x AF    STM32F103VCT6 Pin  
-------------------------------------------------------  
LCD_BLight	TIM3_CH3		PB0  
  
LCD_RD		FSMC_NOE		PD4  
LCD_WR		FSMC_NWE		PD5  
LCD_CS		FSMC_NE1		PD7  
LCD_RS		FSMC_A19		PE3  
  
LCD_DB0		FSMC_D0			PD14  
LCD_DB1		FSMC_D1			PD15  
LCD_DB2		FSMC_D2			PD0  
LCD_DB3		FSMC_D3			PD1  
LCD_DB4		FSMC_D4			PE7  
LCD_DB5		FSMC_D5			PE8  
LCD_DB6		FSMC_D6			PE9  
LCD_DB7		FSMC_D7			PE10  
LCD_DB10	FSMC_D8			PE11  
LCD_DB11	FSMC_D9			PE12  
LCD_DB12	FSMC_D10		PE13  
LCD_DB13	FSMC_D11		PE14  
LCD_DB14	FSMC_D12		PE15  
LCD_DB15	FSMC_D13		PD8  
LCD_DB16 	FSMC_D14		PD9  
LCD_DB17	FSMC_D15		PD10  
```
  
# Authors  
(c) 2015, 2017-2019 Nils Stec <stecdose@gmail.com>  
(c) 2010 poweravr from powermcu.com has written:   
bresenham line drawing function, parts of initialization routine  
  
# Changelog  
- 2015 august	created first version of this driver  
- 2015 august	added pwm backlight control  
- 2015 august	added graphic functions  
- 2015 august	added text functions + linux 8x16-font  
- 2017 jan	first comments appeared...  
- 2017 feb	worked on text functions  
- 2017 march	replaced scrappy text functions  
- 2017 march	1st planned release  
- 2018 march  finally getting a release done...  
- 2018 may
- 2018 june dotmatrix+7seg functions  
- 2018 nov      ADS7843 working now, calibration GUI
- 2019 mar      cleanup
  
