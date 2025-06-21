#   Li-Fi Communication System  
**ESP8266 (Transmitter) + Arduino Uno (Receiver)**

##  Overview

This project demonstrates a simple yet effective **Li-Fi (Light Fidelity)** communication system that uses **visible light** to wirelessly transmit data. The system comprises:

- **Transmitter**: ESP8266 NodeMCU with a blue LED.
- **Receiver**: Arduino Uno with a solar panel or photodiode, and an SSD1306 OLED display.

Messages are sent via light pulses and decoded back into readable text at the receiver end.

---

## ⚙ 1. Methodology

### 1.1 System Overview

The system creates a **one-way optical communication link** using an LED as the light source and a solar panel or photodiode as the sensor. ASCII messages are converted into binary, transmitted via LED blinks, detected by the light sensor, and decoded back into characters on the receiver's OLED display.

---

### 1.2 Transmitter – ESP8266 + LED

####  Hardware Setup

- **Components**:
  - ESP8266 NodeMCU (Wi-Fi & control)
  - Blue LED
  - Resistor (220–330 Ω)
  - USB power for operation and debugging

- **Connections**:
  - **GPIO12 (D6)** → Resistor → **Anode of LED**
  - **Cathode of LED** → GND (NodeMCU)

####  Software Logic

- **Wi-Fi Access Point**:
  - ESP8266 runs as an AP (e.g., SSID: `LiFi_AP`, Password: `12345678`)
  - Hosts a simple web interface for message input

- **Data Conversion**:
  - Each character is converted into a **10-bit frame**:
    - `Start Bit (0)` + `8 Data Bits` (LSB first) + `Stop Bit (1)`

- **LED Bit-Banging**:
  - LED ON → Logic `1`
  - LED OFF → Logic `0`
  - Bit Duration: **120 ms** per bit

---

### 1.3 Receiver – Arduino Uno + Sensor + OLED

####  Hardware Setup

- **Components**:
  - Arduino Uno
  - Solar panel or photodiode (optical sensor)
  - 0.96" SSD1306 OLED display (I2C)

- **Connections**:
  - **Sensor**:
    - +ve → A3 (Analog Pin)
    - -ve → GND
  - **OLED Display**:
    - SDA → A4
    - SCL → A5
    - VCC → 3.3V
    - GND → GND

####  Software Logic

- **Signal Sampling**:
  - Analog reads from A3
  - A **threshold** (e.g., ~500) distinguishes between light ON/OFF

- **Edge Detection**:
  - Looks for falling edge (HIGH to LOW) to detect **start bit**
  - Waits `1.5 × bit duration` (~195 ms) to sample mid-bit

- **Byte Reconstruction**:
  - Reads 8 bits (LSB first) to decode character
  - Special characters like `#` clear the OLED

- **Display**:
  - Messages displayed on Serial Monitor & OLED

---

### 1.4 Calibration & Testing

####  Calibration

- **Threshold Adjustment**:
  - Samples ambient & LED light to compute dynamic threshold
- **Timing Tuning**:
  - Verify 120 ms bit timing with test patterns

#### Testing & Debugging

- End-to-end tests via web interface
- Monitor analog readings, decoding accuracy
- Fine-tune threshold or timing as needed
- Detect transmission errors for future improvements

---

### 1.5 Integration & Future Scope

####  System Integration

- LED and sensor are **optically aligned** for maximum efficiency
- Tested under **controlled lighting** conditions

####  Future Enhancements

- Adaptive thresholding and noise filtering
- Better modulation schemes
- Error detection & correction (e.g., parity or checksums)
- Bidirectional communication extension

---

##  Tools & Libraries

- ESP8266 Arduino Core
- U8g2 or Adafruit SSD1306 library for OLED
- HTML/CSS for web interface (hosted on ESP8266)



