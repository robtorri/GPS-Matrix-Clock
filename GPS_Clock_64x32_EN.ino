// GPS Clock 64x32 Matrix RGB Leds
// It uses Adafruit GFX Library, Timezone Library by J Christensen
// and Tiny GPS++ for the NEO 6-M GPS Module
// Adapted by Roberto Torri 
#include <EEPROM.h>
#include <Adafruit_GFX.h>        // Core graphics library
#include <Fonts\Ch_5x7.h>   // Custom font for the date
#include <Fonts\Ch_12x9.h>  // Custom font for the time
#include <RGBmatrixPanel.h>      // Hardware-specific library
#define CLK 11                   // USE THIS ON ARDUINO MEGA
#define OE 9
#define LAT 10
#define A A0
#define B A1
#define C A2
#define D A3
//
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);
//
#include <Timezone.h>
#include "TinyGPS++.h"
TinyGPSPlus gps;
String list_days[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
String list_months[12] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };
// Change these two rules corresponding to your timezone, see https://github.com/JChristensen/Timezone
//Central European Time (Frankfurt, Paris)  120 = +2 hours in daylight saving time (summer).
TimeChangeRule CEST = { "CEST", Last, Sun, Mar, 2, 120 };
//Central European Time (Frankfurt, Paris)  60  = +1 hour in normal time (winter)
TimeChangeRule CET = { "CET ", Last, Sun, Oct, 3, 60 };
Timezone CE(CEST, CET);
// time variables
time_t local, utc, prev_set;
int timesetinterval = 60;  //set microcontroller time every 60 seconds
char str[3];
// Variables for the time and date
int module = 99;
int hourx10 = 99;
int hourx1 = 99;
int minutex10 = 99;
int minutex1 = 99;
int secondx10 = 99;
int secondx1 = 99;
int gg = 99;
int gweek = 99;
int posit;  // variable to position the weekday
bool btn4;  // variable for the pushbutton on input 4 (Red)
bool btn5;  // variable for the pushbutton on input 5 (Green)
bool btn6;  // variable for the pushbutton on input 6 (Blue)
int col_red;
int col_green;
int col_blue;
bool onlyOne = true;
void setup(void) {
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  Serial1.begin(9600);  // TX  module GPS goes to 19  Mega2560 board - RX  module goes to 18 Mega2560 board
  delay(200);
  //Serial.begin(9600);
  //delay(200);
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.fillRect(0, 0, 64, 32, matrix.Color333(0, 0, 0));  // illRect(int16_t x, int16_t y, int16_t w, int16_t, color)
  while (!gps.time.isValid())                               // wait a valid GPS UTC time (valid is defined in the TinyGPS++ library)
  {
    smartDelay(100);
  }
  setthetime();
  prev_set = now();
  matrix.fillRect(0, 0, 64, 32, matrix.Color333(0, 0, 0));  // illRect(int16_t x, int16_t y, int16_t w, int16_t, color)
  delay(50);
  col_red = EEPROM.read(0);
  delay(50);
  col_green = EEPROM.read(1);
  delay(50);
  col_blue = EEPROM.read(2);
  delay(50);
  if (col_red + col_green + col_blue >= 22 || col_red + col_green + col_blue == 0) {
    col_red = 0;
    col_green = 0;
    col_blue = 2;
  }
  // if you want to show a splash screen by Adafruit press and hold button 4 before to power up
  btn4 = digitalRead(4);
  if(btn4 == LOW) {
    splshScr();
    delay(2000);
    matrix.fillRect(0, 0, 64, 32, matrix.Color333(0, 0, 0));
  }
  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color333(1, 0, 0));
  matrix.setCursor(10, 10);
  matrix.print("WAIT...!");
}
void loop() {
  if (now() - prev_set > timesetinterval && gps.time.isValid())  // set the microcontroller time every interval, only if there is a valid GPS time
  {
    setthetime();
    prev_set = now();

    smartDelay(100);
  }
  while(year() > 2023) {
displaythetime();
  }
  
  smartDelay(100);  // update the time every second

}

static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    // If data has come in from the GPS module
    while (Serial1.available())
      gps.encode(Serial1.read());  // Send it to the encode function
    // tinyGPS.encode(char) continues to "load" the tinGPS object with new
    // data coming in from the GPS module. As full NMEA strings begin to come in
    // the tinyGPS library will be able to start parsing them for pertinent info

  } while (millis() - start < ms);
}

void setthetime(void) {
  int Year = gps.date.year();
  byte Month = gps.date.month();
  byte Day = gps.date.day();
  byte Hour = gps.time.hour();
  byte Minute = gps.time.minute();
  byte Second = gps.time.second();

  // Set Time from GPS data string
  setTime(Hour, Minute, Second, Day, Month, Year);  // set the time of the microcontroller to the UTC time from the GPS
}
void displaythetime(void) {
  utc = now();  // read the time in the correct format to change via the TimeChangeRules
  local = CE.toLocal(utc);
  //Serial.print("Ora locale: "); Serial.print(hour(local)); Serial.print(":"); Serial.print(minute(local)); Serial.print(":"); Serial.print(second(local)); Serial.print("  -  Ora UTC: ");
  //Serial.print(hour(utc)); Serial.print(":"); Serial.print(minute(utc)); Serial.print(":"); Serial.println(second(utc));
  //Serial.print(day(local)); Serial.print(" "); Serial.print (month(local)); Serial.print(" "); Serial.print(year(local)); Serial.println("");
  //Serial.println(weekday());
if(onlyOne == true) {
     matrix.fillRect(0, 0, 64, 32, matrix.Color333(0, 0, 0));
onlyOne = false;
}
   // ************* Week day ********************************
  if (gweek != weekday(local)) {
    matrix.setFont(NULL);
    matrix.fillRect(0, 0, 64, 8, matrix.Color333(0, 0, 0));
    matrix.setTextSize(1);
    matrix.setTextColor(matrix.Color333(col_red, col_green, col_blue));
    if (weekday(local) == 1 || weekday(local) == 2 || weekday(local) == 6) {
      posit = 14;
    }
    if (weekday(local) == 3) {
      posit = 12;
    }
    if (weekday(local) == 4) {
      posit = 5;
    }
    if (weekday(local) == 5 || weekday(local) == 7) {
      posit = 8;
    }

    matrix.setCursor(posit, 0);
    matrix.print(list_days[weekday(local) - 1]);
    gweek = weekday(local);
  }

  // *************** Date ****************************************

  int dayx10 = day(local) / 10;
  int dayx1 = day(local) - (dayx10 * 10);

  if (gg != day(local)) {
    matrix.setFont(&Ch_5x7);
    matrix.fillRect(0, 24, 64, 7, matrix.Color333(0, 0, 0));  // fillRect(int16_t x, int16_t y, int16_t w, int16_t h,uint16_t color)
    matrix.setTextColor(matrix.Color333(col_red, col_green, col_blue));
    if (dayx10 == 0) {
      matrix.fillRect(3, 24, 5, 7, matrix.Color333(0, 0, 0));  // fillRect(int16_t x, int16_t y, int16_t w, int16_t h,uint16_t color)
    } else {
      matrix.setCursor(3, 30);
      matrix.print(dayx10);
    }

    matrix.setCursor(9, 30);
    matrix.print(dayx1);
    matrix.setCursor(17, 30);
    matrix.print(list_months[month(local) - 1]);
    matrix.setCursor(38, 30);
    matrix.print(year());
    gg = day(local);
  }
  // ********************* Time *****************************************************************
  int hrx10 = hour(local) / 10;
  int hrx1 = hour(local) - (hrx10 * 10);
  int minx10 = minute(local) / 10;
  int minx1 = minute(local) - (minx10 * 10);
  int secx10 = second(local) / 10;
  int secx1 = second(local) - (secx10 * 10);
  // *********************** Display time **********************************************************
  // *********************** Hour *******************************************************************
  if (hourx10 != hrx10) {
    if (hrx10 == 0) {
      matrix.fillRect(3, 9, 10, 15, matrix.Color333(0, 0, 0));
      hourx10 = hrx10;
    } else {
      matrix.fillRect(3, 9, 10, 15, matrix.Color333(0, 0, 0));  // fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
      matrix.setTextColor(matrix.Color333(col_red, col_green, col_blue));
      matrix.setFont(&Ch_12x9);
      matrix.setCursor(3, 21);
      matrix.print(hrx10);
      hourx10 = hrx10;
    }
  }
  if (hourx1 != hrx1) {
    matrix.fillRect(13, 9, 10, 15, matrix.Color333(0, 0, 0));
    matrix.setTextColor(matrix.Color333(col_red, col_green, col_blue));
    matrix.setFont(&Ch_12x9);
    matrix.setCursor(13, 21);
    matrix.print(hrx1);
    hourx1 = hrx1;
  }
  // **************** Print separation dots *****************************************
  if (module != secx1) {
    if (secx1 % 2 == 0) {
      matrix.fillRect(23, 9, 4, 15, matrix.Color333(0, 0, 0));
      matrix.setTextColor(matrix.Color333(col_red, col_green, col_blue));
      matrix.setFont(&Ch_12x9);
      matrix.setCursor(23, 21);
      matrix.print(":");
    } else {
      matrix.fillRect(23, 9, 4, 15, matrix.Color333(0, 0, 0));
      matrix.setTextColor(matrix.Color333(col_red, col_green, col_blue));
      matrix.setFont(&Ch_12x9);
      matrix.setCursor(23, 21);
      matrix.print(";");
    }
    module = secx1;
  }
  // *************** Minute **************************************************
  if (minutex10 != minx10) {
    matrix.fillRect(28, 9, 10, 15, matrix.Color333(0, 0, 0));
    matrix.setTextColor(matrix.Color333(col_red, col_green, col_blue));
    matrix.setFont(&Ch_12x9);
    matrix.setCursor(28, 21);
    matrix.print(minx10);
    minutex10 = minx10;
  }
  if (minutex1 != minx1) {
    matrix.fillRect(39, 9, 10, 15, matrix.Color333(0, 0, 0));
    matrix.setTextColor(matrix.Color333(col_red, col_green, col_blue));
    matrix.setFont(&Ch_12x9);
    matrix.setCursor(39, 21);
    matrix.print(minx1);
    minutex1 = minx1;
  }

  // ********************* Second ********************************************
  if (secondx10 != secx10) {
    matrix.setFont(&Ch_5x7);
    matrix.fillRect(52, 15, 5, 7, matrix.Color333(0, 0, 0));
    matrix.setTextColor(matrix.Color333(col_red, col_green, col_blue));
    matrix.setTextSize(1);
    matrix.setCursor(52, 21);
    matrix.print(secx10);
    secondx10 = secx10;
  }
  if (secondx1 != secx1) {
    matrix.setFont(&Ch_5x7);
    matrix.fillRect(58, 15, 5, 7, matrix.Color333(0, 0, 0));
    matrix.setTextColor(matrix.Color333(col_red, col_green, col_blue));
    matrix.setTextSize(1);
    matrix.setCursor(58, 21);
    matrix.print(secx1);
    secondx1 = secx1;
  }
  // °°°°°°°°°°°°°°° Control buttons to set RGB Colours ********************************
  btn4 = digitalRead(4);
  if (btn4 == LOW) {
    col_red++;
    if (col_red > 7) {
      col_red = 0;
    }
    EEPROM.write(0, col_red);  // Store value colour in EEPROM
    delay(80);
    resvar();  // Variables reset
  }
  btn5 = digitalRead(5);
  if (btn5 == LOW) {
    col_green++;
    if (col_green > 7) {
      col_green = 0;
    }
    EEPROM.write(1, col_green);  // Store value colour in EEPROM
    delay(80);
    resvar();  // Variables reset
  }
  btn6 = digitalRead(6);
  if (btn6 == LOW) {
    col_blue++;
    if (col_blue > 7) {
      col_blue = 0;
    }
    EEPROM.write(2, col_blue);  // Store value colour in EEPROM
    delay(80);
    resvar();
  }

 if (btn4 == LOW && btn5 == LOW) {  // If pressed together change the colour in yellow
    col_red = 1;
    col_green = 1;
    col_blue = 0;
    EEPROM.write(0, col_red);  // Store value colour in EEPROM
    delay(50);
    EEPROM.write(1, col_green);  // Store value colour in EEPROM
    delay(50);
    EEPROM.write(2, col_green);  // Store value colour in EEPROM
    delay(50);
    resvar();  // Variables reset
  }
  if (btn5 == LOW && btn6 == LOW) {  // If pressed together change the colour in white
    col_red = 1;
    col_green = 1;
    col_blue = 1;
    EEPROM.write(0, col_red);  // Store value colour in EEPROM
    delay(50);
    EEPROM.write(1, col_green);  // Store value colour in EEPROM
    delay(50);
    EEPROM.write(2, col_green);  // Store value colour in EEPROM
    delay(50);
    resvar();  // Variables reset
  }

  if (btn4 == LOW && btn6 == LOW) {  // Pressed together they turn off the display
    col_red = 0;
    col_green = 0;
    col_blue = 0;
    resvar();  // Variables reset
  }

}

  // **************************************************************************************
 
// Variables reset routine
void resvar() {
  module = 99;
  hourx10 = 99;
  hourx1 = 99;
  minutex10 = 99;
  minutex1 = 99;
  secondx10 = 99;
  secondx1 = 99;
  gg = 99;
  gweek = 99;
}
// ************************* Splash screen *******************************************************
void splshScr() {
  // draw a pixel in solid white
  matrix.drawPixel(0, 0, matrix.Color333(7, 7, 7));
  delay(1500);

  // fix the screen with green
  matrix.fillRect(0, 0, matrix.width(), matrix.height(), matrix.Color333(0, 7, 0));
  delay(1500);

  // draw a box in yellow
  matrix.drawRect(0, 0, matrix.width(), matrix.height(), matrix.Color333(7, 7, 0));
  delay(1500);

  // draw an 'X' in red
  matrix.drawLine(0, 0, matrix.width() - 1, matrix.height() - 1, matrix.Color333(7, 0, 0));
  matrix.drawLine(matrix.width() - 1, 0, 0, matrix.height() - 1, matrix.Color333(7, 0, 0));
  delay(1500);

  // draw a blue circle
  matrix.drawCircle(10, 10, 10, matrix.Color333(0, 0, 7));
  delay(1500);

  // fill a violet circle
  matrix.fillCircle(40, 21, 10, matrix.Color333(7, 0, 7));
  delay(1500);

  // fill the screen with 'black'
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  // draw some text!
  matrix.setTextSize(1);      // size 1 == 8 pixels high
  matrix.setTextWrap(false);  // Don't wrap at end of line - will do ourselves

  matrix.setCursor(8, 0);  // start at top left, with 8 pixel of spacing
  uint8_t w = 0;
  char *str = "AdafruitIndustries";
  for (w = 0; w < 8; w++) {
    matrix.setTextColor(Wheel(w));
    matrix.print(str[w]);
  }
  matrix.setCursor(2, 8);  // next line
  for (w = 8; w < 18; w++) {
    matrix.setTextColor(Wheel(w));
    matrix.print(str[w]);
  }
  matrix.println();
  //matrix.setTextColor(matrix.Color333(4,4,4));
  //matrix.println("Industries");
  matrix.setTextColor(matrix.Color333(7, 7, 7));
  matrix.println("LED MATRIX!");

  // print each letter with a rainbow color
  matrix.setTextColor(matrix.Color333(7, 0, 0));
  matrix.print('3');
  matrix.setTextColor(matrix.Color333(7, 4, 0));
  matrix.print('2');
  matrix.setTextColor(matrix.Color333(7, 7, 0));
  matrix.print('x');
  matrix.setTextColor(matrix.Color333(4, 7, 0));
  matrix.print('6');
  matrix.setTextColor(matrix.Color333(0, 7, 0));
  matrix.print('4');
  matrix.setCursor(34, 24);
  matrix.setTextColor(matrix.Color333(0, 7, 7));
  matrix.print('*');
  matrix.setTextColor(matrix.Color333(0, 4, 7));
  matrix.print('R');
  matrix.setTextColor(matrix.Color333(0, 0, 7));
  matrix.print('G');
  matrix.setTextColor(matrix.Color333(4, 0, 7));
  matrix.print('B');
  matrix.setTextColor(matrix.Color333(7, 0, 4));
  matrix.print('*');

  // whew!
}

// Input a value 0 to 24 to get a color value.
// The colours are a transition r - g - b - back to r.
uint16_t Wheel(byte WheelPos) {
  if (WheelPos < 8) {
    return matrix.Color333(7 - WheelPos, WheelPos, 0);
  } else if (WheelPos < 16) {
    WheelPos -= 8;
    return matrix.Color333(0, 7 - WheelPos, WheelPos);
  } else {
    WheelPos -= 16;
    return matrix.Color333(WheelPos, 0, 7 - WheelPos);
  }
  delay(2000);
  matrix.fillRect(0, 0, 64, 32, matrix.Color333(0, 0, 0));  // illRect(int16_t x, int16_t y, int16_t w, int16_t, color)
}
