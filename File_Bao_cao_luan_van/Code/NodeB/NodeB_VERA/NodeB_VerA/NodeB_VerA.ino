//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_ADS1X15.h>
//include for DHT22
#include "DHT.h"
//***************

//include for Co2
#include "SoftwareSerial.h"
#include <MHZ19.h>
#include <SoftwareSerial.h>
//***************


//define for DHT22
#define DHTpin 4
#define DHTTYPE DHT22   // DHT 22
//***************

//define for GAS
//#define GASpin 35
//***************

//define for H2S
//#define H2Spin 32
//***************


#define pinDOC_PINLITHIUM 36
float gtcambien;
float vol_out;
float vol_in;
int pt;

//***************
//define for MHZ-19
#define RX_PIN 16                                          // Rx pin of the MHZ19 Tx pin is attached to D2
#define TX_PIN 17                                          // Tx pin of the MHZ19 Rx pin is attached to D3
#define BAUDRATE 9600                                     // Device to MH-Z19 Serial baudrate (should not be changed)
MHZ19 myMHZ19;                                             // Constructor for library
SoftwareSerial mySerial(RX_PIN, TX_PIN);                   // (Uno example) create device to MH-Z19 serial
//***************

DHT dht(DHTpin, DHTTYPE);

// ADS1115
Adafruit_ADS1115 ads;

//global variables
String getPPM;
String getHumi;
String getTemper;
String getGas;
//***************

int count = 0;
int hh = 0;
int mm = 0;
int ss = 0;
String getTimetoString;
int co2Value = 0;
float gasValue = 0;
float h2sValue = 0;
int NodeB = 2;
//define the pins used by the LoRa transceiver module
#define ss 5
#define rst 14
#define dio0 2

#define BAND 433E6    //433E6 for Asia, 866E6 for Europe, 915E6 for North America

//packet counter
int readingID = 0;

int counter = 0;
String LoRaMessage = "Node 2 Gui\n";
String LoRaData;
unsigned long Usertime;

//Initialize LoRa module
void startLoRA()
{
  LoRa.setPins(ss, rst, dio0); //setup LoRa transceiver module

  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(500);
  }
  if (counter == 10)
  {
    // Increment readingID on every new reading
    readingID++;
    Serial.println("Starting LoRa failed!");
  }
  Serial.println("LoRa Initialization OK!");
  delay(2000);
}


void readingPacket()
{
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    while (LoRa.available())    // Read packet
    {
      String LoRaData = LoRa.readString();
      Serial.print(LoRaData);
    }
  }
}

void sendingPacket(String s)
{
  LoRa.beginPacket();
  LoRa.print(s);
  LoRa.endPacket();
  Serial.println(s);
}

void setup()
{
  Serial.begin(115200);
  startLoRA();
  //=====================================================================================
  ads.setGain(GAIN_TWOTHIRDS);

  dht.begin();

  // Start Co2
  mySerial.begin(BAUDRATE);                               // (Uno example) device to MH-Z19 serial start
  myMHZ19.begin(mySerial);                                // *Serial(Stream) refence must be passed to library begin().
  myMHZ19.autoCalibration();                              // Turn auto calibration ON (OFF autoCalibration(false))

  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
}
unsigned long currentMillis_Sensor = 0;
float humiValue = 0;
float tempValue = 0;
void loop()
{

  if ((unsigned long)(millis() - currentMillis_Sensor) >= 6000)
  {
    currentMillis_Sensor = millis();

    humiValue = dht.readHumidity();
    tempValue = dht.readTemperature();

    co2Value = myMHZ19.getCO2();

    int16_t adc0, adc1, adc2ToReadGas, adc3ToReadH2S;

    adc2ToReadGas = ads.readADC_SingleEnded(2);
    adc3ToReadH2S = ads.readADC_SingleEnded(3);

    gtcambien = analogRead(pinDOC_PINLITHIUM);
    //Serial.print("Cảm biến: "); Serial.println(gtcambien);

    vol_out = (gtcambien * 5) / 4096.0; // Chuyển từ analog sang V; 0-1023 ~ 0-5v
    //Serial.print("Volt out: "); Serial.println(vol_out);

    vol_in = float(vol_out / float(980.0 / float(10170.0 + 980.0))); // Tính vol nguồn cần đo
    //Serial.print("Volt nguồn: "); Serial.println(vol_in);

    pt = (vol_in * 100) / 8.4;


    float gasValue = adc2ToReadGas * 0.0625; //gas
    float h2sValue = adc3ToReadH2S * 0.0625; //
    //  pt = adc3 * 0.0625;

    //  Serial.println(gasValue);
    //  Serial.println(h2sValue);
  }

  String s =String(NodeB) + ":" + String(humiValue) + ":" + String(tempValue) + ":" + String(co2Value) + ":" + String(h2sValue) + ":" + String(gasValue) + ":" + String(pt);
  if (LoRaData == "N2")
  {
    sendingPacket(s);
  }
  // cập nhật lại biến time

}
