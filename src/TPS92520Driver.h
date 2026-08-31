#pragma once
#include <Arduino.h>
#include <SPI.h>
#include "TPS92520Config.h"
#include "TPS92520Definitions.h"

/************************************************************************************************
  Hardware: https://www.ti.com/product/TPS92520-Q1
  
  This library is designed to simplify SPI communications with the Texas Instruments TPS92520-Q1 
  Dual 1.6-A Synchronous Buck LED Driver.
  
  Author: Matt G. Barr
  Repository: https://github.com/DrMattGBarr/TPS92520-Arduino

  The TPS92520's SPI interface utilises a pipelined workflow, so Response Frames for a given
  Command Frame appear as the response to the subsequent command. Hence, atomic commands such as
  clearStatusFlags() implement two SPI transfers to ensure the required response data is obtained.

  The Command Frames are the only defined frame format that are sent from master to slave on MOSI. 
  A Command Frame can be either a Read Command or a Write Command. A Command Frame consists of:
   - a CMD bit [bit 0]
   - six bits of Register ADDRESS [bits 1:6]
   - a PARITY bit (odd parity is enforced) [bit 7]
   - eight bits of DATA [bits 8:15]

   There are three possible Response Frame formats: Read Response, Write Response, 
   and Write Error/POR (Power On Reset, which is always the first response after a reset).

************************************************************************************************/


namespace TPS92520 {

    class Driver {
      
        public:
            Driver(const DeviceConfig& config);

            void configure();
            void begin();

            bool enableUDIM();
            bool disableUDIM();

            bool setCurrentmA(TPS92520::Channel channel, float currentmA);
            bool enableChannel(TPS92520::Channel channel);
            bool disableChannel(TPS92520::Channel channel);

            float readLEDVoltage(TPS92520::Channel channel);
            float readV5DVoltage();
            float readVIN(TPS92520::Channel channel);
            float readTemperature();
            Status readStatus();

            bool sleep();
            bool wakeUp();


        private:
            // === SPI Primitives ===
            void beginTransaction();
            void endTransaction();
            void assertCS();
            void deassertCS();

            // === SPI Data Link Layer ===
            uint16_t transferFrame(uint16_t frame);

            // === TPS92520 Protocol Layer ===
            uint16_t buildWriteCommandFrame(TPS92520::RegisterMap registerAddr, uint8_t data);
            uint16_t buildReadCommandFrame(TPS92520::RegisterMap registerAddr);
            void writeToRegister(TPS92520::RegisterMap registerAddr, uint8_t data);
            RegisterReadResult  readFromRegister(TPS92520::RegisterMap registerAddr);

            // === TPS92520 Device Management Layer ===
            void clearStatusFlags();
            bool disableWatchdog();

            // === Static Helpers ===
            static uint16_t appendParityBit(uint16_t dataFrame);
            static uint16_t currentmAToCHxIADJ(float currentSenseResistance, float setCurrentmA);
            static float chxIADJToCurrentmA(float currentSenseResistance, uint16_t chxIADJ);
            

            DeviceConfig config;
    };

}