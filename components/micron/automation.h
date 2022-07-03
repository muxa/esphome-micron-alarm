#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "micron.h"

namespace esphome
{
  namespace micron
  {


    template <typename... Ts>
    class MicronPressAction : public Action<Ts...>, public Parented<MicronComponent>
    {
    public:
      MicronPressAction(std::string keys)
      {
        this->keys_ = keys;
      }

      void play(Ts... x) override { this->parent_->press(this->keys_); }

    protected:
      std::string keys_;
    };

  } // namespace micron
} // namespace esphome