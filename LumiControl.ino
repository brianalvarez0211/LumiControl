#include <LiquidCrystal.h>  // Library for controlling the LCD1602 display

// Pin assignments for the sensor inputs and LED output
const int LDR_PIN = A0;  // Reads the light level from the LDR
const int POT_PIN = A1;  // Reads the desired light level from the potentiometer
const int LED_PIN = 9;   // Controls the LED brightness using PWM

// The control program is intended to run every 50 milliseconds
const unsigned long CONTROL_PERIOD_MS = 50;

// Proportional gain used to calculate the LED output from the error
float Kp = 0.8;

// LCD pin connections: RS, E, D4, D5, D6, and D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Stores the time when the previous control cycle was scheduled
unsigned long lastControlTime = 0;

void setup() {
  // Set the LED pin as an output
  pinMode(LED_PIN, OUTPUT);

  // Start serial communication for recording test data
  Serial.begin(115200);

  // Initialize the LCD and display a startup message
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LumiControl");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  // Print the column names for the serial data log
  Serial.println("Time_ms,Setpoint,Light,Error,PWM,Execution_us");

  // Give the system time to show the startup message
  delay(1000);
  lcd.clear();
}

void loop() {
  // Get the current time since the Arduino started
  unsigned long currentTime = millis();

  // Run the control section when the intended 50 ms period has elapsed
  if (currentTime - lastControlTime >= CONTROL_PERIOD_MS) {
    // Advance the scheduled time by one control period
    lastControlTime += CONTROL_PERIOD_MS;

    // Start measuring the execution time of the control section
    unsigned long startTime = micros();

    // Read the desired light level and the actual sensor reading
    int setpoint = analogRead(POT_PIN);
    int processVariable = analogRead(LDR_PIN);

    // Calculate the difference between the desired and measured light levels
    // A positive error means the measured reading is below the setpoint
    int error = setpoint - processVariable;

    // Apply proportional control to calculate the LED output
    float controlOutput = Kp * error;

    // Limit the output to the valid PWM range of 0 to 255
    controlOutput = constrain(controlOutput, 0, 255);

    // Convert the calculated output to an integer PWM value
    int pwmOutput = (int)controlOutput;

    // Update the LED brightness based on the calculated PWM output
    analogWrite(LED_PIN, pwmOutput);

    // Measure the execution time up to the PWM update
    // Serial printing and LCD updates are not included in this measurement
    unsigned long executionTime = micros() - startTime;

    // Send the current readings and control values to the Serial Monitor
    // Each value is separated by a comma so the data can be saved as CSV
    Serial.print(millis());
    Serial.print(",");
    Serial.print(setpoint);
    Serial.print(",");
    Serial.print(processVariable);
    Serial.print(",");
    Serial.print(error);
    Serial.print(",");
    Serial.print(pwmOutput);
    Serial.print(",");
    Serial.println(executionTime);

    // Display the setpoint and measured light reading on the first LCD row
    lcd.setCursor(0, 0);
    lcd.print("SP:");
    lcd.print(setpoint);
    lcd.print(" PV:");
    lcd.print(processVariable);
    lcd.print("   ");  // Clear leftover characters from the previous reading

    // Display the PWM output and measured execution time on the second row
    lcd.setCursor(0, 1);
    lcd.print("PWM:");
    lcd.print(pwmOutput);
    lcd.print(" E:");
    lcd.print(executionTime);
    lcd.print("   ");  // Clear leftover characters from the previous reading
  }
}
```