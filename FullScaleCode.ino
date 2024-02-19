#include <SPI.h>
#include <WiFiNINA.h>
#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>

bool confirm = false;
bool sent = false;
bool started = false;
bool once = false;
bool rotation = false;
String move;
byte posIndex = 0;
String posString = "right";

Adafruit_MotorShield AFMSbot(0x61);  // Rightmost jumper closed
Adafruit_MotorShield AFMStop(0x60);  // Default address, no jumpers

// two stepper motors one on each port
Adafruit_StepperMotor *myStepper1 = AFMStop.getStepper(200, 1);
Adafruit_StepperMotor *myStepper2 = AFMStop.getStepper(200, 2);

int newOnePos;
int newTwoPos;
// you can change these to DOUBLE or INTERLEAVE or MICROSTEP!
// wrappers for the first motor!
void forwardstep1() {
  myStepper1->onestep(BACKWARD, DOUBLE);
}
void backwardstep1() {
  myStepper1->onestep(FORWARD, DOUBLE);
}
// wrappers for the second motor!
void forwardstep2() {
  myStepper2->onestep(FORWARD, DOUBLE);
}
void backwardstep2() {
  myStepper2->onestep(BACKWARD, DOUBLE);
}

AccelStepper stepper1(forwardstep1, backwardstep1);
AccelStepper stepper2(forwardstep2, backwardstep2);

char moveList;

char ssid[] = "TP-LINK_65D55C";
char pass[] = "";
int keyIndex = 0;

int status = WL_IDLE_STATUS;
WiFiServer server(80);

String readString;
unsigned int localPort = 4000;  // assign a port to talk over
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
String datReq;   //string for our data
int packetSize;  //size of the packet
WiFiUDP Udp;

void setup() {
  pinSetUp();
  shieldAndWiFiCheck();
  motorSetup();
  Udp.begin(localPort);  //initalise udp
  Serial.print("Listening on port ");
  Serial.println(localPort);
}

void loop() {
  // put your main code here, to run repeatedly:
  packetSize = Udp.parsePacket();  // read the packet size

  if (packetSize > 0) {                              // check to see if a request is present
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);  //reading the data request on the udp
    String datReq(packetBuffer);                     //Connect packetBudder arrary to string datReq
    Serial.println("Requested Move: ");
    Serial.println(datReq);
    move = datReq;
    confirm = false;
    sent = false;
    started = true;
    memset(packetBuffer, 0, UDP_TX_PACKET_MAX_SIZE);
  }
  if (!once) {
    if (move == "PosY") {
      if (posString != "right") {
        Serial.println("changing to right");
        spotRotate("right");
      } else if (posString == "right") {
        Serial.println("Moving forward");
        moveForward();
      }
    }

    if (move == "PosX") {
      if (posString != "down") {
        Serial.println("changing to down");
        Serial.println(posString);
        spotRotate("down");
      } else if (posString == "down") {
        Serial.println("Moving forward");
        moveForward();
        Serial.println("Move Completed");
      }
    }

    if (move == "NegY") {
      if (posString != "left") {
        Serial.println("changing to left");
        spotRotate("left");
      } else if (posString == "left") {
        Serial.println("Moving forward");
        moveForward();
        Serial.println("Move Completed");
      }
    }

    if (move == "NegX") {
      if (posString != "up") {
        Serial.println("changing to up");
        spotRotate("up");
      } else if (posString == "up") {
        Serial.println("Moving forward");
        moveForward();
        Serial.println("Move Completed");
      }
    }
  }

  if (confirm && !sent && started && !rotation) {
    Udp.beginPacket("192.168.0.100", localPort);
    Udp.print("Next");
    Udp.endPacket();
    sent = true;
  }
  stepper1.run();
  stepper2.run();
  
  // Serial.println(stepper1.distanceToGo());
  if (stepper1.distanceToGo() == 0 && stepper2.distanceToGo() == 0 && !confirm && started) {
    Serial.println("Move Completed");
    confirm = true;
  }
  
  if (stepper1.distanceToGo() == 0 && stepper2.distanceToGo() == 0 && once) {
    once = false;
  }

}

void moveForward() {
  // code here to make the motors move the system forward.
  int newOnePos = stepper1.currentPosition() + 260;
  int newTwoPos = stepper2.currentPosition() + 260;
  rotation = false;
  once = true;
  stepper1.moveTo(newOnePos);
  stepper2.moveTo(newTwoPos);
  move = "None";
}

void spotRotate(String desiredPos) {
  String posArray[4] = {"right", "down", "left", "up"};
  int newOnePos = stepper1.currentPosition() + 176;
  int newTwoPos = stepper2.currentPosition() - 176;
  rotation = true;
  once =true;
  stepper1.moveTo(newOnePos);
  stepper2.moveTo(newTwoPos);
  if (desiredPos != posArray[posIndex]) {
    if (posIndex == 3) {
      posIndex = 0;
    } else {
      posIndex = posIndex + 1;
    }
    posString = posArray[posIndex];
  }
}

void pinSetUp() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  delay(1000);
  Serial.println("***Reset***");
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("LED ON");
  delay(2000);
}

void shieldAndWiFiCheck() {

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(1000);
  }
  server.begin();

  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  Serial.println("UDP has been initalised");
  delay(1500);
  digitalWrite(LED_BUILTIN, HIGH);
}


void motorSetup() {
  AFMSbot.begin();  // Start the bottom shield
  AFMStop.begin();  // Start the top shield
  stepper1.setMaxSpeed(180.0);
  stepper1.setAcceleration(40);

  stepper2.setMaxSpeed(180.0);
  stepper2.setAcceleration(40.0);
}
  