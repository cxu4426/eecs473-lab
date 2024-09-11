#include <assert.h>



int RSPinNum_t;
int RWPinNum_t;
uint8_t DBPinNum_t[8];
int EnablePinNum_t;
char roombaString[] = "Roomba!";
char BLEString[] = "BLE 4ever";
char blank = ' ';
    
class LCD{
  String cur_msg;
  int cur_addr; // top left corner address
  int numBits_t;
  int numLines_t;
  int fontType_t;
  
  byte get_status();
  
  public:
    void clear_screen();
    void write_line(char msg[], int line, int position);
    void write_reg(byte regData, boolean regSelect); //Set regSelect to 0 for IR, 1 for DR
    byte read_reg(boolean regSelect); //Set regSelect to 0 for IR, 1 for DR
    void write_byte(char character, int line, int position);
    void set_pins(int RSPinNum, int RWPinNum, int EnablePinNum, int DBPinNum[]);
    void screen_init(int numBits, int numLines, int fontType);
};

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
    delay(10);
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

// Initialize global LCD class
LCD Display1;

void setup() {
  // put your setup code here, to run once:

  //Delay start so voltage can be stable
  delay(1000)
  
  Serial.begin(9600);
  int RSPinNum     = 2;  
  int RWPinNum     = A4;
  int EnablePinNum = 4;
  int DBPinNum[8]  = {A0, A1, A2, A3, 6, 10, 11, 12};
  
  Display1.set_pins(RSPinNum, RWPinNum, EnablePinNum, DBPinNum);
  //Serial.println("Pins Setup");
  Display1.screen_init(4, 2, 0);
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