#include <assert.h>



int RSPinNum_t;
int RWPinNum_t;
uint8_t DBPinNum_t[8];
int EnablePinNum_t;
    
class LCD{
    String cur_msg;
    int cur_addr; // top left corner address
    int numBits_t;
    int numLines_t;
    int fontType_t;


    byte get_status();

    public:
        void clear_screen();
        void write_string(char msg[], int line, int position);
        void write_reg(byte regData, boolean regSelect); //Set regSelect to 0 for IR, 1 for DR
        byte read_reg(boolean regSelect); //Set regSelect to 0 for IR, 1 for DR
        void write_byte(char character, int line, int position);
        void set_pins(int RSPinNum, int RWPinNum, int EnablePinNum, int DBPinNum[]);
        void screen_init(int numBits, int numLines, int fontType);
};

byte LCD::get_status(){
  for(int i = 0; i < 8; ++i){
    pinMode(DBPinNum_t[i], INPUT);
  }

  byte status = read_reg(0);
  
  for(int i = 0; i < 8; ++i){
    pinMode(DBPinNum_t[i], OUTPUT);
  }

  return status;
}

void LCD::write_string(char msg[], int line, int position){
  assert(line <= numLines_t);
  assert(position < 16);

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
  size_t str_len = sizeof(msg) / sizeof(msg[0]); // will this include the null char \0 uhhhh only one way to find out :D xD
  for(int i = 0; i < str_len - 1; i++){
    write_reg((int) msg[i], 1);
    /*if (write_addr == 0x0F){
      write_addr = 0x40;
      byte regWrite = 128 | 0x40;
      write_reg(regWrite, 0);
    } else if (write_addr == 0x4F){
      write_addr = 0x00;
      byte regWrite = 128 | 0x00;
      write_reg(regWrite, 0);
    }*/
    delay(500);
  }
}

void LCD::write_byte(char character, int line, int position){
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
 //Serial.println("Inside Screen Init");
  assert((numBits == 4) || (numBits == 8));
  assert((numLines == 2) || (numLines == 1));
  assert((fontType == 1) || (fontType == 0));
  
  numBits_t = (numBits/4) - 1;
  numLines_t = numLines - 1;
  fontType_t = fontType;
  
  byte regData = (1 << 5) | (numBits_t << 4) | (numLines_t << 3) | (fontType_t << 2);

  write_reg(regData, 0);
  regData = 0b00000110; //entry mode 
  delay(1);
  write_reg(regData, 0);
  delay(1);
  regData = 0b00001110; //screen on
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
  if (numBits_t == 0){
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
  } else {
    for (int i = 0; i < 8; i++){
      digitalWrite(DBPinNum_t[i], bitRead(regData, i));
    }
    delay(1);
    digitalWrite(EnablePinNum_t, 0);    
  }
}

byte LCD::read_reg(boolean regSelect){
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
  Serial.println(sizeof(writeString1) / sizeof(writeString1[0]));
  for (int i = 0; i < 15; i++){
    Serial.println(writeString1[i]);
  }
  Display1.write_string(writeString1, 1, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  /*Display1.write_string("Roomba!", 2, 0);
  delay(2000);
  Display1.write_string("BLE 4ever", 2, 0);*/
}
