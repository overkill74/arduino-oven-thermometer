
#include <Arduino.h>
#include "temp_ctrl.h"

//#define DEBUG_ME

#ifdef DEBUG_ME
#define DBG_PRINT(...) Serial.print(__VA_ARGS__)
#define DBG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define DBG_PRINT(...)
#define DBG_PRINTLN(...)
#endif

//-------------------------------------------------------------------
TempCtrl::TempCtrl(const int m_out_pin, int pwm_steps, float setpoint)
  : m_out_pin(m_out_pin)
  , m_pwm_steps(pwm_steps)
  , m_setpoint(setpoint)

{
  DBG_PRINTLN("TempCtrl Init!");
}
//-------------------------------------------------------------------
int TempCtrl::getPwm()
{
  if (!m_is_running) {
    return 0;
  }
  return m_pwm_level;
}
//-------------------------------------------------------------------
void TempCtrl::startPwm()
{
  if (m_is_running) {
    return;
  }
  m_pwm_cnt = 0;
  m_is_running = true;
}
//-------------------------------------------------------------------
void TempCtrl::stopPwm()
{
  m_is_running = false;
  m_pwm_cnt = 0;
  digitalWrite(m_out_pin, LOW);
}
//-------------------------------------------------------------------
float TempCtrl::setSetpoint(float new_val)
{
  if (isnan(new_val)) { new_val = 25.0f; }

  Serial.print("New: "); Serial.println(new_val);
  Serial.print("Max: "); Serial.println(m_setp_min);
  Serial.print("Min: "); Serial.println(m_setp_max);
  
  if (new_val < m_setp_min) { new_val = m_setp_min; }
  if (new_val > m_setp_max) { new_val = m_setp_max; }
  m_setpoint = new_val;
  Serial.print("SP : "); Serial.println(m_setpoint);
  return m_setpoint;
}
//-------------------------------------------------------------------
void TempCtrl::doWork(float new_temp)
{
  startPwm();

  float delta = m_setpoint - new_temp;
  DBG_PRINT("Delta= ");   DBG_PRINTLN(delta);

  int pwm = (int)(delta * m_coeff_m + m_coeff_q);
  if (pwm >= 100) {
    pwm = 100;
  }
  if (pwm <= 0) {
    pwm = 0;
  }
  m_pwm_level = pwm;
  DBG_PRINT("PWM= ");   DBG_PRINTLN(m_pwm_level);
}
//-------------------------------------------------------------------
void TempCtrl::doTick()
{
  if (!m_is_running) {
    digitalWrite(m_out_pin, LOW);
    return;
  }

  if (++m_pwm_cnt > 100) {
    m_pwm_cnt = 0;
  }

  if (m_pwm_level >= 98) {
    digitalWrite(m_out_pin, HIGH);
  }
  else if (m_pwm_level <= 2) {
    digitalWrite(m_out_pin, LOW);
  }
  else {
    digitalWrite(m_out_pin, (m_pwm_cnt < m_pwm_level));
  }
}
