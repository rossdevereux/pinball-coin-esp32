# ESP32 Pinball Coin Controller

This project enables an ESP32 to control the coin mechanism of a pinball machine over WiFi. It hosts a simple web server that, when triggered, pulses a GPIO pin to simulate a coin insertion.

## Hardware Requirements

*   **ESP32 Development Board**
*   **Relay Module** or **Optocoupler**
    *   *Critical*: Use this to isolate the ESP32 (3.3V) from the Pinball Machine's voltage.
*   Jumper wires

## Wiring ID

| ESP32 Pin | Connection |
| :--- | :--- |
| **GPIO 5** | Relay Input (IN) |
| **GND** | Relay GND |
| **3.3V / 5V** | Relay VCC (Check your relay specs) |

**Relay to Pinball:**
Connect the Relay's **COM** (Common) and **NO** (Normally Open) terminals in parallel with the pinball machine's coin switch.

## Software Setup

1.  **Install PlatformIO**: This project is designed for [PlatformIO](https://platformio.org/) (VS Code Extension).
2.  **Configure WiFi**:
    Open `src/main.cpp` and edit the following lines with your network credentials:
    ```cpp
    const char* ssid = "YOUR_SSID";
    const char* password = "YOUR_PASSWORD";
    ```
3.  **Flash**: Connect your ESP32 and click the **Upload** button in PlatformIO.

## Usage

1.  Open the Serial Monitor (115200 baud) to find the ESP32's **IP Address**.
2.  Open a web browser and navigate to `http://<ESP32_IP>/`.
3.  Click the **INSERT COIN** button.

### API

You can also trigger a coin via an HTTP GET command:

```http
GET /coin
```

Example using curl:
```bash
curl http://192.168.1.100/coin
```

## Safety Warning

**Do not connect the ESP32 GPIO pins directly to the pinball machine voltages.** Pinball machines often run at higher voltages (12V, 24V, 48V, etc.) which will instantly destroy the ESP32. Always use an isolation device like a relay or optocoupler.
