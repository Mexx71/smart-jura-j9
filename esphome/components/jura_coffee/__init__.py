import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor, text_sensor
from esphome.const import (
    CONF_ID,
    CONF_UPDATE_INTERVAL,
    UNIT_EMPTY,
    ICON_COUNTER,
    STATE_CLASS_TOTAL_INCREASING,
)

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["sensor", "text_sensor"]

CONF_SINGLE_ESPRESSO = "single_espresso"
CONF_DOUBLE_ESPRESSO = "double_espresso"
CONF_COFFEE = "coffee"
CONF_DOUBLE_COFFEE = "double_coffee"
CONF_RISTRETTO = "ristretto"
CONF_DOUBLE_RISTRETTO = "double_ristretto"
CONF_CAPPUCCINO = "cappuccino"
CONF_LATTE = "latte"
CONF_MILK = "milk"
CONF_HOT_WATER = "hot_water"
CONF_GRINDED_COFFEE = "grinded_coffee"
CONF_CLEANINGS = "cleanings"
CONF_DECALCIFICATIONS = "decalcifications"
CONF_FILTER_CHANGES = "filter_changes"
CONF_MILK_CLEANINGS = "milk_cleanings"
CONF_TRAY_STATUS = "tray_status"
CONF_TANK_STATUS = "tank_status"

jura_coffee_ns = cg.esphome_ns.namespace("jura_coffee")
JuraCoffeeComponent = jura_coffee_ns.class_(
    "JuraCoffeeComponent", cg.PollingComponent, uart.UARTDevice
)

SENSOR_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_EMPTY,
    icon=ICON_COUNTER,
    accuracy_decimals=0,
    state_class=STATE_CLASS_TOTAL_INCREASING,
)

TEXT_SENSOR_SCHEMA = text_sensor.text_sensor_schema()

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(JuraCoffeeComponent),
            cv.Optional(CONF_SINGLE_ESPRESSO): SENSOR_SCHEMA,
            cv.Optional(CONF_DOUBLE_ESPRESSO): SENSOR_SCHEMA,
            cv.Optional(CONF_COFFEE): SENSOR_SCHEMA,
            cv.Optional(CONF_DOUBLE_COFFEE): SENSOR_SCHEMA,
            cv.Optional(CONF_RISTRETTO): SENSOR_SCHEMA,
            cv.Optional(CONF_DOUBLE_RISTRETTO): SENSOR_SCHEMA,
            cv.Optional(CONF_CAPPUCCINO): SENSOR_SCHEMA,
            cv.Optional(CONF_LATTE): SENSOR_SCHEMA,
            cv.Optional(CONF_MILK): SENSOR_SCHEMA,
            cv.Optional(CONF_HOT_WATER): SENSOR_SCHEMA,
            cv.Optional(CONF_GRINDED_COFFEE): SENSOR_SCHEMA,
            cv.Optional(CONF_CLEANINGS): SENSOR_SCHEMA,
            cv.Optional(CONF_DECALCIFICATIONS): SENSOR_SCHEMA,
            cv.Optional(CONF_FILTER_CHANGES): SENSOR_SCHEMA,
            cv.Optional(CONF_MILK_CLEANINGS): SENSOR_SCHEMA,
            cv.Optional(CONF_TRAY_STATUS): TEXT_SENSOR_SCHEMA,
            cv.Optional(CONF_TANK_STATUS): TEXT_SENSOR_SCHEMA,
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    sensor_keys = [
        CONF_SINGLE_ESPRESSO,
        CONF_DOUBLE_ESPRESSO,
        CONF_COFFEE,
        CONF_DOUBLE_COFFEE,
        CONF_RISTRETTO,
        CONF_DOUBLE_RISTRETTO,
        CONF_CAPPUCCINO,
        CONF_LATTE,
        CONF_MILK,
        CONF_HOT_WATER,
        CONF_GRINDED_COFFEE,
        CONF_CLEANINGS,
        CONF_DECALCIFICATIONS,
        CONF_FILTER_CHANGES,
        CONF_MILK_CLEANINGS,
    ]

    for key in sensor_keys:
        if key in config:
            sens = await sensor.new_sensor(config[key])
            cg.add(getattr(var, f"set_{key}_sensor")(sens))

    text_sensor_keys = [CONF_TRAY_STATUS, CONF_TANK_STATUS]
    for key in text_sensor_keys:
        if key in config:
            sens = await text_sensor.new_text_sensor(config[key])
            cg.add(getattr(var, f"set_{key}_sensor")(sens))
