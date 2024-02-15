#include <WString.h>
// #include <Arduino.h>

void rfid_init();

String readRFID();

void write_rfid(uint8_t, uint8_t*);

void read_rfid(uint8_t block, uint8_t buffer[]);

bool payment(int); // checks if the user have enough money