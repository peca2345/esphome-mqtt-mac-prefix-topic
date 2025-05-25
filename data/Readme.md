# Externi komponenta `mqtt_mac` pro ESPHome

## Ucel
Komponenta `mqtt_mac` automaticky nastavuje prefix vsech MQTT temat ve tvaru:

```
<nazev_zarizeni>/<MAC_adresa>
```

Kazde zarizeni tak ma unikatni MQTT vetev podle sve MAC adresy.

---

## Struktura slozky

```
mqtt_mac/
├── __init__.py
├── mqtt_mac.h
├── mqtt_mac.cpp
└── mqtt_mac.yaml
```

---

## Instalace a pouziti

1. **Zkopirujte slozku `mqtt_mac`** do adresare s vlastnimi komponentami (napr. `custom_component`).

2. **Pridani do YAML konfigurace ESPHome:**

```yaml
external_components:
  - source:
      type: local
      path: custom_component
    components: [ mqtt_mac ]

mqtt_mac:
  id: mqtt_mac_1
```

3. **Nepouzivejte v substitucich promenne typu `${mac_prefix}` nebo `${device_mac}`!**
Prefix bude automaticky nastaven komponentou.

4. **MQTT temata v senzorech a sablonach pište bez MAC adresy:**

```yaml
sensor:
  - platform: template
    name: "inv_power_pv1"
    id: inv_power_pv1
    mqtt_id: mqtt_1
    state_topic: "habox/unit/inv/sensor/power/pv1"
```

Vysledne MQTT tema bude automaticky:
```
<nazev_zarizeni>/<MAC_adresa>/habox/unit/inv/sensor/power/pv1
```

---

## Jak to funguje

- Komponenta pri startu zarizeni ziska MAC adresu pomoci `esphome::get_mac_address()`.
- Prefix pro MQTT temata nastavi ve tvaru:
  `App.get_name() + "/" + mac`
- Vsechna MQTT temata budou zacinat timto prefixem.

---

## Priklad vysledneho tematu

Pokud je nazev zarizeni `habox5-dev` a MAC adresa `b0b21cf490f0`, vysledne MQTT tema bude napr.:

```
habox5-dev/b0b21cf490f0/sensor/esp_uptime/state
```

---

## Poznamky

- Pokud jste drive pouzivali jiny styl prefixu (napr. s podtrzitkem), stare vetve v MQTT brokeru je potreba smazat rucne.
- Komponenta funguje jak pro WiFi, tak pro Ethernet (vyuziva univerzalni ESPHome funkci pro MAC adresu).
- Linter chyby ohledne include souboru muzete ignorovat – pri kompilaci v ESPHome vse funguje.



## ESPHome code:
```
external_components:
  - source:
      type: local
      path: custom_component
    components: [ mqtt_mac ]

esp32:
  board: esp32dev
  framework:
    type: esp-idf
    version: recommended
  flash_size: 16MB
  partitions: "partitions-16MB.csv"
  
esphome:
  name: ${device_name}
  friendly_name: ${device_friendly_name}
  on_boot:
    priority: 600
    then:
      - output.set_level:
          id: dac1_output
          level: 0
  
ethernet:
  type: LAN8720
  mdc_pin: ${ethernet_mdc_pin}
  mdio_pin: ${ethernet_mdio_pin}
  clk_mode: ${ethernet_clk_mode_pin}_OUT
  phy_addr: ${ethernet_phy_addr}
  manual_ip:
    static_ip: 192.168.1.110
    gateway: 192.168.1.1
    subnet: 255.255.255.0
    dns1: 192.168.1.1
    dns2: 1.1.1.1

logger:
  level: INFO
  logs:
    mqtt: INFO
    mqtt.component: INFO
    esp-idf: NONE  
    api: INFO
    api.connection: INFO
    ethernet: INFO
    ota: INFO
    sensor: NONE
    switch: NONE
    i2c: NONE
    modbus: NONE
    display: NONE

mqtt:
  id: mqtt_1
  broker: "b4f06c9b4dfc441aa688a30032e8947c.s1.eu.hivemq.cloud"
  port: 8883
  username: "habox"
  password: "2ZKuJFYA8DTsNhRut3lBc6ojw9kbAn2N"

mqtt_mac:
  id: mqtt_mac_1

sensor:
  - platform: template
    name: "inv_power_pv1"
    id: inv_power_pv1
    mqtt_id: mqtt_1
    state_topic: "habox/unit/inv/sensor/power/pv1"

mdns:
  disabled: false

http_request:
  useragent: esphome/device
  timeout: 30s

# ota:
  # - platform: http_request
ota:
  - platform: esphome
    password: "4577c48dda57c11d3f5f10692efdd251"

time:
  - platform: sntp
    id: sntp_time
    timezone: Europe/Prague
    servers:
      - 0.cz.pool.ntp.org
      - 1.cz.pool.ntp.org
      - 2.cz.pool.ntp.org
```
