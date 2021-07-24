/*
 * motor_control.ino
 *
 *      Author: junyoung / lgkimjy
 */
#include "motor_control.h"
#include <TimerThree.h>    // Teensy 4.0

/* Motor Class Initialization */
Motor Motor1(E1_CHA, E1_CHB, M1_DIR, M1_PWM, 20.0, 0.0, 0.0);

void go(float spdA)
{
  Motor1.ref_speed_ = spdA;
}

void setup()
{
  Serial.begin(115200);
  
  Timer3.initialize(10000);   // units : 10000microsec = 10ms
  Timer3.attachInterrupt(timer1_ISR);
  
  /* Teensy 4.0 */
  attachInterrupt(E1_CHA, callback1, CHANGE);
  attachInterrupt(E1_CHB, callback2, CHANGE);
}

void loop()
{
  go(-0.5);
  delayMicroseconds(1);
}

void callback1(){
  Motor1.EnCHA_ISR();
}

void callback2(){
  Motor1.EnCHB_ISR();
}

void timer1_ISR()
{
  Motor1.m_speed_ = Motor1.enPos_ * DistancePerCount / 0.01;
  Motor1.SpeedControl(Motor1.ref_speed_);  
  Motor1.enPos_ = 0;
  
  Serial.println("--------------------------------------");
  // Motor1.SerialRead();
  // Motor1.EncoderCounter();
}
