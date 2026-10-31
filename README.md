# PharmoFill

A student-built automated pill-dispensing prototype using three AS7341 multispectral sensors, an Arduino, stepper motors, and a TensorFlow Lite classification model developed using Edge Impulse.

## Prototype Scope and Future Development

PharmoFill is a functional proof-of-concept automated pill-dispensing system that integrates spectral sensing, AI classification, and mechanical dispensing. I used AS7341 multispectral sensors to classify color-coded sample pills, allowing me to develop and demonstrate the complete system within my budget. My goal is to advance PharmoFill toward clinical use by incorporating more advanced near-infrared (NIR) spectroscopy, building on the same light-based identification approach to distinguish medications through more detailed spectral signatures. This next stage would require further testing and validation to ensure accurate identification and safe dispensing.

## Repository contents

- `PharmoFill.ino`: Reads ten spectral channels on each of three AS7341 sensors through a TCA I²C multiplexer (channels 0, 5, and 6), sends the readings to the PC over serial, receives classifications and dispensing requests, and turns the appropriate stepper motor.
- `pharmofill_terminal.py`: Receives spectral readings, runs the exported TensorFlow Lite classifier, reports the predicted color, maps the color to a demonstration sample label, and sends requests back to the Arduino.
- `model.tflite`: Standalone model from the project's Edge Impulse export.
- `requirements.txt`: Python library dependencies.

## How the demonstration works

1. The Arduino selects each sensor channel and sends ten readings per channel as a comma-delimited `SENSOR:` line.
2. The PC feeds the ten readings into the TensorFlow Lite model and returns a classification for that channel.
3. The user chooses a demonstration sample type and quantity in the PC terminal.
4. The Arduino finds the corresponding cartridge and advances its stepper motor for each requested item.

## Software and hardware setup

This repository preserves the **original prototype source code** without behavior changes. It has not been independently re-tested against the physical hardware.

- Arduino libraries: `Wire` (built-in), `Adafruit_AS7341`, `Stepper` (built-in).
- Python packages: see `requirements.txt`.
- The PC script uses a machine-specific model path (`C:/Users/eezra27/Desktop/model.tflite`) and `COM5`. To run this on another computer, update the path to the included `model.tflite` file and set the correct serial port.
- The source assumes three readable sensors and an Arduino/PC serial link at 9600 baud.

## Known limitations in this version

- The Arduino starts a new sensor-reading/classification phase on each `loop()` iteration, while the PC script classifies channels only once before switching permanently to its user-request loop. This can cause a communication stall on subsequent dispensing cycles. A continuously running version would need a coordinated serial-message protocol or a different loop structure.
- The code may wait indefinitely if a sensor fails to produce all three classifications; there is no timeout for some Arduino serial reads.

## Contributions and development

I designed and revised the CAD models for PharmoFill's dispensing mechanism, including changes to the cartridge layout and rotating platforms to reduce jamming. I also worked on assembling and testing the physical prototype, integrating the spectral sensors and stepper motors with the Arduino, and developing the AI classification model using Edge Impulse. I then connected the model to the Python terminal using TensorFlow Lite to coordinate classification with the dispensing mechanism.
