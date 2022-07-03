# ESPHome Micron Lynx Alarm

Control Mucron Lynx alarm system from ESPHome.

## Install

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/muxa/esphome-micron-alarm

```

## Usage

```yaml
micron:
  clock_pin:
    number: D1
    inverted: true
  data_in_pin: 
    number: D2
    inverted: true
  data_out_pin: D5
  update_interval: 1s
  zone1:
    name: "Alarm Presence Living Room"
  zone2:
    name: "Alarm Presence Kitchen"
  zone3:
    name: "Alarm Presence Hallway"
  zone4:
    name: "Alarm Presence Bedrooom"
```
