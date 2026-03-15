#include "jura_coffee.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include "esphome/core/application.h"

namespace esphome {
namespace jura_coffee {

static const char *const TAG = "jura_coffee";

void JuraCoffeeComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Jura Coffee...");
}

void JuraCoffeeComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Jura Coffee Machine:");
  LOG_UPDATE_INTERVAL(this);
}

// Jura protocol encoding: each ASCII char becomes 4 UART bytes.
// Bits 2 and 5 of each UART byte carry 2 bits of the original char.
void JuraCoffeeComponent::encode_and_write_(char c) {
  for (int s = 0; s < 8; s += 2) {
    uint8_t rawbyte = 0xFF;
    if (!(c & (1 << (s + 0)))) rawbyte &= ~(1 << 2);
    if (!(c & (1 << (s + 1)))) rawbyte &= ~(1 << 5);
    this->write_byte(rawbyte);
  }
  // Use delayMicroseconds instead of delay to avoid WDT issues
  // 8ms = 8000us, but we yield periodically
  delayMicroseconds(8000);
}

std::string JuraCoffeeComponent::cmd2jura_(const std::string &cmd) {
  // Flush RX buffer
  while (this->available()) {
    uint8_t dummy;
    this->read_byte(&dummy);
  }

  // Send command with CR+LF
  for (size_t i = 0; i < cmd.length(); i++) {
    this->encode_and_write_(cmd[i]);
    // Feed watchdog every few chars
    if (i % 3 == 0) App.feed_wdt();
  }
  this->encode_and_write_('\r');
  App.feed_wdt();
  this->encode_and_write_('\n');
  App.feed_wdt();

  // Read response
  std::string response;
  int s = 0;
  char inbyte = 0;
  int timeout_counter = 0;

  while (true) {
    if (this->available()) {
      uint8_t rawbyte;
      this->read_byte(&rawbyte);
      if (rawbyte & (1 << 2)) inbyte |= (1 << (s + 0));
      else inbyte &= ~(1 << (s + 0));
      if (rawbyte & (1 << 5)) inbyte |= (1 << (s + 1));
      else inbyte &= ~(1 << (s + 1));

      s += 2;
      if (s >= 8) {
        s = 0;
        response += inbyte;
        inbyte = 0;
        if (response.length() >= 2 &&
            response[response.length() - 2] == '\r' &&
            response[response.length() - 1] == '\n') {
          response.resize(response.length() - 2);
          return response;
        }
      }
    } else {
      delayMicroseconds(10000);  // 10ms wait
    }
    if (++timeout_counter > 500) {
      ESP_LOGW(TAG, "Timeout waiting for Jura response to '%s'", cmd.c_str());
      return "";
    }
    // Feed watchdog every 10 iterations to prevent WDT reset
    if (timeout_counter % 10 == 0) App.feed_wdt();
  }
}

long JuraCoffeeComponent::parse_hex_(const std::string &data, int start, int length) {
  if ((int) data.length() < start + length) return 0;
  std::string hex_str = data.substr(start, length);
  return strtol(hex_str.c_str(), nullptr, 16);
}

void JuraCoffeeComponent::update() {
  App.feed_wdt();

  // Read EEPROM counters (RT:0000)
  std::string rt_result = this->cmd2jura_("RT:0000");
  if (!rt_result.empty()) {
    ESP_LOGD(TAG, "RT:0000 response: %s", rt_result.c_str());

    if (single_espresso_sensor_ != nullptr)
      single_espresso_sensor_->publish_state(parse_hex_(rt_result, 3, 4));
    if (ristretto_sensor_ != nullptr)
      ristretto_sensor_->publish_state(parse_hex_(rt_result, 7, 4));
    if (coffee_sensor_ != nullptr)
      coffee_sensor_->publish_state(parse_hex_(rt_result, 11, 4));
    if (double_coffee_sensor_ != nullptr)
      double_coffee_sensor_->publish_state(parse_hex_(rt_result, 15, 4));
    if (cappuccino_sensor_ != nullptr)
      cappuccino_sensor_->publish_state(parse_hex_(rt_result, 19, 4));
    if (latte_sensor_ != nullptr)
      latte_sensor_->publish_state(parse_hex_(rt_result, 23, 4));
    if (grinded_coffee_sensor_ != nullptr)
      grinded_coffee_sensor_->publish_state(parse_hex_(rt_result, 27, 4));
    if (cleanings_sensor_ != nullptr)
      cleanings_sensor_->publish_state(parse_hex_(rt_result, 35, 4));
    if (decalcifications_sensor_ != nullptr)
      decalcifications_sensor_->publish_state(parse_hex_(rt_result, 39, 4));
    if (double_ristretto_sensor_ != nullptr)
      double_ristretto_sensor_->publish_state(parse_hex_(rt_result, 51, 4));
    if (double_espresso_sensor_ != nullptr)
      double_espresso_sensor_->publish_state(parse_hex_(rt_result, 59, 4));
  } else {
    ESP_LOGW(TAG, "No response from RT:0000 — check UART wiring (TX/RX)");
  }

  App.feed_wdt();

  // Read machine status (IC:)
  std::string ic_result = this->cmd2jura_("IC:");
  if (!ic_result.empty()) {
    ESP_LOGD(TAG, "IC: response: %s", ic_result.c_str());

    if (ic_result.length() >= 5) {
      uint8_t status_byte = (uint8_t) strtol(
          ic_result.substr(3, 2).c_str(), nullptr, 16);
      bool tray_missing = (status_byte >> 4) & 1;
      bool tank_empty = (status_byte >> 5) & 1;

      if (tray_status_sensor_ != nullptr)
        tray_status_sensor_->publish_state(tray_missing ? "Fehlt" : "OK");
      if (tank_status_sensor_ != nullptr)
        tank_status_sensor_->publish_state(tank_empty ? "Auffuellen" : "OK");
    }
  } else {
    ESP_LOGW(TAG, "No response from IC: — check UART wiring (TX/RX)");
  }
}

}  // namespace jura_coffee
}  // namespace esphome
