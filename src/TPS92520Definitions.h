#pragma once
#include <Arduino.h>


namespace TPS92520 {

    struct RegisterReadResult {
        uint8_t data;
        bool spiError = false;
        bool thermalWarning = false;
        bool powerCycled = false;
    };
    

    struct Status {
        bool spiError;
        bool thermalWarning;
        bool powerCycled;
        bool ch1Short;
        bool ch2Short;
    };


    namespace Constants {
        constexpr float VDAC_FULL_SCALE = 2.45f;
        constexpr float CURRENT_GAIN    = 14.0f;
        constexpr uint16_t CHxIADJ_MAX  = 1023;
        constexpr float CHxIADJ_STEPS   = 1024.0f;
        constexpr float CSNx_GAIN       = 65.0f / 255.0f;
        constexpr float V5D_GAIN        = 5.33f / 255.0f;

        constexpr float JUNCTION_TEMP_SCALE_C_PER_LSB = 0.7168f;
        constexpr float JUNCTION_TEMP_OFFSET_C        = 271.51f;

        constexpr uint16_t PARITY_BIT_MASK      = 0x0100;
        constexpr uint16_t WRITE_BIT_MASK       = 0x8000;
        constexpr uint8_t CH1_ENABLE_MASK       = 0x01;
        constexpr uint8_t CH2_ENABLE_MASK       = 0x04;
        constexpr uint8_t LOW_TWO_BITS_MASK     = 0x03;
        constexpr uint8_t SLEEP_ENABLE_MASK     = 0x01;
        constexpr uint8_t SLEEP_DISABLE_MASK    = 0x00;
        constexpr uint8_t WATCHDOG_DISABLE_MASK = 0x00;
        constexpr uint8_t CH1SHORT_BIT_MASK     = 0x02;
        constexpr uint8_t CH2SHORT_BIT_MASK     = 0x20;
    }


    enum class Channel : uint8_t {
        CH1,
        CH2
    };


    enum class RegisterMap : uint8_t {
        SYSCFG1    = 0x00,
        SYSCFG2    = 0x01,
        CMWTAP     = 0x02,
        STATUS1    = 0x03,
        STATUS2    = 0x04,
        STATUS3    = 0x05,
        TWLMT      = 0x06,
        SLEEP      = 0x07,
        CH1IADJL   = 0x08,
        CH1IADJH   = 0x09,
        CH2IADJL   = 0x0A,
        CH2IADJH   = 0x0B,
        PWMDIV     = 0x0C,
        CH1PWML    = 0x0D,
        CH1PWMH    = 0x0E,
        CH2PWML    = 0x0F,
        CH2PWMH    = 0x10,
        CH1TON     = 0x11,
        CH2TON     = 0x12,
        CH1VIN     = 0x13,
        CH1VLED    = 0x14,
        CH1VLEDON  = 0x15,
        CH1VLEDOFF = 0x16,
        CH2VIN     = 0x17,
        CH2VLED    = 0x18,
        CH2VLEDON  = 0x19,
        CH2VLEDOFF = 0x1A,
        TEMPL      = 0x1B,
        TEMPH      = 0x1C,
        V5D        = 0x1D,
        LHCFG1     = 0x1E,
        LHCFG2     = 0x1F,
        LHIL       = 0x20,
        LHIH       = 0x21,
        LHIFILTL   = 0x22,
        LHIFILTH   = 0x23,
        LH1IADJL   = 0x24,
        LH1IADJH   = 0x25,
        LH2IADJL   = 0x26,
        LH2IADJH   = 0x27,
        LHCH1PWML  = 0x28,
        LHCH1PWMH  = 0x29,
        LHCH2PWML  = 0x2A,
        LHCH2PWMH  = 0x2B,
        LH1TON     = 0x2C,
        LH2TON     = 0x2D,
        RESET      = 0x2E
    };

}