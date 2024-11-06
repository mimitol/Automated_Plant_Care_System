#define moisturePin A0

void setup() {
  Serial.begin(9600);
  pinMode(moisturePin, INPUT);
}

void loop() {
  
  int sensorValue = analogRead(moisturePin); 
  Serial.println(sensorValue);
  Serial.print("% moisture");
  delay(1000);
}

