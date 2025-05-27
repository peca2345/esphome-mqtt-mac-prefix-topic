
habox/unit/habox5-dev-topic/b0b21cf490f0/esp/status/debug
habox5-dev-topic-f490f0/sensor/habox_uptime/state

```
esphome:
  name: habox5-dev-topic
  friendly_name: habox5-dev-topic
  name_add_mac_suffix: true
  project:
    name: "habox.habox5"
    version: "1.0.0"
  on_boot:
    priority: -100
    then:
      - lambda: |-
          // Ziskame MAC adresu a ulozime ji do globalni promenne
          auto mac = get_mac_address();
          id(mac_prefix) = mac;
          ESP_LOGD("mac_prefix", "MAC adresa zarizeni: %s", mac.c_str());
          // Nastavime topic prefix s MAC adresou
          std::string mac_topic = "${base_prefix}/${device_name}/" + mac;
          // Pozor, metoda ocekava dva parametry
          id(mqtt_client).set_topic_prefix(mac_topic, mac_topic);

substitutions:
  # Pro definování základních prefixů a názvu zařízení
  device_name: habox5-dev-topic
  base_prefix: "habox/unit"
  # Cesty pro MQTT topiky (budou upraveny dynamicky v kódu)
  mqtt_status_topic: "${base_prefix}/${device_name}/esp/status"
  mqtt_debug_topic: "${base_prefix}/${device_name}/esp/status/debug"

esp32:
  board: esp32dev
  framework:
    type: esp-idf
    version: recommended
  flash_size: 16MB
  partitions: "partitions-16MB.csv"

# Pomocná proměnná pro získání MAC adresy dynamicky
globals:
  - id: mac_prefix
    type: std::string
    restore_value: no
    initial_value: '""'

button:
  - platform: restart
    name: "Habox Restart"
    id: habox_restart_btn

switch:
  - platform: factory_reset
    name: "ESP Factory Reset"
    id: habox_factory_reset_sw
  
  - platform: safe_mode
    name: "ESP Safe Mode"
    id: habox_safe_mode_sw
    
  - platform: shutdown
    name: "ESP Shutdown"
    id: habox_shutdown_sw

# Senzory
sensor:
  - platform: uptime
    name: "Habox Uptime"
    id: habox_uptime
    update_interval: 60s
    filters:
      - lambda: return x / 3600.0;
    unit_of_measurement: "h"

text_sensor:
  - platform: ethernet_info
    ip_address:
      name: "Habox IP Adresa"
      id: habox_ip_address
    mac_address:
      name: "Habox MAC Adresa"
      id: habox_mac_address
      on_value:
        then:
          - lambda: |-
              // Po získání MAC adresy aktualizujeme mac_prefix, pokud ještě nebyla nastavena
              if (id(mac_prefix).empty()) {
                id(mac_prefix) = x;
                ESP_LOGD("mac_prefix", "MAC adresa nastavena z MAC senzoru: %s", x.c_str());
                // Nastavime MAC adresu do MQTT topic
                std::string mac_topic = "${base_prefix}/${device_name}/" + x;
                id(mqtt_client).set_topic_prefix(mac_topic, mac_topic);
              }

  - platform: version
    name: "Habox ESPHome Verze"
    id: habox_version

  # Nový text senzor pro zobrazení MAC adresy v prefixu
  - platform: template
    name: "MAC Prefix"
    id: mac_prefix_sensor
    lambda: |-
      return id(mac_prefix);
    update_interval: 60s
    
  # Pomocný vlastní textový sensor pro aktuální MQTT topic prefix
  - platform: template
    name: "MQTT Topic Prefix"
    id: mqtt_topic_prefix
    lambda: |-
      return id(mqtt_client).get_topic_prefix();
    update_interval: 60s

binary_sensor:
  - platform: status
    name: "Habox Online Status"
    id: habox_online_status
    # Explicitně nastavíme jako komponentu pro status
    # ale necháme topic_prefix nastavit dynamicky

mqtt:
  id: mqtt_client
  broker: !secret habox5_dev_mqtt_broker
  port: !secret habox5_dev_mqtt_port
  username: !secret habox5_dev_mqtt_username
  password: !secret habox5_dev_mqtt_password
  certificate_authority: !secret mqtt_ssl_cert
  discovery: false
  # Základní topic prefix - bude dynamicky upraven podle MAC adresy
  topic_prefix: "${base_prefix}/${device_name}"
  # Nyní používáme absolutní cesty pro systémové topiky
  log_topic: "${mqtt_debug_topic}"
  birth_message:
    topic: "${mqtt_status_topic}"
    payload: online
    retain: true
  will_message:
    topic: "${mqtt_status_topic}"
    payload: offline
    retain: true
  shutdown_message:
    topic: "${mqtt_status_topic}/shutdown"
    payload: shutting_down
    retain: true
  # Přidáváme on_connect callback pro aktualizaci MAC adresy
  on_connect:
    then:
      - delay: 1s
      - lambda: |-
          std::string mac = id(mac_prefix);
          if (!mac.empty()) {
            std::string mac_topic = "${base_prefix}/${device_name}/" + mac;
            // Musíme použít dva parametry pro set_topic_prefix
            id(mqtt_client).set_topic_prefix(mac_topic, mac_topic);
            ESP_LOGD("mqtt", "MQTT connected, nastavuji topic prefix s MAC: %s", mac_topic.c_str());
            
            // Nastavíme log_message_template s MAC adresou
            id(mqtt_client).set_log_message_template({
              .topic = mac_topic + "/esp/status/debug",
              .payload = "", // Payload is set by the logger
              .qos = 0,
              .retain = true
            });
            
            // Vytvoříme birth message s MAC adresou
            id(mqtt_client).set_birth_message({
              .topic = mac_topic + "/esp/status",
              .payload = "online",
              .qos = 0,
              .retain = true
            });
            
            // Poznámka:
            // Will message a shutdown message nelze změnit za běhu,
            // jsou nastaveny při inicializaci klienta
          }

logger:
  level: DEBUG
  logs:
    mqtt: DEBUG
    mqtt.component: DEBUG
    esp-idf: NONE  
    api: DEBUG
    api.connection: DEBUG
    ethernet: DEBUG
    ota: DEBUG
    sensor: DEBUG
    text_sensor: DEBUG

ethernet:
  type: LAN8720
  mdc_pin: "GPIO23"
  mdio_pin: "GPIO18"
  clk_mode: "GPIO17_OUT"
  phy_addr: "0"
  manual_ip:
    static_ip: !secret habox5_dev_ethernet_static_ip
    gateway: !secret habox5_dev_ethernet_gateway
    subnet: !secret habox5_dev_ethernet_subnet
    dns1: !secret habox5_dev_ethernet_dns1
    dns2: !secret habox5_dev_ethernet_dns2

mdns:
  disabled: false

ota:
  - platform: esphome
    password: !secret habox5_dev_ota_password

api:

i2c:
  sda: "GPIO4"
  scl: "GPIO5"
  scan: true
  frequency: "300000"

```
