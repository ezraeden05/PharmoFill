import serial
import numpy as np
import time
import tensorflow as tf

tflite = tf.lite

interpreter = tflite.Interpreter(model_path="C:/Users/eezra27/Desktop/model.tflite")
interpreter.allocate_tensors()
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

labels = ["blue", "red", "yellow"]

color_to_pill = {
    "red": "advil",
    "yellow": "tylenol",
    "blue": "zyrtec"
}

ser = serial.Serial('COM5', 9600, timeout=2)
time.sleep(2)
print("Listening for sensor data...")

classified_channels = set()
while len(classified_channels) < 3:
    try:
        if ser.in_waiting:
            line = ser.readline().decode().strip()
            if not line.startswith("SENSOR:"):
                continue
            parts = line.replace("SENSOR:", "").split(",")
            if len(parts) != 11:
                continue
            spectral = list(map(float, parts[:10]))
            channel = int(parts[10])

            input_data = np.array([spectral], dtype=np.float32)
            interpreter.set_tensor(input_details[0]['index'], input_data)
            interpreter.invoke()
            output = interpreter.get_tensor(output_details[0]['index'])[0]
            predicted_index = int(np.argmax(output))
            color = labels[predicted_index]
            pill = color_to_pill.get(color, "unknown")

            print(f"Channel {channel} → Predicted: {color} → Pill: {pill}")
            ser.write((pill + "\n").encode())
            classified_channels.add(channel)
    except Exception as e:
        print("Error:", e)

# Request loop
while True:
    try:
        user_request = input("Which pill do you want to dispense? (advil, tylenol, zyrtec): ").strip().lower()
        ser.write((user_request + "\n").encode())
        while True:
            quantity = input("How many pills to dispense? ").strip()
            if quantity.isdigit() and int(quantity) > 0:
                break
            print("Enter a valid number.")
        ser.write((quantity + "\n").encode())
        again = input("Do you want to request another pill? (yes/no): ").strip().lower()
        if again != "yes":
            print("Exiting.")
            break
    except Exception as e:
        print("Error during request:", e)
        break
