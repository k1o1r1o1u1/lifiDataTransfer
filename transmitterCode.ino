#include <ESP8266WiFi.h>         // WiFi functionality for ESP8266
#include <ESP8266WebServer.h>    // Web server library for handling HTTP requests

#define LED_PIN D6               // Define the GPIO pin used for LED transmission
#define BIT_DURATION 130         // Duration of each bit in milliseconds

const char* ssid = "LiFi_AP";     // Access Point SSID
const char* password = "12345678"; // Access Point password

ESP8266WebServer server(80);     // Create a web server object on port 80
String serialBuffer;             // Buffer to store serial input before sending

void setup() {
  pinMode(LED_PIN, OUTPUT);       // Set the LED pin as output
  digitalWrite(LED_PIN, HIGH);    // Keep LED ON initially (idle high)

  Serial.begin(115200);           // Start serial communication
  Serial.println("Li‑Fi Transmitter Ready");
  Serial.println("Type a message below and press ENTER to send via Serial:");

  // Create a Wi-Fi Access Point
  WiFi.softAP(ssid, password);

  // Define the HTTP handlers for the root page and sending message
  server.on("/", handleRoot);
  server.on("/send", handleSend);
  
  server.begin();                // Start the web server
}

void loop() {
  server.handleClient();         // Continuously listen and respond to web clients

  // Handle input from Serial Monitor
  if (Serial.available()) {
    char incoming = Serial.read();  // Read one character from serial input

    if (incoming == '\n') {         // If ENTER is pressed (end of message)
      serialBuffer.trim();          // Remove any leading/trailing whitespace
      if (serialBuffer.length() > 0) {
        Serial.print("🖥️ Transmitting (Serial): ");
        Serial.println(serialBuffer);
        sendMessage(serialBuffer);  // Transmit the message using LED
      }
      serialBuffer = "";            // Clear the buffer after sending
    } else {
      serialBuffer += incoming;     // Append character to buffer
    }
  }
}

// Handler to serve the HTML form for message input
void handleRoot() {
  server.send(200, "text/html", R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>Li‑Fi Transmitter</title>
  <style>
    body {
      margin: 0;
      background: #0e0e0e;
      font-family: 'Segoe UI', sans-serif;
      color: #ffffff;
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 20px;
    }
    h1 {
      color: #00ffd0;
      margin-bottom: 10px;
      font-size: 1.8em;
    }
    form {
      width: 100%;
      max-width: 400px;
      display: flex;
      flex-direction: column;
      gap: 15px;
      margin-top: 20px;
    }
    input[type="text"] {
      padding: 12px;
      font-size: 16px;
      border: none;
      border-radius: 8px;
      background-color: #1a1a1a;
      color: #ffffff;
    }
    button {
      padding: 12px;
      font-size: 16px;
      background-color: #00ffd0;
      color: #000000;
      border: none;
      border-radius: 8px;
      cursor: pointer;
      transition: background-color 0.3s ease;
    }
    button:hover {
      background-color: #00c7a6;
    }
    .note {
      margin-top: 20px;
      font-size: 14px;
      color: #aaa;
      text-align: center;
    }
    footer {
      margin-top: 30px;
      font-size: 12px;
      color: #555;
    }
  </style>
</head>
<body>
  <h1>✨ Li‑Fi Message Sender</h1>
  <form action="/send" method="GET">
    <input type="text" name="msg" placeholder="Type your message..." maxlength="100" required />
    <button type="submit">Send</button>
  </form>
  <div class="note">💡 Make sure the LED is aligned with the receiver</div>
  <footer>ESP8266 Li‑Fi Project • 2025</footer>
</body>
</html>
)rawliteral");
}

// Handler to process the message sent from the web interface
void handleSend() {
  if (server.hasArg("msg")) {
    String input = server.arg("msg"); // Get the message from the "msg" input field
    input.trim();                     // Remove whitespace
    input.replace("<", "");           // Sanitize HTML
    input.replace(">", "");
    Serial.print(" Transmitting (Web): ");
    Serial.println(input);
    sendMessage(input);               // Transmit message using LED
    server.send(200, "text/html", "<h2> Message Sent!</h2><a href='/'> Back</a>");
  } else {
    server.send(400, "text/html", "<h2> Message Missing</h2><a href='/'> Back</a>");
  }
}

// Transmit the full message string, character by character
void sendMessage(const String& message) {
  for (unsigned int i = 0; i < message.length(); i++) {
    sendByte(message[i]); // Send one character at a time
  }
}

// Convert a character to bits and send each bit using LED
void sendByte(char c) {
  Serial.print("Sending '");
  Serial.print(c);
  Serial.print("' → ");

  sendBit(0);  // Send start bit (always 0)

  for (int i = 0; i < 8; i++) {
    bool bit = bitRead(c, i); // Read each bit (LSB first)
    Serial.print(bit);
    sendBit(bit);             // Send each bit as light pulse
  }

  sendBit(1);  // Send stop bit (always 1)
  Serial.println();
}

// Send a single bit as ON/OFF LED pulse
void sendBit(bool bitVal) {
  digitalWrite(LED_PIN, bitVal ? HIGH : LOW); // HIGH for 1, LOW for 0
  delay(BIT_DURATION);                        // Wait for bit duration
}
