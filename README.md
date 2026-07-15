## Credit

This module is derived from **[prospector-zmk-module](https://github.com/carrefinho/prospector-zmk-module)
by [carrefinho](https://github.com/carrefinho)** — the Prospector dongle concept, the adapter
shield, the OPERATOR status-screen layout, and the display driver all originate there. Go star
the original. This repo extends it with the touch UI / trackpad and trims it to just what this
hardware combination uses; the other upstream layouts (classic/radii/field) live upstream, not
here. Both repos are MIT licensed.

## Features

- **Touch UI** 
- **Trackpad**: 
- **Calculator** (decimal / binary-bitwise) local to the dongle
- **Macro pad**
- **Function Keys**
- **Symbols**
- **Modifier Keys**
- **Media Buttons**
- **Num Pad**
- **Clipboard Commands**
- **Settings screen**: brightness, trackpad sensitivity, 90°-step screen rotation, custom theme colors
- **Rotation** hardware scan-out (MADCTL) + LVGL resolution swap, buffer free
- CST816S gesture driver included (`touch_input.c`)

## Usage

`config/west.yml` in your zmk-config:

```yaml
  - name: dongle-mk1
    remote: <your-remote>
    revision: <pin a commit>
```

`build.yaml` target (the dongle is the split central):

```yaml
  - board: xiao_ble//zmk
    shield: <your_keyboard_dongle_shield> prospector_adapter
```

## License

MIT — see LICENSE. Portions copyright the ZMK Contributors and carrefinho.
