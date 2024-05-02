// ECE 484
// Pinball Display - SPI Child (Field - SPI Parent)
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

// Pin declarations
const int buttonReset = 2;

// Button vars (HIGH or LOW)
int buttonResetVal = 0;

// SPI child vars
volatile bool received = false;
volatile byte ChildReceive = 0;
volatile byte ChildSend = 0;

// Game logic vars
int level = 0;
int balls = 0;
int currentPoints = 0;
int oldPoints = 0;
int goalPoints = 0;

//-----------------------------------------------------------
// Define statements (other)
//-----------------------------------------------------------

// Game logic vars -------------

// goal points to pass level
#define lvl1points 15
#define lvl2points 60
#define lvl3points 150

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

  lcd.init();             // initialize the lcd 
  lcd.backlight();
  resetGame();
}

//-----------------------------------------------------------
// Helper functions
//-----------------------------------------------------------

/*
 * Value from parent is taken from SPDR and stored in ChildReceive var.
 */
ISR(SPI_STC_vect)
{
  ChildReceive = SPDR;                  
  received = true;                       
}

/*
 * Resets LCD and game variables
 */
void resetGame()
{
  level = 1;
  balls = 3;
  goalPoints = lvl1points;
  currentPoints = goalPoints;
  oldPoints = 0;

  lcd.clear();
  lcd.print("Welcome to");
  lcd.setCursor(0, 1);
  lcd.print("Barbie Pinball!");
  delay(1000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Goal: ");
  lcd.print(currentPoints);

  lcd.setCursor(0, 1);
  lcd.print("LVL: ");
  lcd.print(level);

  lcd.print(" Balls: ");
  lcd.print(balls);
}

//-----------------------------------------------------------
// Main loop
//-----------------------------------------------------------

void loop()
{
  
  buttonResetVal = digitalRead(buttonReset);
  
  if (balls > 0)
  {
    // set child LED to ON or OFF depending on ChildReceive value
    if (received)
    {
      oldPoints = currentPoints;
      currentPoints -= ChildReceive;
      
      // turn master LED ON or OFF based on Masterreceive value
      if(currentPoints != oldPoints)
      {
        balls--;
        
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Goal: ");
        lcd.print(currentPoints);
      
        lcd.setCursor(0, 1);
        lcd.print("LVL: ");
        lcd.print(level);
      
        lcd.print(" Balls: ");
        lcd.print(balls);


        Serial.print("Goal: ");
        Serial.println(currentPoints);
        Serial.print("LVL: ");
        Serial.println(level);
        Serial.print("Balls: ");
        Serial.println(balls);
        Serial.println();
      }
      
      ChildSend = 0;
      SPDR = ChildSend;  // send x to parent vis SPDR
  
      received = false;
      delay(100);
    }
  }
  
  else  // balls == 0
  {
    
    if (currentPoints <= 0)  // Player reached goal points
    {
      balls = 3;

      if (level == 1)
      {
        goalPoints = lvl2points;
      }
      else if (level == 2)
      {
        goalPoints = lvl3points;
      }
      else
      {
        goalPoints += 100;
      }

      currentPoints = goalPoints;

      Serial.print("goalPoints: ");
      Serial.println(goalPoints);
      Serial.println();
      
      level++;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Goal: ");
      lcd.print(currentPoints);
    
      lcd.setCursor(0, 1);
      lcd.print("LVL: ");
      lcd.print(level);
    
      lcd.print(" Balls: ");
      lcd.print(balls);
    }
    
    else  // Player loses, game over
    {
      lcd.clear();
      lcd.print("You lose!");
      delay(1000);

      // Player can reset game by holding down "reset" button until screen updates
      if (buttonResetVal == HIGH)
      {
        resetGame();
      }
    }
  }
}
