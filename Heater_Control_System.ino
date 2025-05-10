#include <OneWire.h>
#include <DallasTemperature.h>

// Pin Configurations
#define ONE_WIRE_BUS 2       // DS18B20 connected to pin D2
#define HEATER_LED 3         // Simulated heater (red LED)

// State Indicator LEDs
#define IDLE_LED 4
#define HEATING_LED 5
#define STABILIZING_LED 6
#define TARGET_LED 7
#define OVERHEAT_LED 8

// Temperature thresholds
#define HEATING_THRESHOLD 30
#define STABILIZING_MIN 70
#define STABILIZING_MAX 90
#define TARGET_MIN 90
#define TARGET_MAX 100
#define OVERHEAT_THRESHOLD 115

// Setup DS18B20 sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Define system states
enum State {
  IDLE,
  HEATING,
  STABILIZING,
  TARGET_REACHED,
  OVERHEAT
};
State currentState = IDLE;
void setup() {
  Serial.begin(9600);
  sensors.begin();
  // Configure pins
  pinMode(HEATER_LED, OUTPUT);
  pinMode(IDLE_LED, OUTPUT);
  pinMode(HEATING_LED, OUTPUT);
  pinMode(STABILIZING_LED, OUTPUT);
  pinMode(TARGET_LED, OUTPUT);
  pinMode(OVERHEAT_LED, OUTPUT);
}
void loop() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  // Determine state
  if (tempC > OVERHEAT_THRESHOLD) {
    currentState = OVERHEAT;
  } else if (tempC >= TARGET_MIN && tempC <= TARGET_MAX) {
    currentState = TARGET_REACHED;
  } else if (tempC > STABILIZING_MIN && tempC < STABILIZING_MAX) {
    currentState = STABILIZING;
  } else if (tempC < HEATING_THRESHOLD) {
    currentState = HEATING;
  } else {
    currentState = IDLE;
  }
  // Control outputs based on state
  updateOutputs(tempC);
  delay(1000); // Wait 1 second before next read
}
void updateOutputs(float temperature) {
  // Turn off all state LEDs
  digitalWrite(IDLE_LED, LOW);
  digitalWrite(HEATING_LED, LOW);
  digitalWrite(STABILIZING_LED, LOW);
  digitalWrite(TARGET_LED, LOW);
  digitalWrite(OVERHEAT_LED, LOW);
  digitalWrite(HEATER_LED, LOW);

  switch (currentState) {
    case IDLE:
      digitalWrite(IDLE_LED, HIGH);
      break;
    case HEATING:
      digitalWrite(HEATING_LED, HIGH);
      digitalWrite(HEATER_LED, HIGH); // Heater ON
      break;
    case STABILIZING:
      digitalWrite(STABILIZING_LED, HIGH);
      digitalWrite(HEATER_LED, HIGH); // Gentle heating
      break;
    case TARGET_REACHED:
      digitalWrite(TARGET_LED, HIGH);
      // Heater OFF to maintain
      break;
    case OVERHEAT:
      digitalWrite(OVERHEAT_LED, HIGH);
      // Safety: Heater remains OFF
      break;
  }

  // Serial output
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C | State: ");
  printState(currentState);
}
void printState(State s) {
  switch (s) {
    case IDLE: Serial.println("IDLE"); break;
    case HEATING: Serial.println("HEATING"); break;
    case STABILIZING: Serial.println("STABILIZING"); break;
    case TARGET_REACHED: Serial.println("TARGET_REACHED"); break;
    case OVERHEAT: Serial.println("OVERHEAT"); break;
  }
}
