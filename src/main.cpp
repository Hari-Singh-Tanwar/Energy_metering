#include <Arduino.h>

// ADE9153A library
#include <ADE9153A.h>
#include <ADE9153AAPI.h>

// PZEM004T
#include <metering_pzem.h>

// WIFI
#include <WiFi.h>
WiFiClient wifiClient;
const char *ssid = " ";
const char *password = " ";

// MQTT
#include <PubSubClient.h>
const char *mqtt_server = "91.121.93.94"; // test.mosquitto.org
const int mqtt_port = 1883;
// const char *mqtt_topic_relay = "esp32/relay";

// SD CARD
#include <SD.h>

// RFID
#include <RFID.h>
#include <SPI.h>
#include <MFRC522.h>
// RFID setup
const uint8_t RFID_CS = 15;
const uint8_t RFIT_RST = 0;
uint8_t trailerBlock; // tailblock to be authenticated

uint8_t nuidPICC[4] = {0, 0, 0, 0};
MFRC522::StatusCode status;
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(RFID_CS, RFIT_RST);

// LCD
#include <Wire.h> // I2C
#include <printLcd.h>
String line1 = "- - - - - - - - ";
String line2 = "- - - - - - - - ";

// RGB to change the colour of led using presets or generate coustom colour
#include "RGB.h"

//***************************************************************************//
// Pin defenations
const uint8_t relay = 27;
const uint8_t Lswitch = 26;

// ADE9152A
const uint8_t ADE_CS = 5, CF1 = 32, CF2 = 35, ADE_RST = 33, ADE_IRQ = 25;

// GSM_Rx_Tx
const uint8_t MODEM_TX = 32;
const uint8_t MODEM_RX = 35;

//****************************************************************************//
// Balance of user variables
unsigned int balance = 0;

uint8_t block1 = 58; // sector 14
uint8_t readAmount1[18];

uint8_t block2 = 57;
uint8_t readAmount2[18];

uint8_t block3 = 56;
uint8_t readAmount3[18];

// ID blocks
uint8_t IDblock1 = 62;
uint8_t readID1[18];

uint8_t IDblock2 = 61;
uint8_t readID2[18];

uint8_t IDblock3 = 60;
uint8_t readID3[18];

uint8_t userblock1 = 54;
uint8_t readName1[18];

uint8_t userblock2 = 53;
uint8_t readName2[18];

uint8_t userblock3 = 52;
uint8_t readName3[18];

uint8_t authID1[16] = {"Ghar_Lele"};
uint8_t authID2[16] = {"Pappu_Naae"};
uint8_t authID3[16] = {"Pleej"};
//***************************************************************************//
// Task handlers
TaskHandle_t connectToWiFiHandler = NULL;
TaskHandle_t connectToMQTTHandler = NULL;
TaskHandle_t readRfidCardHandler = NULL;
TaskHandle_t meateringPZEMHandler = NULL;
TaskHandle_t meateringADEHandler = NULL;

//***************************************************************************//
// Function prototyping
// Functions for tasks
void DispalyStatus(void *parameters); // For 16x2 LCD display
void connectToWifi(void *parameters); // To check and connect the wifi connectivity
void connectToMQTT(void *parameters); // To check and connect to MQTT server
void readRfidCard(void *parameters);  // To read and verify the RFID if detected
void meteringPZEM(void *parameters);  // To read the enegyzk metering by PZEM004T
void meteringADE(void *parameters);   // To read the energy metering by ADE9153A
// Function for subroutines
void relay_state(bool);
#define ON HIGH
#define OFF LOW
// rfid subroutine
void get_amount();
bool verify_ID();
void get_user();
//***************************************************************************//

void setup()
{
  Serial.begin(115200); // Begining the Srial monitor

  // Giving direction to teh i/o pins
  pinMode(relay, OUTPUT);
  pinMode(Lswitch, INPUT_PULLDOWN);
  relay_state(OFF);

  // Initialising the communication protocol
  SPI.begin();  // initialising SPI
  Wire.begin(); // Initialising I2C

  rfid_init(); // RFID Initalisation
  setup_lcd(); // LCD Initialisation

  // pzem.resetEnergy(); // Resetting PZEM
  pzem_reset();

  // Intialising SD card
  //  Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.print("Initializing SD card...");
  if (!SD.begin())
  {
    Serial.println("initialization of SD card is failed!");
    return;
  }
  Serial.println("initialization of SD card is done.");

  //***************************************************************************//
  // Creating tasks

  // Task to print on lcd
  xTaskCreatePinnedToCore(
      DispalyStatus,               // function name which you want to run
      "Display status of charger", // function description
      1000,                        // stack size
      NULL,                        // parameters of function
      1,                           // task priority
      NULL,                        // task handler
      tskNO_AFFINITY               // it will run on either 1 or 0
  );

  // Task for metering with PZEM
  xTaskCreatePinnedToCore(
      meteringPZEM,                   // function name which you want to run
      "Aquires reading of PZEM-004T", // function description
      2000,                           // stack size
      NULL,                           // parameters of function
      2,                              // task priority
      &meateringPZEMHandler,          // task handler
      1                               // it will run in core 0
  );

  // Task for metering with ADE
  xTaskCreatePinnedToCore(
      meteringADE,                   // function name which you want to run
      "Aquires reading of ADE9153A", // function description
      2000,                          // stack size
      NULL,                          // parameters of function
      2,                             // task priority
      &meateringADEHandler,          // task handler
      1                              // it will run in core 0
  );

  // Task for reading RFID card
  xTaskCreatePinnedToCore(
      readRfidCard,                      // function name which you want to rum
      "Reads the RFID card if avalable", // function description
      1000,                              // stack size
      NULL,                              // parameters of the functions
      1,                                 // task priority
      &readRfidCardHandler,              // task handler
      tskNO_AFFINITY                     // this task will run in either core 1 or 0
  );
}

void loop()
{
}

//***************************************************************************//
//****************************TASK******************************************//
void DisplayStatus(void *parameters)
{
  UBaseType_t uxHighWaterMark;

  printLcd(line1, line2);

  uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
  Serial.print("maxdata used by lcd printing is : ");
  Serial.println(uxHighWaterMark);
}

void readRfidCard(void *parameters)
{
  UBaseType_t uxHighWaterMark;

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if (!rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K)
  {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  if (rfid.uid.uidByte[0] != nuidPICC[0] ||
      rfid.uid.uidByte[1] != nuidPICC[1] ||
      rfid.uid.uidByte[2] != nuidPICC[2] ||
      rfid.uid.uidByte[3] != nuidPICC[3])
  {
    Serial.println(F("A new card has been detected."));

    // Store NUID into nuidPICC array
    for (uint8_t i = 0; i < 4; i++)
    {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }

    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    for (uint8_t i = 0; i < rfid.uid.size; i++)
    {
      Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();
    Serial.print(F("In dec: "));
    // printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
  }
  else
    Serial.println(F("Card read previously."));

  get_amount();
  // get_ID();
  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();

  uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
  Serial.print("maxdata used by lcd printing is : ");
  Serial.println(uxHighWaterMark);
}

//***************************************************************************//
// *****************************SUBROUTINES**********************************//
void relay_state(bool state)
{
  digitalWrite(relay, state);
}

void get_amount() // Assuming the cost of 1 unit is 6.29rs
{
  read_rfid(block1, readAmount1); // reading the amount
  read_rfid(block2, readAmount2);
  read_rfid(block3, readAmount3);
  for (int i = 0; i < 16; ++i)
  {
    balance += readAmount1[i];
  }
  for (int i = 0; i < 16; ++i)
  {
    balance += readAmount2[i];
  }
  for (int i = 0; i < 16; ++i)
  {
    balance += readAmount3[i];
  }
  Serial.println("The balanc is -> " + (String)balance);
}

bool verify_ID() // ID at sector 15
{
  read_rfid(IDblock1, readID1); // reading the amount
  read_rfid(IDblock2, readID2);
  read_rfid(IDblock3, readID3);
}

void get_user()
{
  read_rfid(userblock1, readName1);
  read_rfid(userblock2, readName2);
  read_rfid(userblock3, readName3);
}
//***************************************************************************//