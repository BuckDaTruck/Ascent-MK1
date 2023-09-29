#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Servo.h>
#include <MPU6050_tockn.h>
Adafruit_MPU6050 mpu;
const int lpin = 9;
const int buzzer = A3;
const int engine = 2;
bool show = false;
bool showf = false;
bool gimbal = true;
bool oneloop = false;
bool feedbacknoise = true;
bool para = false;
bool canceled = false;
int count = 0;
int time = 10;
int posP;
int posX;
int posY;
float temps;
float yaccel;
float AngleX;
float AngleY;
float AngleZ;
float SAngleX;
float SAngleY;
const float gyroScaleFactor = 1.0;
float offsetX = 80.0;
float offsetY = 0.0;
float offsetZ = 0.0;
float homep = 80;
float homex = 143;
float homey = 113;
Servo myservoX;
Servo myservoY;
Servo myservoP;
sensors_event_t temp;
int linput = 0;
MPU6050 mpu6050(Wire);
///////////////////PID constants///////////////////////
float kpZ = 0.25;
float kiZ = 0.1;
float kdZ = 0.1;
float kpX = 0.25;
float kiX = 0.1;
float kdX = 0.1;
float distance_setpointZ = 0;
float distance_setpointX = 0;
float distance_previous_errorZ, distance_errorZ;
float distance_previous_errorX, distance_errorX;
float PID_pZ, PID_iZ, PID_dZ, PID_totalZ;
float PID_pX, PID_iX, PID_dX, PID_totalX;
///////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  Serial.println();
  myservoP.attach(8);
  myservoX.attach(7);
  myservoY.attach(6);
  home();
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  mpustart();
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(lpin, INPUT);
  pinMode(engine, OUTPUT);
  pinMode(buzzer, OUTPUT);
  myservoY.write(homey);
  delay(500);
  myservoX.write(homex);
  myservoP.write(homep);
}

void loop() {
  linput = digitalRead(lpin);
  serialread();
  if (linput == HIGH) {
    launch();
  }

  if (show == true) {
    forshow();
  }
  if (showf == true) {
    showfancy();
  }
  if (gimbal == true) {
    gimbalmode();
  }
}
void gimbalmode() {
  float gyroX, gyroY, gyroZ;
  mpu6050.update();
  gyroX = mpu6050.getAngleX() - offsetX;  // Read gyro angle for X-axis and apply offset
  gyroY = mpu6050.getAngleY() - offsetY;
  gyroZ = mpu6050.getAngleZ() - offsetZ;

  gyroX /= -gyroScaleFactor;  // Scale the gyro readings
  gyroY /= gyroScaleFactor;
  gyroZ /= -gyroScaleFactor;

  distance_errorZ = distance_setpointZ - gyroZ;  // Calculate Z-axis error
  PID_pZ = kpZ * distance_errorZ;                // Proportional term for Z-axis error

  float dist_differenceZ = distance_errorZ - distance_previous_errorZ;
  PID_dZ = kdZ * (dist_differenceZ / 0.05);  // Derivative term for Z-axis error

  if (-3 < distance_errorZ && distance_errorZ < 3) {
    PID_iZ += kiZ * distance_errorZ * 0.05;  // Integral term for Z-axis error
  } else {
    PID_iZ = 0;  // Reset integral term if error is outside the desired range
  }

  PID_totalZ = PID_pZ + PID_iZ + PID_dZ;           // Total PID output for Z-axis
  PID_totalZ = map(PID_totalZ, -45, 45, -90, 90);  // Map PID output to servo position

  float servoZPosition = homey + PID_totalZ;  // Calculate final servo position for Z-axis

  distance_errorX = distance_setpointX - gyroX;  // Calculate X-axis error
  PID_pX = kpX * distance_errorX;                // Proportional term for X-axis error

  float dist_differenceX = distance_errorX - distance_previous_errorX;
  PID_dX = kdX * (dist_differenceX / 0.05);  // Derivative term for X-axis error

  if (-3 < distance_errorX && distance_errorX < 3) {
    PID_iX += kiX * distance_errorX * 0.05;  // Integral term for X-axis error
  } else {
    PID_iX = 0;  // Reset integral term if error is outside the desired range
  }

  PID_totalX = PID_pX + PID_iX + PID_dX;           // Total PID output for X-axis
  PID_totalX = map(PID_totalX, -45, 45, -90, 90);  // Map PID output to servo position

  float servoXPosition = homex + PID_totalX;  // Calculate final servo position for X-axis

  myservoY.write(servoZPosition);  // Set the servo position for Z-axis
  myservoX.write(servoXPosition);  // Set the servo position for X-axis

  distance_previous_errorZ = distance_errorZ;
  distance_previous_errorX = distance_errorX;
  data();
  delay(0);
}
void forshow() {
  int posX = homex - 30;
  int posY = homey - 30;
  for (posY = homey - 30; posY <= homey + 30; posY += 1) {
    myservoY.write(posY);  // tell servo to go to position in variable 'pos'
    delay(10);             // waits 15 ms for the servo to reach the position
    data();
  }
  for (posY = homey + 30; posY >= homey - 30; posY -= 1) {  // goes from 180 degrees to 0 degrees
    myservoY.write(posY);                                   // tell servo to go to position in variable 'pos'
    delay(10);                                              // waits 15 ms for the servo to reach the position
    data();
  }

  for (posX = homex - 30; posX <= homex + 30; posX += 1) {  // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservoX.write(posX);  // tell servo to go to position in variable 'pos'
    delay(10);             // waits 15 ms for the servo to reach the position
    data();
  }
  for (posX = homex + 30; posX >= homex - 30; posX -= 1) {  // goes from 180 degrees to 0 degrees
    myservoX.write(posX);                                   // tell servo to go to position in variable 'pos'
    delay(10);                                              // waits 15 ms for the servo to reach the position
    data();
  }
}
void showfancy() {
  int posX = 40;
  int posY = 30;
  for (posY = homey - 30; posY <= homey + 30; posY += 1) {  // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservoY.write(posY);  // tell servo to go to position in variable 'pos'
    delay(60);             // waits 15 ms for the servo to reach the position
    data();
  }
  for (posY = homey + 30; posY >= homey - 30; posY -= 1) {  // goes from 180 degrees to 0 degrees
    myservoY.write(posY);                                   // tell servo to go to position in variable 'pos'
    delay(60);                                              // waits 15 ms for the servo to reach the position
    data();
  }

  for (posX = homex - 30; posX <= homex + 30; posX += 1) {  // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservoX.write(posX);  // tell servo to go to position in variable 'pos'
    delay(60);             // waits 15 ms for the servo to reach the position
    data();
  }
  for (posX = homex + 30; posX >= homex - 30; posX -= 1) {  // goes from 180 degrees to 0 degrees
    myservoX.write(posX);                                   // tell servo to go to position in variable 'pos'
    delay(60);                                              // waits 15 ms for the servo to reach the position
    data();
  }
  myservoY.write(homey);
  for (posX = homex - 30; posX <= homex + 30; posX += 1) {  // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservoX.write(posX);  // tell servo to go to position in variable 'pos'
    delay(8);              // waits 15 ms for the servo to reach the position
    data();
  }
  for (posX = homex + 30; posX >= homex - 30; posX -= 1) {  // goes from 180 degrees to 0 degrees
    myservoX.write(posX);                                   // tell servo to go to position in variable 'pos'
    delay(8);                                               // waits 15 ms for the servo to reach the position
    data();
  }
  myservoX.write(homex);
  for (posY = homey - 30; posY <= homey + 30; posY += 1) {  // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservoY.write(posY);  // tell servo to go to position in variable 'pos'
    delay(8);              // waits 15 ms for the servo to reach the position
    data();
  }
  for (posY = homey + 30; posY >= homey - 30; posY -= 1) {  // goes from 180 degrees to 0 degrees
    myservoY.write(posY);                                   // tell servo to go to position in variable 'pos'
    delay(8);                                               // waits 15 ms for the servo to reach the position
    data();
  }
  for (posX = homex - 30; posX <= homex + 30; posX += 1) {  // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservoX.write(posX);  // tell servo to go to position in variable 'pos'
    delay(8);              // waits 15 ms for the servo to reach the position
    data();
  }
  for (posX = homex + 30; posX >= homex - 30; posX -= 1) {  // goes from 180 degrees to 0 degrees
    myservoX.write(posX);                                   // tell servo to go to position in variable 'pos'
    delay(8);                                               // waits 15 ms for the servo to reach the position
    data();
  }
}

void home() {
  myservoX.write(homex);
  myservoY.write(homey);
  myservoP.write(homep);
  Serial.println("Homing ");
  para = false;
  data();
}
void data() {
  mpu6050.update();
  AngleX = mpu6050.getAngleX() - offsetX;
  AngleY = mpu6050.getAngleY() - offsetY;
  AngleZ = mpu6050.getAngleZ() - offsetZ;
  AngleX *= 1;
  AngleY *= 1;
  AngleZ *= -1;
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  if (oneloop == true) {
    int servoXr = myservoX.read();
    int servoYr = myservoY.read();
    Serial.print("  X Position: ");
    Serial.print(servoXr - homex);
    Serial.print("  Y Position:  ");
    Serial.print(servoYr - homey);
    Serial.print("  Sequence Length:  ");
    Serial.print(time);
    Serial.print("  Temperature: ");
    Serial.print(temp.temperature);
    Serial.print(" deg C");
    Serial.print("  Upwards Acceleration:  ");
    Serial.print(a.acceleration.y);
    Serial.print("  Angle X: ");
    Serial.print(AngleX);
    Serial.print("  Angle Y:  ");
    Serial.print(AngleY);
    Serial.print("  AngleZ:  ");
    Serial.print(AngleZ);
    if (para == false) {
      Serial.print(" Parachute Shut");
    } else if (para == true) {
      Serial.print(" Parachute Open");
    }
    if (gimbal == true) {
      Serial.println(" GIMBAL MODE ON");
    }
    if (show == true) {
      Serial.println(" SHOW MODE ON");
    }
    if (showf == true) {
      Serial.println(" FANCY MODE ON");
    } else {
      Serial.println();
    }
    if (a.acceleration.y <= 1) {
      myservoP.write(180);
      para = true;
    }
  }
  oneloop = true;
}
void feedback() {
  if (feedbacknoise == true) {
    digitalWrite(LED_BUILTIN, HIGH);
    tone(buzzer, 200);
    delay(70);
    digitalWrite(LED_BUILTIN, LOW);
    noTone(buzzer);
    delay(70);
  }
}
void launch() {
  float distance_previous_errorZ, distance_errorZ;
  float distance_previous_errorX, distance_errorX;
  float PID_pZ, PID_iZ, PID_dZ, PID_totalZ;
  float PID_pX, PID_iX, PID_dX, PID_totalX;
  gimbal = false;
  mpu6050.calcGyroOffsets(true);
  Serial.println("Testing");
  myservoY.write(30 + homey);
  delay(500);
  myservoY.write(-30 + homey);
  delay(500);
  myservoX.write(30 + homex);
  delay(500);
  myservoX.write(-30 + homex);
  delay(500);
  myservoY.write(30 + homey);
  delay(500);
  myservoY.write(-30 + homey);
  delay(500);
  myservoX.write(30 + homex);
  delay(500);
  myservoY.write(30 + homey);
  delay(500);
  myservoY.write(-30 + homey);
  delay(500);
  home();
  delay(9400);
  cancel();
  if (canceled == false) {
    for (count = 1; count <= time; count += 1) {
      Serial.print("Launching in ");
      Serial.print(time - count + 1);
      Serial.println(" Seconds! ");
      digitalWrite(LED_BUILTIN, HIGH);
      tone(buzzer, 1000);
      delay(500);
      digitalWrite(LED_BUILTIN, LOW);
      noTone(buzzer);
      delay(500);
      cancel();
    }
  }
  /*
  Serial.println("Firing");
  digitalWrite(engine, HIGH);
  delay(1000);
  digitalWrite(engine, LOW);
  Serial.println("Launched!");
  for (int count = 0; count <= 5; count += 1) {
    digitalWrite(LED_BUILTIN, HIGH);
    tone(buzzer, 200);
    delay(70);
    digitalWrite(LED_BUILTIN, LOW);
    noTone(buzzer);
  }*/
  if (canceled == false) {
    Serial.println("Feady for Flight");
    gimbal = true;
    feedback();
    data();
  }
  canceled = false;
}
void cancel() {
  linput = digitalRead(lpin);
  if (linput == HIGH) {
    canceled = true;
    count = time + 1;
    Serial.println("Launch Aborted!");
    delay(1000);
  }
}

void serialread() {
  if (Serial.available() > 0) {

    char command = Serial.read();

    if (command == 'x') {  // set x position
      feedback();
      int x = Serial.parseInt();
      if (x != 0) {
        posX = x + homex;
        myservoX.write(posX);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
        data();
        if (show == true) {

          show = false;
          Serial.println("SHOW MODE OFF");
        }
        if (showf == true) {
          showf = false;
          Serial.println("FANCY MODE OFF");
        }
        if (gimbal == true) {
          gimbal = false;
          Serial.println("GIMBAL MODE OFF");
        }
      }
    } else if (command == 'y') {  // set y position
      feedback();
      int y = Serial.parseInt();
      if (y != 0) {
        posY = y + homey;
        myservoY.write(posY);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
        data();
        if (show == true) {
          show = false;
          Serial.println("SHOW MODE OFF");
        }
        if (showf == true) {
          showf = false;
          Serial.println("FANCY MODE OFF");
        }
        if (gimbal == true) {
          gimbal = false;
          Serial.println("GIMBAL MODE OFF");
        }
      }
    } else if (command == 'p') {  //open/close parachute
      feedback();
      if (para == false) {
        para = true;
        posP = homep + 90;
        myservoP.write(posP);
      } else if (para == true) {
        para = false;
        posP = homep;
        myservoP.write(posP);
      }
      digitalWrite(LED_BUILTIN, HIGH);
      delay(500);
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
      data();
      if (show == true) {
        show = false;
        Serial.println("SHOW MODE OFF");
      }
      if (showf == true) {
        showf = false;
        Serial.println("FANCY MODE OFF");
      }
      if (gimbal == true) {
        gimbal = false;
        Serial.println("GIMBAL MODE OFF");
      }
    } else if (command == 't') {  // set launch sequence
      feedback();
      int t = Serial.parseInt();
      if (t != 0) {
        time = t;
        Serial.print("Launch Sequence is  ");
        Serial.print(time);
        Serial.println(" Seconds long! ");
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
      }
    } else if (command == 'h') {  // home servos
      feedback();
      home();
    } else if (command == 's') {  // turn on display mode
      feedback();
      if (show == false) {
        Serial.println("SHOW MODE ON");
        show = true;
        showf = false;
        gimbal = false;
      } else {
        show = false;
        Serial.println("SHOW MODE OFF");
      }
    } else if (command == 'l') {  // start launch sequence
      show = false;
      showf = false;
      gimbal = true;
      feedback();
      launch();
    } else if (command == 'f') {  //turn on fancy display mode
      feedback();
      if (showf == false) {
        Serial.println("FANCY MODE ON");
        showf = true;
        show = false;
        gimbal = false;
      } else {
        showf = false;
        Serial.println("FANCY MODE OFF");
      }
    } else if (command == 'c') {  // callibrate mpu
      home();
      mpu6050.calcGyroOffsets(true);
      data();
    } else if (command == 'g') {  // turn on gimbal mode
      feedback();
      if (gimbal == false) {
        Serial.println("GIMBAL MODE ON");
        gimbal = true;
        show = false;
        showf = false;
      } else {
        gimbal = false;
        Serial.println("GIMBAL MODE OFF");
      }
    } else if (command == 'n') {  // turn on noise
      feedback();
      if (feedbacknoise == false) {
        Serial.println("NOISE ON");
        feedbacknoise = true;
        feedback();
      } else {
        feedbacknoise = false;
        Serial.println("NOISE OFF");
      }
    }
  }
}
void mpustart() {
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
      Serial.println("+- 250 deg/s");
      break;
    case MPU6050_RANGE_500_DEG:
      Serial.println("+- 500 deg/s");
      break;
    case MPU6050_RANGE_1000_DEG:
      Serial.println("+- 1000 deg/s");
      break;
    case MPU6050_RANGE_2000_DEG:
      Serial.println("+- 2000 deg/s");
      break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
      Serial.println("260 Hz");
      break;
    case MPU6050_BAND_184_HZ:
      Serial.println("184 Hz");
      break;
    case MPU6050_BAND_94_HZ:
      Serial.println("94 Hz");
      break;
    case MPU6050_BAND_44_HZ:
      Serial.println("44 Hz");
      break;
    case MPU6050_BAND_21_HZ:
      Serial.println("21 Hz");
      break;
    case MPU6050_BAND_10_HZ:
      Serial.println("10 Hz");
      break;
    case MPU6050_BAND_5_HZ:
      Serial.println("5 Hz");
      break;
  }
}
