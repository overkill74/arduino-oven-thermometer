/*********************************************************************
  This is an example sketch for our Monochrome Nokia 5110 LCD Displays

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/338

  These displays use SPI to communicate, 4 or 5 pins are required to
  interface

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada  for Adafruit Industries.
  BSD license, check license.txt for more information
  All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#include <max6675.h>

//-----------------------------------------------------------------------
#include <DallasTemperature.h>
#include <OneWire.h>
#include "IvanSensore_DS18B20.h"

//-----------------------------------------------------------------------
// Confiugratore dello sketch
// define to enable SD log
//#define APP_LOG_ON_SD

// true or false to enable temp simulators
#define APP_SIMULATORE false

// define to enable modbus, undef output on Serial
#define APP_MODBUS

// define pseudo frequency of main loop
#define APP_FRQ_HZ   50
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// PIN ASSIGNMENT
//-----------------------------------------------------------------------
// One Wire
#define ONE_WIRE_BUS_MANDATA    8
#define ONE_WIRE_PIN_VCC        9
#define ONE_WIRE_PIN_GND        10

// Thermocouple
#define TC_CLK                  11
#define TC_CS                   12
#define TC_SDO                  13

// LCD
#define LCD_CLK                 7
#define LCD_DIN                 6
#define LCD_DC                  5
#define LCD_CS                  4
#define LCD_RST                 3
#define BKL_PIN                 2

#define ONE_WIRE_RESOLUTION     12

//-----------------------------------------------------------------------
static IvanSensore_DS18B20 m_sens_temp(ONE_WIRE_BUS_MANDATA, ONE_WIRE_RESOLUTION, APP_SIMULATORE);
static MAX6675 m_thermocouple(TC_CLK, TC_CS, TC_SDO);
// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(LCD_CLK, LCD_DIN, LCD_DC, LCD_CS, LCD_RST);

//-----------------------------------------------------------------------
static void stampa_tempo(int linea)
{
  unsigned long runMillis = millis();
  unsigned long allSeconds = millis() / 1000;
  int runHours = allSeconds / 3600;
  int secsRemaining = allSeconds % 3600;
  int runMinutes = secsRemaining / 60;
  int runSeconds = secsRemaining % 60;

  char buf[32];
  sprintf(buf, "%d:%02d:%02d", runHours, runMinutes, runSeconds);
  Serial.println(buf);

  display.setCursor(0, linea);
  display.setTextSize(2);
  display.print(buf);
}

//-----------------------------------------------------------------------
static void stampa_temperatura(float celsius, int line, bool is_tc)
{
  static bool is_high = false;
  bool is_error = false;
  int x = 0;

  if (celsius > 999.0 || celsius < -99.0) {
    is_error = true;
  }

  if (celsius >= 0.0) {
    // Histeresys positive value
    if (is_high && celsius < 90.0) {
      is_high = false;
    }
    if (!is_high && celsius >= 100.0) {
      is_high = true;
    }
  }
  else {
    // Histeresys negative value
    if (is_high && celsius >= -9.0) {
      is_high = false;
    }
    if (!is_high && celsius <= -10.0) {
      is_high = true;
    }
  }

  String tmp;
  if (is_error) {
    // Errore
    tmp = "Err!";
    x = 4;
  }
  else {
    if (is_high) {
      // 100
      int c = (int)celsius;
      tmp = String(c);
      if (c < 100) {
        x = 36;
      }
      else {
        x = 18;
      }
    }
    else {
      // 25.1
      tmp = String(celsius, 1);
      if (celsius >= 0.0 && celsius <= 9.99) {
        x = 18;
      }
    }
  }
  Serial.println(tmp);

  // text display tests
  display.setTextColor(BLACK);
  display.setTextSize(3);

  // Draw temperature
  display.setCursor(x, line);
  display.print(tmp);

  // Draw symbol
  if (!is_error) {
    display.setCursor(72, line);
    display.setTextSize(2);
    display.println("C");
  }

  // TC symbol
  if (is_tc) {
    display.setTextSize(1);
    display.setCursor(72, line+16);
    display.println("TC");
  }
}

//-----------------------------------------------------------------------
static float read_tc()
{
  static float old_tc = -10000.0f;

  float tc = m_thermocouple.readCelsius();

  if (old_tc < -1000.0f) {
    // init value
    old_tc = tc;
  }
  else {
    old_tc = old_tc * 0.6 + tc * 0.4;
  }
  
  return old_tc;
}

//-----------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);

  Serial.println("Init Temperature");
  pinMode(ONE_WIRE_PIN_VCC, OUTPUT);
  pinMode(ONE_WIRE_PIN_GND, OUTPUT);

  digitalWrite(ONE_WIRE_PIN_VCC, HIGH);
  digitalWrite(ONE_WIRE_PIN_GND, LOW);

  pinMode(BKL_PIN, OUTPUT);
  digitalWrite(BKL_PIN, HIGH);

  Serial.println("Init Display");
//  display.clearDisplay();
  display.begin();
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(45);

  // Backlight
  digitalWrite(BKL_PIN, LOW);

  // Init sensor
  for (int x = 0; x < 10; ++x) {
    m_sens_temp.readTemperatureC();
  }
  
  return;
}

//-----------------------------------------------------------------------
void loop()
{
  float temperatura = m_sens_temp.readTemperatureC();
  float temp_tc = read_tc();

  Serial.print("Temperatura: ");
  Serial.println(temperatura);

  Serial.print("Termocoppia: ");
  Serial.println(temp_tc);

  // Clear display
  display.clearDisplay();

  // Select sensor to display
  static bool is_tc = false;
  if (!is_tc) {
    if (temp_tc > temperatura + 30) {
      is_tc = true;
    }
  }
  else {
    if (temp_tc < temperatura + 10) {
      is_tc = false;
    }
  }

  float temp = is_tc ? temp_tc : temperatura;
  stampa_temperatura(temp, 4, is_tc);
  stampa_tempo(32);

  // Update display
  display.display();

  delay(500);
}
