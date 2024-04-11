// ECE 484
// Pinball Display - SPI Child (Field - SPI Parent)
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



//----------------------------------------------------------//
// Working SPI test code                                    //
//----------------------------------------------------------//

//#include <SPI.h>
//#include <Wire.h>
//#include <LiquidCrystal_I2C.h>
//
//LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
//
//// Pin declarations
//const int ipbutton = 7;
//const int LED = 2;
//
//// Variable declarations
//int buttonvalue = 0;
//int x = 0;
//byte ParentReceive = 0;
//byte ParentSend = 0;
//
//void setup()
//{
//  // Start serial com at Baud Rate 115200
//  Serial.begin(115200);
//  Serial.println("Parent Starting");
//
//  // Attach button and LED pins
//  pinMode(ipbutton, INPUT);                
//  pinMode(LED, OUTPUT);
//
//  // SPI stuff
//  SPI.begin();  // begin SPI com
//  SPI.setClockDivider(SPI_CLOCK_DIV8);  // set divider 8
//  digitalWrite(SS, HIGH);  // set SS to HIGH since we didn't start any transfer to child Arduino
//
//  lcd.init();                      // initialize the lcd 
//  lcd.backlight();
//  lcd.print("Ready to go!");
//  delay(500);
//  lcd.clear();
//}
//
//void loop()
//{
//  // Get button value
//  buttonvalue = digitalRead(ipbutton);
//  Serial.print("Button: ");
//  Serial.println(buttonvalue);
//
//  // set x value to be sent to child based on button input
//  if (buttonvalue == HIGH)               
//  {
//    x = 1;
//  }
//  else
//  {
//    x = 0;
//  }
//
//  // Before sending X to the child,
//  // we need to set SS to LOW to begin the transfer from parent to child.
//  digitalWrite(SS, LOW);
//  
//  // Send the push button value stored in ParentSend var to the child
//  // and also receive value from child that will be stored in ParentReceive var
//  ParentSend = x;
//  ParentReceive = SPI.transfer(ParentSend);
//
//  // turn master LED ON or OFF based on Masterreceive value
//  if(ParentReceive == 1)
//  {
//    digitalWrite(LED, HIGH);        //Sets pin 7 HIGH
//    Serial.println("Parent LED ON");
//    lcd.clear();
//    lcd.print("Parent LED ON");
//  }
//  else
//  {
//   digitalWrite(LED, LOW);         //Sets pin 7 LOW
//   Serial.println("Parent LED OFF");
//   lcd.clear();
//   lcd.print("Parent LED OFF");
//  }
//
//  delay(100);
//}
