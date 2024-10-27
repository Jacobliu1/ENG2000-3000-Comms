#include <WiFi.h>
#include <PID_v1.h>
#include <FastLED.h>

#define Encoder_A_Out 34
#define Encoder_B_Out 35
#define Motor_Driver_PWM 26
#define IN1 25
#define IN2 27
#define Encoder_Pulse_per_revolution 700
#define DOOR_SERVO 12
#define PROX_BACK 21
#define PROX_FRONT 22
#define SENSOR_FRONT 14
#define SENSOR_BACK 26
#define LEDS_PIN 2
#define LEDS_COUNT 4
#define LEDS_CHANNEL 2
#define BRIGHTNESS 10
#define PHOTO_SENSOR 34
#define DATA_PIN 19

volatile long encoderCount = 0;
long previousEncoderCount = 0;
unsigned long previousMillis = 0;
int sensitivity = 5;

CRGB leds[LEDS_COUNT]; // Changeable LED array size

const long interval = 1000;
double setPoint = 80.0;
double Input;
double Output;
double Kp = 0.265, Ki = 0.125, Kd = 0.125;
double aggKp = 0.5, aggKi = 0.2, aggKd = 0.1;
bool is_estop = false; // Emergency stop variable

// Servo control variables
bool door_closed = true;
bool door_open = false;
#define DOOR_OPEN_MICROS 1500
#define DOOR_CLOSED_MICROS 1000

// PID CONTROLLER
PID myPID(&Input, &Output, &setPoint, Kp, Ki, Kd, DIRECT);

#define WIFI_SSID "ENGG2K3K"
#define WIFI_PASSWORD ""

void setup() {
  Serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
  }
  Serial.println("WiFi connected!");

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(Motor_Driver_PWM, OUTPUT);
  pinMode(Encoder_A_Out, INPUT);
  pinMode(Encoder_B_Out, INPUT);
  pinMode(DOOR_SERVO, OUTPUT);
  
  // Initialize PID
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 255);
  myPID.SetSampleTime(interval);

  // Initialize LED strip
  FastLED.addLeds<WS2812, LEDS_PIN, GRB>(leds, LEDS_COUNT).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  // Interrupt declaration
  attachInterrupt(digitalPinToInterrupt(Encoder_A_Out), Handle_Encoder, RISING);
}

void Handle_Encoder() {
  if (digitalRead(Encoder_B_Out) == HIGH) {
    encoderCount++;
  } else {
    encoderCount--;
  }
}

int getMotorPower(int power) {
  return is_estop ? 0 : power;
}

void motorLoop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    long cnt = encoderCount;
    encoderCount = 0;
    unsigned long delta = currentMillis - previousMillis;
    previousMillis = currentMillis;

    Input = abs((cnt * 60.0) / (Encoder_Pulse_per_revolution * (delta) / 1000.0));
    double gap = abs(setPoint - Input);
    if (gap < sensitivity) {
      myPID.SetTunings(Kp, Ki, Kd);
    } else {
      myPID.SetTunings(aggKp, aggKi, aggKd);
    }
  }

  myPID.Compute();
  int dir = 1; // Set direction as needed
  setMotor(dir, getMotorPower(Output));
}

void setMotor(int direction, int PWM_Value) {
  if (direction == 1) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else if (direction == 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
  analogWrite(Motor_Driver_PWM, PWM_Value);
}

void decelerate() {
  for (int i = 100; i > 0; i -= 5) {
    setMotor(1, i);
    delay(50);
  }
}

void led_stopped() {
  leds[0] = CRGB::Green;
  FastLED.show();
}

void missed() {
  FastLED.clear();
  FastLED.show();
}

void photo_sensor() {
  if (digitalRead(PHOTO_SENSOR) == HIGH) {
    setMotor(0, 0);
  }
}

void surrounding_sensor() {
  if (digitalRead(SENSOR_FRONT) == HIGH || digitalRead(SENSOR_BACK) == HIGH) {
    setMotor(0, 0);
  }
}

void loop() {
  motorLoop();
  photo_sensor();
  surrounding_sensor();
}
