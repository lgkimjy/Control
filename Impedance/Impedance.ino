#include <TimerFive.h>

//Pin define

#define MA_DIR  12
#define MA_PWM  10
#define PT_ENC  A0
#define PT_REF  A1
#define LED     3

// Global variables
int loop_flag = 0;
int loop_chk = 0;
unsigned int t5_index = 0;
unsigned int first_t = 1;

int enc_val;
int ref_val;

// variables for Control
int err_k = 0;
int err_k_1 = 0;
int derr_k = 0;

int Kp = 60;
int Kd = 150;
//int Kp = 100;
//int Kd = 150;

int Up = 0;
int Ud = 0;
long sU = 0;
float fU = 0.0;
int uU = 0;

void setup() {
  pinMode(MA_DIR, OUTPUT);
  pinMode(MA_PWM, OUTPUT);
  pinMode(PT_ENC, INPUT);
  pinMode(PT_REF, INPUT);
  pinMode(LED, OUTPUT);

  Timer5.initialize(1000);   //Timer interrupt - time setup (1000 usec)
  Timer5.attachInterrupt(Timer5_ISR);
  Serial.begin (230400);
}

void loop(){
  //just 1 time excute
  if(first_t){
    first_t = 0;
    t5_index = 0;
    loop_flag = 0;
  }
  
  if(loop_flag){  //loop time 1msec
    loop_flag = 0;

    switch(t5_index){
      case 0:
        Serial.println("--------------------------------");
        break;

      case 1:
        ref_val = analogRead(PT_REF);
        ref_val = map(ref_val, 0, 1023, -500, 500);
        Serial.print("ref potentio-meter : ");
        Serial.println(ref_val);
        break;

      case 2:
        // Read the potentio-meter of encoder
        enc_val = analogRead(PT_ENC);
        enc_val = map(enc_val, 0, 1023, -500, 500);
        Serial.print("enc potentio-meter : ");
        Serial.println(enc_val);
        err_k = ref_val - enc_val;
        //err_k = 0 - enc_val;
        derr_k = err_k - err_k_1;
        err_k_1 = err_k;
        break;

      case 3:
        // Control
        Up = Kp * err_k;
        Ud = Kd * derr_k;
        sU = (long)Up + (long)Ud;
        fU = (float)sU / 50.0;
        uU = (int)fU;
        break;

      case 4:
        //
        
        break;

      case 5:   //Control loop : 5mSec
        t5_index = 0;
        loop_chk ^= 1;

        if(uU > 255) uU = 255;
        else if(uU <-255) uU = -255;

        if(uU >= 0){
          digitalWrite(MA_DIR, HIGH);
        }
        else{
          uU *= -1;
          digitalWrite(MA_DIR, LOW);
        }
        analogWrite(MA_PWM, uU);
        
        digitalWrite(LED, loop_chk);
        break;

      default:
        t5_index = 0;
        break;
    }
    
  }//end of time-loop
  
  //Idle
  
}

void Timer5_ISR(){
  loop_flag = 1;
  t5_index++;
}

