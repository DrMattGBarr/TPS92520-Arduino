# TPS92520-Arduino
A lightweight, hardware-validated Arduino driver for the Texas Instruments TPS92520-Q1 dual synchronous buck LED driver. Provides a high-level API for current control, channel management, telemetry acquisition, fault reporting and device configuration, whilst abstracting the specifics of the TPS92520 SPI protocol and its pipelined response architecture.

## Features

- SPI communication with the TPS92520-Q1
- Automatic command-frame generation and parity handling
- Channel current programming
- Channel enable and disable control
- UDIM enable and disable control
- Device sleep and wake support
- Read LED output voltage telemetry
- Read VIN telemetry
- Read 5 V rail telemetry
- Read junction temperature telemetry
- Status and fault reporting abstraction
- Hardware validated on real TPS92520-Q1 hardware

## Supported Hardware

The library has currently been validated on:

- Texas Instruments TPS92520-Q1
- Seeed Studio XIAO ESP32-C3
- Arduino Mega 2560

The library uses the standard Arduino SPI interface and is expected to operate on other Arduino-compatible platforms supporting SPI.

## Installation

Clone or download this repository into your Arduino libraries folder:

```text
Documents/
└── Arduino/
    └── libraries/
        └── TPS92520-Arduino/
```

Alternatively, add the library as a Git submodule or PlatformIO dependency.

## Quick Start

```cpp
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
        TPS92520::TPS92520Channel::CH1,
        500.0f
    );

    driver.enableChannel(
        TPS92520::TPS92520Channel::CH1
    );

    driver.enableUDIM();
}

void loop() {
}
```
### Notes

- The SPI peripheral must be initialised before using the driver (for example via `SPI.begin()` or an equivalent SPI wrapper).
- Use `enablePin = -1` if the TPS92520 UDIM pin is permanently tied high.
- Shared-UDIM and independent-UDIM configurations are both supported.

## Example Capabilities

### Set Channel Current

```cpp
driver.setCurrentmA(
    TPS92520::TPS92520Channel::CH1,
    750.0f
);
```

### Enable a Channel

```cpp
driver.enableChannel(
    TPS92520::TPS92520Channel::CH1
);
```

### Disable a Channel

```cpp
driver.disableChannel(
    TPS92520::TPS92520Channel::CH1
);
```

### Put the Device to Sleep

```cpp
driver.sleep();
```

### Wake the Device

```cpp
driver.wakeUp();
```

### Read Temperature

```cpp
float temperatureC =
    driver.readTemperature();
```

### Read Status

```cpp
TPS92520::Status status =
    driver.readStatus();

if (status.ch1Short) {
    // Handle CH1 short-circuit condition
}
```

## Notes

### SPI Initialisation

The application is responsible for initialising the SPI peripheral before using the library:

```cpp
SPI.begin();
```

The library does not perform SPI bus initialisation automatically.

### Pipelined SPI Responses

The TPS92520-Q1 utilises a pipelined SPI architecture where the response to a command is returned during the subsequent transaction.

This behaviour is handled internally by the library and is exposed through a conventional API.

### UDIM Control

The library supports three common hardware configurations:

- UDIM permanently tied high
- One UDIM pin per TPS92520 device
- Multiple TPS92520 devices sharing a common UDIM signal

The application remains responsible for system-level coordination of shared UDIM signals.

### Library Limitations

- During initialisation the library currently disables the TPS92520 watchdog during `begin()` and does not currently utilise any watchdog functionality.
- Applications requiring watchdog supervision should implement their own strategy, or should extend the library to periodically service the watchdog.

## Current Status

This project is under active development.

Implemented and hardware-tested functionality includes:

- SPI communications
- Register access
- Current programming
- Channel control
- Voltage telemetry
- Temperature telemetry
- Sleep and wake control
- Status reporting

Future development may include:

- Additional fault reporting
- Expanded status decoding
- Additional platform validation
- Arduino Library Manager packaging

## Contributing

Contributions, bug reports, and hardware validation results are welcome.

Please open an Issue or Pull Request on GitHub.

## License

Released under the MIT License.

See `LICENSE` for details.
