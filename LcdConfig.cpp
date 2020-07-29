#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C LCD(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display


bool InitLcd(){
  LCD.init();                      // initialize the lcd 
  LCD.backlight();
  //lcd.autoscroll();
}

void LcdPrint(String msg, int rowId){
  char charMsg[msg.length()+1];
  msg.toCharArray(charMsg, msg.length()+1);
  LCD.setCursor(0,rowId);
  LCD.printstr(charMsg);
}

void LcdPrint(String msg){
  LCD.clear();
  LcdPrint(msg, 0);
}

void LcdPrint(String msg1, String msg2){
  LCD.clear();
  LcdPrint(msg1, 0);

  LcdPrint(msg2, 1);
}

void LcdPrint(String msg, bool autoScroll){
  char charMsg[msg.length()+1];
  msg.toCharArray(charMsg, msg.length()+1);
  LCD.setCursor(16,1);
  LCD.autoscroll();
  LCD.printstr(charMsg);
}
