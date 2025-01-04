import serial
import time
import tensorflow as tf
import numpy as np
import cv2
from tensorflow.keras.models import load_model

# Arduino configuration
arduino_port = 'COM3'
baud_rate = 9600

# Establish serial connection
try:
    arduino = serial.Serial(port=arduino_port, baudrate=baud_rate, timeout=1)
    print(f"Successfully connected to {arduino_port}")
except serial.SerialException as e:
    print(f"Error opening serial port {arduino_port}: {e}")
    exit(1)

time.sleep(2)

loaded_model = load_model("D:/Temp/Kuliah/Skripsi/Coding/main/model.h5")
classes = ["Glass", "Metal", "Organic", "Paper", "Plastic", "Textile"]

def turn_on_camera():
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("Error: Could not open camera.")
        return None
    return cap

def turn_off_camera(cap):
    cap.release()
    cv2.destroyAllWindows()

# Save image
def save_image_with_timestamp(frame):
    timestamp = time.strftime("%Y-%m-%d_%H-%M-%S")  # Format timestamp
    filename = f"captured_image_{timestamp}.jpg"
    cv2.imwrite(filename, frame)
    print(f"Image saved as {filename}")
    return filename, timestamp

# Log data
def log_classification_result(filename, predicted_class, confidence, timestamp):
    log_entry = f"{timestamp} - {filename} - Prediction: {predicted_class} ({confidence:.2f}%)\n"
    with open("classification_log.txt", "a") as log_file:
        log_file.write(log_entry)
    print(f"Log saved: {log_entry.strip()}")

# Preprocessing image
def preprocess_image(frame):
    img = cv2.resize(frame, (256, 256))  # Resize ke ukuran input model
    img_array = np.rot90(img, 2)  # Rotasi 180 derajat
    img_array = tf.keras.preprocessing.image.img_to_array(img_array)
    img_array = tf.expand_dims(img_array, 0)  # Tambahkan batch dimension
    return img_array

# Send signal to arduino
def send_signal_to_arduino(predicted_class):
    signal_map = {
        "Glass": b'1',
        "Metal": b'2',
        "Organic": b'3',
        "Paper": b'4',
        "Plastic": b'5',
        "Textile": b'6'
    }
    signal = signal_map.get(predicted_class, b'0')  # Default '0'
    arduino.write(signal)
    print(f"Sent signal {signal.decode()} to Arduino for class '{predicted_class}'")

    time.sleep(0.1)  # Beri waktu agar Arduino memproses data
    if arduino.in_waiting > 0:
        response = arduino.readline().decode('utf-8').strip()
        print(f"Arduino response: {response}")
        if "Received" in response:
            print(f"Arduino confirmed receiving: {signal.decode()}")
        else:
            print("No confirmation received from Arduino.")

# Classification
def capture_save_and_classify(cap):
    ret, frame = cap.read()  # Ambil gambar dari kamera
    if not ret:
        print("Error: Failed to capture image from camera.")
        return

    # Simpan gambar hasil kamera
    filename, timestamp = save_image_with_timestamp(frame)

    # Tampilkan gambar yang diambil
    cv2.imshow("Captured Image", frame)
    cv2.waitKey(500)  # Tampilkan selama 500ms

    # Preprocess gambar dan prediksi
    img_array = preprocess_image(frame)
    predictions = loaded_model.predict(img_array)
    predicted_class = classes[np.argmax(predictions)]
    confidence = predictions[0][np.argmax(predictions)] * 100

    print(f"Prediction: {predicted_class} ({confidence:.2f}%)")
    log_classification_result(filename, predicted_class, confidence, timestamp)
    send_signal_to_arduino(predicted_class)

cap = None

print("Program Successfully Uploaded")

try:
    while True:
        if arduino.in_waiting > 0:
            line = arduino.readline().decode('utf-8').strip()
            print(f"Arduino : {line}")
            if "Object detected" in line:
                print("Turning on camera and capturing image...")
                cap = turn_on_camera()
                if cap is not None:
                    print("Object detected! Capturing image, saving, and classifying...")
                    capture_save_and_classify(cap)
                    time.sleep(1)  # Tunggu 1 detik sebelum lanjut
                    turn_off_camera(cap)
                    cap = None
    
except KeyboardInterrupt:
    print("Program terminated")
finally:
    if cap is not None:
        turn_off_camera(cap)
    arduino.close()
    print("Serial connection closed.")