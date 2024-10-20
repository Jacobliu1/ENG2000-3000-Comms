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

#define IR_FRONT        14
#define IR_BACK         26

#define LEDS_PIN             2
#define LEDS_COUNT           4
#define LEDS_CHANNEL         2
#define BRIGHTNESS          10

volatile long encoderCount = 0;    // To count the encoder pulses
long previousEncoderCount = 0;     // To store the previous encoder count
unsigned long previousMillis = 0;  // To store the previous time
int sensitivity = 5;
const long interval = 100;        // Time interval to calculate RPM (0.1 second)
double setpoint = 0.0;   // Target RPM
double Input;             // Current RPM
double Output;
Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, LEDS_CHANNEL, TYPE_GRB);

#define DA_SENSE        34



#define DATA_PIN 19
CRGB leds[1]; //changeable

const long interval = 1000;        // Time interval to calculate RPM (1 second)
double setPoint = 80.0;   // Target RPM / what RPM to set into 
double Input;             
double Output;  
double Kp = 0.265, Ki = 0.125, Kd = 0.125;
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

  //setting pin for INPUT
  pinMode(Encoder_A_Out, INPUT);
  pinMode(Encoder_B_Out, INPUT);

  //Interrupt declaration

  attachInterrupt(digitalPinToInterrupt(Encoder_A_Out), Handle_Encoder, RISING);

  // Initialize PID
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 255);  // PWM output limits
  myPID.SetSampleTime(interval);  // Set the sample time to the interval (in ms)
}


void setMotor(int direction, int PWM_Value) { 

  if (direction == 1) {  // Forward direction
    runMotor(PWM_Value)
  } else if (direction == 0){
    runMotor (0)
  } else { // Reverse direction
   runMotor(-(PWM_Value))
  }
  
}
void Handle_Encoder() {
 // code Motions will provide
}
void runMotor(speed){
 //code Motions will provide
}

// Function to accelerate motor (for simplicity, a placeholder)
void accelerate() {
  runMotor(5)
  runMotor(10)
  runMotor(15)
  runMotor(20)
  runMotor(30)
}

// Function to decelerate motor (for simplicity, a placeholder)
void decelerate() {
  
  runMotor(30)
  runMotor(20)
  runMotor(15)
  runMotor(10)
  runMotor(5)
}

void stopped() {
  leds[0] = CRGB::Green;
  fastLED.show();
}

void missed() {
  fastLED.clear();
}

void dcBlink() {
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

void esBlink() {
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

void loop() {
  unsigned long currentMillis = millis();

  if(Input == 0.0) digitalWrite(2,LOW); // when RPM is 0
  // Calculate RPM every second
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    calculateDirectionalRPM();
  }

  if(currentMillis <= 30000){
    timeFreeze = currentMillis;
    if(currentMillis <= timeFreeze+5000 && currentMillis < 30000)
      accelerate();

    if(currentMillis >= 25000){ 
      decelerate();
    }

    myPID.Compute(); // Motions code (PID for speed)
    setMotor(1,Output);
  }

}


