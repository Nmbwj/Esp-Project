#define BUTTONPIN 12
#define LEDPIN 13

void setup(){

  pinMode(BUTTONPIN, INPUT_PULLUP);
  pinMode(LEDPIN, OUTPUT);
}
void loop(){
  int buttonRead = digitalRead(BUTTONPIN);

  if(buttonRead == LOW)
  digitalWrite(LEDPIN, HIGH);
  if(buttonRead == HIGH)
  digitalWrite(LEDPIN, LOW);
}