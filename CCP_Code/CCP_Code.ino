#include <WiFi.h>
#include <PID_v1.h>


#define Encoder_A_Out 34                  // Encoder output A connected to interrupt pin 2
#define Encoder_B_Out 35                  // Encoder output B connected to interrupt pin 3
#define Motor_Driver_PWM 26               // Motor driver PWM signal pin
#define IN1 25                            // Motor driver direction pin 1
#define IN2 27                            // Motor driver direction pin 2
#define Encoder_Pulse_per_revolution 700  // Encoder pulses per revolution

volatile long encoderCount = 0;    // To count the encoder pulses
long previousEncoderCount = 0;     // To store the previous encoder count
unsigned long previousMillis = 0;  // To store the previous time

const long interval = 1000;        // Time interval to calculate RPM (1 second)
double Setpoint = 80.0;   // Target RPM
double Input;             // Current RPM
double Output; 
double Kp = 0.265, Ki = 0.125, Kd = 0.125;
//PID CONTROLLER
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

const char ssid [] = "skibidi network name";
const char password [] = "skibidi password name";

void setup() {
  
  Serial.begin(9600);
  WiFi.begin(ssid,password);

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

}

void setMotor(int direction, int PWM_Value) { 

  if (direction == 1) {  // Forward direction
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {  // Reverse direction
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
  analogWrite(Motor_Driver_PWM, PWM_Value);
}
void Handle_Encoder() {
  if (digitalRead(Encoder_A_Out) > digitalRead(Encoder_B_Out)) {
    encoderCount++;
  } else {
    encoderCount--;
  }

}

void loop() {
    unsigned long currentMillis = millis();
  // Calculate RPM every second
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Calculate RPM using the formula
    Input = (encoderCount * 60.0) / Encoder_Pulse_per_revolution;

    Serial.print("RPM OF MOTOR = ");
    Serial.println(Input);

    // Run PID control
    myPID.Compute();
    
    // Set motor speed using PID output
    setMotor(1, Output);



      if (encoderCount > 0) {
        Serial.print("Direction of motor is ANTI CLOCKWISE ");
        Serial.print("      ----->");
      } else if (encoderCount < 0) {
        Serial.print("Direction of motor is CLOCKWISE ");
        Serial.print("      ----->");
      } else {
        Serial.print("MOTOR DOESNOT MOVE! ");
        Serial.print("      ----->");
      }
    encoderCount = 0;
  }
}


