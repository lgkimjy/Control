/*
 * motor_control.h
 *
 *      Author: junyoung / lgkimjy
 */
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <math.h>
#include <map>
#include <string>
#include <Arduino.h>

/* ros_communication msg */
#include <ros.h>
#include <std_msgs/Float64.h>

/* Pin information */
#define E1_CHA 9
#define E1_CHB 10
#define M1_DIR 11
#define M1_PWM 12 // Encoder

/* Mobile and Wheel information */
#define EncoderCountsPerWheel 1976  // per 1 cycle
#define wheelradius 0.076           // units : m
#define M_PI 3.14159265358979323846

extern float DistancePerCount;

/* ROS Callback */
void motor1_cb(const std_msgs::Float64& msg);

/* Motor Class */
class Motor
{
  public:
    Motor(int E_CHA, int E_CHB, int M_DIR, int M_PWM, float K_P, float K_I, float K_D);
    ~Motor();
    void EnCHA_ISR();
    void EnCHB_ISR();
    void SpeedControl(float ref_speed);
    void EncoderCounter();
    void SerialRead();

    int enPos_;
    float m_speed_;
    float ref_speed_;
    
    int E_CHA_, E_CHB_, M_DIR_, M_PWM_;
    float K_P_, K_D_, K_I_;

  private:
    float err_speed;
    float err_speed_k_1;
    float err_speed_sum;
    float err_speed_dot;
    float prev;
};
