
#ifndef __IVANSENSORE_DS18B20__
#define __IVANSENSORE_DS18B20__

#include <DallasTemperature.h>
#include <OneWire.h>

class IvanSensore_DS18B20
{
public:
  IvanSensore_DS18B20(int pin, int res, bool m_simulator);

  float readTemperatureC();

private:
  bool startup();
  bool is_good();

private:
  OneWire            m_bus;
  DallasTemperature  m_sensor;
  int                m_count;
  int                m_error;
  int                m_resolution;
  bool               m_simulator;
  DeviceAddress      m_add;
};

#endif

