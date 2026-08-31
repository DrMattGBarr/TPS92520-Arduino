#include <SPI.h>
#include <TPS92520Driver.h>


constexpr TPS92520::DeviceConfig config = {
    1000000,    // SPI baud rate
    MSBFIRST,   // SPI bit order
    SPI_MODE0,  // SPI mode

    4,          // Chip select pin
    2,          // UDIM pin

    0.1f,       // CH1 sense resistor (Ohms)
    0.1f        // CH2 sense resistor (Ohms)
};

TPS92520::Driver driver(config);


void setup() {
    SPI.begin();

    driver.configure();
    driver.begin();

    driver.setCurrentmA(
        TPS92520::Channel::CH1,
        250.0f
    );

    driver.enableChannel(
        TPS92520::Channel::CH1
    );

    driver.enableUDIM();
}


void loop() {
}