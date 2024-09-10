#include "LCD_Interface.h"

char roombaString[] = "Roomba!";
char BLEString[] = "BLE 4ever";
char blank = ' ';

// Initialize global LCD class
LCD Display1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  int RSPinNum     = 2;  
  int RWPinNum     = 3;
  int EnablePinNum = 4;
  int DBPinNum[8]  = {5, 6, 7, 8, 9, 10, 11, 12};
  
  Display1.set_pins(RSPinNum, RWPinNum, EnablePinNum, DBPinNum);
  //Serial.println("Pins Setup");
  Display1.screen_init(8, 2, 0);
  //Serial.println("Screen Init");
  Serial.println("Done Setup");
  Display1.clear_screen();
  delay(2000);
  char writeString1[] = "ARDUINO RULES!";
  char testChar = 'Z';
  //Serial.println(sizeof(writeString1) / sizeof(writeString1[0]));
  /*for (int i = 0; i < 15; i++){
    Serial.println(writeString1[i]);
  }*/
  //Display1.write_string(writeString1, 1, 0);
  //Display1.write_reg((int) testChar, 1);
  //Display1.write_byte('Z', 2, 0);  
  //Display1.write_reg(0xC0, 0);
  Display1.write_line(writeString1, 1, 0);
  delay(100);
  //byte debugReturn = Display1.read_reg(0);
  //Serial.print("Address: ");
  //Serial.println(debugReturn, BIN);  
}

void loop() {
  // put your main code here, to run repeatedly:
  Display1.write_line(roombaString, 2, 0);
  delay(2000);
  Display1.write_line(BLEString, 2, 0);
  delay(2000);
}
