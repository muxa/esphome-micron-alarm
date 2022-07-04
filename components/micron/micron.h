#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"

namespace esphome
{
  namespace micron
  {

    static const uint32_t MICRON_CLOCK_TIMEOUT_MS = 50;
    static const uint32_t MICRON_MIN_US = 100;
    static const uint32_t MICRON_MAX_MS = 30;
    static const uint8_t MICRON_PACKET_LEN = 3;
    static const uint8_t MICRON_FRAME_SIZE = 24;
    static const uint8_t MICRON_BYTE_COMMAND = 0;
    static const uint8_t MICRON_BYTE_HIGH = 1;
    static const uint8_t MICRON_BYTE_LOW = 2;

    static const uint8_t MICRON_COMMAND_FRAME_SIZE = 7;

    static const uint16_t MICRON_KEYPAD_1 = 0x48;
    static const uint16_t MICRON_KEYPAD_2 = 0x28;
    static const uint16_t MICRON_KEYPAD_3 = 0x18;
    static const uint16_t MICRON_KEYPAD_4 = 0x44;
    static const uint16_t MICRON_KEYPAD_5 = 0x24;
    static const uint16_t MICRON_KEYPAD_6 = 0x14;
    static const uint16_t MICRON_KEYPAD_7 = 0x42;
    static const uint16_t MICRON_KEYPAD_8 = 0x22;
    static const uint16_t MICRON_KEYPAD_9 = 0x12;
    static const uint16_t MICRON_KEYPAD_STAR = 0x41;
    static const uint16_t MICRON_KEYPAD_0 = 0x21;
    static const uint16_t MICRON_KEYPAD_HASH = 0x11;

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

    static const uint32_t MICRON_MAX_COMMAND_DELAY_MS = 240;

    uint8_t key_to_command(const char key);
    std::vector<uint8_t> keys_to_commands(const std::string &keys);

    struct MicronPacket {
      uint8_t command;
      uint16_t status;
    };

    class MicronDataProcessor {
    public:
      void next(uint32_t ms);
      void write(ISRInternalGPIOPin *pin_data_out);
      bool decode(uint32_t ms, bool data);
      MicronPacket *packet = new MicronPacket;
      uint8_t command_out = 0;
      uint8_t command_repeat = 0;
      uint8_t remaining_command_writes = 0;

    protected:
      uint8_t buffer_[MICRON_PACKET_LEN];
      int num_bits_ = 0;
      uint32_t prev_ms_;
    };

    struct MicronStore {
    public:
      uint8_t command;
      uint16_t status;

      uint32_t interrupts = 0;
      uint32_t bits_received = 0;
      uint32_t packet_interrupts = 0;
      uint32_t packet_bits = 0;
      uint32_t packets_received = 0;
      uint32_t packets_with_interference = 0;
      uint32_t commands_sent = 0;

      uint32_t last_packet_ms;

      void setup(InternalGPIOPin *pin_clock, InternalGPIOPin *pin_data, InternalGPIOPin *pin_data_out);
      void write(uint8_t command, uint8_t repeat = 1);
      static void interrupt(MicronStore *arg);

      MicronDataProcessor processor_;
    protected:
      ISRInternalGPIOPin pin_clock_;
      ISRInternalGPIOPin pin_data_;
      ISRInternalGPIOPin pin_data_out_;

      uint32_t last_interrupt_us_;

      void set_data_(MicronPacket *packet);
    };

    class MicronComponent : public PollingComponent
    {
    public:
      void set_pin_clock(InternalGPIOPin *pin_clock) { pin_clock_ = pin_clock; }
      void set_pin_data(InternalGPIOPin *pin_data) { pin_data_ = pin_data; }
      void set_pin_data_out(InternalGPIOPin *pin_data_out) { pin_data_out_ = pin_data_out; }

      void set_connected_binary_sensor(binary_sensor::BinarySensor  *connected_binary_sensor) { connected_binary_sensor_ = connected_binary_sensor; }

      void set_m_binary_sensor(binary_sensor::BinarySensor  *m_binary_sensor) { m_binary_sensor_ = m_binary_sensor; }
      void set_s1_binary_sensor(binary_sensor::BinarySensor  *s1_binary_sensor) { s1_binary_sensor_ = s1_binary_sensor; }
      void set_s2_binary_sensor(binary_sensor::BinarySensor  *s2_binary_sensor) { s2_binary_sensor_ = s2_binary_sensor; }

      void set_beep1_binary_sensor(binary_sensor::BinarySensor  *beep1_binary_sensor) { beep1_binary_sensor_ = beep1_binary_sensor; }
      void set_beep2_binary_sensor(binary_sensor::BinarySensor  *beep2_binary_sensor) { beep2_binary_sensor_ = beep2_binary_sensor; }
      void set_beep3_binary_sensor(binary_sensor::BinarySensor  *beep3_binary_sensor) { beep3_binary_sensor_ = beep3_binary_sensor; }

      void set_zone1_binary_sensor(binary_sensor::BinarySensor  *zone1_binary_sensor) { zone1_binary_sensor_ = zone1_binary_sensor; }
      void set_zone2_binary_sensor(binary_sensor::BinarySensor  *zone2_binary_sensor) { zone2_binary_sensor_ = zone2_binary_sensor; }
      void set_zone3_binary_sensor(binary_sensor::BinarySensor  *zone3_binary_sensor) { zone3_binary_sensor_ = zone3_binary_sensor; }
      void set_zone4_binary_sensor(binary_sensor::BinarySensor  *zone4_binary_sensor) { zone4_binary_sensor_ = zone4_binary_sensor; }
      void set_zone5_binary_sensor(binary_sensor::BinarySensor  *zone5_binary_sensor) { zone5_binary_sensor_ = zone5_binary_sensor; }

      void set_keypad_text_sensor(text_sensor::TextSensor  *keypad_text_sensor) { keypad_text_sensor_ = keypad_text_sensor; }
      void set_status_text_sensor(text_sensor::TextSensor  *status_text_sensor) { status_text_sensor_ = status_text_sensor; }

      void write(uint8_t command);
      void write(std::vector<uint8_t> commands);
      void press(const std::string &keys);

      // ========== INTERNAL METHODS ==========
      // (In most use cases you won't need these)
      void setup() override;
      void dump_config() override;
      void loop() override;
      void update() override;
      float get_setup_priority() const override;
    protected:
      MicronStore store_;
      InternalGPIOPin *pin_clock_;
      InternalGPIOPin *pin_data_;
      InternalGPIOPin *pin_data_out_;

      binary_sensor::BinarySensor *connected_binary_sensor_{nullptr};

      binary_sensor::BinarySensor *m_binary_sensor_{nullptr};
      binary_sensor::BinarySensor *s1_binary_sensor_{nullptr};
      binary_sensor::BinarySensor *s2_binary_sensor_{nullptr};

      binary_sensor::BinarySensor *beep1_binary_sensor_{nullptr};
      binary_sensor::BinarySensor *beep2_binary_sensor_{nullptr};
      binary_sensor::BinarySensor *beep3_binary_sensor_{nullptr};

      binary_sensor::BinarySensor *zone1_binary_sensor_{nullptr};
      binary_sensor::BinarySensor *zone2_binary_sensor_{nullptr};
      binary_sensor::BinarySensor *zone3_binary_sensor_{nullptr};
      binary_sensor::BinarySensor *zone4_binary_sensor_{nullptr};
      binary_sensor::BinarySensor *zone5_binary_sensor_{nullptr};

      text_sensor::TextSensor *keypad_text_sensor_{nullptr};
      text_sensor::TextSensor *status_text_sensor_{nullptr};

      Deduplicator<uint8_t> command_dedupe_;
      Deduplicator<uint16_t> status_dedupe_;

      uint32_t last_command_ms_ = 0;
      std::queue<uint8_t> command_queue_;
    };

  } // namespace micron
} // namespace esphome