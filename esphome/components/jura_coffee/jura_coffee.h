#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace jura_coffee {

class JuraCoffeeComponent : public PollingComponent, public uart::UARTDevice {
 public:
  // Sensor setters (called from generated code)
  void set_single_espresso_sensor(sensor::Sensor *s) { single_espresso_sensor_ = s; }
  void set_double_espresso_sensor(sensor::Sensor *s) { double_espresso_sensor_ = s; }
  void set_coffee_sensor(sensor::Sensor *s) { coffee_sensor_ = s; }
  void set_double_coffee_sensor(sensor::Sensor *s) { double_coffee_sensor_ = s; }
  void set_ristretto_sensor(sensor::Sensor *s) { ristretto_sensor_ = s; }
  void set_double_ristretto_sensor(sensor::Sensor *s) { double_ristretto_sensor_ = s; }
  void set_cappuccino_sensor(sensor::Sensor *s) { cappuccino_sensor_ = s; }
  void set_latte_sensor(sensor::Sensor *s) { latte_sensor_ = s; }
  void set_milk_sensor(sensor::Sensor *s) { milk_sensor_ = s; }
  void set_hot_water_sensor(sensor::Sensor *s) { hot_water_sensor_ = s; }
  void set_grinded_coffee_sensor(sensor::Sensor *s) { grinded_coffee_sensor_ = s; }
  void set_cleanings_sensor(sensor::Sensor *s) { cleanings_sensor_ = s; }
  void set_decalcifications_sensor(sensor::Sensor *s) { decalcifications_sensor_ = s; }
  void set_filter_changes_sensor(sensor::Sensor *s) { filter_changes_sensor_ = s; }
  void set_milk_cleanings_sensor(sensor::Sensor *s) { milk_cleanings_sensor_ = s; }
  void set_tray_status_sensor(text_sensor::TextSensor *s) { tray_status_sensor_ = s; }
  void set_tank_status_sensor(text_sensor::TextSensor *s) { tank_status_sensor_ = s; }

  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  // Jura protocol: encode and send command, return response
  std::string cmd2jura_(const std::string &cmd);

  // Encode a single ASCII char into 4 Jura protocol bytes
  void encode_and_write_(char c);

  // Parse hex substring from response
  long parse_hex_(const std::string &data, int start, int length);

  // Sensors
  sensor::Sensor *single_espresso_sensor_{nullptr};
  sensor::Sensor *double_espresso_sensor_{nullptr};
  sensor::Sensor *coffee_sensor_{nullptr};
  sensor::Sensor *double_coffee_sensor_{nullptr};
  sensor::Sensor *ristretto_sensor_{nullptr};
  sensor::Sensor *double_ristretto_sensor_{nullptr};
  sensor::Sensor *cappuccino_sensor_{nullptr};
  sensor::Sensor *latte_sensor_{nullptr};
  sensor::Sensor *milk_sensor_{nullptr};
  sensor::Sensor *hot_water_sensor_{nullptr};
  sensor::Sensor *grinded_coffee_sensor_{nullptr};
  sensor::Sensor *cleanings_sensor_{nullptr};
  sensor::Sensor *decalcifications_sensor_{nullptr};
  sensor::Sensor *filter_changes_sensor_{nullptr};
  sensor::Sensor *milk_cleanings_sensor_{nullptr};
  text_sensor::TextSensor *tray_status_sensor_{nullptr};
  text_sensor::TextSensor *tank_status_sensor_{nullptr};
};

}  // namespace jura_coffee
}  // namespace esphome
