import logging
import os
import urllib.parse
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome import pins
from esphome.cpp_helpers import gpio_pin_expression

from esphome.const import (
    CONF_ID,
    CONF_CLOCK_PIN,
    CONF_DATA_PIN,
)

_LOGGER = logging.getLogger(__name__)

micron_ns = cg.esphome_ns.namespace("micron")

MicronComponent = micron_ns.class_(
    "MicronComponent", cg.PollingComponent
)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(MicronComponent),
            cv.Required(CONF_CLOCK_PIN): cv.All(pins.internal_gpio_input_pin_schema),
            cv.Required(CONF_DATA_PIN): cv.All(pins.internal_gpio_input_pin_schema),
            # cv.Optional(CONF_NAME): cv.string,
            
        }
    ).extend(cv.polling_component_schema("60s"))#.extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):

    cg.add_global(micron_ns.using)

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    pin_clock = await gpio_pin_expression(config[CONF_CLOCK_PIN])
    cg.add(var.set_pin_clock(pin_clock))
    pin_data = await gpio_pin_expression(config[CONF_DATA_PIN])
    cg.add(var.set_pin_data(pin_data))

    cg.add(var.dump_config())
