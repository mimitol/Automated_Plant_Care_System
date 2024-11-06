#define ldrPin A1
int ldrValue = 0;
void setup() {
  pinMode(ldrPin,INPUT);
  Serial.begin(9600);
}

void loop() {
  ldrValue = analogRead(ldrPin);
  Serial.println(ldrValue);
}

int lightCheck()
{
      Serial.println("light checking");
      int sensorValue = analogRead(ldrPin);
      return sensorValue;
}
