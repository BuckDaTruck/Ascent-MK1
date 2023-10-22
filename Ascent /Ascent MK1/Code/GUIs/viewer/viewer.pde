import processing.serial.*;
import java.util.HashMap;
import java.util.ArrayList;

Serial serialPort;
HashMap<String, ArrayList<Float>> dataPoints = new HashMap<String, ArrayList<Float>>();
int baudRate = 115200;

void setup() {
  size(1200, 1000);
  smooth();
  frameRate(60);

  // Modify the port name based on your Arduino's serial port
  serialPort = new Serial(this, "/dev/cu.usbmodem101", baudRate);

  // Initialize data arrays for each type of data
  dataPoints.put("X Position", new ArrayList<Float>());
  dataPoints.put("Y Position", new ArrayList<Float>());
  dataPoints.put("Sequence Length", new ArrayList<Float>());
  dataPoints.put("Temperature", new ArrayList<Float>());
  dataPoints.put("Upwards Acceleration", new ArrayList<Float>());
  dataPoints.put("Angle X", new ArrayList<Float>());
  dataPoints.put("Angle Y", new ArrayList<Float>());
  dataPoints.put("Angle Z", new ArrayList<Float>());
}

void draw() {
  background(255);
  drawGraphs();

  while (serialPort.available() > 0) {
    String data = serialPort.readStringUntil('\n');

    if (data != null) {
      data = data.trim();

      // Split the data into individual values separated by spaces
      String[] values = data.split("  ");

      // Process each value
      for (String value : values) {
        // Split the value into data type and value
        String[] parts = value.split(": ");

        if (parts.length == 2) {
          String dataType = parts[0].trim();
          String dataValue = parts[1].trim();

          // Check if the data value is a valid floating-point number
          if (dataValue.matches("-?\\d+(\\.\\d+)?")) {
            float floatValue = Float.parseFloat(dataValue);
            displayData(dataType, floatValue);
          }
        }
      }
    }
  }
}

void displayData(String dataType, float value) {
  // Print the data in the console
  println(dataType + ": " + value);

  // Add the data point to the corresponding data array
  if (dataPoints.containsKey(dataType)) {
    ArrayList<Float> points = dataPoints.get(dataType);
    points.add(value);

    // Limit the number of data points to fit within the graph
    if (points.size() > width) {
      points.remove(0);
    }
  }
}

void drawGraphs() {
  // Draw the graph axes
  stroke(0);
  line(0, height / 2, width, height / 2); // X-axis
  line(width / 2, 0, width / 2, height); // Y-axis

  // Set the line colors for each data type
  color[] lineColors = {
    color(255, 0, 0),   // Red for Angle X
    color(0, 255, 0),   // Green for Angle Y
    color(0, 0, 255),   // Blue for Angle Z
    color(255, 255, 0), // Yellow for Y Position
    color(255),         // White for X Position
    color(255, 0, 255), // Magenta for Sequence Length
    color(0, 255, 255), // Cyan for Temperature
    color(128)          // Gray for Upwards Acceleration
  };

  // Draw the graphs for each type of data
  int index = 0;
  for (String dataType : dataPoints.keySet()) {
    ArrayList<Float> points = dataPoints.get(dataType);

    // Set the line color based on the data type
    stroke(lineColors[index % lineColors.length]);

    // Draw the graph line
    noFill();
    beginShape();
    for (int i = 0; i < points.size(); i++) {
      float x = map(i, 0, points.size() - 1, 0, width);
      float y;
      if (dataType.equals("Angle Z")) {
        y = map(points.get(i), -180, 180, height, 0); // Adjust the range for z angle
      } else {
        y = map(points.get(i), -40, 40, height, 0);
      }
      vertex(x, y);
    }
    endShape();

    index++;
  }
}



void serialEvent(Serial port) {
  // Empty serialEvent to prevent buffer overflow
}
