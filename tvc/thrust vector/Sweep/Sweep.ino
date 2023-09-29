/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 https://www.arduino.cc/en/Tutorial/LibraryExamples/Sweep
*/

#include <Servo.h>

Servo myservoX;
Servo myservoY;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int posY = 10;
int posX = 10; 

void setup() {
  myservoY.attach(6);  // attaches the servo on pin 9 to the servo object
  myservoX.attach(5);  // attaches the servo on pin 9 to the servo object
  myservoX.write(posX);
  myservoX.write(posY);
  delay(1000);  
}

void loop() {
 for (posX = 30; posX <= 100; posX += 1) {  // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservoX.write(posX);  // tell servo to go to position in variable 'pos'
    delay(15);             // waits 15 ms for the servo to reach the position
  }
  for (posX = 100; posX >= 30; posX -= 1) {  // goes from 180 degrees to 0 degrees
    myservoX.write(posX);                 // tell servo to go to position in variable 'pos'
    delay(15);                            // waits 15 ms for the servo to reach the position
  }
   for (posY = 30; posY <= 100; posY += 1) {  // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservoY.write(posY);  // tell servo to go to position in variable 'pos'
    delay(15);             // waits 15 ms for the servo to reach the position
  }
  for (posY = 100; posY >= 30; posY -= 1) {  // goes from 180 degrees to 0 degrees
    myservoY.write(posY);                  // tell servo to go to position in variable 'pos'
    delay(15);                             // waits 15 ms for the servo to reach the position
  }
  
}
