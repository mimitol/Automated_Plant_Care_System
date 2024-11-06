#include <Servo.h>


Servo s;
void setup() {
    //pinMode(11,OUTPUT);
    s.attach(11);
    Serial.begin(9600);
}
void loop() {
  for(int i=0;i<=180;i++)
  {
      s.write(i);
      delay(15);
      Serial.println(i);
  }
  delay(2000);
  for(int i=180;i>=0;i--)
  {
      s.write(i);
      delay(15);
       Serial.println(i);
  }
  delay(2000);
}
