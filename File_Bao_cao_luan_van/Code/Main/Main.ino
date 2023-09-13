#include <WiFi.h>
//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

#include<SoftwareSerial.h>
SoftwareSerial sim(32, 33);

String valMess;
int onl = 1;
int testWar = 1;
int Ch42 = 0;
int Ch41 = 0;
int PinM = 1;
int Pin1 = 1;
int Pin2 = 1;
int h2s1 = 1;
int h2s2 = 1;
int Nd1 = 1;
int Nd2 = 1;
int chot1 =1;
int chot2 =1;

//define the pins used by the LoRa transceiver module
#define ss 5
#define rst 14
#define dio0 2

#define pinDOC_PINLITHIUM 36
float gtcambien;
float vol_out;
float vol_in;
int ptm;

#define BAND 433E6    //433E6 for Asia, 866E6 for Europe, 915E6 for North America

// Initialize variables to get and save LoRa data
String LoRaMessage = "";
int rssi;
int nodeCall = 1;
unsigned long Usertime = 1;
int readingID = 0;
String LoRaData = "";
#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#include <CayenneMQTTESP32.h>  //IMPPORT LIBRARY FROM http://easycoding.tn/index.php/resources/

char Cayenneusername[] = "1d3582e0-24ff-11ed-bf0a-bb4ba43bd3f6";
char Cayennepassword[] = "500c2660d1ed84607a6fa3f67e484ff1dd1b7cb0";
char CayenneclientID[] = "23e5da40-24ff-11ed-baf6-35fab7fd0ac8";

int tempValue = 0;
int humiValue = 0;
int count = 0;

String getTimetoString;
unsigned long previousMillis = 0;
unsigned long interval = 30000;

unsigned long currentMillis_Cayenne = 0;
void readingPacket()
{
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    while (LoRa.available())    // Read packet
    {
      LoRaData = LoRa.readString();
    }
  }
}

void sendingPacket(String s)
{
  LoRa.beginPacket();
  LoRa.print(s);
  LoRa.endPacket();
  //Serial.println(s);
}
int checkNode = 0;

String pollingStr = "";
float humi = 0;
float temp = 0;
float co2Value = 0;
float h2sValue = 0;
float gasValue = 0;
int pt = 0;

float humiB = 0;
float tempB = 0;
float co2ValueB = 0;
float h2sValueB = 0;
float gasValueB = 0;
int ptB = 0;

String splitString(String str, String delim, uint16_t pos) {
  String tmp = str;
  for (int i = 0; i < pos; i++) {
    tmp = tmp.substring(tmp.indexOf(delim) + 1);
    if (tmp.indexOf(delim) == -1
        && i != pos - 1 )
      return "";
  }
  return tmp.substring(0, tmp.indexOf(delim));
}



void setup()
{
  Serial.begin(115200);
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
  WiFi.disconnect();
  delay(3000);
  Serial.println("START");
  WiFi.begin("B7-512", "DHCT4139");
  while ((!(WiFi.status() == WL_CONNECTED))) {
    delay(300);
    Serial.print("..");
  }
  Serial.println("Connected");
  Serial.println("Your IP is");
  Serial.println((WiFi.localIP()));
  Cayenne.begin(Cayenneusername, Cayennepassword, CayenneclientID);
  int counter;

  sim.begin(9600);
  delay(100);
  sim.println("AT");
  sim.println("AT+CMGF=1");
  sim.println("AT+CNMI=1,2,0,0,0");
  //setup LoRa transceiver module

  LoRa.setPins(ss, rst, dio0); //setup LoRa transceiver module

  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(2000);
  }
  if (counter == 10) {
    // Increment readingID on every new reading
    Serial.println("Starting LoRa failed!");
  }
  Serial.println("LoRa Initialization OK!");
  delay(2000);
}
unsigned long currentMillis_Lora = 0;
unsigned long currentMillis_reconnectWIFI = 0;
int countA = 0;
int countB = 0;
void loop()
{
  Messonl;
  Cayenne.loop();
  if ((unsigned long)(millis() - currentMillis_Cayenne) >= 60000)
  {
    currentMillis_Cayenne = millis();
    Cayenne.virtualWrite(0, temp, "temp", "Celsius");
    delay(100);
    Cayenne.virtualWrite(1, humi, "rel_hum", "Percent");
    delay(100);
    Cayenne.virtualWrite(2, co2Value, "co2", "Parts per million");
    delay(100);
    Cayenne.virtualWrite(3, gasValue, "analog_sensor", "Analog");
    delay(100);
    Cayenne.virtualWrite(4, h2sValue,  "analog_sensor", "Analog");
    delay(100);
    Cayenne.virtualWrite(5, pt,  "batt", "p");
    delay(100);

    Cayenne.virtualWrite(6, tempB, "temp", "Celsius");
    delay(100);
    Cayenne.virtualWrite(7, humiB, "rel_hum", "Percent");
    delay(100);
    Cayenne.virtualWrite(8, co2ValueB, "co2", "Parts per million");
    delay(100);
    Cayenne.virtualWrite(9, gasValueB, "analog_sensor", "Analog");
    delay(100);
    Cayenne.virtualWrite(10, h2sValueB,  "analog_sensor", "Analog");
    delay(100);
    Cayenne.virtualWrite(11, ptB,  "batt", "p");
    delay(100);
    Cayenne.virtualWrite(12, ptm,  "batt", "p");
    delay(100);

  }


  if ((unsigned long)(millis() - currentMillis_Lora) >= 4000)
  {
    checkNode = 0;
    currentMillis_Lora = millis();
    if (nodeCall == 1)
    {
      sendingPacket("N1");
      nodeCall = 2;
    }
    else if (nodeCall == 2)
    {
      sendingPacket("N2");
      nodeCall = 1;
    }
  }
  readingPacket();
  pollingStr = LoRaData;
  Serial.println(pollingStr);
  Serial.println(String(nodeCall) + String(checkNode));
  checkNode = splitString(pollingStr, ":", 0).toInt();
  if (nodeCall == 2 && checkNode == 1 )
  {
    countB = 0;
    countA += 1;
    humi = splitString(pollingStr, ":", 1).toFloat();
    temp = splitString(pollingStr, ":", 2).toFloat();
    warningNd1(Nd1, 33, "nhiet do cao_1");
    co2Value = splitString(pollingStr, ":", 3).toFloat();
    h2sValue = splitString(pollingStr, ":", 4).toFloat();
    warningh2s1( h2sValue, 310, "Trai cay thung 1 gan hu");
    gasValue = splitString(pollingStr, ":", 5).toFloat();
    warningCh41( gasValue, 795, "Trai cay thung 1 sap hu");
    pt = splitString(pollingStr, ":", 6).toInt();
    warningPin1(pt , 50, "Canh bao Pin 1");
  }
  else if (nodeCall == 1 && checkNode == 2)
  {
    countA = 0;
    countB += 1;
    humiB = splitString(pollingStr, ":", 1).toFloat();
    tempB = splitString(pollingStr, ":", 2).toFloat();
    warningNd2(tempB, 33, "nhiet do cao_2");
    co2ValueB = splitString(pollingStr, ":", 3).toFloat();
    h2sValueB = splitString(pollingStr, ":", 4).toFloat();
    warningh2s2( h2sValueB, 400, "Trai cay thung 2 gan hu");
    gasValueB = splitString(pollingStr, ":", 5).toFloat();
    warningCh42( gasValueB, 1075, "Trai cay thung 2 sap hu");
    ptB = splitString(pollingStr, ":", 6).toInt();
    warningPin2(ptB , 50, "Canh bao Pin 2");
  }

if (countB > 8)
  {
    humi = 0;
    temp = 0;
    co2Value = 0;
    h2sValue = 0;
    gasValue = 0;
    pt = 0;
  }
  else if (countA > 8)
  {
    humiB = 0;
    tempB = 0;
    co2ValueB = 0;
    h2sValueB = 0;
    gasValueB = 0;
    ptB = 0;
  }

  if (nodeCall == 2)
  {
    Serial.println(String(checkNode) + String(humi) + String(temp) + String(co2Value) + String(h2sValue) + String(gasValue) + String(pt));
  }
  else if (nodeCall == 1)
  {
    Serial.println(String(checkNode) + String(humiB) + String(tempB) + String(co2ValueB) + String(h2sValueB) + String(gasValueB) + String(ptB));
  }
  
  gtcambien = analogRead(pinDOC_PINLITHIUM);
  Serial.print("Cảm biến: "); Serial.println(gtcambien);

  vol_out = (gtcambien * 5) / 4096.0; // Chuyển từ analog sang V; 0-1023 ~ 0-5v
  Serial.print("Volt out: "); Serial.println(vol_out);

  vol_in = float(vol_out / float(980.0 / float(9890.0 + 980.0))); // Tính vol nguồn cần đo
  Serial.print("Volt nguồn: "); Serial.println(vol_in);

  ptm = (vol_in * 100) / 11;
  warningPinM( ptm, 50, "Canh bao Pin Master");
  //Serial.println(String(humi) + " " + String(temp) + " " + String(co2Value) + " " + String(h2sValue) + " " + String(gasValue) + " " + String(pt) + "\n");
  delay(1000);

  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
}

void Messonl() {
  if (onl == 1) {
    repMess("Da ket noi");
    delay(100);
    onl = 0;
  }
}

void warningNd1(float valueNd1, int valueSetNd1, String messNd1) {
  if (valueNd1 > valueSetNd1 && Nd1 == 1) {
    repMess(messNd1);
    delay(100);
    Nd1 = 0;
  }

  if (Serial.available()) {
    sim.write(Serial.read());
  }
  if (sim.available() > 0) {
    valMess = sim.readStringUntil('\n');
    Serial.println(valMess);
    if (valMess == "Ok\r") {
      Nd1 = 1;
      Nd2 = 1;
      h2s1 = 1;
      h2s2 = 1;
      Ch41 = 1;
      Ch42 = 1;
      PinM = 1;
      Pin1 = 1;
      Pin2 = 1;
      repMess("Da xac nhan");
      delay(100);
      Serial.print("Nd1: ");
      Serial.println(Nd1);
    }
  }
  Serial.print("Nd1: ");
  Serial.println(Nd1);
  delay(1000);

}

void warningNd2(float valueNd2, int valueSetNd2, String messNd2) {
  if (valueNd2 > valueSetNd2 && Nd2 == 1) {
    repMess(messNd2);
    delay(100);
    Nd2 = 0;
  }

  if (Serial.available()) {
    sim.write(Serial.read());
  }
  if (sim.available() > 0) {
    valMess = sim.readStringUntil('\n');
    Serial.println(valMess);
    if (valMess == "OK\r") {
      Nd1 = 1;
      Nd2 = 1;
      h2s1 = 1;
      h2s2 = 1;
      Ch41 = 1;
      Ch42 = 1;
      PinM = 1;
      Pin1 = 1;
      Pin2 = 1;
      repMess("Da xac nhan");
      delay(100);
      Serial.print("Nd2: ");
      Serial.println(Nd2);
    }
  }
  Serial.print("Nd2: ");
  Serial.println(Nd2);
  delay(1000);

}

void warningPinM(float valuePinM, int valueSetPinM1,  String messPinM) {
  if (valuePinM < valueSetPinM1 && PinM == 1) {
    repMess(messPinM);
    delay(100);
    PinM = 0;
  }

  if (Serial.available()) {
    sim.write(Serial.read());
  }
  if (sim.available() > 0) {
    valMess = sim.readStringUntil('\n');
    Serial.println(valMess);
    if (valMess == "Ok\r") {
      Nd1 = 1;
      Nd2 = 1;
      h2s1 = 1;
      h2s2 = 1;
      Ch41 = 1;
      Ch42 = 1;
      PinM = 1;
      Pin1 = 1;
      Pin2 = 1;
      repMess("Da xac nhan");
      delay(100);
      Serial.print("PinM: ");
      Serial.println(PinM);
    }
  }
  Serial.print("PinM: ");
  Serial.println(PinM);
  delay(1000);

}

void warningPin1(float valuePin1, int valueSetPin1, String messPin1) {
  if (valuePin1 < valueSetPin1 && Pin1 == 1) {
    repMess(messPin1);
    delay(100);
    Pin1 = 0;
  }

  if (Serial.available()) {
    sim.write(Serial.read());
  }
  if (sim.available() > 0) {
    valMess = sim.readStringUntil('\n');
    Serial.println(valMess);
    if (valMess == "Ok\r") {
      Nd1 = 1;
      Nd2 = 1;
      h2s1 = 1;
      h2s2 = 1;
      Ch41 = 1;
      Ch42 = 1;
      PinM = 1;
      Pin1 = 1;
      Pin2 = 1;
      repMess("Da xac nhan");
      delay(100);
      Serial.print("Pin1: ");
      Serial.println(Pin1);
    }
  }
  Serial.print("Pin1: ");
  Serial.println(Pin1);
  delay(1000);

}
void warningPin2(float valuePin2, int valueSetPin2, String messPin2) {
  if (valuePin2 < valueSetPin2 && Pin2 == 1) {
    repMess(messPin2);
    delay(100);
    Pin2 = 0;
  }

  if (Serial.available()) {
    sim.write(Serial.read());
  }
  if (sim.available() > 0) {
    valMess = sim.readStringUntil('\n');
    Serial.println(valMess);
    if (valMess == "Ok\r") {
      Nd1 = 1;
      Nd2 = 1;
      h2s1 = 1;
      h2s2 = 1;
      Ch41 = 1;
      Ch42 = 1;
      PinM = 1;
      Pin1 = 1;
      Pin2 = 1;
      repMess("Da xac nhan");
      delay(100);
      Serial.print("Pin2: ");
      Serial.println(Pin2);
    }
  }
  Serial.print("Pin2: ");
  Serial.println(Pin2);
  delay(1000);

}

void warningCh41(float valueCh41, int valueSetCh41, String messCh41) {
  if (valueCh41 > valueSetCh41 && Ch41 == 1) {
    repMess(messCh41);
    delay(100);
    Ch41 = 0;
  }

  if (Serial.available()) {
    sim.write(Serial.read());
  }
  if (sim.available() > 0) {
    valMess = sim.readStringUntil('\n');
    Serial.println(valMess);
    if (valMess == "Ok\r") {
      Nd1 = 1;
      Nd2 = 1;
      h2s1 = 1;
      h2s2 = 1;
      Ch41 = 1;
      Ch42 = 1;
      PinM = 1;
      Pin1 = 1;
      Pin2 = 1;
      repMess("Da xac nhan");
      delay(100);
      Serial.print("Ch41: ");
      Serial.println(Ch41);
    }
  }
  Serial.print("Ch41: ");
  Serial.println(Ch41);
  delay(1000);

}

void warningCh42(float valueCh42, int valueSetCh42, String messCh42) {
  if (valueCh42 > valueSetCh42 && Ch42 == 1) {
    repMess(messCh42);
    delay(100);
    Ch42 = 0;
  }

  if (Serial.available()) {
    sim.write(Serial.read());
  }
  if (sim.available() > 0) {
    valMess = sim.readStringUntil('\n');
    Serial.println(valMess);
    if (valMess == "Ok\r") {
      Nd1 = 1;
      Nd2 = 1;
      h2s1 = 1;
      h2s2 = 1;
      Ch41 = 1;
      Ch42 = 1;
      PinM = 1;
      Pin1 = 1;
      Pin2 = 1;
      repMess("Da xac nhan");
      delay(100);
      Serial.print("Ch42: ");
      Serial.println(Ch42);
    }
  }
  Serial.print("Ch41: ");
  Serial.println(Ch41);
  delay(1000);

}

void warningh2s1(float valueh2s1, int valueSeth2s1, String messh2s1) {
  if (valueh2s1 > valueSeth2s1 && h2s1 == 1) {
    repMess(messh2s1);
    delay(100);
    h2s1 = 0;
  }

  if (Serial.available()) {
    sim.write(Serial.read());
  }
  if (sim.available() > 0) {
    valMess = sim.readStringUntil('\n');
    Serial.println(valMess);
    if (valMess == "Ok\r") {
      Nd1 = 1;
      Nd2 = 1;
      h2s1 = 1;
      h2s2 = 1;
      Ch41 = 1;
      Ch42 = 1;
      PinM = 1;
      Pin1 = 1;
      Pin2 = 1;
      repMess("Da xac nhan");
      delay(100);
      Serial.print("h2s1: ");
      Serial.println(h2s1);
    }
  }
  Serial.print("h2s1: ");
  Serial.println(h2s1);
  delay(1000);

}

void warningh2s2(float valueh2s2, int valueSeth2s2, String messh2s2) {
  if (valueh2s2 > valueSeth2s2 && h2s2 == 1) {
    repMess(messh2s2);
    delay(100);
    h2s2 = 0;
  }

  if (Serial.available()) {
    sim.write(Serial.read());
  }
  if (sim.available() > 0) {
    valMess = sim.readStringUntil('\n');
    Serial.println(valMess);
    if (valMess == "Ok\r") {
      Nd1 = 1;
      Nd2 = 1;
      h2s1 = 1;
      h2s2 = 1;
      Ch41 = 1;
      Ch42 = 1;
      PinM = 1;
      Pin1 = 1;
      Pin2 = 1;
      repMess("Da xac nhan");
      delay(100);
      Serial.print("h2s2: ");
      Serial.println(h2s2);
    }
  }
  Serial.print("h2s2: ");
  Serial.println(h2s2);
  delay(1000);

}

void repMess(String txt) {
  sim.println("AT+CMGF=1");
  delay(1000);
  sim.println("AT+CMGS=\"0345599855\"\r");
  delay(1000);
  sim.println(txt);
  delay(100);
  sim.println((char)26);
  delay(1000);
}
