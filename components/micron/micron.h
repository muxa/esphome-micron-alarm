#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
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

    static const uint16_t MICRON_ZONE_1_MASK = 0x01;
    static const uint16_t MICRON_ZONE_2_MASK = 0x02;
    static const uint16_t MICRON_ZONE_3_MASK = 0x04;
    static const uint16_t MICRON_ZONE_4_MASK = 0x08;
    static const uint16_t MICRON_ZONE_5_MASK = 0x10;
    static const uint16_t MICRON_S1_MASK = 0x40;    
    static const uint16_t MICRON_KEY_BEEP_3_MASK = 0x80;
    static const uint16_t MICRON_KEY_BEEP_2_MASK = 0x0100;
    static const uint16_t MICRON_KEY_BEEP_1_MASK = 0x8000;
    static const uint16_t MICRON_S2_MASK = 0x4000;
    static const uint16_t MICRON_M_MASK = 0x2000;

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

      void set_zone1_binary_sensor(binary_sensor::BinarySensor  *zone1_binary_sensor) { zone1_binary_sensor_ = zone1_binary_sensor; }
      void set_zone2_binary_sensor(binary_sensor::BinarySensor  *zone2_binary_sensor) { zone2_binary_sensor_ = zone2_binary_sensor; }
      void set_zone3_binary_sensor(binary_sensor::BinarySensor  *zone3_binary_sensor) { zone3_binary_sensor_ = zone3_binary_sensor; }
      void set_zone4_binary_sensor(binary_sensor::BinarySensor  *zone4_binary_sensor) { zone4_binary_sensor_ = zone4_binary_sensor; }
      void set_zone5_binary_sensor(binary_sensor::BinarySensor  *zone5_binary_sensor) { zone5_binary_sensor_ = zone5_binary_sensor; }

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

      binary_sensor::BinarySensor *zone1_binary_sensor_{nullptr};
      binary_sensor::BinarySensor *zone2_binary_sensor_{nullptr};
      binary_sensor::BinarySensor *zone3_binary_sensor_{nullptr};
      binary_sensor::BinarySensor *zone4_binary_sensor_{nullptr};
      binary_sensor::BinarySensor *zone5_binary_sensor_{nullptr};
    };

  } // namespace micron
} // namespace esphome