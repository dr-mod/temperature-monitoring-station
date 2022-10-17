#include <DHT.h>

#include <RF24.h>
#include <nRF24L01.h>
#include "LowPower.h"

#define PIN_RF24_CSN            10
#define PIN_RF24_CE             14

#define NRF24_CHANNEL           100
#define NRF24_CRC_LENGTH        RF24_CRC_16
#define NRF24_DATA_RATE         RF24_250KBPS
#define NRF24_PAYLOAD_SIZE      32
#define NRF24_PA_LEVEL          RF24_PA_MIN
#define NRF24_RETRY_DELAY       2
#define NRF24_RETRY_COUNT       5

#define PROTOCOL 0x01


RF24 radio(PIN_RF24_CE, PIN_RF24_CSN);
DHT dht(15, DHT21);

byte rf24_tx[6] = "0IWSO";
byte payload[32];

void setup() {
  delay(100);
  nrf24_setup();
  radio.printDetails();
  dht.begin();
}

void loop() {
    float t, h;
    dht.begin();
    h = dht.readHumidity();
    t = dht.readTemperature();

    radio.powerUp();
    delay(150);
    send_reading(PROTOCOL, t, h);
    radio.powerDown();
    delay(150);
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}

void send_reading(byte protocol, float temperature, float humidity)
{
  int offset = 0;
  memcpy(payload + offset, (byte *)(&protocol), sizeof(protocol)); 
  offset += sizeof(protocol); 
  memcpy(payload + offset, (byte *)(&temperature), sizeof(temperature)); 
  offset += sizeof(temperature);
  memcpy(payload + offset, (byte *)(&humidity), sizeof(humidity)); 
  offset += sizeof(humidity);
  radio.write(payload, offset);
}

void nrf24_setup()
{
  radio.begin();
  radio.enableDynamicPayloads();
  radio.setAutoAck(true);                 
  radio.setPALevel(NRF24_PA_LEVEL);
  radio.setRetries(NRF24_RETRY_DELAY, NRF24_RETRY_COUNT);              
  radio.setDataRate(NRF24_DATA_RATE);          
  radio.setChannel(NRF24_CHANNEL);
  radio.setCRCLength(NRF24_CRC_LENGTH);
  radio.setPayloadSize(NRF24_PAYLOAD_SIZE);
  radio.openWritingPipe(rf24_tx);  
  radio.stopListening();
}
