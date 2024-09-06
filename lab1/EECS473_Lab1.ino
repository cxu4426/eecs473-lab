int ADC_val = 0;

void setup() {
  // put your setup code here, to run once:
  DDRB = B00111111;
  pinMode(A0, INPUT);
  Serial.begin(38400);
}

void loop() {
  // put your main code here, to run repeatedly:
  ADC_val = analogRead(A0);
  //PORTB = B00011110;
  if (ADC_val < 255){
    PORTB = B00000010;
  } else if (255 < ADC_val && ADC_val < 511){
    PORTB = B00000110;
  } else if (511 < ADC_val && ADC_val < 767){
    PORTB = B00001110;
  } else if (767 < ADC_val){
    PORTB = B00011110;
  }
  //Serial.println(ADC_val);
}
