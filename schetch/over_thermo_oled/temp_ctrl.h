///
/// \brief Temperature control object
/// The object has a setpoint and confront it with the current temperature value.
/// The tick function must be called with a timer and set the PWM out.
///

#ifndef TEMP_CTRL_H
#define TEMP_CTRL_H

///
/// \brief Temperature control class
///
class TempCtrl
{
public:
  TempCtrl(const int m_out_pin=LED_BUILTIN, int pwm_steps=100, float setpoint=50.0f);

  ///
  /// \brief read setpoint
  ///
  float getSetpoint() { return m_setpoint; }
  ///
  /// \prief get PWM level
  ///
  int getPwm();
  ///
  /// \brief write setpoint
  ///
  float setSetpoint(float new_val);
  float setSetpoint(int new_val) {
    if (new_val < m_isetp_min) { new_val = m_isetp_min; }
    if (new_val > m_isetp_max) { new_val = m_isetp_max; }
    return setSetpoint((float)new_val);
  }
  ///
  /// \brief Do work
  ///
  void doWork(float new_temp);
  ///
  /// \brief tick callback
  ///
  void doTick();
  ///
  /// \brief start PWM
  ///
  void startPwm();
  ///
  /// \brief stop PWM
  ///
  void stopPwm();

private:
  const int m_out_pin;              //! Output PIN
  const int m_pwm_steps;            //! PWM steps
  float     m_setpoint;             //! The temperature setpoint 

private:
  int       m_pwm_cnt     = 0;      //! Step counter
  int       m_pwm_level   = 0;      //! Out PWM level
  float     m_coeff_m     = 2.5f;   //! coefficiente m
  float     m_coeff_q     = -0.5f;  //! coefficiente q
  bool      m_is_running  = true;   //! PWM is running
  float     m_setp_min    = 10.0f;  //! The temperature setpoint 
  float     m_setp_max    = 300.0f; //! The temperature setpoint 
  int       m_isetp_min   = 10;     //! The temperature setpoint 
  int       m_isetp_max   = 300;    //! The temperature setpoint 
};



#endif
