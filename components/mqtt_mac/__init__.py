import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import mqtt

mqtt_mac_ns = cg.esphome_ns.namespace('mqtt_mac')
MQTTMacComponent = mqtt_mac_ns.class_('MQTTMacComponent', cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(MQTTMacComponent),
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config) 