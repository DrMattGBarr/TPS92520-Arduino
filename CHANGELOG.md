# Changelog

All notable changes to this project will be documented in this file.

This project follows **Semantic Versioning** (`MAJOR.MINOR.PATCH`) and uses version tags like `v0.1.0`, `v1.0.0`, etc.

---

## [0.1.1] - 2026-09-22

### Fixed
- Updated library metadata for Arduino Library Manager compatibility.
- Renamed the library to `TPS92520Driver` to match the primary header.
- Added the project URL to `library.properties`.
- Declared `TPS92520Driver.h` as the primary library include.
- Declared support for the AVR and ESP32 architectures.

---

## [0.1.0] - 2026-08-31

### Added
- TPS92520 SPI communications layer
- Register read and write helpers
- Channel current programming
- Channel enable and disable control
- UDIM control
- Sleep and wake functionality
- Voltage and temperature telemetry
- Status reporting
- Hardware validation on TPS92520-Q1 devices

### Notes
- Tested on ESP32-C3 hardware.
- Additional Arduino and ESP32 platform validation is planned.

---