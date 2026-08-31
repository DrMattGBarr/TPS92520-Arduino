#include "TPS92520Driver.h"


namespace TPS92520 {

    Driver::Driver(const DeviceConfig& config_)
        : config(config_) {
    }


    void Driver::configure() {
        pinMode(config.csPin, OUTPUT);
        digitalWrite(config.csPin, HIGH);

        if (config.enablePin >= 0) {
            pinMode(config.enablePin, OUTPUT);
            digitalWrite(config.enablePin, LOW);
        }
    }


    bool Driver::enableUDIM() {
        if (config.enablePin < 0) {
            return false;
        }
        digitalWrite(config.enablePin, HIGH);
        return true;      
    }


    bool Driver::disableUDIM() {
        if (config.enablePin < 0) {
            return false;
        }
        digitalWrite(config.enablePin, LOW);
        return true;
    }


    void Driver::assertCS() {
        digitalWrite(config.csPin, LOW);
    }


    void Driver::deassertCS() {
        digitalWrite(config.csPin, HIGH);
    }


    void Driver::begin() {
        clearStatusFlags();
        disableWatchdog();

    }


    void Driver::beginTransaction() {
        SPI.beginTransaction(
            SPISettings(
                config.baud,
                config.bitOrder,
                config.mode
            )
        );
    }


    void Driver::endTransaction() {
        SPI.endTransaction();
    }


    uint16_t Driver::transferFrame(uint16_t frame) {
        beginTransaction();
        assertCS();

        uint8_t txHi = frame >> 8;
        uint8_t txLo = frame & 0xFF;

        uint8_t rxHi = SPI.transfer(txHi);
        uint8_t rxLo = SPI.transfer(txLo);

        deassertCS();
        endTransaction();

        return (static_cast<uint16_t>(rxHi) << 8) | rxLo;
    }


    uint16_t Driver::buildWriteCommandFrame(TPS92520::RegisterMap registerAddr, uint8_t data) {
        uint16_t outputRegister = (0x8000 |
                                  (static_cast<uint8_t>(registerAddr) << 9) |
                                  data
                                  );
        outputRegister = appendParityBit(outputRegister);                          
        return outputRegister;
    }


    uint16_t Driver::buildReadCommandFrame(TPS92520::RegisterMap registerAddr) {
        uint16_t outputRegister = ((static_cast<uint8_t>(registerAddr) << 9));
        outputRegister = appendParityBit(outputRegister);                          
        return outputRegister;        
    }


    void Driver::writeToRegister(TPS92520::RegisterMap registerAddr, uint8_t data) {
        transferFrame(buildWriteCommandFrame(registerAddr, data));
    }


    RegisterReadResult Driver::readFromRegister(TPS92520::RegisterMap registerAddr) {
        RegisterReadResult response;

        transferFrame(buildReadCommandFrame(registerAddr));
        uint16_t rawData = transferFrame(buildReadCommandFrame(registerAddr));
        response.data           = rawData & 0x00FF;                // Bits 0:7
        response.thermalWarning = ((rawData & 0x0100) == 0x0100);  // Bit 8 (TW)
        response.powerCycled    = ((rawData & 0x0200) == 0x0200);  // Bit 9 (PC)
        response.spiError       = ((rawData & 0x8000) == 0x8000);  // Bit 15 (SPE)

        return response;
    }


    // === TPS92520 Protocol Layer ===

    void Driver::clearStatusFlags() {
        readFromRegister(TPS92520::RegisterMap::STATUS3);
    }


    bool Driver::disableWatchdog() {
        writeToRegister(TPS92520::RegisterMap::SYSCFG1, TPS92520::Constants::WATCHDOG_DISABLE_MASK);
        RegisterReadResult response = readFromRegister(TPS92520::RegisterMap::SYSCFG1);
        return (response.data == TPS92520::Constants::WATCHDOG_DISABLE_MASK);
    }


    bool Driver::setCurrentmA(TPS92520::Channel channel, float currentmA) {
        float csResistance;
        switch (channel) {
            case TPS92520::Channel::CH1 : {
                csResistance = config.ch1SenseResistanceOhms;
                break;
            }
            case TPS92520::Channel::CH2 : {
                csResistance = config.ch2SenseResistanceOhms;
                break;
            }
            default:
                return false;
        }

        uint16_t chxIADJ = currentmAToCHxIADJ(csResistance, currentmA);
        uint8_t loData = chxIADJ & TPS92520::Constants::LOW_TWO_BITS_MASK;
        uint8_t hiData = chxIADJ >> 2;

        uint8_t readLoData = 0x00;
        uint8_t readHiData = 0x00;

        switch (channel) {
            case TPS92520::Channel::CH1 : {
                writeToRegister(TPS92520::RegisterMap::CH1IADJL, loData);
                RegisterReadResult responseLo = readFromRegister(TPS92520::RegisterMap::CH1IADJL);
                readLoData = responseLo.data;

                writeToRegister(TPS92520::RegisterMap::CH1IADJH, hiData);
                RegisterReadResult responseHi = readFromRegister(TPS92520::RegisterMap::CH1IADJH);
                readHiData = responseHi.data;
                break;
            }
            case TPS92520::Channel::CH2 : {
                writeToRegister(TPS92520::RegisterMap::CH2IADJL, loData);
                RegisterReadResult responseLo = readFromRegister(TPS92520::RegisterMap::CH2IADJL);
                readLoData = responseLo.data;

                writeToRegister(TPS92520::RegisterMap::CH2IADJH, hiData);
                RegisterReadResult responseHi = readFromRegister(TPS92520::RegisterMap::CH2IADJH);
                readHiData = responseHi.data;
                break;
            }     
        }

        return ((readLoData == loData) && (readHiData == hiData));
    }


    bool Driver::enableChannel(TPS92520::Channel channel) {
        RegisterReadResult response = readFromRegister(TPS92520::RegisterMap::SYSCFG1);
        uint8_t data = response.data;
        
        switch (channel) {
            case TPS92520::Channel::CH1 : {
                writeToRegister(TPS92520::RegisterMap::SYSCFG1, (data | TPS92520::Constants::CH1_ENABLE_MASK));
                response = readFromRegister(TPS92520::RegisterMap::SYSCFG1);
                if (response.data != (data | TPS92520::Constants::CH1_ENABLE_MASK)) {
                    return false;
                }
                break;
            }
            case TPS92520::Channel::CH2 : {
                writeToRegister(TPS92520::RegisterMap::SYSCFG1, (data | TPS92520::Constants::CH2_ENABLE_MASK));
                response = readFromRegister(TPS92520::RegisterMap::SYSCFG1);
                if (response.data != (data | TPS92520::Constants::CH2_ENABLE_MASK)) {
                    return false;
                }
                break;
            }
            default:
                return false;
        }

        return true;
    }


    bool Driver::disableChannel(TPS92520::Channel channel) {
        RegisterReadResult response = readFromRegister(TPS92520::RegisterMap::SYSCFG1);
        uint8_t data = response.data;
        
        switch (channel) {
            case TPS92520::Channel::CH1 : {
                writeToRegister(TPS92520::RegisterMap::SYSCFG1, (data & static_cast<uint8_t>(~TPS92520::Constants::CH1_ENABLE_MASK)));
                response = readFromRegister(TPS92520::RegisterMap::SYSCFG1);
                if (response.data != (data & static_cast<uint8_t>(~TPS92520::Constants::CH1_ENABLE_MASK))) {
                    return false;
                }
                break;
            }
            case TPS92520::Channel::CH2 : {
                writeToRegister(TPS92520::RegisterMap::SYSCFG1, (data & static_cast<uint8_t>(~TPS92520::Constants::CH2_ENABLE_MASK)));
                response = readFromRegister(TPS92520::RegisterMap::SYSCFG1);
                if (response.data != (data & static_cast<uint8_t>(~TPS92520::Constants::CH2_ENABLE_MASK))) {
                    return false;
                }
                break;
            }
            default:
                return false;
        }

        return true;
    }


    float Driver::readLEDVoltage(TPS92520::Channel channel) {
        RegisterReadResult response;

        switch (channel) {
            case TPS92520::Channel::CH1 : {
                response = readFromRegister(TPS92520::RegisterMap::CH1VLED);
                break;
            }
            case TPS92520::Channel::CH2 : {
                response = readFromRegister(TPS92520::RegisterMap::CH2VLED);
                break;
            }
            default:
                return -1;
        }

        return TPS92520::Constants::CSNx_GAIN * response.data;
    }


    float Driver::readV5DVoltage() {
        RegisterReadResult response = readFromRegister(TPS92520::RegisterMap::V5D);
        return TPS92520::Constants::V5D_GAIN * response.data;
    }


    float Driver::readVIN(TPS92520::Channel channel) {
        RegisterReadResult response;

        switch (channel) {
            case TPS92520::Channel::CH1 : {
                response = readFromRegister(TPS92520::RegisterMap::CH1VIN);
                break;
            }
            case TPS92520::Channel::CH2 : {
                response = readFromRegister(TPS92520::RegisterMap::CH2VIN);
                break;
            }
            default:
                return -1;
        }

        return TPS92520::Constants::CSNx_GAIN * response.data;
    }


    float Driver::readTemperature() {
        RegisterReadResult responseLSB = readFromRegister(TPS92520::RegisterMap::TEMPL);
        uint8_t tempLSB = (responseLSB.data & TPS92520::Constants::LOW_TWO_BITS_MASK);
        RegisterReadResult responseMSB = readFromRegister(TPS92520::RegisterMap::TEMPH);
        uint8_t tempMSB = responseMSB.data;
        uint16_t temp10Bit = (tempMSB << 2) | tempLSB;
        
        return ((TPS92520::Constants::JUNCTION_TEMP_SCALE_C_PER_LSB * temp10Bit) - TPS92520::Constants::JUNCTION_TEMP_OFFSET_C);
    }


    bool Driver::sleep() {
        writeToRegister(TPS92520::RegisterMap::SLEEP, TPS92520::Constants::SLEEP_ENABLE_MASK);
        return (readFromRegister(TPS92520::RegisterMap::SLEEP).data == TPS92520::Constants::SLEEP_ENABLE_MASK);
    }


    bool Driver::wakeUp() {
        writeToRegister(TPS92520::RegisterMap::SLEEP, TPS92520::Constants::SLEEP_DISABLE_MASK);
        return (readFromRegister(TPS92520::RegisterMap::SLEEP).data == TPS92520::Constants::SLEEP_DISABLE_MASK);
    }


    Status Driver::readStatus() {
        RegisterReadResult responseStatus1 = readFromRegister(TPS92520::RegisterMap::STATUS1);
        uint8_t status1 = responseStatus1.data;
        
        Status status {};
        status.spiError = responseStatus1.spiError;
        status.powerCycled = responseStatus1.powerCycled;
        status.thermalWarning = responseStatus1.thermalWarning;
        status.ch1Short = ((status1 & TPS92520::Constants::CH1SHORT_BIT_MASK) == TPS92520::Constants::CH1SHORT_BIT_MASK);
        status.ch2Short = ((status1 & TPS92520::Constants::CH2SHORT_BIT_MASK) == TPS92520::Constants::CH2SHORT_BIT_MASK);

        return status;
    }


    // === Static Helpers ===

    uint16_t Driver::appendParityBit(uint16_t dataFrame) {
        uint8_t count = 0;
        uint16_t tempData = dataFrame;
        while (tempData) {
            count += tempData & 0x01;
            tempData >>= 1;
        }

        if (count % 2 == 0) {
            dataFrame |= TPS92520::Constants::PARITY_BIT_MASK;
        }

        return dataFrame;
    }


    uint16_t Driver::currentmAToCHxIADJ(float currentSenseResistance, float setCurrentmA) {
        float chxIADJ = (setCurrentmA / 1000.0f) * TPS92520::Constants::CHxIADJ_STEPS * 
                        TPS92520::Constants::CURRENT_GAIN * currentSenseResistance / TPS92520::Constants::VDAC_FULL_SCALE;

        if (chxIADJ < 0.0f) {
            chxIADJ = 0.0f;
        }

        if (chxIADJ > TPS92520::Constants::CHxIADJ_MAX) {
            chxIADJ = TPS92520::Constants::CHxIADJ_MAX;
        }

        return static_cast<uint16_t>(chxIADJ + 0.5f);
    }


    float Driver::chxIADJToCurrentmA(float currentSenseResistance, uint16_t chxIADJ) {
        if (chxIADJ > TPS92520::Constants::CHxIADJ_MAX) {
            chxIADJ = TPS92520::Constants::CHxIADJ_MAX;
        }

        return (1000.0f * TPS92520::Constants::VDAC_FULL_SCALE / TPS92520::Constants::CHxIADJ_STEPS) * static_cast<float>(chxIADJ) / 
                (TPS92520::Constants::CURRENT_GAIN * currentSenseResistance
                );
    }

} // namespace TPS92520
