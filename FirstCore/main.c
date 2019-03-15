#include "i2c_lcd.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_spi.h"

void gpioInit(void);
void i2cInit(void);
void delay(uint32_t t);
void clearLCD(void);
void sentMass(uint8_t mass[]) {
	int i = 0;
	for (i = 0; i < 5; i++) {
		SPI_I2S_SendData(SPI1, mass[i]);
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
			;
	}
}
SPI_InitTypeDef spi;
GPIO_InitTypeDef port;
uint8_t data;
uint8_t needUpdate;
uint8_t bool;
const uint8_t mass[] = "0123456789";
uint8_t dataMass[1];

void enableIRQSPI(void) {
	__enable_irq();
	NVIC_EnableIRQ(SPI1_IRQn);
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
}

void disableIRQSPI(void) {
	__disable_irq();
	NVIC_DisableIRQ(SPI1_IRQn);
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, DISABLE);
}

void itoa(int n) {
	switch (n){
		case 1:
			lcd_Goto(1, 0);
			lcd_PrintC("1");
			break;
		case 2:

			lcd_Goto(1, 0);
			lcd_PrintC("2");
			break;
		case 3:

			lcd_Goto(1, 0);
			lcd_PrintC("3");
			break;
		case 4:

			lcd_Goto(1, 0);
			lcd_PrintC("4");
			break;
		case 5:

			lcd_Goto(1, 0);
			lcd_PrintC("5");
			break;
		case 6:

			lcd_Goto(1, 0);
			lcd_PrintC("6");
			break;
		case 7:

			lcd_Goto(1, 0);
			lcd_PrintC("7");
			break;
		case 8:

			lcd_Goto(1, 0);
			lcd_PrintC("8");
			break;
		case 9:

			lcd_Goto(1, 0);
			lcd_PrintC("9");
			break;
		case 0:

			lcd_Goto(1, 0);
			lcd_PrintC("0");
			break;
	}
}

int main(void) {

	gpioInit();
	i2cInit();
	lcd_Init();
	uint8_t dataMass[1] = { 0x3 };
	bool = 1;
	enableIRQSPI();

	lcd_Goto(2, 0);
	lcd_PrintC("Starting ");
	delay(10000000);

	uint16_t data = 0;

	while (1) {
		clearLCD();
		disableIRQSPI();
		uint16_t sentData = 3;
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);
		SPI_I2S_SendData(SPI1, sentData);
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
			;

		GPIO_SetBits(GPIOA, GPIO_Pin_4);
//			enableIRQSPI();
		clearLCD();
		if (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == SET) {

			if (5 == SPI_I2S_ReceiveData(SPI1)) {
				clearLCD();
				lcd_Goto(2, 0);
				lcd_PrintC("get data good");
				delay(10000000);
			} else {
				clearLCD();
				lcd_Goto(1, 0);
				itoa(SPI_I2S_ReceiveData(SPI1));
				delay(100000);

			}

		}
		delay(100000);

	}
}

void clearLCD() {
	lcd_Goto(1, 0);
	lcd_PrintC("                                      ");
	lcd_Goto(2, 0);
	lcd_PrintC("                                      ");
}

void delay(uint32_t t) {
	uint32_t i = 0;
	for (; i < t; i++)
		;
}

void Delay_ms(uint32_t ms) {
	volatile uint32_t nCount;
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	nCount = (RCC_Clocks.HCLK_Frequency / 10000) * ms;
	for (; nCount != 0; nCount--)
		;
}

void gpioInit(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	SPI_StructInit(&spi);
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi.SPI_Mode = SPI_Mode_Master;
	spi.SPI_DataSize = SPI_DataSize_16b;
	spi.SPI_CPOL = SPI_CPOL_Low;
	spi.SPI_CPHA = SPI_CPHA_2Edge;
	spi.SPI_NSS = SPI_NSS_Soft;
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	spi.SPI_FirstBit = SPI_FirstBit_MSB;
	spi.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &spi);
	GPIO_StructInit(&port);

	port.GPIO_Mode = GPIO_Mode_Out_PP;
	port.GPIO_Speed = GPIO_Speed_50MHz;
	port.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOA, &port);

	port.GPIO_Mode = GPIO_Mode_AF_PP;
	port.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	port.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &port);
	SPI_Cmd(SPI1, ENABLE);
	I2C_Cmd(I2C1, ENABLE);
	SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
	/* Apply I2C configuration after enabling it */
	I2C_Init(I2C1, &I2C_InitStructure);

}

void i2cInit(void) {
	I2C_InitTypeDef i2c;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	i2c.I2C_ClockSpeed = 50000;
	i2c.I2C_Mode = I2C_Mode_I2C;
	i2c.I2C_DutyCycle = I2C_DutyCycle_2;
	i2c.I2C_OwnAddress1 = 0x00;
	i2c.I2C_Ack = I2C_Ack_Enable;
	i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C1, &i2c);
	I2C_Cmd(I2C1, ENABLE);
}

void SPI1_IRQHandler() {

	data = SPI_I2S_ReceiveData(SPI1);
	if (data == 0x3) {
		lcd_Goto(2, 0);
		lcd_PrintC("GOOD" + (data + 32));
		delay(10000000);
	}
	lcd_Goto(2, 0);
	lcd_PrintC("IRQ start");
}
