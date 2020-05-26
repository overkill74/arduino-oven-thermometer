
#include "IvanSensore_DS18B20.h"

//-----------------------------------------------------------------------
IvanSensore_DS18B20::IvanSensore_DS18B20(int pin, int res, bool simula)
  : m_bus(pin)
  , m_sensor(&m_bus)
  , m_count(0)
  , m_error(0)
  , m_resolution(res)
  , m_simulator(simula)
{
}

//-----------------------------------------------------------------------
bool IvanSensore_DS18B20::startup()
{
  m_sensor.begin();
  m_count = m_sensor.getDeviceCount();
  if (m_count) {
    m_sensor.getAddress(m_add, 0);
    m_sensor.setResolution(m_add, m_resolution);
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------
bool IvanSensore_DS18B20::is_good() {
  if (m_count > 0) return true;
  return false;
}

//-----------------------------------------------------------------------
float IvanSensore_DS18B20::readTemperatureC()
{
  if (m_simulator) {
    return random(7000, 9000) / 100.0f;
  }

  if (!is_good()) startup();

  m_sensor.requestTemperatures(); // Send the command to get temperatures
  delay(10);
  float temp = m_sensor.getTempC(m_add);
  if (temp < -125.0f) {
    m_error++;
    if (m_error > 10) {
      m_count = 0;
      m_error = 100;
    }
  }
  else m_error = 0;

  return temp * 1.0f;
}
