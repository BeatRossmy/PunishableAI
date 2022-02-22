#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <elapsedMillis.h>

#define SERVOMIN 120
#define SERVOMAX 550

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

byte button = A0; // on/off button
bool lastButtonState;
bool currentButtonState;
bool on;

int lightIn = A1; // light pin and value for light punishment
int lightValue;

int jiggleCounter = 0;

// mid,upR,upRB,upL,upLB,fwdR,fwdL,bwdR,bwdL
int movement[3][9] = {
  {90,110,70,70,110,125,60,65,120}, // straight
  {90,110,70,70,110,130,60,70,120}, // left
  {90,110,70,70,110,120,60,60,120}  // right
};

int currentMovement = 0;

struct VECTOR {
  int a, b;
};

struct LEG {
  int sPin;
  int rPin;
  int switchPin;
  int touchPin;
  VECTOR pos;
  VECTOR target;
  bool switchState;
  bool touchState;

  void move () {
    if (target.a != pos.a) {
      pos.a += ((target.a > pos.a) ? 1 : (-1));
      pwm.setPWM(sPin, 0, map(pos.a, 0, 180, SERVOMIN, SERVOMAX));
    }
    if (target.b != pos.b) {
      pos.b += ((target.b > pos.b) ? 1 : (-1));
      pwm.setPWM(rPin, 0, map(pos.b, 0, 180, SERVOMIN, SERVOMAX));
    }
  }

  void setTarget (VECTOR t) {
    target = t;
  }
};

LEG a, b, c, d, e, f;

elapsedMillis t;

int phase;

void setup() {
  Serial.begin(9600);

  pwm.begin();
  pwm.setPWMFreq(50);

  stand();
  delay(1000); // WICHTIG ANSONSTEN BEWEGUNGEN NICHT ABGESCHLOSSEN

  a = {0, 1, 2, 8, {90, 90}, {90, 90}};
  b = {2, 3, 3, 9, {90, 90}, {90, 90}};
  c = {4, 5, 4, 10, {90, 90}, {90, 90}};
  d = {6, 7, 5, 11, {90, 90}, {90, 90}};
  e = {8, 9, 6, 12, {90, 90}, {90, 90}};
  f = {10, 11, 7, 13, {90, 90}, {90, 90}};

  pinMode(button, INPUT_PULLUP);

  for (int i = 2; i < 14; i++) {
    pinMode(i, INPUT_PULLUP);
  }
}

void loop() {
  a.move();
  b.move();
  c.move();
  d.move();
  e.move();
  f.move();
  delay(10); // ZEIT ZUM BEWEGEN DES SERVOS


  lastButtonState = currentButtonState;
  currentButtonState = digitalRead(button);

  if (currentButtonState != lastButtonState && currentButtonState == LOW) {
    on = !on;
    currentMovement = random(0,3); // -> 0-2
  }

  if (on == false) {
    stand();

    for (int i = 8; i < 14; i++) {
      if (digitalRead(i) == HIGH) {
        Serial.println("TOUCH DETECTED");
        jiggleCounter = 100;
      }
    }

    int lightValue = analogRead(lightIn);
    Serial.println(lightValue);
    if (lightValue <= 400) {
      jiggleCounter = 100;
    }

    if (jiggleCounter > 0) {
      jiggleCounter--;
      jiggle();
    }

    return;
  }


  if (t > 500) {
    t = 0;

    if (phase == 0) {
      a.setTarget({movement[currentMovement][6], movement[currentMovement][3]});
      e.setTarget({movement[currentMovement][5], movement[currentMovement][1]});

      b.setTarget({movement[currentMovement][8], movement[currentMovement][0]});
      f.setTarget({movement[currentMovement][7], movement[currentMovement][0]});

      c.setTarget({movement[currentMovement][0], movement[currentMovement][0]});
      d.setTarget({movement[currentMovement][0], movement[currentMovement][0]});

    }
    if (phase == 1) {
      a.setTarget({movement[currentMovement][0], movement[currentMovement][0]});
      e.setTarget({movement[currentMovement][0], movement[currentMovement][0]});

      b.setTarget({movement[currentMovement][6], movement[currentMovement][3]});
      f.setTarget({movement[currentMovement][4], movement[currentMovement][2]});

      c.setTarget({movement[currentMovement][8], movement[currentMovement][0]});
      d.setTarget({movement[currentMovement][7], movement[currentMovement][0]});


    }
    if (phase == 2) {
      a.setTarget({movement[currentMovement][8], movement[currentMovement][0]});
      e.setTarget({movement[currentMovement][7], movement[currentMovement][0]});

      b.setTarget({movement[currentMovement][0], movement[currentMovement][0]});
      f.setTarget({movement[currentMovement][0], movement[currentMovement][0]});

      c.setTarget({movement[currentMovement][6], movement[currentMovement][4]});
      d.setTarget({movement[currentMovement][5], movement[currentMovement][1]});

    }

    phase = (phase + 1) % 3;

  }


}

void stand() {
  a.setTarget({movement[currentMovement][0], movement[currentMovement][0]});
  b.setTarget({movement[currentMovement][0], movement[currentMovement][0]});
  c.setTarget({movement[currentMovement][0], movement[currentMovement][0]});
  d.setTarget({movement[currentMovement][0], movement[currentMovement][0]});
  e.setTarget({movement[currentMovement][0], movement[currentMovement][0]});
  f.setTarget({movement[currentMovement][0], movement[currentMovement][0]});
}

void jiggle() {
  a.setTarget({(int)random(80, 100), (int)random(80, 100)});
  b.setTarget({(int)random(80, 100), (int)random(80, 100)});
  c.setTarget({(int)random(80, 100), (int)random(80, 100)});
  d.setTarget({(int)random(80, 100), (int)random(80, 100)});
  e.setTarget({(int)random(80, 100), (int)random(80, 100)});
  f.setTarget({(int)random(80, 100), (int)random(80, 100)});
}
