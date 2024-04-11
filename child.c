#include<SPI.h>

// Pin declarations
const int ipbutton = 7;
const int LED = 2;

// Variable declarations
int buttonvalue = 0;
int x = 0;
volatile bool received = false;
volatile byte ChildReceive = 0;
volatile byte ChildSend = 0;

void setup()
{
  // Start serial com at Baud Rate 115200
  Serial.begin(115200);
  Serial.println("Child Starting");

  // Attach button and LED pins
  pinMode(ipbutton, INPUT);                
  pinMode(LED, OUTPUT);

  // SPI stuff
  pinMode(MISO, OUTPUT);  // sets MISO as output (enable send data to parent in)
  SPCR |= _BV(SPE);  // turn on SPI in child mode by using SPI control reg
  received = false;
  SPI.attachInterrupt();  // turn ON interrupt for SPI com. 
                          // If data is received from master, the interrupt routine is called and the received value is taken from SPDR (SPI data reg)
}

/*
 * Value from parent is taken from SPDR and stored in ChildReceive var.
 */
ISR (SPI_STC_vect)
{
  ChildReceive = SPDR;                  
  received = true;                       
}

void loop()
{
  // set child LED to ON or OFF depending on ChildReceive value
  if (received)
  {
    if (ChildReceive == 1)
    {
      digitalWrite(LED, HIGH); //Sets pin 7 as HIGH LED ON
      Serial.println("Child LED ON");
    }
    else
    {
      digitalWrite(LED, LOW);     //Sets pin 7 as LOW LED OFF
      Serial.println("Child LED OFF");
    }

    // read status of child button and store value in ChildSend to send to parent arduino via SPDR
    buttonvalue = digitalRead(ipbutton); 
    Serial.print("Button: ");
    Serial.println(buttonvalue);
    
    if (buttonvalue == HIGH)              
    {
      x = 1;
    }
    else
    {
      x = 0;
    }
    
    ChildSend = x;
    SPDR = ChildSend;  // send x to parent vis SPDR

    received = false;
    delay(100);
  }
}
