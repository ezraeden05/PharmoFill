# PharmoFill

A student-built automated dispensing prototype using three AS7341 spectral sensors, an Arduino, stepper motors, and a TensorFlow Lite color-classification model developed using Edge Impulse.

**Important:** The labels Advil, Tylenol, and Zyrtec in the prototype code map to **colors of demonstration samples**. The model does not authenticate real medicines, and the prototype is **not a medical device** or suitable for dispensing medication to patients.

## Repository contents

- `arduino/PharmoFill.ino`: Reads ten spectral channels on each of three AS7341 sensors through a TCA I²C multiplexer (channels 0, 5, and 6), sends the readings to the PC over serial, receives classifications and dispensing requests, and turns the appropriate stepper motor.
- `pc/pharmofill_terminal.py`: Receives spectral readings, runs the exported TensorFlow Lite classifier, reports the predicted color, maps the color to a demo-sample label, and sends requests back to the Arduino.
- `model/model.tflite`: Standalone model from the project’s Edge Impulse export.
- `requirements.txt`: Python library dependencies.

## How the demonstration works

1. The Arduino selects each sensor channel and sends ten readings per channel as a comma-delimited `SENSOR:` line.
2. The PC feeds the ten readings into the TensorFlow Lite model and returns a classification for that channel.
3. The user chooses a demonstration sample type and quantity in the PC terminal.
4. The Arduino finds the corresponding cartridge and advances its stepper motor for each requested item.

## Software and hardware setup

This repository preserves the **original provided source code** without behavior changes. It has not been re-tested against hardware here.

- Arduino libraries: `Wire` (built-in), `Adafruit_AS7341`, `Stepper` (built-in).
- Python packages: see `requirements.txt`.
- The PC script uses a machine-specific model path (`C:/Users/eezra27/Desktop/model.tflite`) and `COM5`. To run this on another computer, update the path to the included `model/model.tflite` file and set the correct serial port.
- The source assumes three readable sensors and an Arduino/PC serial link at 9600 baud.

## Known limitations in this version

- The Arduino starts a new sensor-reading/classification phase on each `loop()` iteration, while the PC script classifies channels only once before switching permanently to its user-request loop. This can cause a communication stall on subsequent dispensing cycles. A continuously running version would need a coordinated serial-message protocol or a different loop structure.
- The code may wait indefinitely if a sensor fails to produce all three classifications; there is no timeout for some Arduino serial reads.
- The mapping relies on **color classification** and is not suitable for distinguishing or validating actual medications.

## Contributions and development

The project involved CAD and mechanical prototype iteration, Arduino motor and spectral-sensor integration, a PC-based TensorFlow Lite inference pipeline, and Edge Impulse model development. For an admissions portfolio, accompany this code with photos of the mechanism, tests, and a truthful explanation of the maker’s specific responsibilities and collaborators.
