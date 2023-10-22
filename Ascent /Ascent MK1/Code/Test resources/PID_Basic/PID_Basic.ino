#include <Wire.h>
#include <MPU6050_tockn.h>
#include <Servo.h>

///////////////////////Inputs/outputs///////////////////////
Servo myservoX;
Servo myservoZ;
MPU6050 mpu6050(Wire);
///////////////////////////////////////////////////////

////////////////////////Variables///////////////////////
float offsetX = 85.0;
float offsetY = 0.0;
float offsetZ = 5.0;
const float gyroScaleFactor = 1.0;
const int servoXPin = 7;
const int servoZPin = 6;
float homeZ = 120;
float homeX = 143;
///////////////////////////////////////////////////////

///////////////////PID constants///////////////////////
float kpZ = 0.25;
float kiZ = 0.1;
float kdZ = 0.1;
float kpX = 0.25;
float kiX = 0.1;
float kdX = 0.1;
float distance_setpointZ = 21;
float distance_setpointX = 0;
float distance_previous_errorZ, distance_errorZ;
float distance_previous_errorX, distance_errorX;
float PID_pZ, PID_iZ, PID_dZ, PID_totalZ;
float PID_pX, PID_iX, PID_dX, PID_totalX;
///////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);

  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);

  myservoX.attach(servoXPin);
  myservoZ.attach(servoZPin);
}

void loop() {
  float gyroX, gyroY, gyroZ;
  mpu6050.update();
  gyroX = mpu6050.getAngleX() - offsetX;
  gyroY = mpu6050.getAngleY() - offsetY;
  gyroZ = -mpu6050.getAngleZ() - offsetZ;

  gyroX -= offsetX;
  gyroY -= offsetY;
  gyroZ -= offsetZ;

  gyroX /= gyroScaleFactor;
  gyroY /= gyroScaleFactor;
  gyroZ /= gyroScaleFactor;

  distance_errorZ = distance_setpointZ - gyroZ;
  PID_pZ = kpZ * distance_errorZ;

  float dist_differenceZ = distance_errorZ - distance_previous_errorZ;
  PID_dZ = kdZ * (dist_differenceZ / 0.05);

  if (-3 < distance_errorZ && distance_errorZ < 3) {
    PID_iZ += kiZ * distance_errorZ * 0.05;
  } else {
    PID_iZ = 0;
  }

  PID_totalZ = PID_pZ + PID_iZ + PID_dZ;
  PID_totalZ = map(PID_totalZ, -90, 90, -90, 90);

  float servoZPosition = homeZ + PID_totalZ;

  distance_errorX = distance_setpointX - gyroX;
  PID_pX = kpX * distance_errorX;

  float dist_differenceX = distance_errorX - distance_previous_errorX;
  PID_dX = kdX * (dist_differenceX / 0.05);

  if (-3 < distance_errorX && distance_errorX < 3) {
    PID_iX += kiX * distance_errorX * 0.05;
  } else {
    PID_iX = 0;
  }

  PID_totalX = PID_pX + PID_iX + PID_dX;
  PID_totalX = map(PID_totalX, -90, 90, -90, 90);

  float servoXPosition = homeX + PID_totalX;

  myservoZ.write(servoZPosition);
  myservoX.write(servoXPosition);

  Serial.print("GyroZ: ");
  Serial.print(gyroZ);
  Serial.print(" OutputZ: ");
  Serial.print(PID_totalZ);
  Serial.print(" ServoZ: ");
  Serial.print(servoZPosition);

  Serial.print(" | GyroX: ");
  Serial.print(gyroX);
  Serial.print(" OutputX: ");
  Serial.print(PID_totalX);
  Serial.print(" ServoX: ");
  Serial.println(servoXPosition);

  distance_previous_errorZ = distance_errorZ;
  distance_previous_errorX = distance_errorX;

  delay(50);
}
