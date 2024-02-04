/*
Moves two servos randomly between fixed angles

Used for a laser turret working as a cat toy

NOTE: If you build a laser turret, make sure to diffract/dimmen the laser
enough to make it not dangerous

To make this work, connect

Servo0_power -> 5V
Servo1_power -> 5V
Servo0_GND -> GND
Servo1_GND -> GND
Servo0_signal -> A5
Servo1_signal -> A4

Additionally, to power the laser, I connected

Laser_power -> 5V
Laser_GND -> GND
*/

#include <Servo.h>

Servo servo0;
Servo servo1;

int servoPin0 = A5;
int servoPin1 = A4;

int time = 0;
float speed0 = 0;
float speed1 = 0;

const int servo0Min = 30, servo0Max = 55;
const int servo1Min = 110, servo1Max = 180;
float currentAngle0 = 20;
float currentAngle1 = 180;

void setup() {
  servo0.attach(servoPin0);
  servo1.attach(servoPin1);
  servo0.write(currentAngle0);
  servo1.write(currentAngle1);
}

void loop() {

  delay(10);
  
  if (time==0) {
    int r0 = random(servo0Min, servo0Max);
    int r1 = random(servo1Min, servo1Max);
    time = random(100, 160);
    speed0 = (r0 - currentAngle0)/(time/2);
    speed1 = (r1 - currentAngle1)/(time/2);
  }

  // Move only one motor at time, otherwise problems arise, 
  // I suspect not enough current for both motors
  if (time%2 == 0) {
    currentAngle0 += speed0;
    servo0.write(currentAngle0);
  } else {
    currentAngle1 += speed1;
    servo1.write(currentAngle1);
  }
  time -= 1;

}

