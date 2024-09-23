# ESPHome Micron Lynx Alarm

Control Micron Lynx alarm system from ESPHome.

## Schematic

![Schematic](Schematic.png)

[Schematic PDF](Schematic.pdf)


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
  update_interval: 60s
  connected:
    name: "Alarm Connected"
  s1:
    name: "Alarm S1 Indicator"
  s2:
    name: "Alarm S2 Indicator"
  m:
    name: "Alarm M Indicator"
  beep1:
    name: "Alarm Beep 1"
  beep2:
    name: "Alarm Beep 2"
  beep3:
    name: "Alarm Beep 3"
  zone1:
    name: "Alarm Presence Living Room"
  zone2:
    name: "Alarm Presence Kitchen"
  zone3:
    name: "Alarm Presence Hallway"
  zone4:
    name: "Alarm Presence Bedrooom"
  zone5:
    name: "Alarm Presense Zone 5"
```
