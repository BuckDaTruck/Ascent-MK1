

#include <ESP32Servo.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
// On an esp32:       21(SDA), 22(SCL)
#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 32     // OLED display height, in pixels
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Servo myservoA;
Servo myservoB;
bool open = true;
int homea = 18;
int homeb = 9;
int time = 30;
int count;
bool cancel = false;
const int rocket = A3;
const int engine = 9;
const int buzzer = 14;
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(engine, OUTPUT);
  pinMode(rocket, OUTPUT);
  Serial.begin(115200);
  myservoA.attach(A0);
  myservoB.attach(A1);
  myservoA.write(homea);
  myservoB.write(homeb);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  display.clearDisplay();
  delay(1000);
  Serial.println("  Wiley Defense Industries");
  Serial.println("=============================");
  display.setTextSize(2);  // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("  Wiley"));
  display.setCursor(5, 16);
  display.println(F("Industries"));
  display.display();
  delay(1000);
  Serial.println("Ascent Launch Controller V1.0");
  Serial.println("=============================");
  delay(1000);
  Serial.println("Good luck with the launch!");
  Serial.println("=============================");
  delay(1000);
  Serial.println("Did you check all CONNECTIONS?");
  Serial.println("=============================");
  delay(1000);
  Serial.println("Type h for help!");
  Serial.println("=============================");
  delay(1000);
}
void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 'a') {  // set servo A position
      int A = Serial.parseInt();
      if (A != 0) {
        int posA = A + homea;
        myservoA.write(posA);
        Serial.println(posA);
      }
    } else if (command == 'b') {  // set servo B position
      int B = Serial.parseInt();
      if (B != 0) {
        int posB = B + homeb;
        myservoB.write(posB);
        Serial.println(posB);
      }
    } else if (command == 'l') {  // start launch sequence
      launch();
    } else if (command == 'h') {
      Serial.println("a followed by an integer will set servo A position.");
      Serial.println("b followed by an integer will set servo B position");
      Serial.println("t will toggle the clamps");
      Serial.println("l will start the launch sequence!");
      Serial.println("c will abort the launch");
    } else if (command == 't') { //toggle clamps
      if (open == true) {
        myservoA.write(90 + homea);
        myservoB.write(90 + homeb);
        open = false;
        display.clearDisplay();
        display.setTextSize(4.8);  // Draw 2X-scale text
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println("CLOSE");
        display.display();
        delay(2000);
        display.clearDisplay();
        display.setTextSize(2);  // Draw 2X-scale text
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(10, 0);
        display.println(F("  Wiley"));
        display.setCursor(5, 16);
        display.println(F("Industries"));
        display.display();
      } else if (open == false) {
        myservoA.write(homea);
        myservoB.write(homeb);
        open = true;
        display.clearDisplay();
        display.setTextSize(4.8);  // Draw 2X-scale text
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println("OPEN");
        display.display();
        delay(2000);
        display.clearDisplay();
        display.setTextSize(2);  // Draw 2X-scale text
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(10, 0);
        display.println(F("  Wiley"));
        display.setCursor(5, 16);
        display.println(F("Industries"));
        display.display();
      }
    }
  }
}
void launch() {
  if (open == true) {
    myservoA.write(90 + homea);
    myservoB.write(90 + homeb);
    open = false;
    display.clearDisplay();
    display.setTextSize(4.8);  // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("CLOSE");
    display.display();
    delay(2000);
    }
  display.clearDisplay();
    display.setTextSize(2);  // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(30, 0);
    display.println(F("Ascent"));
    display.setTextSize(1);
    display.setCursor(32, 18);
    display.println(F("Version 1.1"));
    display.display();
       delay(1000);
  myservoA.write(90 + homea);
  myservoB.write(90 + homeb);
  open = false;
  digitalWrite(rocket, HIGH);
  delay(500);
  digitalWrite(rocket, LOW);
  delay(250);
  for (count = 1; count <= time; count += 1) {
    int countdown = time - count + 1;
    display.clearDisplay();
    display.setTextSize(4.8);  // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(45, 0);
    display.println(countdown);
    display.display();
    Serial.print("Launching in ");
    Serial.print(countdown);
    Serial.println(" Seconds! ");
    digitalWrite(LED_BUILTIN, HIGH);
    tone(buzzer, 1000);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    noTone(buzzer);
    delay(500);
    cancellaunch();
  }
  if (cancel == false) {
    digitalWrite(engine, HIGH);
    Serial.println("Rocket Launched!");
    delay(400);
    digitalWrite(engine, LOW);
    delay(100);
    myservoA.write(homea);
    myservoB.write(homeb);
    open = true;
    display.clearDisplay();
    display.setTextSize(2.5);  // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 10);
    display.println("Launched");
    display.display();
    delay(10000);
    display.clearDisplay();
    display.setTextSize(2);  // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 0);
    display.println(F("  Wiley"));
    display.setCursor(5, 16);
    display.println(F("Industries"));
    display.display();
  }
  cancel = false;
}
void cancellaunch() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 'c') {  // cancel
      count = time + 1;
      cancel = true;
      digitalWrite(rocket, HIGH);
      delay(1000);
      digitalWrite(rocket, LOW);
      Serial.println("Launch Aborted!");
      display.clearDisplay();
      display.setTextSize(4.8);  // Draw 2X-scale text
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("Abort");
      display.display();
      delay(5000);
      display.clearDisplay();
      display.setTextSize(2);  // Draw 2X-scale text
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 0);
      display.println(F("  Wiley"));
      display.setCursor(5, 16);
      display.println(F("Industries"));
      display.display();
    }
  }
}