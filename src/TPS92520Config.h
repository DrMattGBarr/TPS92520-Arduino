#pragma once
#include <Arduino.h>


/************************************************************************************************
  TPS92520Config is a header-only module that describes how the TPS92520 LED Drivers are wired 
  to the SPI bus.
************************************************************************************************/

namespace TPS92520 {

    struct DeviceConfig {
        uint32_t baud;
        uint8_t  bitOrder;
        uint8_t  mode;
        
        uint8_t  csPin;
        int8_t  enablePin;  // UDIM pin (use -1 if tied high in hardware)

        float    ch1SenseResistanceOhms;
        float    ch2SenseResistanceOhms;
    };

}
