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
