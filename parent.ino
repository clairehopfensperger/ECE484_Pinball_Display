// ECE 484
// Pinball Field - SPI Parent (Display - SPI Child)
// Claire Hopfensperger

//-----------------------------------------------------------
// Include libraries, set up objects, define pins
//-----------------------------------------------------------

// SPI stuff
#include <SPI.h>

// Servo stuff
#include <Servo.h>

//-----------------------------------------------------------
// Pin/var delcarations
//-----------------------------------------------------------

// IR sensor
const int sense1 = 2;
const int sense2 = 3;
const int sense3 = 4;
const int sense4 = 5;  // 

//// Button vars (HIGH or LOW)
int sense1Val = 0;
int sense2Val = 0;
int sense3Val = 0;
int sense4Val = 0;

// SPI Parent vars
byte ParentReceive = 0;
byte ParentSend = 0;

// Game logic vars
int points = 0;

// Servo stuff
Servo myservo; 
int pos = 0; 
int direct = 0;

// Photoresistor & LED
const int photoRes = A3;
double photoVal = 0;
const int ledPin = 6;

//-----------------------------------------------------------
// Define statements (other)
//-----------------------------------------------------------

// Game logic vars -------------

// goal points to pass level
#define pointsL 5
#define pointsM 20
#define pointsH 50
#define points0 1
#define pointsPhoto 10

//-----------------------------------------------------------
// Setup
//-----------------------------------------------------------

void setup()
{
  // Start serial com at Baud Rate 115200
  Serial.begin(115200);
  Serial.println("Parent Starting");

  // SPI stuff
  SPI.begin();                          // begin SPI com
  SPI.setClockDivider(SPI_CLOCK_DIV8);  // set divider 8
  digitalWrite(SS, HIGH);               // set SS to HIGH since we didn't start any transfer to child Arduino

  // Attach sensor pins
  pinMode(sense1, INPUT);
  pinMode(sense2, INPUT);
  pinMode(sense3, INPUT);
  pinMode(sense4, INPUT);

  // Attach servo
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object

  // Photoresistor & LED
  pinMode(ledPin, OUTPUT);
}

//-----------------------------------------------------------
// Helper functions
//-----------------------------------------------------------


//-----------------------------------------------------------
// Main loop
//-----------------------------------------------------------

void loop()
{
  // read status of child button and store value in ChildSend to send to parent arduino via SPDR
  sense1Val = digitalRead(sense1);
  sense2Val = digitalRead(sense2);
  sense3Val = digitalRead(sense3);
  sense4Val = digitalRead(sense4);

  // read status of photoresistor
  photoVal = analogRead(photoRes);
  Serial.println(photoVal);

  if (sense1Val == LOW)
  {
    points = pointsH;
    //Serial.println(points);
    delay(200);
  }
  else if (sense2Val == LOW)
  {
    points = pointsM;
    //Serial.println(points);
    delay(200);
  }
  else if (sense3Val == LOW)
  {
    points = pointsL;
    //Serial.println(points);
    delay(200);
  }
  else if (sense4Val == LOW)
  {
    points = points0;
    //Serial.println(points);
    delay(200);
  }
  else if (photoVal < 130) {
    points = pointsPhoto;
    digitalWrite(ledPin, HIGH);
    Serial.println(points);
    Serial.println("LED on");
    Serial.println();
    delay(200);
  }
  else
  {
    points = 0;
    digitalWrite(ledPin, LOW);
    Serial.println("LED off");
    //Serial.println();
  }

  Serial.println(points);
  
  // Before sending X to the child,
  // we need to set SS to LOW to begin the transfer from parent to child.
  digitalWrite(SS, LOW);
  
  // Send the push button value stored in ParentSend var to the child
  // and also receive value from child that will be stored in ParentReceive var
  ParentSend = points;
  ParentReceive = SPI.transfer(ParentSend);

  // Move servo motor
  if (pos == 180)
  {
    direct = 1;
    pos -= 2;
    myservo.write(pos);
    delay(15);
  }
  else if (pos == 0)
  {
    direct = 0;
    pos += 2;
    myservo.write(pos);
    delay(15);
  }
  else if (direct == 0)
  {
    pos += 2;
    myservo.write(pos);
    delay(15);
  }
  else  // direct == 1
  {
    pos -= 2;
    myservo.write(pos);
    delay(15);
  }

  Serial.println(pos);
  Serial.println();

  //delay(200);
//
//  for (pos = 0; pos <= 180; pos += 3 ) { // goes from 0 degrees 180 degrees
//    // in steps of 1 degree
//    myservo.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(15);                       // waits 15ms for the servo to reach the position
//  }
}
