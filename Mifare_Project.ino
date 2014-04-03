/**************************************************************************/
/*! 
  This is a fork of readMiFareClassic by Adafruit.
  It has been extremely heavily modified to add several new features and functionality, but carries the same BSD License that
  is included in the library.

  You must have the library installed for this code to function proprtly.
  
  This sketch is designed to be used as a RFID door lock with
  the Adafruit NFC PN532 Breakout Board.

  There is a fritzing diagram included in this repository.
  
***************************************************************************
  
  Connect PN532 Breakout as shown in Adafruit tutorial located at:

  Connect Lcd as shown at Adafruit tutorial located at:

  Connect Green LED to pin 13

  Connect Red LED to pin 14 (A0)

  Connect 5V All in one Piezo (buy it at: ) to pin 15 (A1)

  Connect Solid State Relay or Relay Board to pin 6
  
  
*/
/**************************************************************************/

#include <Adafruit_PN532.h>
#include <LiquidCrystal.h>
#define SCK  (2)
#define MOSI (3)
#define SS   (4)
#define MISO (5)

//define some variables for the leds
const int greenled = 13;
const int redled = 14;
const int buzzer = 15;
const int relay = 6;
int rlread = 0;





//start the libraries
Adafruit_PN532 nfc(SCK, MISO, MOSI, SS);
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

//Our setup phase
void setup(void) {
  //Make relay HIGH (Off)
  digitalWrite(relay, HIGH);


  //start serial
  Serial.begin(115200);
  Serial.println("Hello!");

  //define the pinmodes
  pinMode(greenled, OUTPUT);
  pinMode(redled, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(relay, OUTPUT);

  //Begin NFC.
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  lcd.setCursor(0,1);
  Serial.println("Waiting for an ISO14443A Card ...");
}


void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
  //Scan for the state of the relay
  rlread = digitalRead(relay);

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    
    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ... 
      uint32_t cardid = uid[0];
      cardid <<= 8;
      cardid |= uid[1];
      cardid <<= 8;
      cardid |= uid[2];  
      cardid <<= 8;
      cardid |= uid[3]; 
      Serial.print("Seems to be a Mifare Classic card #");
      Serial.println(cardid);
      
      if (cardid == 2366586084)  {    //Change to your card number
        lcd.setCursor(0,0);
        lcd.println("Authorized");
        Serial.println("Correct Card");
        digitalWrite(greenled, HIGH);

          //If the lights are off, turn them on.
          if (rlread == HIGH) {
            digitalWrite(relay, LOW);
            Serial.print("Light is now on.");
            lcd.setCursor(0,1);
            lcd.print("Light is on.");
          }

          //If the lights are on turn them off.
          else if (rlread == LOW) {
            digitalWrite(relay, HIGH);
            Serial.println("Light is now off.");
            lcd.setCursor(0,1);
            lcd.print("Light is off.");
          }
          delay(2000);
        digitalWrite(greenled, LOW);
      }
      
      else {
        lcd.print("Not Authorized");
        digitalWrite(buzzer, HIGH);
        Serial.println("Incorrect Card.");
        digitalWrite(redled, HIGH);
        delay(200);
        digitalWrite(redled, LOW);
        delay(200);
        digitalWrite(redled, HIGH);
        delay(200);
        digitalWrite(redled, LOW);
        delay(200);
        digitalWrite(buzzer, LOW);
        
      }
      lcd.clear();
    }
    Serial.println("");
  }
}

