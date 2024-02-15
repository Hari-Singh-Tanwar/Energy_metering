#include <PZEM004Tv30.h>
#define PZEM_SERIAL Serial2
#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17
PZEM004Tv30 pzem(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN);

// PZEM DATA
float voltage;
float current;
float power;
float energy;
float frequency;
float pf;

void pzem_reset(){
    pzem.resetEnergy();
}

void read_pzem()
{
    if (isnan(voltage))
    {
        Serial.println("Error reading voltage");
    }
    else if (isnan(current))
    {
        Serial.println("Error reading current");
    }
    else if (isnan(power))
    {
        Serial.println("Error reading power");
    }
    else if (isnan(energy))
    {
        Serial.println("Error reading energy");
    }
    else if (isnan(frequency))
    {
        Serial.println("Error reading frequency");
    }
    else if (isnan(pf))
    {
        Serial.println("Error reading power factor");
    }
    else
    {

        // Print the values to the Serial console
        voltage = pzem.voltage(); // Records the value in the variables of voltage
        Serial.print("Voltage: ");
        Serial.print(voltage, 2);
        Serial.println("V");

        pzem.current(); // Records the value in the variables of current
        Serial.print("Current: ");
        Serial.print(current, 2);
        Serial.println("A");

        pzem.power(); // Records the value in the variables of power
        Serial.print("Power: ");
        Serial.print(power, 2);
        Serial.println("W");

        pzem.energy(); // Records the value in the variables of energy
        Serial.print("Energy: ");
        Serial.print(energy, 2);
        Serial.println("kWh");

        pzem.frequency(); // Records the value in the variables of frequency
        Serial.print("Frequency: ");
        Serial.print(frequency, 2);
        Serial.println("Hz");

        pzem.pf(); // Records the value in the variables of power factor
        Serial.print("PF: ");
        Serial.println(pf, 2);
    }
}