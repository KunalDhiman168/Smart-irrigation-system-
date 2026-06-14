
# Smart Automated Irrigation System using AWS IoT and ESP32

## Overview

This project implements a cloud-connected smart irrigation system that automates water valve control based on real-time soil moisture measurements. The system integrates ESP32 microcontrollers, AWS IoT Core, AWS Lambda, MQTT messaging, a continuous rotation servo motor, and a Flutter-based mobile dashboard to provide a complete precision agriculture solution.

The system continuously monitors soil moisture levels and automatically opens or closes a water valve depending on predefined thresholds. Users can remotely monitor system status through a mobile application and receive cloud-generated notifications.

---

## Features

* Real-time soil moisture monitoring
* Automated irrigation based on moisture threshold
* AWS IoT Core integration using MQTT
* Serverless decision-making using AWS Lambda
* Remote actuator control using ESP32
* Continuous rotation servo valve actuation
* Mobile dashboard for monitoring
* Email notifications through AWS services
* Cloud-based architecture
* Low-cost and scalable implementation

---

## System Architecture

```text
Soil Moisture Sensor
          |
          v
     ESP32 Sender
          |
      MQTT Publish
          |
          v
    AWS IoT Core
          |
          v
     AWS Lambda
          |
   Decision Logic
          |
          v
     MQTT Publish
          |
          v
    AWS IoT Core
          |
          v
    ESP32 Receiver
          |
          v
 Continuous Servo
          |
          v
 Water Valve Control
```

---

## Hardware Components

### Sender Node

* ESP32 Development Board
* Capacitive Soil Moisture Sensor
* WiFi Connection

### Receiver Node

* ESP32 Development Board
* Continuous Rotation Servo Motor
* Water Valve Mechanism

### Cloud Infrastructure

* AWS IoT Core
* AWS Lambda
* MQTT Broker
* Email Notification Service

### User Interface

* Flutter Mobile Application

---

## Working Principle

### Soil Monitoring

The sender ESP32 continuously reads soil moisture values from the sensor.

Typical readings:

```text
Wet Soil   : 0 - 1500
Normal     : 1500 - 3100
Dry Soil   : > 3100
```

---

### Data Transmission

The sender publishes moisture readings to AWS IoT Core:

```json
{
  "moisture": 3500
}
```

Topic:

```text
soil/data
```

---

### Cloud Processing

AWS Lambda subscribes to soil moisture data and performs threshold-based decision making.

Logic:

```python
if moisture > 3100:
    action = "OPEN"
else:
    action = "CLOSE"
```

---

### Command Generation

Lambda publishes commands to:

```text
motor/control
```

Payload:

```json
{
  "action": "OPEN"
}
```

or

```json
{
  "action": "CLOSE"
}
```

---

### Valve Actuation

The receiver ESP32 subscribes to:

```text
motor/control
```

Actions:

```text
OPEN  -> Rotate servo to open valve
CLOSE -> Rotate servo to close valve
```

The servo rotates for a fixed duration and then stops automatically.

---

## MQTT Topics

### Sensor Data

```text
soil/data
```

Example:

```json
{
  "moisture": 4095
}
```

---

### Valve Commands

```text
motor/control
```

Example:

```json
{
  "action": "OPEN"
}
```

---

## ESP32 Receiver Wiring

### Continuous Rotation Servo

```text
Brown Wire  -> GND
Red Wire    -> 5V / VIN
Yellow Wire -> GPIO26
```

---

## AWS Configuration

### AWS IoT Core

Created Things:

```text
SenderESP
MotorESP
```

---

### MQTT Topics

```text
soil/data
motor/control
```

---

### Policy Permissions

```text
iot:Connect
iot:Publish
iot:Subscribe
iot:Receive
```

---

### Certificates Used

```text
AmazonRootCA1.pem
device.pem.crt
private.pem.key
```

---

## Mobile Application

The Flutter application provides:

* Real-time moisture monitoring
* Valve status display
* Remote visibility
* Cloud-connected dashboard
* System status updates

---

## Project Workflow

```text
1. Soil moisture measured
2. ESP32 publishes sensor data
3. AWS IoT receives data
4. Lambda evaluates threshold
5. Command generated
6. Receiver ESP32 receives command
7. Servo actuates valve
8. Dashboard updates status
```

---

## Example Outputs

### Dry Soil

```json
{
  "moisture": 4095
}
```

Lambda:

```json
{
  "action": "OPEN"
}
```

Receiver:

```text
Opening Valve...
OPEN Complete
```

---

### Wet Soil

```json
{
  "moisture": 1200
}
```

Lambda:

```json
{
  "action": "CLOSE"
}
```

Receiver:

```text
Closing Valve...
CLOSE Complete
```

---

## Technologies Used

### Hardware

* ESP32
* Soil Moisture Sensor
* Continuous Rotation Servo

### Cloud

* AWS IoT Core
* AWS Lambda
* MQTT

### Software

* Arduino IDE
* Flutter
* Python

---

## Applications

* Smart Irrigation
* Precision Agriculture
* Water Conservation
* Remote Farm Monitoring
* IoT Automation Systems

---

## Future Improvements

* Weather Forecast Integration
* Machine Learning-Based Irrigation
* Multi-Zone Irrigation Control
* Solar-Powered Deployment
* Mobile-Based Manual Override
* Real-Time Analytics Dashboard

---

## Authors

Kunal Dhiman
Rahul Sharma
Electrical Engineering
Indian Institute of Technology Ropar

---

## License

This project is developed for educational, research, and agricultural automation purposes.
