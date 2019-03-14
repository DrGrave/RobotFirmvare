#ifndef _I2C_LCD_H_
#define _I2C_LCD_H_

#include "stm32f10x_i2c.h"

// Адрес PCF8574
#define LCD_ADDR	0x07

// Раскомментировать нужную строку для своего ЖКИ
#define LCD_2004
//#define LCD_1602

// Указать название вункции задержки
extern void delay(uint32_t t);
#define lcd_pause	delay(1000) // а тут указать значение задержки

// Не нужно редактировать.
// Описание пинов расширителя портов
#define PCF_P0	0
#define PCF_P1	1
#define PCF_P2	2
#define PCF_P3	3
#define PCF_P4	4
#define PCF_P5	5
#define PCF_P6	6
#define PCF_P7	7

// Соответствие пинов ЖКИ и расширителя портов. Возможно тут подредактировать под себя
#define DB4		PCF_P4
#define DB5		PCF_P5
#define DB6		PCF_P6
#define DB7		PCF_P7
#define EN		PCF_P2
#define RW		PCF_P1
#define RS		PCF_P0
#define BL		PCF_P3

// Внутрення переменная
uint8_t backlightState;

// Функции API
void lcd_Init(void); // Инициализация ЖКИ
void lcd_Backlight(uint8_t state); // Включение/отключение подсветки
void lcd_Goto(uint8_t row, uint8_t col); // Переход на строка/позицию
void lcd_PrintC(const uint8_t *str); // Вывести строку


// Функции внурненние
void lcd_Send(uint8_t data); // отправить значение в ЖКИ
void lcd_Command(uint8_t com); // отправить комманду в ЖКИ
void lcd_Data(uint8_t com);  // отправить данные в ЖКИ



#endif
