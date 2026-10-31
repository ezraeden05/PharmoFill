#include <Wire.h>
#include <Adafruit_AS7341.h>
#include <Stepper.h>

#define TCA_ADDR 0x70
#define stepsPerRevolution 2048
#define blockedSegments 6
#define stepsPerSegment (stepsPerRevolution / blockedSegments)

Stepper motor0(stepsPerRevolution, 22, 24, 26, 28);  // Sensor SD0/SC0
Stepper motor5(stepsPerRevolution, 8, 9, 10, 11);    // Sensor SD5/SC5
Stepper motor6(stepsPerRevolution, 3, 4, 5, 6);      // Sensor SD6/SC6

Adafruit_AS7341 sensor;

const int activeChannels[] = {0, 5, 6};
const int numChannels = sizeof(activeChannels) / sizeof(activeChannels[0]);

String knownPills[8] = {"", "", "", "", "", "", "", ""};  // Per channel

void tcaSelect(uint8_t channel) {
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << channel);
  Wire.endTransmission();
}

void smoothStep(Stepper& motor, int steps) {
  for (int i = 0; i < steps; i++) {
    motor.step(1);
    delay(5);
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  motor0.setSpeed(10);
  motor5.setSpeed(10);
  motor6.setSpeed(10);
  Serial.println("PharmoFill system initialized.");
}

void loop() {
  // === SENSOR READING PHASE ===
  for (int i = 0; i < numChannels; i++) {
    int ch = activeChannels[i];
    tcaSelect(ch);
    delay(100);
    sensor = Adafruit_AS7341();
    if (!sensor.begin()) {
      Serial.print("Sensor not found on channel ");
      Serial.println(ch);
      continue;
    }
    if (!sensor.readAllChannels()) {
      Serial.print("Sensor read failed on channel ");
      Serial.println(ch);
      continue;
    }
    Serial.print("SENSOR:");
    Serial.print(sensor.getChannel(AS7341_CHANNEL_415nm_F1)); Serial.print(",");
    Serial.print(sensor.getChannel(AS7341_CHANNEL_445nm_F2)); Serial.print(",");
    Serial.print(sensor.getChannel(AS7341_CHANNEL_480nm_F3)); Serial.print(",");
    Serial.print(sensor.getChannel(AS7341_CHANNEL_515nm_F4)); Serial.print(",");
    Serial.print(sensor.getChannel(AS7341_CHANNEL_555nm_F5)); Serial.print(",");
    Serial.print(sensor.getChannel(AS7341_CHANNEL_590nm_F6)); Serial.print(",");
    Serial.print(sensor.getChannel(AS7341_CHANNEL_630nm_F7)); Serial.print(",");
    Serial.print(sensor.getChannel(AS7341_CHANNEL_680nm_F8)); Serial.print(",");
    Serial.print(sensor.getChannel(AS7341_CHANNEL_CLEAR)); Serial.print(",");
    Serial.print(sensor.getChannel(AS7341_CHANNEL_NIR)); Serial.print(",");
    Serial.println(ch);
  }

  // === RECEIVE CLASSIFICATIONS FROM PYTHON ===
  for (int i = 0; i < numChannels; i++) {
    while (Serial.available() == 0);
    String pill = Serial.readStringUntil('\n');
    pill.trim(); pill.toLowerCase();
    int ch = activeChannels[i];
    knownPills[ch] = pill;
    Serial.print("Channel ");
    Serial.print(ch);
    Serial.print(" detected pill: ");
    Serial.println(pill);
  }

  // === USER INPUT PHASE ===
  Serial.println("Enter pill name to dispense:");
  String request = "";
  while (request == "") {
    if (Serial.available()) {
      request = Serial.readStringUntil('\n');
      request.trim(); request.toLowerCase();
    }
  }
  Serial.print("User requested: ");
  Serial.println(request);
  Serial.println("How many pills?");
  int quantity = 1;
  while (Serial.available() == 0);
  quantity = Serial.readStringUntil('\n').toInt();
  Serial.print("Quantity: ");
  Serial.println(quantity);

  // === DISPENSING LOGIC ===
  int matchedChannel = -1;
  for (int i = 0; i < numChannels; i++) {
    int ch = activeChannels[i];
    if (knownPills[ch] == request) {
      matchedChannel = ch;
      break;
    }
  }

  if (matchedChannel != -1) {
    Serial.print("Match found! Dispensing from channel ");
    Serial.println(matchedChannel);
    for (int i = 0; i < quantity; i++) {
      if (matchedChannel == 0) {
        Serial.println("Activating motor for channel 0 (pins 22,24,26,28)");
        smoothStep(motor0, stepsPerSegment);
      } else if (matchedChannel == 5) {
        Serial.println("Activating motor for channel 5 (pins 8,9,10,11)");
        smoothStep(motor5, stepsPerSegment);
      } else if (matchedChannel == 6) {
        Serial.println("Activating motor for channel 6 (pins 3,4,5,6)");
        smoothStep(motor6, stepsPerSegment);
      }
      delay(500);
    }
    Serial.println("Done dispensing.");
  } else {
    Serial.println("Requested pill not found in detected pills.");
  }

  // === RESET ===
  for (int i = 0; i < numChannels; i++) {
    knownPills[activeChannels[i]] = "";
  }
  Serial.println("Ready for next cycle.\n");
  delay(1000);
}
