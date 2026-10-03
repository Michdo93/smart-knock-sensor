# Smart Knock Sensor (MQTT)

A DIY IoT knock and vibration sensor using a **Piezo Ceramic Vibration Sensor Module** powered by a **3x AA battery holder (4.5V)** and controlled by either an **ESP8266 (Wemos D1 Mini)** or an **ESP32**. When a knock or vibration is detected, the device publishes an event to your local MQTT broker (compatible with Home Assistant, Node-RED, OpenHAB, etc.).

---

## 📂 Repository Structure

```text
smart-knock-sensor/
├── README.md
├── esp8266_d1_mini/
│   └── esp8266_d1_mini.ino
└── esp32/
    └── esp32.ino
```

---

## 🛠️ Hardware Wiring Guide

Both microcontrollers use the **Piezo Ceramic Vibration Sensor Module** (which features an onboard operational amplifier/comparator for signal preprocessing) and are powered by a **3x AA battery holder (yielding ~4.5V)**.

### 1. Wiring for ESP8266 (Wemos D1 Mini)

| Component | Component Pin | D1 Mini Pin | Description |
| --- | --- | --- | --- |
| **Piezo Sensor Module** | **+** (VCC) | **3V3** | Powers the sensor module with stable 3.3V |
| **Piezo Sensor Module** | **-** (GND) | **GND** | Shared ground connection |
| **Piezo Sensor Module** | **S** (Signal) | **A0** | Analog signal input (reads vibration intensity) |
| **3x AA Battery** | **+** (Red, 4.5V) | **5V** | Direct power supply to the board (bypasses internal regulator) |
| **3x AA Battery** | **-** (Black) | **GND** | Shared ground connection (common ground) |

### 2. Wiring for ESP32

| Component | Component Pin | ESP32 Pin | Description |
| --- | --- | --- | --- |
| **Piezo Sensor Module** | **+** (VCC) | **3V3** | Powers the sensor module with stable 3.3V |
| **Piezo Sensor Module** | **-** (GND) | **GND** | Shared ground connection |
| **Piezo Sensor Module** | **S** (Signal) | **GPIO 34 (VP)** | ADC Analog signal input |
| **3x AA Battery** | **+** (Red, 4.5V) | **VIN / 5V** | Direct power supply to the board |
| **3x AA Battery** | **-** (Black) | **GND** | Shared ground connection (common ground) |

> ⚠️ **Important Note on Ground (GND):** Make sure that both the battery's negative wire and the sensor module's GND pin share a physical connection with the microcontroller's `GND` pin (using a breadboard or perfboard).

---

## 💻 Flashing Guide (Step-by-Step)

### Prerequisites

1. Download and install the **Arduino IDE**.
2. Install the necessary Board Support Packages via **Tools > Board > Boards Manager**:
* For ESP8266: Search for `esp8266` (by ESP8266 Community).
* For ESP32: Search for `esp32` (by Espressif Systems).


3. Install the **PubSubClient** library via **Tools > Manage Libraries...** (search for `PubSubClient` by Nick O'Leary).

### Configuration (Before Flashing)

Open either `esp8266_d1_mini/esp8266_d1_mini.ino` or `esp32/esp32.ino` and replace the placeholder variables with your network and MQTT broker details:

```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "192.168.1.100"; // IP address of your MQTT Broker (e.g., Mosquitto, Home Assistant)
```

### Flashing Steps:

1. Connect your microcontroller to your computer via a USB cable.
2. Select your board in **Tools > Board** (e.g., *LOLIN(WEMOS) D1 R2 & mini* or *ESP32 Dev Module*).
3. Select the correct COM Port under **Tools > Port**.
4. Click the **Upload** (right arrow) button in the Arduino IDE to compile and flash the code.

---

## 📡 MQTT Integration & Subscribing

Once the device boots up, it connects to your WiFi and establishes an MQTT connection with your broker.

### MQTT Topics

* **Status Topic:** `home/knock/status` (Publishes `online` upon connection)
* **Event Topic:** `home/knock/event` (Publishes `knock_detected` when a physical impact occurs)

### How to Subscribe and Test (Command Line / Terminal)

If you have the Mosquitto MQTT client tools installed on your computer or server, you can listen to incoming messages using this command:

```bash
mosquitto_sub -h 192.168.1.100 -t "home/knock/#" -v
```

*(Replace `192.168.1.100` with your actual MQTT broker IP address).*

### What You Will Receive & When Events Are Triggered

* **When is an event triggered?**
When someone knocks on the surface or hits the piezo sensor, the ceramic element generates a voltage spike. The onboard module scales this into an analog voltage.
* **ESP8266 (0–1023 ADC Range):** Defaults to a threshold of `400`. If `analogRead(A0) > 400`, a knock is considered registered.
* **ESP32 (0–4095 ADC Range):** Defaults to a threshold of `1500`. If `analogRead(34) > 1500`, a knock is registered.


* **Debouncing Mechanism:**
To prevent a single knock from triggering dozens of rapid MQTT messages due to physical vibrations echoing through the material, a **cooldown delay (debounce delay)** of `300 ms` is implemented. If multiple hits occur within 300 ms, subsequent hits are ignored until the timer resets.
* **Payload Received:**
When a valid knock passes the threshold and debounce check, the device publishes:
```text
Topic: home/knock/event
Payload: knock_detected
```

You can use this payload in **Home Assistant** (via MQTT Integration / Binary Sensors), in **openHAB** (via the MQTT-Binding) or **Node-RED** to trigger automations, such as turning on lights, playing a sound, or sending a notification when someone knocks on your door/desk.
