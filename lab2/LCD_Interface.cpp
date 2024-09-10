#include "LCD_Interface.h"
#include <assert.h>

int RSPinNum_t;
int RWPinNum_t;
uint8_t DBPinNum_t[8];
int EnablePinNum_t;

byte LCD::get_status(){
  /*
    Get the busy bit and the current AC
  */
  for(int i = 0; i < 8; ++i){
    pinMode(DBPinNum_t[i], INPUT);
  }

  byte status = read_reg(0);
  
  for(int i = 0; i < 8; ++i){
    pinMode(DBPinNum_t[i], OUTPUT);
  }

  return status;
}

void LCD::write_line(char msg[], int line, int position){
  /*
    Write given string to the specified line starting at position.
    The spaces that don't have chars are left blank.
  */
  assert(line <= (numLines_t + 1));
  assert(position < 16);

  int write_addr = (line - 1) * 64 + position;
  // Serial.print("Write addr: ");
  // Serial.println(write_addr, HEX);

  byte regData = 128 | write_addr;
  write_reg(regData, 0);

  size_t str_len = strlen(msg);
  // Serial.print("String Length: ");
  // Serial.println(str_len);

  for(int i = 0; i < ((int)str_len); i++){
    write_reg((int) msg[i], 1);
    // Serial.print("Printing ");
    // Serial.println(msg[i]);

    // update address correctly when it needs to wrap around
    if (write_addr == 0x0F){
      write_addr = 0x40;
      byte regWrite = 128 | 0x40;
      write_reg(regWrite, 0);
    }
    else if (write_addr == 0x4F){
      write_addr = 0x00;
      byte regWrite = 128 | 0x00;
      write_reg(regWrite, 0);
    }

    delay(25);
  }

  for(int i = str_len; i < 16; i++){
    write_reg((int) blank, 1);
    delay(25);
  }
  //Serial.println("Left Print");
}

void LCD::write_byte(char character, int line, int position){
  /*
    Write a single character onto the LCD at the specified line and position
  */
  int write_addr = (line - 1) * 64 + position;
  byte regData = 128 | 
                 (bitRead(write_addr, 6)) | 
                 (bitRead(write_addr, 5)) | 
                 (bitRead(write_addr, 4)) |
                 (bitRead(write_addr, 3)) | 
                 (bitRead(write_addr, 2)) | 
                 (bitRead(write_addr, 1)) | 
                 (bitRead(write_addr, 0));
  write_reg(regData, 0);
  write_reg((int) character, 1);
}

void LCD::set_pins(int RSPinNum, int RWPinNum, int EnablePinNum, int DBPinNum[]) {
  /*
    Sets the pin numbers on the Arduino to the correct mode and 
    connects them to the corresponding pins on the LCD.
  */
  RSPinNum_t = RSPinNum;
  RWPinNum_t = RWPinNum;
  EnablePinNum_t = EnablePinNum;

  for(int i = 0; i < 8; ++i) {
    DBPinNum_t[i] = DBPinNum[i];
  }
  
  pinMode(RSPinNum_t, OUTPUT);  
  pinMode(RWPinNum_t, OUTPUT);
  pinMode(EnablePinNum_t, OUTPUT);

  for(int i = 0; i < 8; ++i) {
    pinMode(DBPinNum_t[i], OUTPUT);
  }

}

void LCD::screen_init(int numBits, int numLines, int fontType){
  /*
    function set instruction
    entry mode set
    turn on display
  */
  assert((numBits == 4) || (numBits == 8));
  assert((numLines == 2) || (numLines == 1));
  assert((fontType == 1) || (fontType == 0));
  
  numBits_t = (numBits/4) - 1;
  numLines_t = numLines - 1;
  fontType_t = fontType;
  
  byte regData = (1 << 5) | (numBits_t << 4) | (numLines_t << 3) | (fontType_t << 2);

  // Serial.print("regData: ");
  // Serial.println(regData, BIN);
  write_reg(regData, 0);
  delay(1);

  // Set entry mode
  regData = 0b00000110;
  write_reg(regData, 0);
  delay(1);

  // Turn on display
  regData = 0b00001110;
  write_reg(regData, 0);
}


void LCD::clear_screen(){
  write_reg(0x01, 0);
}

void LCD::write_reg(byte regData, boolean regSelect){
  /*
  write to IR if regSelect == 0
  write to DR if regSelect == 1
  */
  digitalWrite(RSPinNum_t, regSelect);
  digitalWrite(RWPinNum_t, 0);
  delay(1);

  digitalWrite(EnablePinNum_t, 1);
  delay(1);

  if (numBits_t == 0){ // 4-bit mode
    for (int i = 4; i < 8; i++){
      digitalWrite(DBPinNum_t[i], bitRead(regData, i));
    }
    delay(1);
    digitalWrite(EnablePinNum_t, 0);
    delay(1);
    digitalWrite(EnablePinNum_t, 1);
    delay(1);
    for (int i = 0; i < 4; i++){
      digitalWrite(DBPinNum_t[i+4], bitRead(regData, i));
    }
    delay(1);
    digitalWrite(EnablePinNum_t, 0);
    delay(1);
  } 
  else { // if numBits_t == 1 (8-bit mode)
    for (int i = 0; i < 8; i++){
      digitalWrite(DBPinNum_t[i], bitRead(regData, i));
    }
    delay(1);
    digitalWrite(EnablePinNum_t, 0);    
  }
  delay(5);
}

byte LCD::read_reg(boolean regSelect){
  /*
    Read value from registers.
    If regSelect is 0, read from IR
    If regSelect is 1, read from DR
  */
  digitalWrite(RSPinNum_t, regSelect);
  digitalWrite(RWPinNum_t, 1);
  delay(1);
  digitalWrite(EnablePinNum_t, 1);
  delay(1);

  byte readData;
  for(int i = 0; i < 8; i++){
    int data = digitalRead(DBPinNum_t[i]);
    bitWrite(readData, i, data);
  }

  return readData;
}
