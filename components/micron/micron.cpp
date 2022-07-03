#include "micron.h"
#include "esphome/core/log.h"

namespace esphome
{
  namespace micron
  {

    static const char *const TAG = "micron";

    uint8_t key_to_command(const char key) {
      // ESP_LOGD("key_to_command", "Key: %x", key);
      switch (key) {
        case '1': return MICRON_KEYPAD_1;
        case '2': return MICRON_KEYPAD_2;
        case '3': return MICRON_KEYPAD_3;
        case '4': return MICRON_KEYPAD_4;
        case '5': return MICRON_KEYPAD_5;
        case '6': return MICRON_KEYPAD_6;
        case '7': return MICRON_KEYPAD_7;
        case '8': return MICRON_KEYPAD_8;
        case '9': return MICRON_KEYPAD_9;
        case '0': return MICRON_KEYPAD_0;
        case '*': return MICRON_KEYPAD_STAR;
        case '#': return MICRON_KEYPAD_HASH;
      }
      return 0;
    }

    std::vector<uint8_t> keys_to_commands(const std::string &keys) {
      //ESP_LOGD("keys_to_commands", "Keys: %s", keys.c_str());

      std::vector<uint8_t> commands;
      for(char c : keys) {
        //ESP_LOGD("keys_to_command", "Key: %c", c);
        commands.push_back(key_to_command(c));
      }
      return commands;
    }

    bool IRAM_ATTR MicronDataProcessor::decode(uint32_t ms, bool data, ISRInternalGPIOPin *pin_data_out) {
      // check if a new message has started, based on time since previous bit
      if ((ms - this->prev_ms_) > MICRON_MAX_MS) {
        this->num_bits_ = 0;

        if (this->command_repeat > 0) {
          // more repeats. restart write bits
          this->remaining_command_writes = MICRON_COMMAND_FRAME_SIZE + 1;
        }
      }

      if ((ms - this->prev_ms_) < MICRON_MIN_MS) {
        this->prev_ms_ = ms;
        // too short
        return false;
      }
      this->prev_ms_ = ms;

      if (this->remaining_command_writes > 0) {
        // some bits to write remaining

        if (this->num_bits_ < MICRON_COMMAND_FRAME_SIZE) {
          uint8_t bit_mask = 1 << (MICRON_COMMAND_FRAME_SIZE - this->num_bits_ - 1);
          bool out_bit = (this->command_out & bit_mask) == bit_mask;

          pin_data_out->digital_write(out_bit);
        } else {
          // turn back data to 0
          pin_data_out->digital_write(false);

          if (this->command_repeat > 0)
            this->command_repeat--;
        }

        this->remaining_command_writes--;
      } else {
        pin_data_out->digital_write(false);
      }

      //pin_data_out->digital_write(this->num_bits_ % 2 == 0);

      // number of bits received is basically the "state"
      if (this->num_bits_ < MICRON_FRAME_SIZE) {
        // store it while it fits
        int idx = this->num_bits_ / 8;
        this->buffer_[idx] = (this->buffer_[idx] << 1) | (data ? 1 : 0);
        this->num_bits_++;

        // are we done yet?
        if (this->num_bits_ == MICRON_FRAME_SIZE) {

          this->packet->command = this->buffer_[MICRON_BYTE_COMMAND] >> 1;
          this->packet->status = this->buffer_[MICRON_BYTE_HIGH] << 8 | this->buffer_[MICRON_BYTE_LOW];

          return true;
        }
      }

      return false;
    }

    void MicronStore::setup(InternalGPIOPin *pin_clock, InternalGPIOPin *pin_data, InternalGPIOPin *pin_data_out) {
      pin_clock->setup();
      pin_data->setup();
      pin_data_out->setup();
      this->pin_clock_ = pin_clock->to_isr();
      this->pin_data_ = pin_data->to_isr();
      this->pin_data_out_ = pin_data_out->to_isr();
      pin_clock->attach_interrupt(MicronStore::interrupt, this, gpio::INTERRUPT_FALLING_EDGE);
      // pin_clock->attach_interrupt(MicronStore::interrupt, this, gpio::INTERRUPT_RISING_EDGE);
    }

    void MicronStore::write(uint8_t command, uint8_t repeat) {
      this->processor_.command_out = command;
      this->processor_.command_repeat = repeat;
      //this->processor_.remaining_command_writes = MICRON_COMMAND_FRAME_SIZE + 1;
    }

    void IRAM_ATTR MicronStore::interrupt(MicronStore *arg) {
      uint32_t now = micros();
      bool data_bit = arg->pin_data_.digital_read();

      arg->bits_received++;      

      if (arg->processor_.decode(now, data_bit, &arg->pin_data_out_)) {
        arg->packets_received++;
        arg->set_data_(arg->processor_.packet);
      }
    }

    void IRAM_ATTR MicronStore::set_data_(MicronPacket *packet) {
      this->command = packet->command;
      this->status = packet->status;
    }

    void MicronComponent::write(uint8_t command) {
      // ESP_LOGD(TAG, "Enqueue command: 0x%02x", command);
      this->command_queue_.push(command);
    }

    void MicronComponent::write(std::vector<uint8_t> commands) {
      for (const auto command : commands)
      {
          this->write(command);
      }      
    }

    void MicronComponent::press(const std::string &keys) {
      ESP_LOGD(TAG, "Press keys: %s", keys.c_str());
      this->write(keys_to_commands(keys));
    }

    void MicronComponent::setup()
    {
      ESP_LOGCONFIG(TAG, "Setting up Micron...");

      this->store_.setup(this->pin_clock_, this->pin_data_, this->pin_data_out_);
      
      // this->pin_clock_->attach_interrupt(MicronStore::gpio_intr, &this->store_, gpio::INTERRUPT_ANY_EDGE);
    }

    void MicronComponent::dump_config() {
      //LOG_SENSOR("", "Micron", this);
      ESP_LOGCONFIG(TAG, "Micron:");
      LOG_PIN("  Pin Clock: ", this->pin_clock_);
      LOG_PIN("  Pin Data: ", this->pin_data_);
      LOG_PIN("  Pin Data Out: ", this->pin_data_out_);
      LOG_BINARY_SENSOR("  ", "Zone 1", this->zone1_binary_sensor_);
      LOG_BINARY_SENSOR("  ", "Zone 2", this->zone2_binary_sensor_);
      LOG_BINARY_SENSOR("  ", "Zone 3", this->zone3_binary_sensor_);
      LOG_BINARY_SENSOR("  ", "Zone 4", this->zone4_binary_sensor_);
      LOG_BINARY_SENSOR("  ", "Zone 5", this->zone5_binary_sensor_);
    }

    void MicronComponent::loop() {

      if (this->m_binary_sensor_) {
        this->m_binary_sensor_->publish_state((this->store_.status & MICRON_M_MASK) == MICRON_M_MASK);
      }
      if (this->s1_binary_sensor_) {
        this->s1_binary_sensor_->publish_state((this->store_.status & MICRON_S1_MASK) == MICRON_S1_MASK);
      }
      if (this->s2_binary_sensor_) {
        this->s2_binary_sensor_->publish_state((this->store_.status & MICRON_S2_MASK) == MICRON_S2_MASK);
      }

      if (this->beep1_binary_sensor_) {
        this->beep1_binary_sensor_->publish_state((this->store_.status & MICRON_KEY_BEEP_1_MASK) == MICRON_KEY_BEEP_1_MASK);
      }
      if (this->beep2_binary_sensor_) {
        this->beep2_binary_sensor_->publish_state((this->store_.status & MICRON_KEY_BEEP_2_MASK) == MICRON_KEY_BEEP_2_MASK);
      }
      if (this->beep3_binary_sensor_) {
        this->beep3_binary_sensor_->publish_state((this->store_.status & MICRON_KEY_BEEP_3_MASK) == MICRON_KEY_BEEP_3_MASK);
      }

      if (this->zone1_binary_sensor_) {
        this->zone1_binary_sensor_->publish_state((this->store_.status & MICRON_ZONE_1_MASK) == MICRON_ZONE_1_MASK);
      }
      if (this->zone2_binary_sensor_) {
        this->zone2_binary_sensor_->publish_state((this->store_.status & MICRON_ZONE_2_MASK) == MICRON_ZONE_2_MASK);
      }
      if (this->zone3_binary_sensor_) {
        this->zone3_binary_sensor_->publish_state((this->store_.status & MICRON_ZONE_3_MASK) == MICRON_ZONE_3_MASK);
      }
      if (this->zone4_binary_sensor_) {
        this->zone4_binary_sensor_->publish_state((this->store_.status & MICRON_ZONE_4_MASK) == MICRON_ZONE_4_MASK);
      }
      if (this->zone5_binary_sensor_) {
        this->zone5_binary_sensor_->publish_state((this->store_.status & MICRON_ZONE_5_MASK) == MICRON_ZONE_5_MASK);
      }
      
      if (this->keypad_text_sensor_ && this->command_dedupe_.next(this->store_.command)) {
        this->keypad_text_sensor_->publish_state(str_sprintf("0x%02x", this->store_.command));
      }
      if (this->status_text_sensor_ && this->status_dedupe_.next(this->store_.status)) {
        this->status_text_sensor_->publish_state(str_sprintf("0x%04x", this->store_.status));
      }

      if (!this->command_queue_.empty() && (millis() - this->last_command_ms_) >= MICRON_MAX_COMMAND_DELAY_MS) {
        this->last_command_ms_ = millis();
        auto command = this->command_queue_.front();
        ESP_LOGD(TAG, "Write command: 0x%02x", command);
        this->store_.write(command, 2);
        this->command_queue_.pop();
        if (this->command_queue_.empty()) {
          ESP_LOGD(TAG, "All commands written");
        }
      }
    }

    void MicronComponent::update() {
      ESP_LOGD(TAG, "Command: %x,  Status: %x, Bits: %d, Packets: %d", this->store_.command, this->store_.status, this->store_.bits_received, this->store_.packets_received);
      ESP_LOGD(TAG, "Write stats: 0x%02x  repeat %d bits %d", this->store_.processor_.command_out, this->store_.processor_.command_repeat, this->store_.processor_.remaining_command_writes);
    }

    float MicronComponent::get_setup_priority() const { return setup_priority::DATA; }

  } // namespace micron
} // namespace esphome