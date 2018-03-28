/* ssd1289 driver for stm32f10x (v0.2)
 * 
 * authors:
 *  (c) 2015, 2017 Nils Stec <stecdose@gmail.com>
 *  (c) 2010 poweravr from powermcu.com has written: 
 *	bresenham line drawing function,
 *	parts of initialization routine
 * 
 * changelog:
 *	2015 august	created first version of this driver
 *	2015 august	added pwm backlight control
 *	2015 august	added graphic functions
 *	2015 august	added text functions + linux 8x16-font
 *	2017 jan	first comments appeared...
 *	2017 feb	worked on text functions
 *	2017 march	replaced scrappy text functions
 *	2017 march	1st planned release
 * 
 * description:
 *	the mcu communicates via FSMC with your display.
 *	mode is 16bit 8080 data bus
 *	display color format is RGB565
 *
 *	The display will be mapped into memory at a base of 0x60000000.
 * 	The RS (register select) line used by my hardware-setup is FSMC_A19,
 *	this means we get an offset of 0x00020000.
 * 	All the bus-stuff is done by the chip's FSMC, so reading/writing
 * 	to 0x60000000 means access to the registers, r/w to 0x60020000
 * 	means access to the RAM. We don't have to deal with A19/RS by ourself.
 *
 * code-/data-size:
 *
 * hardware connections:
 *	controller: STM32F103VCT6
 *	lcd: HY32D
 *	(you'll find a circuit diagram in the source tree)
 *	LCD Pin		STM32F10x AF 	STM32F103VCT6 Pin
 * -------------------------------------------------------
 *	LCD_BLight	TIM3_CH3	PB0
 * 
 *	LCD_RD		FSMC_NOE	PD4
 *	LCD_WR		FSMC_NWE	PD5
 *	LCD_CS		FSMC_NE1	PD7
 *	LCD_RS		FSMC_A19	PE3
 * 
 *	LCD_DB0		FSMC_D0		PD14
 *	LCD_DB1		FSMC_D1		PD15
 *	LCD_DB2		FSMC_D2		PD0
 *	LCD_DB3		FSMC_D3		PD1
 *	LCD_DB4		FSMC_D4		PE7
 *	LCD_DB5		FSMC_D5		PE8
 *	LCD_DB6		FSMC_D6		PE9
 *	LCD_DB7		FSMC_D7		PE10
 *	LCD_DB10	FSMC_D8		PE11
 *	LCD_DB11	FSMC_D9		PE12
 *	LCD_DB12	FSMC_D10	PE13
 *	LCD_DB13	FSMC_D11	PE14
 *	LCD_DB14	FSMC_D12	PE15
 *	LCD_DB15	FSMC_D13	PD8
 *	LCD_DB16 	FSMC_D14	PD9
 *	LCD_DB17	FSMC_D15	PD10
 *
 *
 * TODO TODO TODO 
 *	- comment cryptic init routine, makes easier to adapt to other displays
 * 	- circle, pie, ...
 * 	- helper function to support animations
 * 	- add support for image formats: PNG, JPG, GIF for animations
 * 	- add ssd1289 driver to freertos skeleton project
 * 	- comments
 * 	- documentation
 * 	- examples + photos + videos
 * 	- font converter
 * 	- image converter
 * 	- ads7843 touchscreen code is not working
 */

#include "ssd1289.h"

/* private functions, only needed inside this driver */
static void ssd1289_init_gpio();
static void ssd1289_init_fsmc();
static void ssd1289_controller_init();
static void ssd1289_write_reg(uint8_t reg_addr, uint16_t reg_value);
static uint16_t ssd1289_read_reg(uint8_t reg_addr);

int ssd1289_init() { 
	uint16_t lcd_id;

	ssd1289_init_gpio();        
	ssd1289_init_fsmc();

	/* debugging helper... read register 0x00, display will return an ID */
 	lcd_id = ssd1289_read_reg(0x00);	

 	if((lcd_id == SSD1289_ID0) || (lcd_id == SSD1289_ID1)) {
		ssd1289_controller_init();
		ssd1289_bl_init();
		ssd1289_bl_set(100);
		return SSD1289_FOUND;
 	}

 	return SSD1289_NOT_FOUND;
}
/* set x/y registers, a "pixel-cursor" */
void ssd1289_set_cursor(uint16_t x, uint16_t y) {
	ssd1289_write_reg(0x4e, x);
	ssd1289_write_reg(0x4f, y);

	LCD_REG = 0x22;		/* set register for RAM, next access to LCD_RAM is pixel data */
	
	return;
}

static void ssd1289_init_gpio() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | 
								  GPIO_Pin_1 | 
								  GPIO_Pin_4 | 
								  GPIO_Pin_5 |
								  GPIO_Pin_7 | 
								  GPIO_Pin_8 | 
								  GPIO_Pin_9 | 
								  GPIO_Pin_10 |
                                  GPIO_Pin_11 | 
                                  GPIO_Pin_14 | 
                                  GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);   
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | 
								  GPIO_Pin_7 | 
								  GPIO_Pin_8 | 
								  GPIO_Pin_9 |
                                  GPIO_Pin_10 | 
                                  GPIO_Pin_11 | 
                                  GPIO_Pin_12 | 
                                  GPIO_Pin_13 |
                                  GPIO_Pin_14 | 
                                  GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
  
	return;
}

static void ssd1289_init_fsmc() {
	FSMC_NORSRAMInitTypeDef        FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  FSMC_NORSRAMTimingInitStructure;
  
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);  
  
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = LCD_FSMC_ADDR_SETUP_TIME;
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 1;
	FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = LCD_FSMC_DATA_SETUP_TIME;
	FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0;
	FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 0;
	FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0;
	FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;
	
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;   
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;  
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;  
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;  
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;  
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;  
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTimingInitStructure; 
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;
	
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
	
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
	
	return;
}

static void ssd1289_controller_init() {
	int i;
	const static uint16_t init_data[45][2] = {
		                    // REGISTER           | value description
		{ 0x0007, 0x0021 },	// display control    |
		{ 0x0000, 0x0001 },	// oscillation start  |
		{ 0x0007, 0x0023 },	// display control
		{ 0x0010, 0x0000 }, // sleep mode
		{ 0x0007, 0x0033 }, // display control
		{ 0x0011, 0x6838 },	// entry mode
		{ 0x0002, 0x0600 },	// lcd drive ac ctrl
		{ 0x0012, 0x6CEB },	// opt access speed 3
		{ 0x0003, 0xA8A4 },	// power control 1
		{ 0x000C, 0x0000 },	// power control 2
		{ 0x000D, 0x080C },	// power control 3
		{ 0x000E, 0x2B00 }, // power control 4
		{ 0x001E, 0x00B0 }, // power control 5
		{ 0x0001, 0x2B3F }, // driver output ctrl
		{ 0x0005, 0x0000 }, // compare register 1
		{ 0x0006, 0x0000 }, // compare register 2
		{ 0x0016, 0xEF1C }, // horizontal porch
		{ 0x0017, 0x0103 },	// vertical porch
		{ 0x000B, 0x0000 }, // frame cycle control
		{ 0x000F, 0x0000 },	// gate scan start position
		{ 0x0041, 0x0000 },	// vertical scroll control 1
		{ 0x0042, 0x0000 },	// vertical scroll control 2
		{ 0x0048, 0x0000 }, // first window start
		{ 0x0049, 0x013F }, // first window end
		{ 0x004A, 0x0000 }, // second window start
		{ 0x004B, 0x0000 }, // second window end
		{ 0x0044, 0xEF00 }, // h ram address position
		{ 0x0045, 0x0000 }, // v ram address position
		{ 0x0046, 0x013F }, // v ram end position
		{ 0x0030, 0x0707 }, // gamma control 1
		{ 0x0031, 0x0204 }, // gamma control 2
		{ 0x0032, 0x0204 }, // gamma control 3
		{ 0x0033, 0x0502 }, // gamma control 4
		{ 0x0034, 0x0507 }, // gamma control 5
		{ 0x0035, 0x0204 }, // gamma control 6
		{ 0x0036, 0x0204 }, // gamma control 7
		{ 0x0037, 0x0502 }, // gamma control 8
		{ 0x003A, 0x0302 }, // gamma control 9
		{ 0x002F, 0x12BE }, // optimize access speed 2
		{ 0x003B, 0x0302 }, // gamma control 10 
		{ 0x0023, 0x0000 }, // ram write data mask 1
		{ 0x0024, 0x0000 }, // ram write data mask 2
		{ 0x0025, 0x8000 }, // frame frequency
		{ 0x004e, 0x0000 },	// X-counter
		{ 0x004f, 0x0000 },	// Y-counter
 
	};
	
	for(i = 0; i < 44; i ++) {
		ssd1289_write_reg(init_data[i][0], init_data[i][1]);
	}
	
	return;
}

/* read directly from display memory */
static uint16_t ssd1289_read_reg(uint8_t reg_addr) {
	LCD_REG = reg_addr;
	
	return LCD_RAM;
}

/* write directly to display memory */
static void ssd1289_write_reg(uint8_t reg_addr, uint16_t reg_value) {
	LCD_REG = reg_addr;
	LCD_RAM = reg_value; 
	
	return;
}

/* set backlight, put in 0 to 100 */
void ssd1289_bl_set(int percent) {
	TIM_OCInitTypeDef oc_bl;
	
	oc_bl.TIM_OCMode = TIM_OCMode_PWM1;
	oc_bl.TIM_OutputState = TIM_OutputState_Enable;
	oc_bl.TIM_Pulse = percent*10;	// percent*10
	oc_bl.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC2Init(TIM3, &oc_bl);

	return;
}

/* backlight initialization */
void ssd1289_bl_init() {
	GPIO_InitTypeDef GPIO_InitData;
	TIM_TimeBaseInitTypeDef TimeBase_InitData;
	
	const uint16_t period = (uint16_t) (SystemCoreClock / 1000000) - 1;

	/* enable clock for GPIO, PWM pin and TIM3 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	/* configure PWM pin */
	GPIO_InitData.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitData.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitData.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitData);
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
	
	/* setup PWM */
	TimeBase_InitData.TIM_ClockDivision = 0;
	TimeBase_InitData.TIM_CounterMode = TIM_CounterMode_Up;
	TimeBase_InitData.TIM_RepetitionCounter = 0;
	TimeBase_InitData.TIM_Prescaler = period;
	TimeBase_InitData.TIM_Period = 999;
	TIM_TimeBaseInit(TIM3, &TimeBase_InitData);
	
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	
	return;
}

