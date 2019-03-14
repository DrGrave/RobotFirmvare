//#include "stm32f10x.h"
//#include "stm32f10x_gpio.h"
//#include "stm32f10x_rcc.h"
//
//
//int main(void)
//{
//  int i;
//  /* Initialize Leds mounted on STM32 board */
//  GPIO_InitTypeDef  GPIO_InitStructure;
//  /* Initialize LED which connected to PC13, Enable the Clock*/
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//  /* Configure the GPIO_LED pin */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_O;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//  while (1)
//  {
//    /* Toggle LED which connected to PC13*/
//    GPIOC->ODR ^= GPIO_Pin_13;
//
//    /* delay */
//    for(i=0;i<0x100000;i++);
//  }
//}

#include "i2c_lcd.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_spi.h"



void gpioInit(void);
void i2cInit(void);
void delay(uint32_t t);

const uint8_t mes[] = "STM32F4 + I2C + LCD";

int main(void) {

	gpioInit();
//	i2cInit();
//	lcd_Init();

//	lcd_PrintC(mes);
//
//	lcd_Goto(2, 0);
//	lcd_PrintC("LCD4x20 with PCF8574");




    while(1) {}
}

void delay(uint32_t t) {
	uint32_t i = 0;
	for (; i < t; i++);
}

void SPI1_IRQHandler (void) {
	if (SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==SET) {
	// Прерывание вызвано приемом байта ?
		uint8_t data = SPI1->DR; //Читаем то что пришло
		SPI1->DR = data; //И отправляем обратно то что приняли
	}
}

void gpioInit(void) {
	GPIO_InitTypeDef  GPIO_InitStructure;
	I2C_InitTypeDef  I2C_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//Заполняем структуру с параметрами SPI модуля
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //полный дуплекс
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; // передаем по 8 бит
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; // Полярность и
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; // фаза тактового сигнала
	SPI_InitStructure.SPI_NSS = SPI_NSS_Hard; // Управлять состоянием сигнала NSS аппаратно
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; // Предделитель SCK
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; // Первым отправляется старший бит
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave; // Режим - слейв
	SPI_Init(SPI1, &SPI_InitStructure); //Настраиваем SPI1
	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE,ENABLE); //Включаем прерывание по приему байта
	SPI_Cmd(SPI1, ENABLE); // Включаем модуль SPI1....
	NVIC_EnableIRQ(SPI1_IRQn);

	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x38;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 100000;

	I2C_Cmd(I2C1, ENABLE);
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

