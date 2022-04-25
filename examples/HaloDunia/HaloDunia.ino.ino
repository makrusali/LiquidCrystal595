#include "LiquidCrystal595.h"


/*
 * LiquidCrystal595 lcd(data_pin,clock_pin,latch_pin);
 */
LiquidCrystal595 lcd(2,3,4);

void setup(){
  lcd.begin(20,4);
  //turn on the backlight
  lcd.backlight();
  //goto col=3 - row=0
  lcd.setCursor(3,0);
  lcd.print("Halo Dunia!");
  //goto col=4 - row=1
  lcd.setCursor(4,1);
  lcd.print("INDONESIA");
}

void loop() {
  //blink the backlight
  lcd.backlight();
  delay(1000);
  lcd.noBacklight();
  delay(1000);
}
