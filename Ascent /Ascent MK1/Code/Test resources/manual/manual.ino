#include <Servo.h>

int posX = 70;  //home is 70
int posY = 60;  //home is 60
Servo myservoX;
Servo myservoY;

void setup() {
  Serial.begin(115200);
  myservoX.attach(5);  // attaches the servo on pin A5 to the servo object
  myservoY.attach(6);  // attaches the servo on pin A4 to the servo object
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    // read the incoming byte:
    char command = Serial.read();

    if (command == 'x') {
      int x = Serial.parseInt();
      if (x != 0) {
        posX = x;
      }
    } else if (command == 'y') {
      int y = Serial.parseInt();
      if (y != 0) {
        posY = y;
      }
       } else if (command == 'h') {
            home();
    }
  }
    } else if (command == 'l') {
            launch();
    }
  }

  myservoX.write(posX);
  myservoY.write(posY);

  int servoXr = myservoX.read();
  int servoYr = myservoY.read();

  Serial.print("Servo X position: ");
  Serial.print(servoXr);
  Serial.print(", Servo Y position: ");
  Serial.println(servoYr);
}
void home(){
    myservoX.write(70);
  myservoY.write(60);
}
void launch(){
  for (posX = 0; posX <= 100; posX += 1){
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
      delay(50);                       // wait for a second
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
      delay(50);                       // wait for a second
    
  }
}
