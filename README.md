# arduino-oven-thermometer
Arduino oven thermometer with LCD

## Specification
Over thermometer, useful to check the temperature of the oven. There are two sensors, one for low temperature ( < 100 °C) and one for higher one. The selection is made automatically. The display also shown the time since the gauge is powered to be used as timer. No programming menu interface is available, it works just as it is.

## Library used
For LCD:
 - Adafruit_PCD8544 NOKIA 5110 LCD Library
 - Adafruit_GFX
 - Adafruit BusIO
For Low temperature sensor
 - DallasTemperature DS18B20
 - OneWire
For the thermocouple
 - Max6675 Library

## Possible expansions
Add a Bluetooth UART interface to allow the setup of the gauge.
Add a buzzer to alert the possible temperature alarm as to count the time.