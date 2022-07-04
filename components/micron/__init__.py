import logging
import os
import urllib.parse
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome import pins
from esphome.components import binary_sensor, text_sensor
from esphome.cpp_helpers import gpio_pin_expression

from esphome.const import (
    CONF_ID,
    CONF_CLOCK_PIN,
    CONF_DATA_PIN,
    DEVICE_CLASS_OCCUPANCY,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_PROBLEM,
    DEVICE_CLASS_SOUND,
    DEVICE_CLASS_CONNECTIVITY,
    ENTITY_CATEGORY_DIAGNOSTIC,
)

_LOGGER = logging.getLogger(__name__)

CODEOWNERS = ["@muxa"]
AUTO_LOAD = ["binary_sensor", "text_sensor"]

micron_ns = cg.esphome_ns.namespace("micron")

MicronComponent = micron_ns.class_(
    "MicronComponent", cg.PollingComponent
)

MicronPressAction = micron_ns.class_("MicronPressAction", automation.Action)

CONF_DATA_IN_PIN = "data_in_pin"
CONF_DATA_OUT_PIN = "data_out_pin"
CONF_PRESS_KEYS = "keys"

CONF_CONNECTED = "connected"

CONF_M = "m"
CONF_S1 = "s1"
CONF_S2 = "s2"
CONF_BEEP_1 = "beep1"
CONF_BEEP_2 = "beep2"
CONF_BEEP_3 = "beep3"

CONF_ZONE_1 = "zone1"
CONF_ZONE_2 = "zone2"
CONF_ZONE_3 = "zone3"
CONF_ZONE_4 = "zone4"
CONF_ZONE_5 = "zone5"

CONF_KEYPAD = "keypad"
CONF_STATUS = "status"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(MicronComponent),
            cv.Required(CONF_CLOCK_PIN): cv.All(pins.internal_gpio_input_pin_schema),
            cv.Required(CONF_DATA_IN_PIN): cv.All(pins.internal_gpio_input_pin_schema),
            cv.Required(CONF_DATA_OUT_PIN): cv.All(pins.internal_gpio_output_pin_schema),
            cv.Optional(CONF_CONNECTED): binary_sensor.binary_sensor_schema(
                device_class = DEVICE_CLASS_CONNECTIVITY,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
            cv.Optional(CONF_M): binary_sensor.binary_sensor_schema(
                device_class = DEVICE_CLASS_POWER,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
            cv.Optional(CONF_S1): binary_sensor.binary_sensor_schema(
                device_class = DEVICE_CLASS_PROBLEM,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
            cv.Optional(CONF_S2): binary_sensor.binary_sensor_schema(
                device_class = DEVICE_CLASS_PROBLEM,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
            cv.Optional(CONF_BEEP_1): binary_sensor.binary_sensor_schema(
                device_class = DEVICE_CLASS_SOUND,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
            cv.Optional(CONF_BEEP_2): binary_sensor.binary_sensor_schema(
                device_class = DEVICE_CLASS_SOUND,
            ),
            cv.Optional(CONF_BEEP_3): binary_sensor.binary_sensor_schema(
                device_class = DEVICE_CLASS_SOUND,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
            cv.Optional(CONF_ZONE_1): binary_sensor.binary_sensor_schema(
                device_class = DEVICE_CLASS_OCCUPANCY,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
            cv.Optional(CONF_ZONE_2): binary_sensor.binary_sensor_schema(
                device_class = DEVICE_CLASS_OCCUPANCY,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
            cv.Optional(CONF_ZONE_3): binary_sensor.binary_sensor_schema(
                device_class = DEVICE_CLASS_OCCUPANCY,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
            cv.Optional(CONF_ZONE_4): binary_sensor.binary_sensor_schema(
                device_class = DEVICE_CLASS_OCCUPANCY,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
            cv.Optional(CONF_ZONE_5): binary_sensor.binary_sensor_schema(
                device_class = DEVICE_CLASS_OCCUPANCY,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
            cv.Optional(CONF_KEYPAD): text_sensor.text_sensor_schema(                
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
                icon="mdi:dialpad"
            ),
            cv.Optional(CONF_STATUS): text_sensor.text_sensor_schema(                
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
                icon="mdi:list-status"
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
    pin_data = await gpio_pin_expression(config[CONF_DATA_IN_PIN])
    cg.add(var.set_pin_data(pin_data))
    pin_data_out = await gpio_pin_expression(config[CONF_DATA_OUT_PIN])
    cg.add(var.set_pin_data_out(pin_data_out))

    if CONF_CONNECTED in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_CONNECTED])
        cg.add(var.set_connected_binary_sensor(sens))

    if CONF_M in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_M])
        cg.add(var.set_m_binary_sensor(sens))

    if CONF_S1 in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_S1])
        cg.add(var.set_s1_binary_sensor(sens))

    if CONF_S2 in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_S2])
        cg.add(var.set_s2_binary_sensor(sens))

    if CONF_BEEP_1 in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_BEEP_1])
        cg.add(var.set_beep1_binary_sensor(sens))

    if CONF_BEEP_2 in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_BEEP_2])
        cg.add(var.set_beep2_binary_sensor(sens))

    if CONF_BEEP_3 in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_BEEP_3])
        cg.add(var.set_beep3_binary_sensor(sens))

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

    if CONF_KEYPAD in config:
        sens = await text_sensor.new_text_sensor(config[CONF_KEYPAD])
        cg.add(var.set_keypad_text_sensor(sens))

    if CONF_STATUS in config:
        sens = await text_sensor.new_text_sensor(config[CONF_STATUS])
        cg.add(var.set_status_text_sensor(sens))

    cg.add(var.dump_config())

@automation.register_action(
    "micron.press",
    MicronPressAction,
    cv.maybe_simple_value(
        {
            cv.GenerateID(): cv.use_id(MicronComponent),
            cv.Required(CONF_PRESS_KEYS): cv.string_strict,
        },
        key=CONF_PRESS_KEYS
    ),
)
def micron_press_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg, config[CONF_PRESS_KEYS])
    yield cg.register_parented(var, config[CONF_ID])
    yield var