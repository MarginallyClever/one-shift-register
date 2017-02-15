//---------------------------------------
// arduino shift register 8 LED demo
// dan@marginallyclever.com
// see also https://www.marginallyclever.com/2017/02/use-74hc959-shift-register/
// see also https://www.marginallyclever.com/product/74hc959n-shift-register/
// see also http://www.ti.com/lit/ds/symlink/sn74hc595.pdf
//---------------------------------------

//---------------------------------------
// CONSTANTS
//---------------------------------------

// name of SR pin -> Arduino pin # connected to SR pin
#define SER    6  // data in
#define SRCLK  5  // shift register clock
#define SRCLR  4  // clear shift register
#define RCLK   3  // storage register (sometimes called the latch pin)
#define OE     2  // enable output

#define TOTAL_SHIFT_PINS  8

//---------------------------------------
// METHODS
//---------------------------------------

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  
  pinMode(SER,OUTPUT);
  pinMode(SRCLK,OUTPUT);
  pinMode(SRCLR,OUTPUT);
  pinMode(RCLK,OUTPUT);
  pinMode(OE,OUTPUT);
  clearShiftRegisters();
  turnOutputsOn();
}


void loop() {
  // put your main code here, to run repeatedly:
  //lightFirst();
  //lightAll();
  //blinkA();
  //blinkB();
  //blinkC();
  //oneAtATime();
  //pingPong();
  //pingPongSine();
  marquee();
}

void lightFirst() {
  shiftDataIn(HIGH);
  copyShiftToStorage();
  while(1);
}

void lightAll() {
  shiftDataIn(HIGH);
  copyShiftToStorage();
  delay(200);
}

void marquee() {
  shiftDataIn(HIGH);
  copyShiftToStorage();
  delay(200);
  shiftDataIn(LOW);
  copyShiftToStorage();
  delay(200);
  shiftDataIn(LOW);
  copyShiftToStorage();
  delay(200);
}

void blinkC() {
  turnOutputsOn();

  int i;
  for(i=0;i<TOTAL_SHIFT_PINS;++i) {
    shiftDataIn(HIGH);
  }
  copyShiftToStorage();
  delay(250);

  for(i=0;i<TOTAL_SHIFT_PINS;++i) {
    shiftDataIn(LOW);
  }
  copyShiftToStorage();
  delay(250);

  turnOutputsOff();
}


void blinkB() {
  int i;
  for(i=0;i<TOTAL_SHIFT_PINS;++i) {
    shiftDataIn(HIGH);
  }

  copyShiftToStorage();
  turnOutputsOn();
  delay(250);
  turnOutputsOff();
  delay(250);
}


void blinkA() {
  turnOutputsOn();
  int i;
  for(i=0;i<TOTAL_SHIFT_PINS;++i) {
    shiftDataIn(HIGH);
    copyShiftToStorage();
  }
  delay(250);
  turnOutputsOff();
  delay(250);
}


void oneAtATime() {
  turnOutputsOn();
  int data = HIGH;
  int i;
  for(i=0;i<TOTAL_SHIFT_PINS;++i) {
    shiftDataIn(data);  // first time here data will be HIGH.
    copyShiftToStorage();
    delay(50);
    data = LOW;  // after first time it will always be LOW.
  }
  turnOutputsOff();
}

// we can't shift backwards, so this gets a little tricky.
void pingPong() {
  int i;
  
  turnOutputsOn();
  
  for(i=0;i<TOTAL_SHIFT_PINS;++i) {
    lightOnlyOnePin(i);
    delay(50);
  }
  
  for(i=TOTAL_SHIFT_PINS-1;i>=0;--i) {  // <-- backwards!
    lightOnlyOnePin(i);
    delay(50);
  }
  //delay(500);
  turnOutputsOff();
}

// use the clock to control a ping pong along a sine() curve.
// one thousand milliseconds per second.
void pingPongSine() {
  int i;
  
  turnOutputsOn();

  const float TIME_TO_BOUNCE = 1.5;  // in seconds
  const float HALF_TOTAL_SHIFT_PINS = TOTAL_SHIFT_PINS/2;
  
  // remember when the animation started
  long b = millis();  // clock time since arduino power on

  do {
    long a = millis();  // time now
    float fa = (float)a / 1000.0f;
    float fraction = fa / TIME_TO_BOUNCE;  // will be 0....1
    float fractionOfTwoPi = PI * 2.0f * fraction;
    // sin() curve returns a number between -1 and 1, inclusive.
    float i = HALF_TOTAL_SHIFT_PINS + HALF_TOTAL_SHIFT_PINS * sin(fractionOfTwoPi);
    // now we know which pin to light.
    lightOnlyOnePin(i);
    //delay(50);  // pop quiz: why don't we need a delay this time?
  } while(millis() - b < TIME_TO_BOUNCE * 1000);

  turnOutputsOff();
}

void lightOnlyOnePin(int i) {
  int j;
  for(j=0;j<TOTAL_SHIFT_PINS;++j) {
    int data = (i==j? HIGH : LOW);
    Serial.print(data?'1':'0');
    shiftDataIn(data);
  }
  copyShiftToStorage();
}


// This doesn't change the value stored in the storage registers.
void turnOutputsOn() {
  digitalWrite(OE,LOW);
}

// This doesn't change the value stored in the storage registers.
void turnOutputsOff() {
  digitalWrite(OE,HIGH);
}

// clear the shift registers without affecting the storage registers.
void clearShiftRegisters() {
  digitalWrite(SRCLR,LOW);
  digitalWrite(SRCLR,HIGH);
}

// All the data in the shift registers moves over 1 unit and the new data goes in at shift register 0.
// The data that was in the shift register 7 goes to the next register (if any).
void shiftDataIn(int data) {
  digitalWrite(SER,data);
  digitalWrite(SRCLK,HIGH);
  digitalWrite(SRCLK,LOW);
}

// copy the 8 shift registers into the shift registers,
// which changes the output voltage of the pins.
void copyShiftToStorage() {
  digitalWrite(RCLK,HIGH);
  digitalWrite(RCLK,LOW);
}
