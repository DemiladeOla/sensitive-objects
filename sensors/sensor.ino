#define echoPin 7
#define trigPin 8
#define lightPin A0
#define motionPin 2

long duration;
long distance;
long previousDistance = 0;
int lightValue;
int personDetected;
boolean isDark;
boolean restingState;
boolean dayState;
boolean nightState;
boolean sleepState;
boolean release;
int blinkRate = 1000;
boolean ledState = false;
long lastTimeYouBlinked = 0;
int blueLedsPins[] = {11, 13};
int whiteLedPin = 12;
int ledTotal = sizeof(blueLedsPins) / sizeof(int);
int releaseInt = 0;


int setState(int state) {
  restingState = (state == 1) ? true : false;
  dayState = (state == 2) ? true : false;
  nightState = (state == 3) ? true : false;
  sleepState = (state == 4) ? true : false;
}

boolean getDarkness(int value) {
  if (value < 800) {
    return true;
  } else {
    return false;
  }
}
void setup() {
  Serial.begin(9600);
  pinMode(motionPin, INPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  for (int i = 0; i < ledTotal; i++) {
    pinMode(blueLedsPins[i], OUTPUT);
  }
  pinMode(whiteLedPin, OUTPUT);
}
void loop() {
  lightValue = analogRead(lightPin);
  isDark = getDarkness(lightValue);
  personDetected = digitalRead(motionPin);

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(100);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  //  Convert to cm
  distance = duration / 58.2;
  Serial.print("distance:");
  Serial.print(distance);
  
  Serial.print(":");
  
  Serial.print("State:");

  bool ignore = ((distance - previousDistance) > 120) ? true : false;
  if(ignore) {
    releaseInt++;
    if(!((distance > previousDistance) && (releaseInt > 500))) {
      distance = ((distance - previousDistance) > 100) ? previousDistance : distance;
    }
  } else {
    releaseInt = 0;
  }
  int st = (distance < 120) && isDark ? 4 : (distance < 100) && !isDark ? 3 : personDetected ? 2 : 1;
  setState(st);
  Serial.println(st);

  if(dayState) {
    // Blink blue lights
    blinkRate = 100;
    for(int i = 0; i < ledTotal; i++) {
        if (millis() - lastTimeYouBlinked >= blinkRate) {
            ledState = !ledState;
            lastTimeYouBlinked = millis();
        }
        digitalWrite(blueLedsPins[i], ledState);
    }
    digitalWrite(whiteLedPin, LOW);
  } else if(restingState) {
    // Display blue light
    for(int i = 0; i < ledTotal; i++) {
      digitalWrite(blueLedsPins[i], HIGH);
    }
      digitalWrite(whiteLedPin, LOW);
  } else {
      for(int i = 0; i < ledTotal; i++) {
          digitalWrite(blueLedsPins[i], LOW);
      }
      digitalWrite(whiteLedPin, HIGH);
  }
  previousDistance = distance;
}
