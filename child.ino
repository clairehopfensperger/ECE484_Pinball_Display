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

// time counter vars
int levelTime = 0;            // Time in seconds for each level
unsigned long startTime = 0;  // Start time for the current level
unsigned long currentTime = 0;
unsigned long elapsedTime = 0;
unsigned long oldElapsedTime = 0;

//-----------------------------------------------------------
// Define statements (other)
//-----------------------------------------------------------

// Game logic vars -------------

// goal points to pass level
#define lvl1points 15
#define lvl2points 30
#define lvl3points 50

// goal points
#define pointsPhoto 10

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
  SPCR |= _BV(SPE);       // turn on SPI in child mode by using SPI control reg
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
  
  // Timer stuff
  levelTime = 60;       // Set initial time for level 1
  startTime = millis(); // Start the timer
  elapsedTime = levelTime;
  oldElapsedTime = levelTime;
  
  // Reset LCD
  lcd.clear();
  lcd.print("Welcome to");
  lcd.setCursor(0, 1);
  lcd.print("Barbie Pinball!");
  delay(2000);
  lcd.clear();

  // Level announcement
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LEVEL ");
  lcd.print(level);
  lcd.print("!!");
  delay(2000);

  lcd.clear()
  lcd.setCursor(0, 0);
  lcd.print("Goal: ");
  lcd.print(currentPoints);

  lcd.setCursor(0, 1);
  lcd.print("LVL: ");
  lcd.print(level);

  lcd.print(" Time: ");
  lcd.print(levelTime);
}

//-----------------------------------------------------------
// Main loop
//-----------------------------------------------------------

void loop()
{
  
  buttonResetVal = digitalRead(buttonReset);

  // Keep track of time
  oldElapsedTime = elapsedTime;
  
  currentTime = millis();
  elapsedTime = (currentTime - startTime) / 1000;  // Convert milliseconds to seconds

  if (elapsedTime == oldElapsedTime + 1)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Goal: ");
    lcd.print(currentPoints);
    
    lcd.setCursor(0, 1);
    lcd.print("LVL: ");
    lcd.print(level);
  
    lcd.print(" Time: ");
    lcd.print(levelTime - elapsedTime);
  }

  if (elapsedTime < levelTime && currentPoints > 0)
  {
    // set child LED to ON or OFF depending on ChildReceive value
    if (received)
    {
      oldPoints = currentPoints;
      currentPoints -= ChildReceive;
      
      // turn master LED ON or OFF based on Masterreceive value
      if(currentPoints != oldPoints)
      { 
        // Refresh display
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Goal: ");
        lcd.print(currentPoints);
        
        lcd.setCursor(0, 1);
        lcd.print("LVL: ");
        lcd.print(level);
      
        lcd.print(" Time: ");
        lcd.print(levelTime - elapsedTime);
      }
      
      ChildSend = 0;
      SPDR = ChildSend;  // send x to parent vis SPDR
  
      received = false;
      delay(100);
    }
  }
  
  else  // if (elapsedTime >= levelTime || currentPoints <= 0)
  {
    
    if (currentPoints <= 0)  // Player reached goal points
    {

      if (level == 1)
      {
        goalPoints = lvl2points;
        levelTime += 30;
      }
      else if (level == 2)
      {
        goalPoints = lvl3points;
        levelTime += 20;
      }
      else
      {
        goalPoints += 10;
        levelTime += 10;
      }

      currentPoints = goalPoints;
      level++;
  
      // Level up announcement
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("LEVEL ");
      lcd.print(level);
      lcd.print("!!");
      delay(2000);

      // Refresh display
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Goal: ");
      lcd.print(currentPoints);
    
      lcd.setCursor(0, 1);
      lcd.print("LVL: ");
      lcd.print(level);
    
      lcd.print(" Time: ");
      lcd.print(levelTime - elapsedTime);
    }
    
    else  // Player loses, game over
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Game over!");
      lcd.setCursor(0, 1);
      lcd.print("Top LVL: ");
      lcd.print(level);
      delay(5000);
    }
  }
}
