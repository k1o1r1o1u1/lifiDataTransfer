#include <ESP8266WiFi.h>        
#include <ESP8266WebServer.h> 

#define LED_PIN D6             
#define BIT_DURATION 130        

const char* ssid = "LiFi_AP";     
const char* password = "12345678";

ESP8266WebServer server(80);   
String serialBuffer;             

void setup() {
  pinMode(LED_PIN, OUTPUT);      
  digitalWrite(LED_PIN, HIGH);   

  Serial.begin(115200);         
  Serial.println("Li‑Fi Transmitter Ready");
  Serial.println("Type a message below and press ENTER to send via Serial:");

  
  WiFi.softAP(ssid, password);


  server.on("/", handleRoot);
  server.on("/send", handleSend);
  
  server.begin();               
}

void loop() {
  server.handleClient();         

 
  if (Serial.available()) {
    char incoming = Serial.read();  

    if (incoming == '\n') {         
      serialBuffer.trim();         
      if (serialBuffer.length() > 0) {
        Serial.print("🖥️ Transmitting (Serial): ");
        Serial.println(serialBuffer);
        sendMessage(serialBuffer);  
      }
      serialBuffer = "";          
    } else {
      serialBuffer += incoming;     
    }
  }
}


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


void handleSend() {
  if (server.hasArg("msg")) {
    String input = server.arg("msg"); 
    input.trim();                     
    input.replace("<", "");          
    input.replace(">", "");
    Serial.print(" Transmitting (Web): ");
    Serial.println(input);
    sendMessage(input);               
    server.send(200, "text/html", "<h2> Message Sent!</h2><a href='/'> Back</a>");
  } else {
    server.send(400, "text/html", "<h2> Message Missing</h2><a href='/'> Back</a>");
  }
}


void sendMessage(const String& message) {
  for (unsigned int i = 0; i < message.length(); i++) {
    sendByte(message[i]); 
  }
}


void sendByte(char c) {
  Serial.print("Sending '");
  Serial.print(c);
  Serial.print("' → ");

  sendBit(0);  

  for (int i = 0; i < 8; i++) {
    bool bit = bitRead(c, i); 
    Serial.print(bit);
    sendBit(bit);             
  }

  sendBit(1); 
  Serial.println();
}


void sendBit(bool bitVal) {
  digitalWrite(LED_PIN, bitVal ? HIGH : LOW); 
  delay(BIT_DURATION);                       
}
