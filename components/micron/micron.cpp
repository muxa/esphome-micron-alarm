#include "micron.h"
#include "esphome/core/log.h"

namespace esphome
{
  namespace micron
  {

    static const char *const TAG = "micron";

    bool IRAM_ATTR MicronDataProcessor::decode(uint32_t ms, bool data) {
      // check if a new message has started, based on time since previous bit
      if ((ms - this->prev_ms_) > MICRON_MAX_MS) {
        this->num_bits_ = 0;
      }
      this->prev_ms_ = ms;

      // number of bits received is basically the "state"
      if (this->num_bits_ < MICRON_FRAME_SIZE) {
        // store it while it fits
        int idx = this->num_bits_ / 8;
        this->buffer_[idx] = (this->buffer_[idx] << 1) | (data ? 1 : 0);
        this->num_bits_++;

        // are we done yet?
        if (this->num_bits_ == MICRON_FRAME_SIZE) {

          this->packet->command = this->buffer_[MICRON_BYTE_COMMAND];
          this->packet->status = this->buffer_[MICRON_BYTE_HIGH] << 8 | this->buffer_[MICRON_BYTE_LOW];
          return true;
        }
      }

      return false;
    }

    void MicronStore::setup(InternalGPIOPin *pin_clock, InternalGPIOPin *pin_data) {
      pin_clock->setup();
      pin_data->setup();
      this->pin_clock_ = pin_clock->to_isr();
      this->pin_data_ = pin_data->to_isr();
      pin_clock->attach_interrupt(MicronStore::interrupt, this, gpio::INTERRUPT_FALLING_EDGE);
      // pin_clock->attach_interrupt(MicronStore::interrupt, this, gpio::INTERRUPT_RISING_EDGE);
    }

    void IRAM_ATTR MicronStore::interrupt(MicronStore *arg) {
      uint32_t now = millis();
      bool data_bit = arg->pin_data_.digital_read();

      arg->bits_received++;      

      if (arg->processor_.decode(now, data_bit)) {
        arg->packets_received++;
        arg->set_data_(arg->processor_.packet);
      }
    }

    void IRAM_ATTR MicronStore::set_data_(MicronPacket *packet) {
      this->command = packet->command;
      this->status = packet->status;
    }

    void MicronComponent::setup()
    {
      ESP_LOGCONFIG(TAG, "Setting up Micron...");

      this->store_.setup(this->pin_clock_, this->pin_data_);
      
      // this->pin_clock_->attach_interrupt(MicronStore::gpio_intr, &this->store_, gpio::INTERRUPT_ANY_EDGE);
    }

    void MicronComponent::dump_config() {
      //LOG_SENSOR("", "Micron", this);
      ESP_LOGCONFIG(TAG, "Micron:");
      LOG_PIN("  Pin Clock: ", this->pin_clock_);
      LOG_PIN("  Pin Data: ", this->pin_data_);
      LOG_BINARY_SENSOR("  ", "Zone 1", this->zone1_binary_sensor_);
      LOG_BINARY_SENSOR("  ", "Zone 2", this->zone2_binary_sensor_);
      LOG_BINARY_SENSOR("  ", "Zone 3", this->zone3_binary_sensor_);
      LOG_BINARY_SENSOR("  ", "Zone 4", this->zone4_binary_sensor_);
      LOG_BINARY_SENSOR("  ", "Zone 5", this->zone5_binary_sensor_);
    }

    void MicronComponent::update() {
      ESP_LOGD(TAG, "Command: %x,  Status: %x, Bits: %d, Packets: %d", this->store_.command, this->store_.status, this->store_.bits_received, this->store_.packets_received);

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
    }

    float MicronComponent::get_setup_priority() const { return setup_priority::DATA; }

  } // namespace micron
} // namespace esphome