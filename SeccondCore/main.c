#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_spi.h"
#include "stm32f10x.h"

/*******************************************************************/
GPIO_InitTypeDef port;
SPI_InitTypeDef spi;
uint8_t data;
uint8_t bool;
uint8_t needUpdate;

void enableIRQSPI(void){
	__enable_irq();
	NVIC_EnableIRQ(SPI2_IRQn);
	SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);
}

void disableIRQSPI(){
	__disable_irq();
	NVIC_DisableIRQ(SPI2_IRQn);
	SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);
}

void sendData(uint8_t mass[]){
	int i;
	for (i = 0; i < 1; i++){
		SPI_I2S_SendData(SPI2, 0x93);
				while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET)
								;
	}
}

/*******************************************************************/
void initAll() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;

	SPI_StructInit(&spi);
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi.SPI_Mode = SPI_Mode_Slave;
	spi.SPI_DataSize = SPI_DataSize_16b;
	spi.SPI_CPOL = SPI_CPOL_Low;
	spi.SPI_CPHA = SPI_CPHA_2Edge;
	spi.SPI_NSS = SPI_NSS_Hard;
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	spi.SPI_FirstBit = SPI_FirstBit_MSB;
	spi.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &spi);

	port.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_14 ;
	port.GPIO_Speed = GPIO_Speed_50MHz;
	port.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &port);

	GPIO_StructInit(&port);
	port.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	port.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	port.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &port);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/*******************************************************************/
int main() {

	initAll();

	SPI_Cmd(SPI2, ENABLE);
	enableIRQSPI();
	bool = 0;
	//“ут мы разрешаем прерывание по приему
	//Ќу вот прин€ли, теперь просто зажигаем диоды
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	int i;

	while (1) {
	}
}

/*******************************************************************/
void SPI2_IRQHandler() {
	if (SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE)==SET) {
		uint16_t data = SPI2->DR;
		if (data == 3){
			GPIOC->ODR ^= GPIO_Pin_13;
			SPI2->DR = 5;
		}
	}
}

