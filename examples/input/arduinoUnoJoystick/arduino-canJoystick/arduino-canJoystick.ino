
#include <Arduino.h>
#include <CAN.h>
#include <FastLED.h>  // Include FastLED library

#define NUM_LEDS 8    // Number of LEDs in the chain
#define DATA_PIN 6    // Data pin for LED control

CRGB leds[NUM_LEDS];  // Array to hold LED color data

const int xPin = A0;  //the VRX attach to
const int yPin = A1;  //the VRY attach to
const int swPin = 8;  //the SW attach to
int breakSignal =0;
uint8_t const msg_data[] = {0,0,0,0,0,0,0,0};

void setup() {
    // Initialize Serial for debug
    Serial.begin(115200);
    while (!Serial) {
        delay(1000);
    }
  Serial.println("CAN Joystick");
  //Initialize Joystick
  pinMode(swPin, INPUT_PULLUP);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // Initialize LEDs

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
  
    delay(300);
}

void loop() {
  // try to parse packet
  int packetSize = CAN.parsePacket();
  int msgdata[packetSize];

  if (packetSize) {
    // received a packet
    Serial.print("Received ");

    if (CAN.packetExtended()) {
      Serial.print("extended ");
    }

    if (CAN.packetRtr()) {
      // Remote transmission request, packet contains no data
      Serial.print("RTR ");
    }

    Serial.print("packet with id 0x");
    Serial.print(CAN.packetId(), HEX);

    if (CAN.packetRtr()) {
      Serial.print(" and requested length ");
      Serial.println(CAN.packetDlc());
    } else {
      Serial.print(" and length ");
      Serial.println(packetSize);

      if(CAN.packetId() == 0x13)
      {
        // only print packet data for non-RTR packets
        int i=0;
        while (CAN.available()) {
          Serial.print(i);
          Serial.print(" ");
          msgdata[i] = CAN.read();
          Serial.println(msgdata[i]);
          if(i==0)
          {
            breakSignal = msgdata[i];
          }
          i++;
          }
        Serial.println(" DataEND");
      }
      
    }

    Serial.println();
  }


  if(breakSignal == 0)
  {
    // Loop through each LED and set it to blue
    for (int dot = 0; dot < NUM_LEDS; dot++) {
      leds[dot] = CRGB::Blue;   // Set the current LED to blue
      FastLED.show();           // Update LEDs
      leds[dot] = CRGB::Black;  // Clear the current LED
      delay(10);                // Wait for a short period before moving to the next LED
    }
  }


    Serial.print("X: ");
    Serial.print(analogRead(xPin)/4, DEC);  // print the value of VRX in DEC
    Serial.print("|Y: ");
    Serial.print(analogRead(yPin)/4, DEC);  // print the value of VRX in DEC
    Serial.print("|Z: ");
    Serial.println(digitalRead(swPin));  // print the value of SW

    // send packet: id is 11 bits, packet can contain up to 8 bytes of data
    Serial.print("Sending packet ... ");
    CAN.beginPacket(0x12);
    CAN.write(analogRead(xPin)/4);
    CAN.write(analogRead(yPin)/4);
    CAN.write(digitalRead(swPin));
    CAN.endPacket();
    Serial.println("CAN send done");
    
    delay(100);

}
