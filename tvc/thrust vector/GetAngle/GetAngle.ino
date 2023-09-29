
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <Servo.h>
Servo myservoX;
Servo myservoY;
int homex = 145;
int homey = 55;
float AngleX;
float AngleY;
float AngleZ;
float SAngleX;
float SAngleY;
float offsetX = 90.0;
float offsetY = 0.0;
float offsetZ = 0.0;
MPU6050 mpu6050(Wire);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  myservoX.attach(7);
  myservoY.attach(6);
  myservoX.write(homex);
  myservoY.write(homey);
}

void loop() {
  mpu6050.update();
  AngleX = mpu6050.getAngleX() - offsetX;
  AngleY = mpu6050.getAngleY() - offsetY;
  AngleZ = mpu6050.getAngleZ() - offsetZ;
  AngleX *= 1;
  AngleY *= 1;
  AngleZ *= 1;
  Serial.print("angleX : ");
  Serial.print(AngleX);
  Serial.print("\tangleY : ");
  Serial.print(AngleY);
  Serial.print("\tangleZ : ");
  Serial.println(AngleZ);
  float servoAngleD7 = map(AngleX, -30, 30, homex-30, homex + 30);          // Adjusted range for servo on pin 7
  float servoAngleD6 = map(AngleZ, 30, -30, homey + 30, homey - 30);  // Adjusted range for servo on pin 6

  // Set servo positions
  myservoX.write(servoAngleD7);
  myservoY.write(servoAngleD6);
}
