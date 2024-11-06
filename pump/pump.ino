#define pumpPin1 3
#define pumpPin2 4

void setup() {
    pinMode(pumpPin1, OUTPUT);
    pinMode(pumpPin2, OUTPUT);
}

void turnOnPump() {
    unsigned long pumpStartTime = millis();
    unsigned long currentTime = millis();
    // הפעלת המשאבה
    digitalWrite(pumpPin1, HIGH);
    digitalWrite(pumpPin2, LOW);
    while ((currentTime - pumpStartTime) < 2000) {
      currentTime = millis();
    }
    // כיבוי המשאבה
    digitalWrite(pumpPin1, LOW);
    // presentMessage("watered!");
}

void loop() {
    
    turnOnPump();
    delay(5000);

}
    // timesOfWatering+=1;
    // //בדיקה האם מתבצעם השקיה לא פרופורציונאלית
    // if(hours+1<timesOfWatering)
    // {
    //     tooMuchWatering();
    //     return;
    // }
    // unsigned long pumpStartTime = millis();
    // unsigned long currentTime = millis();
    // // הפעלת המשאבה
    // analogWrite(pumpPowerPin,100);
    // digitalWrite(pumpPin1, HIGH);
    // digitalWrite(pumpPin2, LOW);
    // //המתנה לכיבוי המשאבה
    // do {
    //     currentTime = millis();
    // } while (currentTime - pumpStartTime < 2500);
    // // כיבוי המשאבה
    // digitalWrite(pumpPin1, LOW);
    // presentMessage("watered!");


