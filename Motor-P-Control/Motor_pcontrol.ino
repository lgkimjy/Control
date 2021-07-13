#include <TimerOne.h>

#define EA_CHA 2
#define EA_CHB 3
#define MA_DIR 7
#define MA_PWM 6 // Encoder

#define K_P 30.0
#define K_I 0.0
#define K_D 0.0

volatile int ref_speed = 0;
volatile int mA_speed = 0;
volatile int enAPos = 0;
int err_speed = 0;
int err_speed_k_1 = 0;
int err_speed_sum = 0;
int err_speed_dot = 0;

volatile int t1_index = 0;

int state = LOW;

void setup() {
  Timer1.initialize(10000);
  Timer1.attachInterrupt(timer1_ISR);
  Serial.begin(115200);
  
  pinMode(EA_CHA, INPUT);
  pinMode(EA_CHB, INPUT);
  pinMode(MA_DIR, OUTPUT);
  pinMode(MA_PWM, OUTPUT); 
  attachInterrupt(digitalPinToInterrupt(EA_CHA), enAchA_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(EA_CHB), enAchB_ISR, CHANGE); 
}


void loop(){
  go(20);
  delayMicroseconds(1);
  }
  
void go(int spdA){
  ref_speed = spdA;
}

void enAchA_ISR(){
// a low-to-high edge on channel A 
  if(digitalRead(EA_CHA) == HIGH) {
    if(digitalRead(EA_CHB) == LOW) {
      enAPos = enAPos - 1;
    }
    else {
      enAPos = enAPos + 1;
    }
   }
    // must be a high-to-low edge on channel A 
  else {
      if(digitalRead(EA_CHB) == HIGH) {
        enAPos = enAPos - 1;
      }
      else {
        enAPos = enAPos + 1;
      }
    }
  }

void enAchB_ISR(){
// a low-to-high edge on channel B 
    if(digitalRead(EA_CHB) == HIGH) {
      if(digitalRead(EA_CHA) == HIGH) {
        enAPos = enAPos - 1; 
        }
      else {
        enAPos = enAPos +1; 
        }
     }
      // must be a high-to-low edge on channel B 
    else {
      if(digitalRead(EA_CHA) == LOW) {
         enAPos = enAPos - 1; 
     }
      else {
        enAPos = enAPos + 1; 
        }
     }
}

void timer1_ISR(){
  t1_index++;
  mA_speed = enAPos;
  Serial.println(mA_speed);
  ControlSpeed(ref_speed);

  enAPos = 0; 
  if(state == HIGH) state = LOW;
  else              state = HIGH;
}

void ControlSpeed(long ref_spd){
    float up, ui,ud, usum  = 0;
    unsigned int u_in = 0;

    err_speed  =  ref_spd - mA_speed;
    err_speed_dot  =  err_speed - err_speed_k_1;
    err_speed_sum += err_speed;
  
    up = K_P * (float)err_speed;
    ui = K_I * (float)err_speed_sum;
    ud = K_D * (float)err_speed_dot;
    usum = up + ui + ud;
  
    err_speed_k_1 = err_speed;
  
    if(usum < 0){
      u_in = usum * (-1);
      digitalWrite(MA_DIR, LOW);
    }
    else{
      u_in = usum;
      digitalWrite(MA_DIR, HIGH);
    }
  
    if(u_in > 255){
      u_in = 255;
    }
    
    analogWrite(MA_PWM, u_in);
}
