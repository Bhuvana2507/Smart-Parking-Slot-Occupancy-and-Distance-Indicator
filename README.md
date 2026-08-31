# Smart Parking Slot Occupancy and Distance Indicator

An IoT-based smart parking system that detects parking-slot occupancy using a **VL53L0X Time-of-Flight distance sensor** and an **ESP32**. The system provides real-time distance and parking-status information through an ESP32 web dashboard and publishes data to **ThingSpeak using MQTT**.

## Project Overview

Finding an available parking space in crowded areas such as shopping malls, hospitals, universities, and office buildings can be difficult. Drivers may spend several minutes searching for an empty slot, resulting in traffic congestion, fuel wastage, increased pollution, and driver frustration.

This project demonstrates an IoT-based approach for monitoring parking-slot availability in real time.

## Objectives

* Measure the distance between the vehicle and parking sensor.
* Determine whether a parking slot is vacant, approaching, or occupied.
* Indicate the slot status using an RGB LED.
* Stream parking data to the cloud using MQTT.
* Provide a web-based dashboard for real-time monitoring.
* Recommend the next available parking slot.
* Demonstrate an architecture that can be expanded to multiple parking spaces.

## Hardware Components

* ESP32
* VL53L0X ToF Distance Sensor
* RGB LED
* Toy Car for parking-slot simulation

## Software and Technologies

* Arduino IDE
* Embedded C/C++
* Wi-Fi
* MQTT
* ThingSpeak
* HTTP Web Server
* HTML/CSS
* I2C

## System Architecture

```text
             ┌─────────────┐
             │   Toy Car   │
             └──────┬──────┘
                    │
                    ▼
          ┌──────────────────┐
          │ VL53L0X ToF      │
          │ Distance Sensor  │
          └────────┬─────────┘
                   │ I2C
                   ▼
             ┌───────────┐
             │   ESP32   │
             └─────┬─────┘
              ┌────┴─────┐
              │          │
              ▼          ▼
        ┌──────────┐  ┌─────────────┐
        │ RGB LED  │  │ MQTT / Wi-Fi│
        └──────────┘  └──────┬──────┘
                             ▼
                       ┌───────────┐
                       │ ThingSpeak│
                       └───────────┘

             ESP32 Web Server
                    │
                    ▼
           Parking Dashboard
```

## Working Principle

The VL53L0X sensor measures the distance between the sensor and the vehicle using Time-of-Flight technology.

The ESP32 reads the measured distance and classifies the parking-slot condition into three states:

| Distance | Status      | RGB LED     |
| -------- | ----------- | ----------- |
| > 12 cm  | VACANT      | Green       |
| 6–12 cm  | APPROACHING | Red + Green |
| ≤ 6 cm   | OCCUPIED    | Red         |

The ESP32 then:

1. Reads the distance from the VL53L0X sensor.
2. Converts the measured distance into centimetres.
3. Determines the current parking status.
4. Updates the RGB LED.
5. Updates the local web dashboard.
6. Publishes distance and status data through MQTT.
7. Sends the data to ThingSpeak for cloud monitoring.

## Parking Slot Simulation

The prototype contains **one physical parking slot** implemented using the VL53L0X sensor.

The remaining parking slots are simulated in software using an array. The dashboard displays the status of four slots and identifies the next available slot.

Example:

```text
Slot 1 : EMPTY
Slot 2 : OCCUPIED
Slot 3 : EMPTY
Slot 4 : OCCUPIED

Recommended Slot : 1
```

When the physical vehicle occupies Slot 1, the system changes its status to occupied and automatically recommends the next available slot.

## Web Dashboard

The ESP32 hosts a local HTTP web server that displays:

* Current measured distance
* Current parking status
* Status of all parking slots
* Recommended available parking slot

The dashboard automatically refreshes every 2 seconds to display updated information.

## MQTT and ThingSpeak

The ESP32 connects to Wi-Fi and communicates with the ThingSpeak MQTT broker.

The following data is published:

```text
field1 = Distance in cm
field2 = Parking status code
```

Status codes:

```text
0 → VACANT
1 → APPROACHING
2 → OCCUPIED
```

This allows the parking data to be monitored remotely through the cloud.

## Features

* Real-time distance measurement
* Three-state parking detection
* RGB LED status indication
* ESP32-based web dashboard
* MQTT-based cloud communication
* ThingSpeak integration
* Automatic available-slot recommendation
* Expandable multi-slot architecture

## Advantages

* Low-cost implementation
* Real-time monitoring
* Easy installation
* Low power consumption
* Cloud connectivity
* Expandable architecture

## Applications

The concept can be used in:

* Smart cities
* Shopping malls
* Airports
* Hospitals
* Universities
* Office buildings
* Residential apartments

## Future Scope

The system can be further enhanced by:

* Installing one sensor for each parking slot
* Developing a dedicated mobile application
* Integrating an automatic boom barrier
* Implementing camera-based vehicle detection
* Adding number-plate recognition
* Supporting offline data storage
* Implementing AI-based parking prediction
* Providing navigation to the nearest available parking slot

## Repository Structure

```text
Smart-Parking-System/
│
├── parking_slot.ino
├── README.md
└── PARKING SLOT OCCUPANCY AND DISTANCE INDICATOR.pptx
```

## Conclusion

The Smart Parking Slot Occupancy and Distance Indicator demonstrates how IoT technology can improve parking management by combining real-time sensing, wireless communication, cloud connectivity, and web-based visualization.

The ESP32 processes sensor data from the VL53L0X, MQTT is used for cloud communication with ThingSpeak, and the web dashboard provides real-time parking information. Although the prototype uses one physical sensor with additional software-simulated slots, the architecture can be extended to support multiple physical parking spaces.
