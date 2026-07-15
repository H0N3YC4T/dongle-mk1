#pragma once

/* INHERENT colours only: things that are a certain colour by nature (battery
 * tiers, BT blue, output/layer widgets) and deliberately ignore the theme.
 * Everything themeable is a role in theme.h. */

// ── Modifier indicator ────────────────────────────────────────────────────────
#define COLOR_MOD_SEPARATOR  0x606060

// ── Layer display ─────────────────────────────────────────────────────────────
#define COLOR_LAYER_TEXT          0xffffff
#define COLOR_LAYER_DOT_ACTIVE    0xe0e0e0
#define COLOR_LAYER_DOT_INACTIVE  0x575757

// ── Battery tiers: HIGH >60% (blue), MID 30-60% (yellow), LOW <30% (rose) ────
#define COLOR_BATTERY_HIGH                0xa8d0e6
#define COLOR_BATTERY_LOW                 0xc2526a
#define COLOR_BATTERY_RING                0x32424d
#define COLOR_BATTERY_BG                  0x505050
#define COLOR_BATTERY_LABEL               0xffffff
#define COLOR_BATTERY_DISCONNECTED_FILL   0x383c42
#define COLOR_BATTERY_DISCONNECTED_RING   0x282c30
#define COLOR_BATTERY_DISCONNECTED_LABEL  0x000000
#define COLOR_BATTERY_MID_FILL            0xf5e08c
#define COLOR_BATTERY_MID_RING            0x4a4530
#define COLOR_BATTERY_LOW_RING            0x4a2730

// ── Output widget ─────────────────────────────────────────────────────────────
#define COLOR_USB_ACTIVE_BG         0xb9b9a7
#define COLOR_USB_INACTIVE_BG       0x4F4F40
#define COLOR_BLE_ACTIVE_BG         0x569FA7
#define COLOR_BLE_INACTIVE_BG       0x353f40
#define COLOR_OUTPUT_ACTIVE_TEXT    0x000000
#define COLOR_OUTPUT_INACTIVE_TEXT  0x7b7d93
#define COLOR_SLOT_ACTIVE_BG        0x7b7d93
#define COLOR_SLOT_INACTIVE_BG      0x353640
#define COLOR_SLOT_TEXT             0xffffff
