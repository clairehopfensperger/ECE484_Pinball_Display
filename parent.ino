// ECE 484
// Pinball Field - SPI Parent (Display - SPI Child)
// Claire Hopfensperger

//-----------------------------------------------------------
// Include libraries, set up objects, define pins
//-----------------------------------------------------------

// SPI stuff
#include <SPI.h>

//-----------------------------------------------------------
// Pin/var delcarations
//-----------------------------------------------------------

// Pin declarations
const int buttonG = 5;
const int buttonY = 6;
const int buttonO = 7;

// Button vars (HIGH or LOW)
int buttonGValue = 0;
int buttonYValue = 0;
int buttonOValue = 0;

// SPI Parent vars
byte ParentReceive = 0;
byte ParentSend = 0;

// Game logic vars
int points = 0;

//-----------------------------------------------------------
// Define statements (other)
//-----------------------------------------------------------

// Game logic vars -------------

// goal points to pass level
#define pointsG 5
#define pointsY 20
#define pointsO 50

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

  // Attach button and LED pins
  pinMode(buttonG, INPUT);
  pinMode(buttonY, INPUT);
  pinMode(buttonO, INPUT);
}

//-----------------------------------------------------------
// Helper functions
//-----------------------------------------------------------

/*
 * Debounces button and doesn't read held-down button as multiple presses
 */
int buttonDebounce(int button_press)
{
  int button_on;
  
  if (button_press == HIGH)
  {
    delay(150);
    if(button_press == LOW)
    {
      button_on = 1;
    }
    else
    {
      button_on = 0;
    }
  }
  else
  {
    button_on = 0;
  }
  return button_on;
}

//-----------------------------------------------------------
// Main loop
//-----------------------------------------------------------

void loop()
{
  // read status of child button and store value in ChildSend to send to parent arduino via SPDR
  buttonGValue = digitalRead(buttonG);
  buttonYValue = digitalRead(buttonY);
  buttonOValue = digitalRead(buttonO);

  if (buttonGValue == HIGH)
  {
    points = pointsG;
    Serial.println(points);
    delay(200);
  }
  else if (buttonYValue == HIGH)
  {
    points = pointsY;
    Serial.println(points);
    delay(200);
  }
  else if (buttonOValue == HIGH)
  {
    points = pointsO;
    Serial.println(points);
    delay(200);
  }
  else
  {
    points = 0;
  }
  
  // Before sending X to the child,
  // we need to set SS to LOW to begin the transfer from parent to child.
  digitalWrite(SS, LOW);
  
  // Send the push button value stored in ParentSend var to the child
  // and also receive value from child that will be stored in ParentReceive var
  ParentSend = points;
  ParentReceive = SPI.transfer(ParentSend);

  delay(100);
}
