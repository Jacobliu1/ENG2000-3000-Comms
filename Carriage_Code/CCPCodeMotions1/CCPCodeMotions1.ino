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

CRGB leds[LEDS_COUNT];

const long interval = 1000;
double setPoint = 80.0;
double Input;
double Output;
double Kp = 0.265, Ki = 0.125, Kd = 0.125;
double aggKp = 0.5, aggKi = 0.2, aggKd = 0.1;

// Servo control variables
bool door_closed = true;
bool door_open = false;
#define DOOR_OPEN_MICROS 1500
#define DOOR_CLOSED_MICROS 1000

// PID CONTROLLER
PID myPID(&Input, &Output, &setPoint, Kp, Ki, Kd, DIRECT);

// WiFi credentials
#define WIFI_SSID "ENGG2K3K"
#define WIFI_PASSWORD ""

// Additional variables
String status = "STOPC";
bool is_estop = false;

void setup() {
  Serial.begin(9600);

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi connected!");

  // Setup pin modes
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
  
  // LED initialization
  FastLED.addLeds<WS2812, LEDS_PIN, GRB>(leds, LEDS_COUNT).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  attachInterrupt(digitalPinToInterrupt(Encoder_A_Out), encoderISR, RISING);
}

void encoderISR() {
  encoderCount++;
}

void motorLoop() {
  int dir = 1;  // Default direction; replace with actual direction logic as needed
  setMotor(dir, getMotorPower(Output));
}

int getMotorPower(int power) {
  return is_estop ? 0 : power;
}

void setMotor(int direction, int power) {
  if (direction == 1) { // Forward direction
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else if (direction == 0) { // Reverse direction
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
  analogWrite(Motor_Driver_PWM, power);
}

void decelerate() {
  for (int i = 100; i > 0; i -= 5) {
    analogWrite(Motor_Driver_PWM, i);
    delay(50);
  }
}

void led_stopped() {
  FastLED.clear();
  FastLED.show();
}

void missed() {
  FastLED.clear();
  FastLED.show();
}

void message_handle() {
  WiFiClient client;
  if (client.connect("127.0.0.1", 8080)) {
    client.println("Hello from ESP32!");
  }

  if (client.connected()) {
    String message = client.readStringUntil('\n');
    Serial.println("Received from server: " + message);
  }
}

void loop() {
  motorLoop();
  message_handle();
  delay(1000);  // Adjust as needed
}
