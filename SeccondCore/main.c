#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_spi.h"
#include "stm32f10x.h"

/*******************************************************************/
GPIO_InitTypeDef port;
SPI_InitTypeDef spi;
uint8_t data;
uint16_t getedData;
uint8_t indexData = 0;
uint8_t boolFirstBit;

void enableIRQSPI(void) {
	__enable_irq();
	NVIC_EnableIRQ(SPI1_IRQn);
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
}

void disableIRQSPI() {
	__disable_irq();
	NVIC_DisableIRQ(SPI1_IRQn);
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, DISABLE);
}

void sendData(uint8_t mass[]) {
	int i;
	for (i = 0; i < 1; i++) {
		SPI_I2S_SendData(SPI1, 0x93);
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
			;
	}
}

/*******************************************************************/
void initAll() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);



	SPI_StructInit(&spi);
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi.SPI_Mode = SPI_Mode_Slave;
	spi.SPI_DataSize = SPI_DataSize_16b;
	spi.SPI_CPOL = SPI_CPOL_Low;
	spi.SPI_CPHA = SPI_CPHA_2Edge;
	spi.SPI_NSS = SPI_NSS_Soft;
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	spi.SPI_FirstBit = SPI_FirstBit_MSB;
	spi.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &spi);
	GPIO_StructInit(&port);

	port.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	port.GPIO_Speed = GPIO_Speed_50MHz;
	port.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOA, &port);

	port.GPIO_Mode = GPIO_Mode_AF_PP;
	port.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	port.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &port);

	SPI_Cmd(SPI1, ENABLE);
	SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Reset);
	/* Apply I2C configuration after enabling it */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/*******************************************************************/
int main() {

	initAll();

	enableIRQSPI();
	//Тут мы разрешаем прерывание по приему
	//Ну вот приняли, теперь просто зажигаем диоды
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	while (1) {
	}
}

void SPI1_Write(uint16_t data) {
	//Ждем, пока не освободится буфер передатчика
	while (!(SPI1->SR & SPI_SR_TXE))
		;

	//заполняем буфер передатчика
	SPI1->DR = data;
}

uint16_t SPI1_Read(void) {
	//Ждем, пока не появится новое значение
	//в буфере приемника
	while (!(SPI1->SR & SPI_SR_RXNE))
		;

	//возвращаем значение буфера приемника
	return SPI1->DR;
}

/*******************************************************************/
void SPI1_IRQHandler() {
	getedData = SPI1_Read();
	if (getedData == 10) {
		GPIOC->ODR ^= GPIO_Pin_13;
		//заполняем буфер передатчика
		SPI1_Write(6);
	} else if (getedData == 5) {

		GPIOC->ODR ^= GPIO_Pin_13;
		SPI1_Write(3);
	}
}

