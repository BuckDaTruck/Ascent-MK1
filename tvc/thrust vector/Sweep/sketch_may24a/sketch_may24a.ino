/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 https://www.arduino.cc/en/Tutorial/LibraryExamples/Sweep
*/

#include <Servo.h>


int posX = 40;
int posY = 30;  
Servo myservoX; 
Servo myservoY; 

void setup() {
    myservoX.attach(A5);  // attaches the servo on pin 9 to the servo object
  myservoY.attach(A4);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  for (posY = 30; posY <= 90; posY += 1) {  // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservoY.write(posY);  // tell servo to go to position in variable 'pos'
    delay(15);             // waits 15 ms for the servo to reach the position
  }
  for (posY = 90; posY >= 30; posY -= 1) {  // goes from 180 degrees to 0 degrees
    myservoY.write(posY);                   // tell servo to go to position in variable 'pos'
    delay(15);                              // waits 15 ms for the servo to reach the position
  }

    for (posX = 40; posX <= 100; posX += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservoX.write(posX);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
  for (posX = 100; posX >= 40; posX -= 1) { // goes from 180 degrees to 0 degrees
    myservoX.write(posX);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
}
