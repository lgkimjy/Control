/*
 * motor_control.cpp
 *
 *      Author: junyoung / lgkimjy
 */
#include "motor_control.h"

float DistancePerCount = (M_PI * 2 * wheelradius) /  EncoderCountsPerWheel;

Motor::Motor(int E_CHA, int E_CHB, int M_DIR, int M_PWM, float K_P, float K_I, float K_D)
{
  pinMode(E_CHA, INPUT_PULLUP);
  pinMode(E_CHB, INPUT_PULLUP);
  pinMode(M_DIR, OUTPUT);
  pinMode(M_PWM, OUTPUT);

  analogWriteFrequency(M_PWM, 20000);

  E_CHA_ = E_CHA;
  E_CHB_ = E_CHB;
  M_DIR_ = M_DIR;
  M_PWM_ = M_PWM;

  enPos_ = 0;
  m_speed_ = 0;
  ref_speed_ = 0;

  err_speed = 0.0;
  err_speed_k_1 = 0.0;
  err_speed_sum = 0.0;
  err_speed_dot = 0.0;
  prev = 0.0;

  K_D_ = K_D;
  K_P_ = K_P;
  K_I_ = K_I;
}

void Motor::EnCHA_ISR()
{
  // a low-to-high edge on channel A
  if (digitalRead(E_CHA_) == HIGH){
    if (digitalRead(E_CHB_) == LOW)
      enPos_ = enPos_ - 1;
    else
      enPos_ = enPos_ + 1;
  }
  // must be a high-to-low edge on channel A
  else{
    if (digitalRead(E_CHB_) == HIGH)
      enPos_ = enPos_ - 1;
    else
      enPos_ = enPos_ + 1;
  }
}

void Motor::EnCHB_ISR()
{
  // a low-to-high edge on channel B
  if (digitalRead(E_CHB_) == HIGH){
    if (digitalRead(E_CHA_) == HIGH)
      enPos_ = enPos_ - 1;
    else
      enPos_ = enPos_ + 1;
  }
  // must be a high-to-low edge on channel B
  else{
    if (digitalRead(E_CHA_) == LOW)
      enPos_ = enPos_ - 1;
    else
      enPos_ = enPos_ + 1;
  }
}

void Motor::SpeedControl(float ref_spd)
{
  float up, ui, ud, usum = 0;
  int u_in = 0;
  float dt = 0.01;

  ref_speed_ = ref_spd;

  err_speed = ref_speed_ - m_speed_;
  err_speed_dot = (err_speed - err_speed_k_1)/dt;
  err_speed_sum += err_speed*dt;

  up = K_P_ * (float)err_speed;
  ui = K_I_ * (float)err_speed_sum;
  ud = K_D_ * (float)err_speed_dot;
  usum = up + ui + ud + prev;

  err_speed_k_1 = err_speed;

  if (usum < 0)
    digitalWrite(M_DIR_, LOW);
  else
    digitalWrite(M_DIR_, HIGH);

  u_in = fabs(usum);
  if (u_in > 255)
    u_in = 255;

  prev = constrain(usum, -255, 255);

  analogWrite(M_PWM_, u_in);
}

void Motor::EncoderCounter()
{
  Serial.print("mA_speed = ");
  Serial.println(enPos_);
}

void Motor::SerialRead()
{
  Serial.print("mA speed : ");
  Serial.print(m_speed_, 4);
  Serial.print(" , ");
  Serial.print("ref speed : ");
  Serial.println(ref_speed_, 4);
}

Motor::~Motor()
{
}