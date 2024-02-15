#include <LiquidCrystal_I2C.h>

uint32_t ADDRESS_LCD = 0X3F;
uint8_t COL = 16;
uint8_t ROW = 2;


LiquidCrystal_I2C lcd(ADDRESS_LCD, COL, ROW); // set the LCD address to 0x27 for a 16 chars and 2 line display


void setup_lcd()
{
    lcd.init(); // initialize the lcd.
    lcd.backlight();
}

void printLcd(String line1, String line2)
{
    UBaseType_t uxHighWaterMark;

  lcd.clear();

  vTaskDelay(100 / portTICK_PERIOD_MS);
  lcd.setCursor(0, 0);
  lcd.print(line1); // Prints line1

  vTaskDelay(5 / portTICK_PERIOD_MS);
  lcd.setCursor(0, 1);
  lcd.print(line2); // Prints line2

  vTaskDelay(250 / portTICK_PERIOD_MS);

  uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
}