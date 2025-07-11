#include "EspUsbHostKeybord.h"
#include <WiFi.h>
#include <HTTPClient.h>

const char *ssid = "YOUR_SSID";  // Enter your Wi-Fi SSID
const char *password = "YOUR_PASSWORD";  // Enter your Wi-Fi password
const char *googleScriptURL = "https://script.google.com/macros/s/...";  // Replace with your Google Script web app URL
const int switchPin = 4;  // Define the GPIO pin for the pull-up switch
const int stage = 1;  // Define the stage constant

class BarcodeScanning : public EspUsbHostKeybord {
public:
  String scannedBarcode;  // Variable to store the scanned barcode

  void onKey(usb_transfer_t *transfer) {
    uint8_t *const p = transfer->data_buffer;
    char asciiChar = convertKeyCodeToASCII(p[2],p[0]);
    if (asciiChar != '\0') {  // If it is not null
      scannedBarcode += asciiChar;  // Add it to scannedbarcode string
    }
    if (asciiChar == '\n') {  // If it is the end of the barcode
      processBarcode();  // Process the barcode
    }
  }

  // This function translates USB key codes to ASCII characters
  char convertKeyCodeToASCII(uint8_t keyCode, uint8_t shift) {
    char ascii_lower[] = "abcdefghijklmnopqrstuvwxyz1234567890\n\0\0\0\0-=[]\\\0;\'`,./";
    char ascii_upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()\0\0\0\0\0_+{}|\0:\"~<>?";

    if (keyCode >= 0x04 && keyCode <= 0x38) {
      return shift == 0x02 ? ascii_upper[keyCode - 0x04] : ascii_lower[keyCode - 0x04];
    }
    return '\0';  // Null character for unsupported keys
  }

  void processBarcode() {
    // Remove newline character from the end of the string
    scannedBarcode.trim();
    
    // Find the positions of the first and second "-" in the barcode
    int firstDashIndex = scannedBarcode.indexOf('-');
    int secondDashIndex = scannedBarcode.indexOf('-', firstDashIndex + 1);
    
    // Extract the stage digits from the barcode
    String barcodeStage = scannedBarcode.substring(firstDashIndex + 1, secondDashIndex);
    
    // Convert the extracted stage digits to an integer
    int barcodeStageInt = barcodeStage.toInt();
    Serial.printf("barcode stage: %d\n", barcodeStageInt);
    
    if (barcodeStageInt == stage) {
      Serial.println("Scanned Barcode: " + scannedBarcode); // Print the scanned barcode
      Serial.println("Direction: " + String(digitalRead(switchPin) == HIGH ? "out" : "in"));// Print the status of the switch
      neopixelWrite(RGB_BUILTIN, 64, 64, 0); // Yellow
      delay(200);
      neopixelWrite(RGB_BUILTIN, 0, 0, 0); // Turn off the RGB LED
      sendDataToGoogle(scannedBarcode); // Send data to Google Sheets
    } else {
      Serial.println("This barcode does not belong to this stage!"); // Error message
      neopixelWrite(RGB_BUILTIN, 64, 0, 0); // Red
      delay(200);
      neopixelWrite(RGB_BUILTIN, 0, 0, 0); // Turn off the RGB LED
    }
    
    scannedBarcode = ""; // Reset the scannedBarcode variable
  }

  void sendDataToGoogle(String data) {
    HTTPClient http;
    http.begin(googleScriptURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Remove the newline character from the end of the string
    data.trim();
    
    int out = digitalRead(switchPin); // Read the state of the pull-up switch
    int httpResponseCode = http.POST("data=" + data + "&out=" + (out == HIGH ? "true" : "false") + "&stage=" + String(stage));
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      neopixelWrite(RGB_BUILTIN, httpResponseCode == 302 ? 0 : 64, 0, httpResponseCode == 302 ? 64 : 0); // Blue for success, red for failure
      delay(httpResponseCode == 302 ? 100 : 300);
      neopixelWrite(RGB_BUILTIN, 0, 0, 0); // Turn off the RGB LED
    }
    http.end();
  }
};  // End of BarcodeScanning class 

BarcodeScanning usbHost;  // Creating an object of BarcodeScaning called usbHost

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  connectToWiFi();

  // Set the switch pin mode to input
  pinMode(switchPin, INPUT_PULLUP);
  Serial.println(digitalRead(switchPin) == HIGH ? "out" : "in"); // Print the status of the switch (in or out) at the beginning of the program

  usbHost.begin();
}

void loop() {
  // Check Wi-Fi connection status
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi(); // Reconnect to Wi-Fi
  }

  usbHost.task();
}

void connectToWiFi() {
  Serial.printf("Connecting to %s\n", ssid);
  neopixelWrite(RGB_BUILTIN, 64, 0, 0); // Red
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  neopixelWrite(RGB_BUILTIN, 0, 64, 0); // Green
  delay(1000);
  neopixelWrite(RGB_BUILTIN, 0, 0, 0); // Turn off the RGB LED
}
