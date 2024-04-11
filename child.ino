// ECE 484
// Pinball Field - SPI Parent (Display - SPI Child)
// Claire Hopfensperger

//-----------------------------------------------------------
// Include libraries, set up objects, define pins
//-----------------------------------------------------------

// SPI stuff
#include<SPI.h>

// I2C LCD stuff
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//-----------------------------------------------------------
// Pin/var delcarations
//-----------------------------------------------------------

// SPI child vars
volatile bool received = false;
volatile byte ChildReceive = 0;
volatile byte ChildSend = 0;

// Game logic vars
int level = 0;
int currentPoints = 0;
int oldPoints = 0;
int goalPoints = 0;

//-----------------------------------------------------------
// Define statements (other)
//-----------------------------------------------------------

// Game logic vars -------------

// goal points to pass level
#define pointsG 50
#define pointsY 200
#define pointsO 500

//-----------------------------------------------------------
// Setup
//-----------------------------------------------------------

void setup()
{
  // Start serial com at Baud Rate 115200
  Serial.begin(115200);
  Serial.println("Child Starting");

  // SPI stuff
  pinMode(MISO, OUTPUT);  // sets MISO as output (enable send data to parent in)
  SPCR |= _BV(SPE);  // turn on SPI in child mode by using SPI control reg
  received = false;
  SPI.attachInterrupt();  // turn ON interrupt for SPI com. 
                          // If data is received from master, the interrupt routine is called and the received value is taken from SPDR (SPI data reg)

  lcd.init();                           // initialize the lcd 
  lcd.backlight();
  lcd.print("Ready to go!");
  delay(500);
  lcd.clear();

  lcd.print("Points: ");
  lcd.print(currentPoints);

}

//-----------------------------------------------------------
// Helper functions
//-----------------------------------------------------------

/*
 * Value from parent is taken from SPDR and stored in ChildReceive var.
 */
ISR (SPI_STC_vect)
{
  ChildReceive = SPDR;                  
  received = true;                       
}

//-----------------------------------------------------------
// Main loop
//-----------------------------------------------------------

void loop()
{
  // set child LED to ON or OFF depending on ChildReceive value
  if (received)
  {
    oldPoints = currentPoints;
    currentPoints += ChildReceive;
    
    // turn master LED ON or OFF based on Masterreceive value
    if(currentPoints != oldPoints)
    {
      lcd.clear();
      lcd.print("Points: ");
      lcd.print(currentPoints);
    }
    
    ChildSend = 0;
    SPDR = ChildSend;  // send x to parent vis SPDR

    received = false;
    delay(100);
  }
}



//--------------------------------------------------------
// Working SPI test code
//--------------------------------------------------------

//#include<SPI.h>
//
//// Pin declarations
//const int ipbutton = 7;
//const int LED = 2;
//
//// Variable declarations
//int buttonvalue = 0;
//int x = 0;
//volatile bool received = false;
//volatile byte ChildReceive = 0;
//volatile byte ChildSend = 0;
//
//void setup()
//{
//  // Start serial com at Baud Rate 115200
//  Serial.begin(115200);
//  Serial.println("Child Starting");
//
//  // Attach button and LED pins
//  pinMode(ipbutton, INPUT);                
//  pinMode(LED, OUTPUT);
//
//  // SPI stuff
//  pinMode(MISO, OUTPUT);  // sets MISO as output (enable send data to parent in)
//  SPCR |= _BV(SPE);  // turn on SPI in child mode by using SPI control reg
//  received = false;
//  SPI.attachInterrupt();  // turn ON interrupt for SPI com. 
//                          // If data is received from master, the interrupt routine is called and the received value is taken from SPDR (SPI data reg)
//}
//
///*
// * Value from parent is taken from SPDR and stored in ChildReceive var.
// */
//ISR (SPI_STC_vect)
//{
//  ChildReceive = SPDR;                  
//  received = true;                       
//}
//
//void loop()
//{
//  // set child LED to ON or OFF depending on ChildReceive value
//  if (received)
//  {
//    if (ChildReceive == 1)
//    {
//      digitalWrite(LED, HIGH); //Sets pin 7 as HIGH LED ON
//      Serial.println("Child LED ON");
//    }
//    else
//    {
//      digitalWrite(LED, LOW);     //Sets pin 7 as LOW LED OFF
//      Serial.println("Child LED OFF");
//    }
//
//    // read status of child button and store value in ChildSend to send to parent arduino via SPDR
//    buttonvalue = digitalRead(ipbutton); 
//    Serial.print("Button: ");
//    Serial.println(buttonvalue);
//    
//    if (buttonvalue == HIGH)              
//    {
//      x = 1;
//    }
//    else
//    {
//      x = 0;
//    }
//    
//    ChildSend = x;
//    SPDR = ChildSend;  // send x to parent vis SPDR
//
//    received = false;
//    delay(100);
//  }
//}
