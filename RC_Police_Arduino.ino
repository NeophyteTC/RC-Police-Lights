#include <EEPROM.h>

/* ================= KONFIG ================= */
#define NUM_MODES 4
#define EEPROM_ADDR 0

/* ================= PINSETUP ================= */
// Dach: R W R W B W B
int roofPins[7] = {2,3,4,5,6,7,8};

// Heck: R B R B R B R B
int rearPins[8] = {9,11,A0,A1,A2,A3,A4,A5};

// Steuerung
int buttonPin = 12;
int rcPin     = A7;

/* ================= VARIABLEN ================= */
int mode = 0;
unsigned long lastButtonTime = 0;

bool rcLastState = false;
unsigned int loopCounter = 0;

/* ================= SETUP ================= */
void setup() {
  for (int i=0;i<7;i++) pinMode(roofPins[i], OUTPUT);
  for (int i=0;i<8;i++) pinMode(rearPins[i], OUTPUT);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(rcPin, INPUT);

  mode = EEPROM.read(EEPROM_ADDR);
  if (mode >= NUM_MODES) mode = 0;

  allOff();
}

/* ================= LOOP ================= */
void loop() {
  loopCounter++;

  handleButton();
  handleRC();

  switch (mode) {
    case 0: modeCruise(); break;
    case 1: modeCode1();  break;
    case 2: modeWigWag(); break;
    case 3: modeCode3();  break;
  }
}

/* ================= INPUT ================= */

// Button = Modus wechseln
void handleButton() {
  if (!digitalRead(buttonPin) && millis() - lastButtonTime > 300) {
    nextMode();
    lastButtonTime = millis();
  }
}

// RC Channel 3 = Modus wechseln (Flanke!)
void handleRC() {
  unsigned long pulse = pulseIn(rcPin, HIGH, 25000);
  if (pulse == 0) return;

  bool rcState = (pulse > 1500);

  // LOW → HIGH = Trigger
  if (rcState && !rcLastState) {
    nextMode();
  }

  rcLastState = rcState;
}

// Gemeinsame Mode-Wechsel-Funktion
void nextMode() {
  mode = (mode + 1) % NUM_MODES;
  EEPROM.update(EEPROM_ADDR, mode);
}

/* ================= BASIS ================= */
void allOff() {
  for (int i=0;i<7;i++) digitalWrite(roofPins[i], LOW);
  for (int i=0;i<8;i++) digitalWrite(rearPins[i], LOW);
}

/* ================= MODES ================= */

// MODE 0 – Cruise
void modeCruise() {
  roofRedBlue(300);
  roofWhiteStrobeAsym(50);
  rearAlternate(300);
}

// MODE 1 – Code 1
void modeCode1() {
  roofRedBlue(200);
  roofWhiteStrobeAsym(45);
  rearLeftRight(200);
}

// MODE 2 – WigWag
void modeWigWag() {
  roofWigWag(120);
  roofWhiteStrobeAsym(40);
  rearAlternate(150);
}

// MODE 3 – Code 3
void modeCode3() {
  roofRedBlue(80);
  roofWhiteStrobeAsym(30);
  rearLeftRight(80);
}

/* ================= ROOF ================= */
void roofRedBlue(int d) {
  digitalWrite(roofPins[0], HIGH);
  digitalWrite(roofPins[2], HIGH);
  delay(d);
  digitalWrite(roofPins[0], LOW);
  digitalWrite(roofPins[2], LOW);

  digitalWrite(roofPins[4], HIGH);
  digitalWrite(roofPins[6], HIGH);
  delay(d);
  digitalWrite(roofPins[4], LOW);
  digitalWrite(roofPins[6], LOW);
}

void roofWigWag(int d) {
  digitalWrite(roofPins[0], HIGH);
  delay(d);
  digitalWrite(roofPins[0], LOW);

  digitalWrite(roofPins[6], HIGH);
  delay(d);
  digitalWrite(roofPins[6], LOW);
}

// White: nur jeder 3. Loop, asymmetrisch
void roofWhiteStrobeAsym(int d) {
  if (loopCounter % 3 != 0) return;

  // Links (stärker)
  digitalWrite(roofPins[1], HIGH);
  delay(d);
  digitalWrite(roofPins[1], LOW);
  delay(d / 2);
  digitalWrite(roofPins[1], HIGH);
  delay(d / 2);
  digitalWrite(roofPins[1], LOW);

  delay(d / 2);

  // Mitte (kurz)
  digitalWrite(roofPins[3], HIGH);
  delay(d / 2);
  digitalWrite(roofPins[3], LOW);

  delay(d / 2);

  // Rechts
  digitalWrite(roofPins[5], HIGH);
  delay(d);
  digitalWrite(roofPins[5], LOW);
}

/* ================= REAR ================= */
void rearAlternate(int d) {
  for (int i=0;i<8;i+=2) digitalWrite(rearPins[i], HIGH);
  delay(d);
  for (int i=0;i<8;i+=2) digitalWrite(rearPins[i], LOW);

  for (int i=1;i<8;i+=2) digitalWrite(rearPins[i], HIGH);
  delay(d);
  for (int i=1;i<8;i+=2) digitalWrite(rearPins[i], LOW);
}

void rearLeftRight(int d) {
  for (int i=0;i<4;i++) digitalWrite(rearPins[i], HIGH);
  for (int i=4;i<8;i++) digitalWrite(rearPins[i], LOW);
  delay(d);

  for (int i=0;i<4;i++) digitalWrite(rearPins[i], LOW);
  for (int i=4;i<8;i++) digitalWrite(rearPins[i], HIGH);
  delay(d);
}