#include "CytronMotorDriver.h"

#define LEFT 0
#define RIGHT 1
#define BOTH 2


// Configure the motor driver.
CytronMD motor2(PWM_PWM, 9, 3);   // PWM 1A = Pin 3, PWM 1B = Pin 9.
CytronMD motor1(PWM_PWM, 10, 11); // PWM 2A = Pin 10, PWM 2B = Pin 11.

//Initialisation
const int sensors_num = 4;
int sensors[sensors_num] = { A3, A2, A1, A0 };

bool calibrated = true;
int value_max[sensors_num] = { 406, 364, 427, 474 };
int value_min[sensors_num] = { 186, 163, 194, 204 };
//474/204 427/194 364/163 406/186
//GLOBALS
int values[sensors_num];
int percents[sensors_num];

//FUNCTIONS
void calibrate(bool debug = false);
void getValues(bool debug = false);
void mapValues(bool debug = false);
void PID(int leftValue, int rightValue, bool debug = false);
void debug_rawValues();
void debug_mappedValues();
void debug_calibrationValues();




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(2, INPUT);
//  calibrate(true);
  detectJunction();
  move(0,0);
}

void loop() {
  // put your main code here, to run repeatedly:
}

bool detectJunction(int type, bool debug) {
  while (true){
    mapValues();
    int leftValue = percents[1];
    int rightValue = percents[2];
    PID(leftValue,rightValue);

    if (type == LEFT) {
      if ( (percents[1] > 75 || percents[2] > 75) && percents[0] > 75) break;
    } else if (type == RIGHT) {
      if ( (percents[1] > 75 || percents[2] > 75) && percents[3] > 75) break;
    } else if (type == BOTH) {
      if ( (percents[1] > 75 || percents[2] > 75) && (percents[0] > 75 || percents[3] > 75)) break;
    }
    
  }

  
  if (type == LEFT) {
    while (percents[1] < 75 && percents[0] < 75) {
      
    }
  }
}

void PID(int leftValue, int rightValue, bool debug) {
  int error = leftValue - rightValue;
  int result = error * 2;
  move(255 - result, 255 + result);
  if (debug) Serial.print("Error: "); Serial.println(error);
}

void move(int left, int right) {
  motor1.setSpeed(left);     // Motor 1 stops.
  motor2.setSpeed(right); 
}



void calibrate(bool debug) {
  calibrated = true;
  Serial.println("Starting Calibration in..");
  for (int i = 0; i < 3; i++) {
    Serial.println("1..");
    delay(1000);
  }
  long start = millis();

  while (start + 5000 > millis()) {
    Serial.print("CALIBRATING: ");
    Serial.println(millis());

    getValues();

    for (int i = 0; i < sensors_num; i++) {
      if (values[i] > value_max[i]) value_max[i] = values[i];
      else if (values[i] < value_min[i]) value_min[i] = values[i];
    }
  }

  if (debug) debug_calibrationValues(); delay(5000);

  int disconnect_check = 0;
  for (int i = 0; i < sensors_num; i++) {
    disconnect_check += value_max[i] + value_min[i];
  }
  if (disconnect_check < 10){
    Serial.println("\n\n--------------!!!!PLEASE CHECK CONNECTION OF ARRAY!!!-------------\n\n");
    delay(5000);
  } 
}

void mapValues(bool debug = false) {
  if (!calibrated) {
    Serial.println("\n\n--------------!!!!CALIBRATE FIRST!!!!-------------\n\n"); 
    delay(5000);
    return;
  }
  getValues();
  for (int i = 0; i < sensors_num; i++) {
 
    percents[i] = map(values[i], value_min[i], value_max[i], 0, 100);
  }

  if (debug) debug_mappedValues();
}

void getValues(bool debug) {
  for (int i = 0; i < sensors_num; i++) {
    values[i] = analogRead(sensors[i]);
  }
  if (debug) debug_rawValues();
}

void debug_calibrationValues() {
  Serial.print("Max / Min Sensor Readings: ");
  for (int i = 0; i < sensors_num; i++) {
    Serial.print(value_max[i]);
    Serial.print("/");
    Serial.print(value_min[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void debug_rawValues() {
  Serial.println("Raw Sensor Readings: ");
  for (int i = 0; i < sensors_num; i++) {
    Serial.print(values[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void debug_mappedValues() {
  Serial.print("Mapped Sensor Readings: "); 
  for (int i = 0; i < sensors_num; i++) {
    Serial.print(percents[i]);
    Serial.print(" ");
  }
  Serial.println();
}
