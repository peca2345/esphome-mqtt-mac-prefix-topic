#pragma once

#include "esphome/core/component.h"
#include "esphome/components/mqtt/mqtt_client.h"
#include "esphome/core/helpers.h"
#include "esphome/components/ethernet/ethernet_component.h"

namespace esphome {
namespace mqtt_mac {

class MQTTMacComponent : public Component {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_CONNECTION; }

 protected:
  std::string get_mac_address();
  void update_topic_prefix();
};

}  // namespace mqtt_mac
}  // namespace esphome 