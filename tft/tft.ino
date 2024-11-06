#include "TFT9341Touch.h"
#include <Servo.h>
#include <stdio.h>
#include <ArdRTOS.h>
#define ldrPin A1
#define moisturePin A0
#define pumpPowerPin 6
#define pumpPin1 3
#define pumpPin2 4
#define ledPin 8
tft9341touch LcdTouch(10, 9, 7, 2); //cs, dc ,tcs, tirq
int moistureSensorResult, lightSensorResult, lightHours = 0, darkHours = 0,wantedMoisture = -1, wantedLight = -1;
char currentMoisture[20], currentLight[20];
unsigned long previousMillisForLdr = 0 ,currentMillisForLdr,previousMillisForMoist=0,currentMillisForMoist;
const long hour = 10000 ,fiveMinutes=1500; 
bool flagForStart=false;
Servo myServo;
int timesOfWatering=0,hours=0;
void setup()
{
    Serial.begin(9600);    
    pinMode(ldrPin, INPUT);
    myServo.attach(5);      
    pinMode(moisturePin, INPUT);
    pinMode(pumpPin1, OUTPUT);
    pinMode(pumpPin2, OUTPUT);
    pinMode(ledPin, OUTPUT);
    LcdTouch.begin();
    LcdTouch.clearButton();
    LcdTouch.setRotation(0);//0,1,2,3
    LcdTouch.setTextSize(2);
    LcdTouch.setCursor(40, 40);
    LcdTouch.set(3780, 372, 489, 3811);
    openingScreen();
}

void loop()
{
  lightFunction();
  moistureFunction();
}
//פונקציה שאחראית על הפעלת הפונקציה שמקבלת נתונים מחיישן האור,הדפסה על המסך ועדכון הפרמטרים הרלוונטים
void lightFunction()
{
    currentMillisForLdr = millis();
    //כל שעה
    if (currentMillisForLdr - previousMillisForLdr >= hour||hours==0)
    {
        hours+=1;
        lightSensorResult = lightSensorCheck();
        sprintf(currentLight, "%d", lightSensorResult);
        LcdTouch.fillRoundRect(135, 140 ,100, 45, 3, GREEN);
        LcdTouch.print(145, 150, currentLight, 3, WHITE);
        previousMillisForLdr = currentMillisForLdr; // אתחול הטיימר
        //אם מואר
        if (lightSensorResult > 650)
        {
            lightHours += 1;
            darkHours = 0;
        }
        //אם חשוך
        else
        {
            darkHours += 1;
            lightHours = 0;
        }
        lightConditions();
    }
}

//פונקציה שאחראית על הפעלת הפונקציה שמקבלת נתונים מחיישן לחות האדמה ,הדפסה על המסך ועדכון הפרמטרים הרלוונטים
void moistureFunction()
{
    currentMillisForMoist = millis();
    //כל חמש דקות
    if (currentMillisForMoist - previousMillisForMoist >= fiveMinutes)
    {
       moistureSensorResult = moistureSensorCheck();
       sprintf(currentMoisture, "%d", moistureSensorResult);
       LcdTouch.fillRoundRect(190, 80 ,100, 45, 3, GREEN);
       LcdTouch.print(200, 90, currentMoisture, 3, WHITE);
       moistureConditions();
      previousMillisForMoist = currentMillisForMoist; //אתחול הטיימר
    }  
}
//פונקציה שקוראת בפועל מחיישן האור
int lightSensorCheck()
{
    int sensorValue = analogRead(ldrPin);
    return sensorValue;
}
//פונקציה שקוראת בפועל מחיישן לחות אדמה
int moistureSensorCheck()
{
    int sensorValue = analogRead(moisturePin);
    return sensorValue;
}
//פונקציה שאחראית לפרש את הנתונים שהתקבלו מחישן האור ולפעול בהתאם
void lightConditions()
{
    switch (wantedLight)
    {
        //רמת אור רצויה-נמוכה
        case 0:
            if (darkHours > 23)
              {darkHours=0;
              turnOnServo(1);} 
            else if (lightHours > 3)
                {lightHours=0;
                turnOnServo(0);}
                
            break;
       //רמת אור רצויה-בינונית
        case 1:
            if (darkHours > 20)
              {darkHours=0;
              turnOnServo(1);}
            else if (lightHours > 8)
              {lightHours=0;
              turnOnServo(0);}
            break;
       //רמת אור רצויה-גבוהה
        case 2:
            if (darkHours > 15)
             { darkHours=0;
              turnOnServo(1);}
            break;

    }
}

//פונקציה שאחראית לפרש את הנתונים שהתקבלו מחישן לחות אדמה ולפעול בהתאם
void moistureConditions()
{
  //לחות די גבוהה, רמת השקיה רצויה גבוהה
    if (moistureSensorResult > 400 && wantedMoisture > 1)
    {
        turnOnPump(400);
    }
    else 
      // לחות בינונית, רמת השקיה רצויה בינונית או גבוהה
      if (moistureSensorResult > 600  && wantedMoisture > 0)
      {
        turnOnPump(600);

      }
      else 
         //לחות נמוכה, רמת השקיה רצויה נמוכה, בינונית או גבוהה
        if (moistureSensorResult > 800)
          {
          turnOnPump(800);}
            
}
//פונקציה שמפעילה את מנוע הסרוו, מקבלת משתנה מצב שאומר האם לפתוח או לסגור את ההצללה
void turnOnServo(int state)
{

//לכסות
  if(state==0)
  {
      for(int i=90;i<=180;i++)
      {
          myServo.write(i);
          delay(15);
      } 
      presentMessage("covered");
  }
  //לגלות
  else
  {
      for(int i=180;i>=90;i--)
      {
          myServo.write(i);
          delay(15);

      }
      presentMessage("uncovered");
  }
}
//פונקציה שמפעילה את המשאבה
void turnOnPump(int goalMouisture) {

     timesOfWatering+=1;
    //בדיקה האם מתבצעם השקיה לא פרופורציונאלית
    Serial.println(timesOfWatering);
    Serial.println(hours);
    if(hours+1<timesOfWatering)
    {
        tooMuchWatering();
        return;
    }
    int moistureWhileWatering;
    // הפעלת המשאבה
    analogWrite(pumpPowerPin,70);
    digitalWrite(pumpPin1, HIGH);
    digitalWrite(pumpPin2, LOW);
    //המתנה לכיבוי המשאבה
    do {
        moistureWhileWatering = moistureSensorCheck();
    } while (moistureWhileWatering>goalMouisture);
    // כיבוי המשאבה
    digitalWrite(pumpPin1, LOW);
    analogWrite(pumpPowerPin,0);
    presentMessage("watered!");
}
//פונקציה שמציגה הודעה על המסך כאשר הצמח מושקה, מכוסה או מתגלה לשמש
void presentMessage(char *message) {
    unsigned long startMillis = millis(); 
    unsigned long current;
    LcdTouch.fillRoundRect(20, 20 ,280, 195, 3, WHITE);
    LcdTouch.print(76, 90, "The plant", 3, GREEN);
    LcdTouch.print(50, 140, "was", 3, GREEN);
    LcdTouch.print(130, 140, message, 3, GREEN);
    do {
        current = millis();
    } while (current - startMillis < 3000);
    homeScreen();
}
//פונקציה שמופעלת במקרה שזוהתה השקיה לא פרופורציונאלית
void tooMuchWatering()
{
  int x,y,ButtonNum;
  hours=0;
  timesOfWatering=0;
   LcdTouch.clearButton();
   LcdTouch.fillScreen(WHITE);
   LcdTouch.print(10, 30, "someting is wrong!", 3, GREEN);
   LcdTouch.print(50, 90, "too much watering", 2, GREEN);
   LcdTouch.print(40, 120, "replace the sensors", 2, GREEN);
   LcdTouch.drawButton(8, 110, 150, 100, 45, 10, GREEN, WHITE, "ok", 2); // NumButton, x, y, width, height, r, Color, textcolor, label, textsize);
   digitalWrite(ledPin,HIGH);
  do
    {
        if (LcdTouch.touched())
        {
            LcdTouch.readTouch();
            x = LcdTouch.xTouch;
            y = LcdTouch.yTouch;
            ButtonNum = LcdTouch.ButtonTouch(x, y);
        }
    } while (ButtonNum != 8);
    digitalWrite(ledPin,LOW);
    homeScreen();
}
//פונקציה שאחראית על התצוגה על מסך הבית
void homeScreen()
{
    LcdTouch.clearButton();
    LcdTouch.fillScreen(MAGENTA);
    LcdTouch.print(30, 30, "PlantPal...", 3, WHITE);
    LcdTouch.print(30, 90, "mouistre:", 3, WHITE);
    LcdTouch.print(30, 150, "light:", 3, WHITE);

}
//פונקציה שאחראית על התצוגה וההגדרות במסך הפתיחה
void openingScreen()
{
    int ButtonNum;
    LcdTouch.fillScreen(MAGENTA);
    LcdTouch.print(30, 10, "welcome!", 3, WHITE);
    LcdTouch.print(30, 40, "please choose settings", 2, WHITE);

    LcdTouch.print(30, 70, "light hours:", 2, WHITE);
    LcdTouch.drawButton(1, 20, 90, 85, 30, 10, WHITE, GREEN, "1-3", 2); // NumButton, x, y, width, height, r, Color, textcolor, label, textsize);
    LcdTouch.drawButton(2, 120, 90, 85, 30, 10, WHITE, GREEN, "4-8", 2); // NumButton, x, y, width, height, r, Color, textcolor, label, textsize);
    LcdTouch.drawButton(3, 220, 90, 85, 30, 10, WHITE, GREEN, "9-12", 2); // NumButton, x, y, width, height, r, Color, textcolor, label, textsize);

    LcdTouch.print(30, 130, "water:", 2, WHITE);
    LcdTouch.drawButton(4, 20, 150, 85, 30, 10, WHITE, GREEN, "low", 2); // NumButton, x, y, width, height, r, Color, textcolor, label, textsize);
    LcdTouch.drawButton(5, 120, 150, 85, 30, 10, WHITE, GREEN, "mid", 2); // NumButton, x, y, width, height, r, Color, textcolor, label, textsize);
    LcdTouch.drawButton(6, 220, 150, 85, 30, 10, WHITE, GREEN, "high", 2); // NumButton, x, y, width, height, r, Color, textcolor, label, textsize); 
    uint16_t x, y;
    while (wantedLight == -1 || wantedMoisture == -1)
    {
        if (LcdTouch.touched())
        {
            LcdTouch.readTouch();
            x = LcdTouch.xTouch;
            y = LcdTouch.yTouch;
            ButtonNum = LcdTouch.ButtonTouch(x, y);
            switch (ButtonNum)
            {
                case 1:
                    wantedLight = 0;
                    LcdTouch.drawButton(1, 20, 90, 85, 30, 10, GREEN, WHITE, "1-3", 2); 
                    break;
                case 2:
                    wantedLight = 1;
                    LcdTouch.drawButton(2, 120, 90, 85, 30, 10, GREEN, WHITE, "4-8", 2);
                    break;
                case 3:
                    wantedLight = 2;
                    LcdTouch.drawButton(3, 220, 90, 85, 30, 10, GREEN, WHITE, "9-12", 2);
                    break;
                case 4:
                    wantedMoisture = 0;
                    LcdTouch.drawButton(4, 20, 150, 85, 30, 10, GREEN, WHITE, "low", 2);
                    break;
                case 5:
                    wantedMoisture = 1;
                    LcdTouch.drawButton(5, 120, 150, 85, 30, 10, GREEN, WHITE, "mid", 2); 
                    break;
                case 6:
                    wantedMoisture = 2;
                    LcdTouch.drawButton(6, 220, 150, 85, 30, 10, GREEN, WHITE, "high", 2);  
                    break;
            }

        }
    }
    
    LcdTouch.drawButton(7, 120, 200, 100, 30, 10, WHITE, GREEN, "next", 2);
  
    do
    {
        if (LcdTouch.touched())
        {
            LcdTouch.readTouch();
            x = LcdTouch.xTouch;
            y = LcdTouch.yTouch;
            ButtonNum = LcdTouch.ButtonTouch(x, y);
        }
    } while (ButtonNum != 7);

    homeScreen();
}







