import logging
import os
import urllib.parse
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome import pins
from esphome.components import binary_sensor
from esphome.cpp_helpers import gpio_pin_expression

from esphome.const import (
    CONF_ID,
    CONF_CLOCK_PIN,
    CONF_DATA_PIN,
    DEVICE_CLASS_OCCUPANCY
)

_LOGGER = logging.getLogger(__name__)

CODEOWNERS = ["@muxa"]
AUTO_LOAD = ["binary_sensor"]

micron_ns = cg.esphome_ns.namespace("micron")

MicronComponent = micron_ns.class_(
    "MicronComponent", cg.PollingComponent
)

CONF_ZONE_1 = "zone1"
CONF_ZONE_2 = "zone2"
CONF_ZONE_3 = "zone3"
CONF_ZONE_4 = "zone4"
CONF_ZONE_5 = "zone5"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(MicronComponent),
            cv.Required(CONF_CLOCK_PIN): cv.All(pins.internal_gpio_input_pin_schema),
            cv.Required(CONF_DATA_PIN): cv.All(pins.internal_gpio_input_pin_schema),
            # cv.Optional(CONF_NAME): cv.string,
            cv.Optional(CONF_ZONE_1): binary_sensor.binary_sensor_schema(
                device_class = DEVICE_CLASS_OCCUPANCY,
            ),
            cv.Optional(CONF_ZONE_2): binary_sensor.binary_sensor_schema(
                device_class = DEVICE_CLASS_OCCUPANCY,
            ),
            cv.Optional(CONF_ZONE_3): binary_sensor.binary_sensor_schema(
                device_class = DEVICE_CLASS_OCCUPANCY,
            ),
            cv.Optional(CONF_ZONE_4): binary_sensor.binary_sensor_schema(
                device_class = DEVICE_CLASS_OCCUPANCY,
            ),
            cv.Optional(CONF_ZONE_5): binary_sensor.binary_sensor_schema(
                device_class = DEVICE_CLASS_OCCUPANCY,
            ),
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

    if CONF_ZONE_1 in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_ZONE_1])
        cg.add(var.set_zone1_binary_sensor(sens))

    if CONF_ZONE_2 in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_ZONE_2])
        cg.add(var.set_zone2_binary_sensor(sens))

    if CONF_ZONE_3 in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_ZONE_3])
        cg.add(var.set_zone3_binary_sensor(sens))
    
    if CONF_ZONE_4 in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_ZONE_4])
        cg.add(var.set_zone4_binary_sensor(sens))
    
    if CONF_ZONE_5 in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_ZONE_5])
        cg.add(var.set_zone5_binary_sensor(sens))

    cg.add(var.dump_config())
