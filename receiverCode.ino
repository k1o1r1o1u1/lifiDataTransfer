

#include <U8g2lib.h>  


#define SOLAR_PIN A3


#define SAMPLING_TIME 130  // ms

#define THRESHOLD 500    


U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);


bool previous_state = true;
bool current_state = true;


String displayLine = "";
const int maxLineLength = 18;  

void setup() {
  Serial.begin(9600);     

  u8g2.begin();             
  u8g2.clearBuffer();       
  u8g2.setFont(u8g2_font_ncenB08_tr);  
  u8g2.drawStr(5, 20, "Hello LiFi");   
  u8g2.sendBuffer();        
  delay(2000);             

  u8g2.clearBuffer();       
  u8g2.sendBuffer();
}

void loop() {
  
  current_state = get_solar();

 
  if (!current_state && previous_state) {
    char received = get_byte();     
    Serial.print(received);         


    static const int lineHeight = 10;     
    static const int leftMargin = 5;      
    static const int maxCols = 19;        
    static const int maxLines = 6;       
    static String lines[maxLines];      
    static int currentLine = 0;             
    if (received == '#') {
      
      for (int i = 0; i < maxLines; i++) lines[i] = "";
      currentLine = 0;
      currentCol = 0;
      u8g2.clearBuffer();
      u8g2.sendBuffer();
    } else {
      
      lines[currentLine] += received;
      currentCol++;

      
      if (currentCol >= maxCols) {
        currentLine++;
        currentCol = 0;

        
        if (currentLine >= maxLines) {
          currentLine = 0;
        }
        lines[currentLine] = ""; 
      }

      
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_ncenB08_tr);
      for (int i = 0; i < maxLines; i++) {
        if (lines[i].length() > 0) {
          
          u8g2.drawStr(leftMargin, (i + 1) * lineHeight, lines[i].c_str());
        }
      }
      u8g2.sendBuffer();  
    }
  }

 
  previous_state = current_state;
}


bool get_solar() {
  int lightLevel = analogRead(SOLAR_PIN); 

  return (lightLevel > THRESHOLD);        
}


char get_byte() {
  char data_byte = 0;


  delay(SAMPLING_TIME * 1.5);


  for (int i = 0; i < 8; i++) {
    bool bit = get_solar();
    data_byte |= (bit << i);           
    delay(SAMPLING_TIME);              
  }

  return data_byte; 
}
