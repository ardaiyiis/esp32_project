

# ESP32 Servo Control with Web Interface

This project demonstrates the use of two servo motors connected to an ESP32 board, allowing control via a WiFi connection and a web interface. The device has pan and tilt movement capabilities and controls a pointer light source. Users can connect to the ESP32's WiFi network and use a browser-based interface to control the servo motors.

## Features
- **Two Servo Motors**: Control two servos for pan and tilt movement.
- **ESP32 WiFi**: Connect to the ESP32 board via its built-in WiFi.
- **Web Interface**: Control the servo motors through a simple web interface on your browser.
- **Adjustable Limits**: Servo limits are set in the code but can be customized to suit your specific device.

## Requirements
- ESP32 board with WiFi capabilities
- Two servo motors
- Power supply appropriate for your servo motors and ESP32
- Micro-USB cable for ESP32 connection and programming
- A web browser to access the web interface

## Installation
1. **Clone or Download the Project**:
   - Clone this repository to your local machine, or download the zip file and extract it.
   - Ensure you have the Arduino IDE installed with the appropriate ESP32 board support.

2. **Connect the Hardware**:
   - Connect the servo motors to the ESP32's GPIO pins. By default, the code uses GPIO pins 12 and 13, but you can change these in the code.
   - Ensure your power supply can handle the current requirements of the servo motors and ESP32.

3. **Upload the Code**:
   - Open the Arduino IDE and load the provided `.ino` file.
   - Select the appropriate ESP32 board type and COM port in the Arduino IDE.
   - Compile and upload the code to your ESP32 board.

## Usage
1. **Connect to ESP32 WiFi**:
   - Once the code is uploaded and the ESP32 is running, look for a new WiFi network with a name like "ESP32_Servo_Controller".
   - Connect to this network. If prompted for a password, check the code for the default password (you can change it in the code).

2. **Access the Web Interface**:
   - Open a web browser and navigate to `http://192.168.4.1`.
   - You should see a simple interface with controls for the servo motors.

3. **Control the Servos**:
   - Use the web interface to control the pan and tilt movements.
   - The default limits for the servos are set in the code but can be adjusted as needed.

## Customization
- You can customize the servo limits by editing the code and changing the `minAngle` and `maxAngle` values.
- To change the default WiFi SSID and password, look for the WiFi setup section in the code and adjust the values accordingly.

## Troubleshooting
- If you cannot connect to the ESP32 WiFi, ensure the board is powered and the code is running.
- If the servo motors are not responding, double-check the connections and power supply.
- For additional support, consult the ESP32 documentation or the servo motor datasheets.

## License
This project is open source. Feel free to use, modify, and distribute it as needed. Please provide attribution if you use this project in your work.

## Contributions
Contributions are welcome! If you have improvements, suggestions, or bug fixes, please submit a pull request.

