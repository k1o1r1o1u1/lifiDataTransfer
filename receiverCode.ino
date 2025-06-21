// ARDUINO LIFI RECEIVER WITH OLED (U8g2lib) & SOLAR PANEL

#include <U8g2lib.h>  // OLED display library

// Define analog pin for solar panel/light sensor
#define SOLAR_PIN A3

// Duration of each bit (must match transmitter)
#define SAMPLING_TIME 130  // milliseconds

// Light threshold to distinguish LED ON vs OFF
#define THRESHOLD 500       // Tune based on ambient light

// Create OLED object for 128x64 screen using hardware I2C
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Variables to detect falling edge (start bit)
bool previous_state = true;
bool current_state = true;

// Current line content to be displayed
String displayLine = "";
const int maxLineLength = 18;  // Max chars per line for chosen font

void setup() {
  Serial.begin(9600);       // Start Serial for debugging

  u8g2.begin();             // Initialize OLED
  u8g2.clearBuffer();       // Clear display memory
  u8g2.setFont(u8g2_font_ncenB08_tr);  // Set readable font
  u8g2.drawStr(5, 20, "Hello LiFi");   // Splash screen text
  u8g2.sendBuffer();        // Push to screen
  delay(2000);              // Wait 2 seconds

  u8g2.clearBuffer();       // Clear splash
  u8g2.sendBuffer();
}

void loop() {
  // Read current light state
  current_state = get_solar();

  // Detect falling edge → start bit detected
  if (!current_state && previous_state) {
    char received = get_byte();      // Read 1 full byte from light signal
    Serial.print(received);          // Print it to serial for debugging

    // --- OLED LINE DISPLAY LOGIC ---
    static const int lineHeight = 10;     // Pixel height per line
    static const int leftMargin = 5;      // Left indent in pixels
    static const int maxCols = 19;        // Max characters per line
    static const int maxLines = 6;        // OLED can hold ~6 lines
    static String lines[maxLines];        // Store each line of text
    static int currentLine = 0;           // Current row index
    static int currentCol = 0;            // Character position in current line

    if (received == '#') {
      // Special character: Clear screen and reset lines
      for (int i = 0; i < maxLines; i++) lines[i] = "";
      currentLine = 0;
      currentCol = 0;
      u8g2.clearBuffer();
      u8g2.sendBuffer();
    } else {
      // Add received char to current line
      lines[currentLine] += received;
      currentCol++;

      // If line full, go to next line
      if (currentCol >= maxCols) {
        currentLine++;
        currentCol = 0;

        // If screen is full, wrap around to the top
        if (currentLine >= maxLines) {
          currentLine = 0;
        }
        lines[currentLine] = "";  // Clear new line for reuse
      }

      // Draw all lines to OLED
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_ncenB08_tr);
      for (int i = 0; i < maxLines; i++) {
        if (lines[i].length() > 0) {
          // Draw line i at vertical position (i+1) * lineHeight
          u8g2.drawStr(leftMargin, (i + 1) * lineHeight, lines[i].c_str());
        }
      }
      u8g2.sendBuffer();  // Push updates to screen
    }
  }

  // Save the current state for edge detection in next loop
  previous_state = current_state;
}

// Function to determine if LED light is ON or OFF
bool get_solar() {
  int lightLevel = analogRead(SOLAR_PIN);  // Read light intensity
  // Serial.println(lightLevel);           // Uncomment for debugging
  return (lightLevel > THRESHOLD);         // Return true if light is ON
}

// Function to receive a byte from light signal
char get_byte() {
  char data_byte = 0;

  // Align sampling to middle of first data bit
  delay(SAMPLING_TIME * 1.5);

  // Read 8 data bits, LSB first
  for (int i = 0; i < 8; i++) {
    bool bit = get_solar();             // Read bit as light level
    data_byte |= (bit << i);            // Add bit to correct position
    delay(SAMPLING_TIME);               // Wait for next bit duration
  }

  return data_byte;  // Return constructed character
}
