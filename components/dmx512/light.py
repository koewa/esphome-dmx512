import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import light, dmx512
from esphome.const import (
    CONF_CHANNEL,
    CONF_METHOD,
    CONF_NUM_LEDS,
    CONF_TYPE,
    CONF_OUTPUT_ID,
)
from esphome.core import CORE, coroutine

dmx512_ns = cg.esphome_ns.namespace("dmx512")
DMX512LightOutput = dmx512_ns.class_(
    "DMX512LightOutput", light.AddressableLight
)
DMX512PixelOrder = dmx512_ns.namespace("DMX512PixelOrder")

CONF_UNIVERSE = 'universe'

def validate_type(value):
    value = cv.string(value).upper()
    if "R" not in value:
        raise cv.Invalid("Must have R in type")
    if "G" not in value:
        raise cv.Invalid("Must have G in type")
    if "B" not in value:
        raise cv.Invalid("Must have B in type")
    rest = set(value) - set("RGBWCE")
    if rest:
        raise cv.Invalid(f"Types has invalid color: {', '.join(rest)}")
    if len(set(value)) != len(value):
        raise cv.Invalid("Type has duplicate color!")
    return value

def _declare_type(value):
    if CORE.is_esp32:
        if CORE.using_arduino:
            return cv.use_id(dmx512.DMX512ESP32)(value)
        else:
            return cv.use_id(dmx512.DMX512ESP32IDF)(value)
    elif CORE.is_esp8266:
        return cv.use_id(dmx512.DMX512ESP8266)(value)
    raise NotImplementedError


CONF_OFFSET = "offset"
CONFIG_SCHEMA = cv.All(
    light.ADDRESSABLE_LIGHT_SCHEMA.extend(
        {
            cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(DMX512LightOutput),
            cv.Optional(CONF_TYPE, default="GRB"): validate_type,
            cv.Optional(CONF_OFFSET, default=0): cv.int_range(min=0, max=512),
            cv.Required(CONF_NUM_LEDS): cv.positive_not_null_int,
            cv.GenerateID(CONF_UNIVERSE): _declare_type,
        }
    ).extend(cv.COMPONENT_SCHEMA),
)

def to_code(config):
    rhs = DMX512LightOutput.new()
    var = cg.Pvariable(config[CONF_OUTPUT_ID], rhs, DMX512LightOutput)
    yield light.register_light(var, config)
    yield cg.register_component(var, config)

    dmx = yield cg.get_variable(config[CONF_UNIVERSE])
    cg.add(var.set_universe(dmx))
    cg.add(var.add_leds(config[CONF_NUM_LEDS], getattr(DMX512PixelOrder, config[CONF_TYPE]), config[CONF_OFFSET]))
