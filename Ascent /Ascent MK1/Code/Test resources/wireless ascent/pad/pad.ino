#include <WiFi.h>
#include <Servo.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
const char* ssid = "Ascent";
const char* password = "AscentB1";
const char* serverIP = "192.168.4.1";
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
const int rocket = 27;
const int engine = 34;
const int buzzer = 4;
WiFiClient client;

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(engine, OUTPUT);
  pinMode(rocket, OUTPUT);
  Serial.begin(115200);
  myservoA.attach(12);
  myservoB.attach(14);
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
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  display.clearDisplay();
  display.setTextSize(2);  // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  Serial.println("Connected to WiFi");
  display.setCursor(5, 16);
  display.println(F("Connected"));
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setCursor(10, 0);
  display.println(F("  Wiley"));
  display.setCursor(5, 16);
  display.println(F("Industries"));
  display.display();
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
  if (client.connect(serverIP, 80)) {
    Serial.println("Connected to server");

    // Send HTTP GET request to turn the LED on
    client.println("GET /H");
    Serial.println("on");
    delay(1000);  // Wait for server response

    // Clear any data received from the server
    while (client.available()) {
      char c = client.read();
      Serial.print(c);
    }

    // Send HTTP GET request to turn the LED off
    client.println("GET /L");
    Serial.println("off");

    delay(1000);  // Wait for server response
    while (client.available()) {
      char c = client.read();
      Serial.print(c);
    }


    // Close the connection
    client.stop();
  } else {
    Serial.println("Connection failed");
  }
}