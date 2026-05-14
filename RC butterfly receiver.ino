#include <SPI.h>
#include <RF24.h>
#include <ESP32Servo.h> 

RF24 radio(4,5);
const byte addr[6] = "00001";

Servo servoL;
Servo servoR;

int pinServoL = 25;
int pinServoR = 26;

float joyL = 0;
float joyR = 0;

struct __attribute__((packed)) ControlData {
  float L;
  float R;
  uint8_t toggle; 
};

ControlData ctrl;

unsigned long lastReceive = 0;
const unsigned long timeoutMs = 500;

// === NAYA MASTER SYNC LOGIC ===
float globalPhase = 0; // Ab dono pankh isi ek phase se chalenge taaki out of sync na ho
unsigned long lastTime = 0;

// SPEED FIX: Ise 3.5 se 1.5 kar diya hai taaki Servo poora 180 degree ghoom sake
float maxFlapFreq = 1.5; 

// ==========================================
// 🛠 YAHAN PAR TUMHARE 180 DEGREE ANGLES HAIN 🛠
// ==========================================

int topAngleL = 180;    
int bottomAngleL = 0;   

int topAngleR = 0;      
int bottomAngleR = 180; 

// ==========================================

void setup() {
  Serial.begin(115200);

  servoL.attach(pinServoL);
  servoR.attach(pinServoR);

  servoL.write(topAngleL);
  servoR.write(topAngleR);

  radio.begin();
  radio.setChannel(108);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.setAutoAck(true);

  radio.openReadingPipe(1, addr);
  radio.startListening();

  lastReceive = millis();
  lastTime = millis();
}

void loop() {
  if(radio.available()){
    radio.read(&ctrl, sizeof(ctrl));
    joyL = ctrl.L;  
    joyR = ctrl.R;  
    lastReceive = millis();
  }

  if (millis() - lastReceive > timeoutMs) {
    joyL = 0;
    joyR = 0;
  }

  controlMotors();
}

// ===== MOTOR CONTROL (MASTER SYNC & AUTO-AMPLITUDE) =====
void controlMotors(){

  float L = constrain(joyL, 0, 255);
  float R = constrain(joyR, 0, 255);

  unsigned long currentTime = millis();
  float dt = (currentTime - lastTime) / 1000.0; 
  lastTime = currentTime;

  // Agar dono slider band hain, toh pankh upar lock karke phase reset kar do
  if (L < 10 && R < 10) {
    servoL.write(topAngleL);
    servoR.write(topAngleR);
    globalPhase = 0; 
    return;
  }

  // Jo slider zyada power par hoga, woh titli ki "Main Speed" tay karega
  float masterSpeed = L;
  if (R > L) {
    masterSpeed = R;
  }

  // Flapping ki frequency calculate karo
  float freq = (masterSpeed / 255.0) * maxFlapFreq;

  // Master Phase update karo (Dono pankh isko follow karenge)
  globalPhase += 2 * PI * freq * dt;
  if (globalPhase >= 2 * PI) globalPhase -= 2 * PI;

  float wave = cos(globalPhase); // wave hamesha +1 se -1 tak jayegi ek sath

  // --- LEFT WING LOGIC ---
  float midL = (topAngleL + bottomAngleL) / 2.0;
  float ampL = (topAngleL - bottomAngleL) / 2.0;
  // Agar Left slider Right se kam hai, toh Left pankh thoda kam khulega (Mudne ke liye)
  float sweepFactorL = L / masterSpeed; 
  int angleL = midL + (ampL * sweepFactorL * wave);

  // --- RIGHT WING LOGIC ---
  float midR = (topAngleR + bottomAngleR) / 2.0;
  float ampR = (topAngleR - bottomAngleR) / 2.0;
  // Agar Right slider Left se kam hai, toh Right pankh thoda kam khulega
  float sweepFactorR = R / masterSpeed;
  int angleR = midR + (ampR * sweepFactorR * wave);

  servoL.write(angleL);
  servoR.write(angleR);
}
