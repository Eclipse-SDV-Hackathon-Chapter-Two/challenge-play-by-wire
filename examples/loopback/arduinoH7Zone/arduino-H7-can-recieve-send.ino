/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino_CAN.h>

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/
static uint32_t const CAN_ID = 0x13;

uint8_t breakSignal = 0;
static uint32_t msg_cnt = 0;

void setup()
{
  Serial.begin(115200);

  if (!CAN.begin(CanBitRate::BR_500k))
  {
    Serial.println("CAN.begin(...) failed.");
    for (;;) {}
  }
}

void loop()
{
  if (CAN.available())
  {
    CanMsg const msg = CAN.read();
    if(msg.id == 0x12)
    {
      Serial.print("X: ");
      Serial.println(msg.data[0]);
      Serial.print("Y: ");
      Serial.println(msg.data[1]);
      Serial.print("Z: ");
      Serial.println(msg.data[2]);
      breakSignal = msg.data[2];
    }

  }
    // send packet: id is 11 bits, packet can contain up to 8 bytes of data
  Serial.println("Sending packet ... ");
  /* Assemble a CAN message with the format of
   * 0xCA 0xFE 0x00 0x00 [4 byte message counter]
   */
  uint8_t const msg_data[] = {breakSignal,0,0,0,0,0,0,0};
  memcpy((void *)(msg_data + 4), &msg_cnt, sizeof(msg_cnt));
  CanMsg const msg(CanStandardId(CAN_ID), sizeof(msg_data), msg_data);

  /* Transmit the CAN message, capture and display an
   * error core in case of failure.
   */
  if (int const rc = CAN.write(msg); rc < 0)
  {
    Serial.print  ("CAN.write(...) failed with error code ");
    Serial.println(rc);
    for (;;) { }
  }

  /* Increase the message counter. */
  msg_cnt++;


  // CAN.beginPacket(0x13);
  // //Send Break Light signal
  // CAN.write(breakSignal);
  // CAN.endPacket();
  // Serial.println("CAN send done");
  delay(100);
  //Serial.println(msg);
}
