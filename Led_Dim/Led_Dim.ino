#define PWM_PIN 26

void setup(){
  pinMode(PWM_PIN, OUTPUT);
}

void loop(){
  for(int duty=0;duty<256; duty++){
    analogWrite(PWM_PIN, duty);
    delay(10);
  }
  
}