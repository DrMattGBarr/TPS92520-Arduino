#include <SPI.h>
#include <TPS92520Driver.h>


constexpr TPS92520::DeviceConfig LED_DRIVER_1 = {
    1000000,    // SPI baud rate
    MSBFIRST,   // SPI bit order
    SPI_MODE0,  // SPI mode

    4,          // Chip select pin
    2,          // UDIM pin (Use -1 if UDIM is tied high in hardware. If tying all UDIMs to a common GPIO, set all instances to the same value)

    4.7f,       // CH1 sense resistor (Ohms)
    4.7f        // CH2 sense resistor (Ohms)
};

TPS92520::Driver driver1(LED_DRIVER_1);

void setup() {
    Serial.begin(115200);
    
    SPI.begin();

    driver1.configure();
    driver1.begin();

    driver1.setCurrentmA(
        TPS92520::Channel::CH1,
        15.0f
    );

    driver1.enableChannel(
        TPS92520::Channel::CH1
    );

    driver1.enableUDIM();
}

void loop() {
    delay(2000);
    Serial.print("TPS92520 Junction Temperature = "); Serial.print(driver1.readTemperature()); Serial.println(" C");
    Serial.print("V5D Voltage = "); Serial.print(driver1.readV5DVoltage()); Serial.println(" V");
    Serial.print("CH1 Input Voltage = "); Serial.print(driver1.readVIN(TPS92520::Channel::CH1)); Serial.println(" V");
    Serial.print("CH1 Output Voltage = "); Serial.print(driver1.readLEDVoltage(TPS92520::Channel::CH1)); Serial.println(" V\n");
}