#include "i2c_lcd.h"

uint8_t backlightState = 1;


void lcd_Goto(uint8_t row, uint8_t col) {
#ifdef LCD_2004
	switch (row){
		case 1:
			lcd_Command(0x80 + col);
			break;
		case 2:
			lcd_Command(0x80 + col + 0x40);
			break;
		case 3:
			lcd_Command(0x80 + col + 0x14);
			break;
		case 4:
			lcd_Command(0x80 + col + 0x54);
			break;
	}
#endif
}

void lcd_PrintC(const uint8_t *str) {
 	uint8_t i;
 	while (i = *str++){
    	lcd_Data(i);
 	}
}

void lcd_Init(void) {
	lcd_Command(0x33);
	lcd_pause;
	lcd_Command(0x32); // установка режима: 4 линии
	lcd_Command(0x28); // 2 строки и 5*8 точек
	lcd_Command(0x08); // выключить отображение
	lcd_Command(0x01); // очистка экрана
	lcd_pause;
	lcd_Command(0x06); // направление сдвига курсора
	lcd_Command(0x0C); // включить отображение
}

void lcd_Send(uint8_t data) {

	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C1, ENABLE);

	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, ((0x20+LCD_ADDR) << 1), I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C1, data);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTOP(I2C1, ENABLE);
}

void lcd_Command(uint8_t com) {
	uint8_t data = 0;

	data |= (backlightState & 0x01) << BL;

	data |= (((com & 0x10) >> 4) << DB4);
	data |= (((com & 0x20) >> 5) << DB5);
	data |= (((com & 0x40) >> 6) << DB6);
	data |= (((com & 0x80) >> 7) << DB7);
	lcd_Send(data);

	data |= (1 << EN);
	lcd_Send(data);
	lcd_pause;

	data &= ~(1 << EN);
	lcd_Send(data);
	lcd_pause;

	data = 0;

	data |= (backlightState & 0x01) << BL;

	data |= (((com & 0x01) >> 0) << DB4);
	data |= (((com & 0x02) >> 1) << DB5);
	data |= (((com & 0x04) >> 2) << DB6);
	data |= (((com & 0x08) >> 3) << DB7);
	lcd_Send(data);

	data |= (1 << EN);
	lcd_Send(data);
	lcd_pause;

	data &= ~(1 << EN);
	lcd_Send(data);
	lcd_pause;
}

void lcd_Backlight(uint8_t state) {
	backlightState = (state & 0x01) << BL;
	lcd_Send(backlightState);
}

void lcd_Data(uint8_t com) {
	uint8_t data = 0;

	data |= (1 << EN);
	data |= (1 << RS);
	data |= (backlightState & 0x01) << BL;

	data |= (((com & 0x10) >> 4) << DB4);
	data |= (((com & 0x20) >> 5) << DB5);
	data |= (((com & 0x40) >> 6) << DB6);
	data |= (((com & 0x80) >> 7) << DB7);
	lcd_Send(data);
	lcd_pause;

	data &= ~(1 << EN);
	lcd_Send(data);
	lcd_pause;

	data = 0;

	data |= (1 << EN);
	data |= (1 << RS);
	data |= (backlightState & 0x01) << BL;

	data |= (((com & 0x01) >> 0) << DB4);
	data |= (((com & 0x02) >> 1) << DB5);
	data |= (((com & 0x04) >> 2) << DB6);
	data |= (((com & 0x08) >> 3) << DB7);
	lcd_Send(data);
	lcd_pause;

	data &= ~(1 << EN);
	lcd_Send(data);
	lcd_pause;
}
