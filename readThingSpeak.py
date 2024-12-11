import pyserial
import requests
import time

# Configure these
API_KEY = ""  # API Key
PORT = ""  # Arduino Port
BAUD_RATE = 9600  # Baud Rate

def send_to_thingspeak(value): # send_to_thingspeak function to send values to ThingSpeak
    """Send data to ThingSpeak."""
    url = f"https://api.thingspeak.com/update?api_key={API_KEY}&field1={value}"
    response = requests.get(url)
    return response.status_code == 200

def read_from_arduino(): # read_from_arduino function to retrjeve values from Arduino
    """Read a line of data from the Arduino."""
    if arduino.in_waiting > 0:
        line = arduino.readline().decode().strip()
        return line
    return None

try:
    # Open the serial connection
    arduino = serial.Serial(PORT, BAUD_RATE, timeout=1)
    print(f"Connected to Arduino on {PORT}")
    time.sleep(2)  # Allow time for the connection to stabilize

    while True:
        data = read_from_arduino()
        if data:
            print(f"Read from Arduino: {data}")
            try:
                value = float(data)  # Convert to a float for ThingSpeak
                if send_to_thingspeak(value):
                    print("Data sent to ThingSpeak successfully!") # Successful update
                else:
                    print("Failed to send data to ThingSpeak.") # Failed update
            except ValueError:
                print(f"Invalid data received: {data}")
        time.sleep(15)  # ThingSpeak free accounts require a 15-second delay between updates

except serial.SerialException as e:
    print(f"Error: {e}")
finally:
    if 'arduino' in locals() and arduino.is_open:
        arduino.close()
