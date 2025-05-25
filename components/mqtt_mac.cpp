#include "mqtt_mac.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/components/mqtt/mqtt_client.h"
#include "esphome/components/ethernet/ethernet_component.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace mqtt_mac {

static const char *const TAG = "mqtt_mac";

void MQTTMacComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MQTT MAC component...");
  this->update_topic_prefix();
}

void MQTTMacComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "MQTT MAC component:");
  ESP_LOGCONFIG(TAG, "  MAC Address: %s", this->get_mac_address().c_str());
}

std::string MQTTMacComponent::get_mac_address() {
  return esphome::get_mac_address();
}

void MQTTMacComponent::update_topic_prefix() {
  std::string mac = this->get_mac_address();
  std::string new_prefix = App.get_name() + "/" + mac;
  
  // Update MQTT client topic prefix
  mqtt::global_mqtt_client->set_topic_prefix(new_prefix, new_prefix);
  ESP_LOGD(TAG, "Updated MQTT topic prefix to: %s", new_prefix.c_str());
}

}  // namespace mqtt_mac
}  // namespace esphome 