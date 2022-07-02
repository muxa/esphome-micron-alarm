#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"

namespace esphome
{
  namespace micron
  {

    static const uint8_t MICRON_MAX_MS = 30;
    static const uint8_t MICRON_PACKET_LEN = 3;
    static const uint8_t MICRON_FRAME_SIZE = 24;
    static const uint8_t MICRON_BYTE_COMMAND = 0;
    static const uint8_t MICRON_BYTE_HIGH = 1;
    static const uint8_t MICRON_BYTE_LOW = 2;

    struct MicronPacket {
      uint8_t command;
      uint16_t status;
    };

    class MicronDataProcessor {
    public:
      bool decode(uint32_t ms, bool data);
      MicronPacket *packet = new MicronPacket;

    protected:
      uint8_t buffer_[MICRON_PACKET_LEN];
      int num_bits_ = 0;
      uint32_t prev_ms_;
    };

    struct MicronStore {
    public:
      uint8_t command;
      uint16_t status;
      uint32_t bits_received = 0;
      uint32_t packets_received = 0;

      void setup(InternalGPIOPin *pin_clock, InternalGPIOPin *pin_data);
      static void interrupt(MicronStore *arg);

    protected:
      ISRInternalGPIOPin pin_clock_;
      ISRInternalGPIOPin pin_data_;
      MicronDataProcessor processor_;

      void set_data_(MicronPacket *packet);
    };

    class MicronComponent : public PollingComponent
    {
    public:
      void set_pin_clock(InternalGPIOPin *pin_clock) { pin_clock_ = pin_clock; }
      void set_pin_data(InternalGPIOPin *pin_data) { pin_data_ = pin_data; }

      // ========== INTERNAL METHODS ==========
      // (In most use cases you won't need these)
      void setup() override;
      void dump_config() override;
      // void loop() override;
      void update() override;
      float get_setup_priority() const override;
    protected:
      MicronStore store_;
      InternalGPIOPin *pin_clock_;
      InternalGPIOPin *pin_data_;      
    };

  } // namespace micron
} // namespace esphome