#include <WiFi.h>
#include <PID_v1.h>
#include <FastLED.h>

#define Encoder_A_Out 34                  // Encoder output A connected to interrupt pin 2
#define Encoder_B_Out 35                  // Encoder output B connected to interrupt pin 3
#define Motor_Driver_PWM 26               // Motor driver PWM signal pin
#define IN1 25                            // Motor driver direction pin 1
#define IN2 27                            // Motor driver direction pin 2
#define Encoder_Pulse_per_revolution 700  // Encoder pulses per revolution
#define DOOR_SERVO      12
#define PROX_BACK       21
#define PROX_FRONT      22
#define SENSOR_FRONT        14
#define SENSOR_BACK         26
#define LEDS_PIN             2
#define LEDS_COUNT           4
#define LEDS_CHANNEL         2
#define BRIGHTNESS          10
#define PHOTO_SENSOR        34
#define DATA_PIN 19

volatile long encoderCount = 0;    // To count the encoder pulses
long previousEncoderCount = 0;     // To store the previous encoder count
unsigned long previousMillis = 0;  // To store the previous time
int sensitivity = 5;
812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, LEDS_CHANNEL, TYPE_GRB);

CRGB leds[1]; //changeable

const long interval = 1000;        // Time interval to calculate RPM (1 second)
double setPoint = 80.0;   // Target RPM / what RPM to set into 
double Input;             
double Output;  
double Kp = 0.265, Ki = 0.125, Kd = 0.125;
double aggKp = 0.5, aggKi = 0.2, aggKd = 0.1; // Aggressive tuning parameters

// Servo control variables
bool door_closed = true;
bool door_open = false;
#define DOOR_OPEN_MICROS 1500
#define DOOR_CLOSED_MICROS 1000

//PID CONTROLLER
PID myPID(&Input, &Output, &setPoint, Kp, Ki, Kd, DIRECT);

#define WIFI_SSID       "ENGG2K3K"
#define WIFI_PASSWORD   ""

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED){
    serial.print(".");
  }
  serial.print("WiFi connected!");

  //setting pin for OUTPUT
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(Motor_Driver_PWM, OUTPUT);
  pinMode(Encoder_A_Out, INPUT);
  pinMode(Encoder_B_Out, INPUT);
  pinMode(DOOR_SERVO, OUTPUT);
  
  //Initialise servo and LED
  setup_servo();
  //setup_led();

  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 255);  // PWM output limits
  myPID.SetSampleTime(interval);  // Set the sample time to the interval (in ms)

  //Interrupt declaration

  attachInterrupt(digitalPinToInterrupt(Encoder_A_Out), Handle_Encoder, RISING);

  // Initialize PID
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 255);  // PWM output limits
  myPID.SetSampleTime(interval);  // Set the sample time to the interval (in ms)
}
// Servo control functions
void setup_servo() {
    pinMode(DOOR_SERVO, OUTPUT);
}

int door_Err() {
    if(door_closed == door_open){
      return 1;
    }
    return 0;
}

void write_door_open() {
    digitalWrite(DOOR_SERVO, HIGH);
    delayMicroseconds(DOOR_OPEN_MICROS);
    digitalWrite(DOOR_SERVO, LOW);
}

void write_door_closed() {
    digitalWrite(DOOR_SERVO, HIGH);
    delayMicroseconds(DOOR_CLOSED_MICROS);
    digitalWrite(DOOR_SERVO, LOW);
}



// Motor control functions
int getMotorPower(int power) {
    return is_estop ? 0 : power;
}

double getRPM() {
    if(encoderCount < 0) {
        return Input * -1;
    } else {
        return Input;
    }
}

void motorLoop() {
    unsigned long currentMillis = millis();
    // Calculate RPM every second
    if (currentMillis - previousMillis >= interval) {
        long cnt = encoderCount;
        encoderCount = 0;
        unsigned long delta = currentMillis - previousMillis;
        previousMillis = currentMillis;
        
        // Calculate RPM using the formula
        Input = abs((cnt * 60.0) / (Encoder_Pulse_per_revolution * (delta) / 1000.0));
      
        // Adaptive PID tuning based on RPM gap
        double gap = abs(setPoint - Input);
        if(gap < sensitivity){
            myPID.SetTunings(Kp, Ki, Kd);
        } else {
            myPID.SetTunings(aggKp, aggKi, aggKd);
        }
    }

    myPID.Compute();
    setMotor(dir, getMotorPower(Output));
}


void setMotor(int direction, int PWM_Value) {
    if (direction == 1) {  // Forward direction
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
    } else (direction == 0){  // Reverse direction
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
    }
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(Motor_Driver_PWM, PWM_Value);
}

void Handle_Encoder() {
    if (digitalRead(Encoder_B_Out) == HIGH) {
        encoderCount++;
    } else {
        encoderCount--;
    }
}

// Function to accelerate motor (for simplicity, a placeholder)
void accelerate() {
  for(int i = 5; i<30; i+=5){
    setMotor(1,i);
  }
}

// Function to decelerate motor (for simplicity, a placeholder)
void decelerate() {
  for(int i = ; i<30; i-=5){
    setMotor(1,i);
  }
}

void led_stopped() {
  leds[0] = CRGB::Green;
  fastLED.show();
}

void missed() {
  fastLED.clear();
}

void disconnected_blink() {
  int s = 0;
  EVERY_N_MILLISECONDS(500) {
    s = !s;
    if (s) {
      leds[0] = CRGB::LightYellow;
    } else {
      leds[0] = CRGB::Black;
    }
    FastLED.show();
  }
}

void emergency_blink() {
  int s = 0;
  EVERY_N_MILLISECONDS(1000) {
    s = !s;
    if (s) {
      leds[0] = CRGB::Red;
    } else {
      leds[0] = CRGB::Black;
    }
    FastLED.show();
  }
}
void photo_sensor(){

  if (digitalRead(PHOTO_SENSOR) == HIGH){
    setMotor(0,0);
  }

}

void surrounding_sensor(){
  if(digitalRead(SENSOR_FRONT) == HIGH || digitalRead(SENSOR_BACK) == HIGH){
    setMotor(0,0);
  }
}
void loop() {
  
  photo_sensor();
  surrounding_sensor();
}


