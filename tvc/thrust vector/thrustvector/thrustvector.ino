#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Servo.h>
#include <math.h>

Adafruit_MPU6050 mpu;

// Offset values for calibration
float offsetX = 0.0;
float offsetY = 0.0;
float offsetZ = 270.0;

Servo servoD7;  // Servo object for pin D5
Servo servoD6;  // Servo object for pin D6

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10);  // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
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

  // Calibrate accelerometer
  calibrateAccelerometer();

  // Attach servos to respective pins
  servoD7.attach(7);  // Pin 5
  servoD6.attach(6);  // Pin 6

  // Set servo initial positions
  servoD7.write(70);
  servoD6.write(60);

  Serial.println("");
  delay(1);
}

void loop() {
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Apply offset calibration */
  g.gyro.x -= offsetX;
  g.gyro.y -= offsetY;
  g.gyro.z -= offsetZ;

  /* Convert acceleration to degrees */
  float accelX_deg = atan2(a.acceleration.x, sqrt(pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2))) * (180.0 / M_PI);
  float accelY_deg = atan2(a.acceleration.y, sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.z, 2))) * (180.0 / M_PI);
  float accelZ_deg = atan2(sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2)), a.acceleration.z) * (180.0 / M_PI);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(" m/s^2, Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(" m/s^2, Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(" rad/s, Y: ");
  Serial.print(g.gyro.y);
  Serial.print(" rad/s, Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  Serial.print("Acceleration X: ");
  Serial.print(accelX_deg);
  Serial.print(" deg, Y: ");
  Serial.print(accelY_deg);
  Serial.print(" deg, Z: ");
  Serial.print(accelZ_deg);
  Serial.println(" deg");

  // Map the acceleration values to servo angles
  int servoAngleD7 = map(accelX_deg, -90, 90, 115, 175);  // Adjusted range for servo on pin 7
  int servoAngleD6 = map(accelZ_deg, 0, 180 , 10, 70);   // Adjusted range for servo on pin 6

  // Set servo positions
  servoD7.write(servoAngleD7);
  servoD6.write(servoAngleD6);

  Serial.print("Servo D7 angle: ");
  Serial.print(servoAngleD7);
  Serial.print(" Servo D6 angle: ");
  Serial.print(servoAngleD6);
  Serial.println();

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");

  delay(0);
}

void calibrateAccelerometer() {
  const uint16_t numSamples = 1000;  // Number of samples to collect for calibration

  float sumX = 0.0;
  float sumY = 0.0;
  float sumZ = 0.0;

  for (uint16_t i = 0; i < numSamples; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    sumX += a.acceleration.x;
    sumY += a.acceleration.y;
    sumZ += a.acceleration.z;

    delay(0);
  }

  offsetX = sumX / numSamples;
  offsetY = sumY / numSamples;
  offsetZ = sumZ / numSamples;
}
