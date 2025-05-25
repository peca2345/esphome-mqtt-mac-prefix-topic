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
