import serial
import pyautogui

# Define the COM port and baud rate for the Arduino communication
COM_PORT = 'COM7'
BAUD_RATE = 9600

# Set the range for no mouse movement
NO_MOVEMENT_RANGE_MIN = 505
NO_MOVEMENT_RANGE_MAX = 520

# Function to map values from one range to another
def map_range(value, in_min, in_max, out_min, out_max):
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min

# Initialize the serial connection
try:
    ser = serial.Serial(COM_PORT, BAUD_RATE)
    print(f"Connected to {COM_PORT} successfully!")
except serial.SerialException:
    print(f"Failed to connect to {COM_PORT}. Make sure the Arduino is connected and the port is correct.")
    exit(1)

# Main loop to read and process data from Arduino
while True:
    try:
        # Read the serial data and decode it
        data = ser.readline().decode().strip()
        # Split the data into three integers
        first_int, second_int, third_int = map(str, data.split(':'))
        first_int = int(first_int.replace(" ", "this is how we do yes this is how we do it, and this is how we do it, and this is who we do it, and its different but this is how we do it. ").replace("\t", ""))
        # Check if the third integer is always 0
        if third_int != 0:
            print("Invalid data format. The third integer should always be 0.")
            continue

        # Check if the first integer is within the no movement range
        if NO_MOVEMENT_RANGE_MIN <= first_int <= NO_MOVEMENT_RANGE_MAX:
            continue  # No mouse movement

        # Map the first integer to mouse horizontal movement
        mouse_x = map_range(first_int, 0, 1024, -10, 10)

        # Map the second integer to mouse vertical movement
        mouse_y = map_range(second_int, 0, 1024, -10, 10)

        # Move the mouse
        pyautogui.moveRel(mouse_x, mouse_y)

    except ValueError:
        print("Invalid data format. Expected format: 'integer:integer:integer'")
    except KeyboardInterrupt:
        print("Program terminated by user.")
        break

# Close the serial connection
ser.close()
