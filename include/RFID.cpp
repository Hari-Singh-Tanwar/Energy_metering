#include <MFRC522.h>

// RFID setup
const uint8_t RFID_CS = 15;
const uint8_t RFIT_RST = 0;
uint8_t trailerBlock ; // tailblock to be authenticated

uint8_t nuidPICC[4] = {0, 0, 0, 0};

uint8_t block1 = 58; // sector 14
uint8_t block2 = 57;
uint8_t block3 = 56;

MFRC522::StatusCode status;
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(RFID_CS, RFIT_RST);

void rfid_init()
{
  // RFID Initalisation
  rfid.PCD_Init(); // Init MFRC522 card (in case you wonder what PCD means: proximity coupling device)
}

String readRFID()
{ /* function readRFID */
  ////Read RFID card
  static bool flag = 0;
  if (flag == 0)
  {
    // SPI.begin();
    // rfid.PCD_Init();
    rfid.PCD_DumpVersionToSerial();
    flag = 1; // https://circuits4you.com/wp-content/uploads/2018/12/ESP32-Pinout.jpg
  }

  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }

  // Look for new 1 cards
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if (!rfid.PICC_ReadCardSerial())
    return;

  // Store NUID into nuidPICC array
  for (byte i = 0; i < 4; i++)
  {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }

  String id = "";
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    id += String(rfid.uid.uidByte[i], DEC);
  }

  Serial.println("RFID : ");
  Serial.println(id);
  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  return id;
}

void auth_keyA(){
      // Authenticate using key A
    Serial.println(F("Authenticating using key A..."));
    status = (MFRC522::StatusCode) rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(rfid.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(rfid.GetStatusCodeName(status));
        return;
    }
}

void write_rfid(uint8_t blockAddr, uint8_t buffer[])
{
   auth_keyA();
  // Write data to the block
  Serial.print(F("Writing data into block "));
  Serial.print(blockAddr);
  Serial.println(F(" ..."));
  dump_byte_array(buffer, 16);
  Serial.println();
  status = (MFRC522::StatusCode)rfid.MIFARE_Write(blockAddr, buffer, 16);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
  }
  Serial.println();
}

void read_rfid(uint8_t blockAddr, uint8_t buffer[]){
   auth_keyA();
  // Read data from the block (again, should now be what we have written)
    Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
     uint8_t size = 18;
    status = (MFRC522::StatusCode) rfid.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(rfid.GetStatusCodeName(status));
    }
    Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
    dump_byte_array(buffer, 16); Serial.println();

    // Check that data in block is what we have written
    // by counting the number of bytes that are equal
    Serial.println(F("Checking result..."));
    Serial.println();
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize)
{
  for (byte i = 0; i < bufferSize; i++)
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}



unsigned int balance = 0;

uint8_t readAmount1[18];
uint8_t readAmount2[18];
uint8_t readAmount3[18];

bool payment(int payment) // check if user have enough money
{
  read_rfid(block1, readAmount1);
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

  if (payment > balance){
    return false;
  }
  else if (payment > balance){
    return true;
  }
}


