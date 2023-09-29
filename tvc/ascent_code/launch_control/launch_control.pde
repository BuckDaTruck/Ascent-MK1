import processing.serial.*;

Serial arduino;
boolean open = true;
boolean launchStarted = false;
int countdown;
int countdownTimer;
int launchDuration = 30;  // Set the launch duration in seconds

void setup() {
  size(1920, 1080);
  arduino = new Serial(this, "/dev/cu.usbmodem101", 115200);
  countdown = launchDuration;
  countdownTimer = millis();
}

void draw() {
  background(28);
  textSize(100);

  // Display the status of the clamps
  if (open) {
    fill(0, 255, 0);
    text("Clamps: OPEN", 50, height/5);
  } else {
    fill(255, 0, 0);
    text("Clamps: CLOSED", 50, height/5*2);
  }

  // Display the launch status
  fill(230);
  text("Launch Status:", 50, 100);

  if (launchStarted) {
    fill(255, 0, 0);
    text("Countdown: " + countdown, 700, 100);
    if (millis() - countdownTimer >= 1000) {
      countdown--;
      countdownTimer = millis();
    }
    if (countdown < 1) {
      launchStarted = false;
      countdown = launchDuration;
      open = true;
      text("In Flight", 400, 100);
      delay(3000);
    }
  } else {
    fill(230);
    text("Ready", 700, 100);
  }
}

void keyPressed() {
  if (key == 't') {
    toggleClamps();
  } else if (key == 'l') {
    startLaunchSequence();
  } else if (key == 'c') {
    cancelLaunch();
  }
}

void toggleClamps() {
  arduino.write('t');
  open = !open;
}

void startLaunchSequence() {
  open = false;
  arduino.write('l');
  if (open) {
    delay(4400);
  } else {
    delay(2400);
  }
  launchStarted = true;
  countdownTimer = millis();
}

void cancelLaunch() {
  arduino.write('c');
  launchStarted = false;
  countdown = launchDuration;
}
