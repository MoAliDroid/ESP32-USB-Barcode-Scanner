# ESP32-USB-Barcode-Scanner
My senior project/ ESP32-S3 USB barcode scanner

This repository contains the Arduino code for the **ESP32-S3 DevKit**, developed as part of my senior project:  
**Integrated Logistic System for Streamlined Operations Enhancement** at King Abdulaziz University.

The system enables **barcode scanning**, stage validation, and **Google Sheets integration** over Wi-Fi, making it suitable for smart logistics and inventory management.

---

##  Features

-  USB Host Barcode Scanner support via `EspUsbHostKeybord` library
-  Wi-Fi connectivity (auto-reconnect on disconnection)
-  Stage-based validation logic (specific for my project)
-  Direction detection via digital switch (in/out)
-  Google Sheets integration via Apps Script Web App
-  Visual feedback using built-in RGB LED (NeoPixel)

---

##  Hardware

- **Board**: ESP32-S3 DevKit
- **Scanner**: USB Barcode Scanner (HID-compatible)
- **Input**: Pull-up switch (connected to GPIO 4)
- **Indicator**: Built-in RGB LED (WS2812 aka NeoPixel)

---

##  Software & Libraries

- **Arduino core for ESP32**  
- `WiFi.h` – Wi-Fi connection  
- `HTTPClient.h` – HTTP POST to Google Script  
- [`EspUsbHostKeybord`](https://github.com/tanakamasayuki/EspUsbHost) – USB HID keyboard host for barcode scanning

> Make sure to install the **EspUsbHostKeybord** library by *TANAKA Masayuki* via Arduino Library Manager or from GitHub.

---

##  Google Script Integration

This project includes a `GoogleAppScript.gs` file for logging scanned barcodes to **Google Sheets**.

### 📄 How it Works

- The ESP32 sends a `POST` request with the barcode (`data`), the direction (`out`), and current stage (`stage`)
- The Apps Script:
  - Parses the barcode and extracts the `prodID` from the first 4 characters
  - Determines which sheet to log to (e.g., `Stage1_in`, `Stage1_out`, etc.)
  - Appends the record with timestamp

### 🔗 Google Sheets Setup

1. Create a Google Sheet with the following tabs:
   - `Stage1_in`, `Stage1_out`
   - `Stage2_in`, `Stage2_out`
   - `Stage3_in`, `Stage3_out`

2. Open [https://script.google.com](https://script.google.com) and paste the contents of `GoogleAppScript.gs`.

3. Replace the spreadsheet ID in this line:
   ```js
   SpreadsheetApp.openById('YOUR_SPREADSHEET_ID_HERE')

---

##  File Overview

- `main.ino`: Main source code file with setup, Wi-Fi handling, USB host logic, and barcode processing.

---

##  How to Use

1. Clone/download this repository
2. Open `main.ino` in Arduino IDE
3. Select the correct **board**: `ESP32S3 Dev Module`
4. Install required libraries (see above)
5. Flash the code to your ESP32-S3
6. Monitor Serial output at 115200 baud
7. Scan barcodes and watch them get processed and logged!

---

##  Configuration

In `main.ino`, configure:
```cpp
const char *ssid = "YOUR_SSID";
const char *password = "YOUR_PASSWORD";
const char *googleScriptURL = "https://script.google.com/macros/s/...";
const int switchPin = 4;
const int stage = 1;
