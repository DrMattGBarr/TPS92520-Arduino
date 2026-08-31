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

constexpr TPS92520::DeviceConfig LED_DRIVER_2 = {
    1000000,    // SPI baud rate
    MSBFIRST,   // SPI bit order
    SPI_MODE0,  // SPI mode

    5,          // Chip select pin
    3,          // UDIM pin 

    4.7f,       // CH1 sense resistor (Ohms)
    4.7f        // CH2 sense resistor (Ohms)
};

TPS92520::Driver driver1(LED_DRIVER_1);
TPS92520::Driver driver2(LED_DRIVER_2);


void setup() {
    SPI.begin();

    driver1.configure();
    driver2.configure();

    driver1.begin();
    driver2.begin();

    driver1.setCurrentmA(
        TPS92520::Channel::CH1,
        15.0f
    );

    driver1.enableChannel(
        TPS92520::Channel::CH1
    );

    driver2.setCurrentmA(
        TPS92520::Channel::CH1,
        16.0f
    );

    
    driver2.enableChannel(
        TPS92520::Channel::CH1
    );

    driver1.enableUDIM();
    driver2.enableUDIM();
}


void loop() {
}