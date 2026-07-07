/* Status-screen entry point: assembles the widgets and attaches the touch UI
 * overlay. The touch UI is split by feature under src/touch/ (see touch_ui.h);
 * the widgets live under src/widgets/. */

#include <lvgl.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "modifier_indicator.h"
#include "wpm_meter.h"
#include "layer_display.h"
#include "battery_circles.h"
#include "output.h"
#include "touch_ui.h"

LOG_MODULE_REGISTER(prospector_touch_ui, LOG_LEVEL_INF);

static struct zmk_widget_modifier_indicator modifier_indicator_widget;
static struct zmk_widget_wpm_meter wpm_meter_widget;
static struct zmk_widget_layer_display layer_display_widget;
static struct zmk_widget_battery_circles battery_circles_widget;
static struct zmk_widget_output output_widget;

/* Position the status-screen widgets for the current orientation. All widgets
 * stay at their original full sizes -- earlier this trimmed the WPM meter,
 * battery, and output widgets down to fit battery UNDER output on landscape,
 * but that shrink wasn't wanted, so it's reverted (widgets are full-size again
 * in both orientations).
 *
 * Portrait (240x280) has slack to spare: battery UNDER output at full size
 * fits cleanly in the same two vertical slots the widgets already used pre-
 * reorder (152/216), just swapped which widget sits in which slot.
 *
 * Landscape (280x240) does not have the room: mod(24) + wpm(90) + layer(6) +
 * output(62) + battery(62) alone is 244px, already past the 240px screen
 * height with ZERO gaps between them, let alone margins -- stacking here is
 * not achievable without shrinking something, which was explicitly rejected.
 * So landscape keeps its original side-by-side arrangement. */
void status_screen_reflow(void) {
    if (ui_rot & 1) { /* portrait: 240x280 */
        lv_obj_set_pos(zmk_widget_modifier_indicator_obj(&modifier_indicator_widget), 5, 8);
        zmk_widget_wpm_meter_set_width(&wpm_meter_widget, 220);
        lv_obj_set_pos(zmk_widget_wpm_meter_obj(&wpm_meter_widget), 10, 40);
        zmk_widget_layer_display_set_width(&layer_display_widget, 220);
        lv_obj_set_pos(zmk_widget_layer_display_obj(&layer_display_widget), 10, 140);
        lv_obj_set_pos(zmk_widget_output_obj(&output_widget), 62, 152);
        lv_obj_set_pos(zmk_widget_battery_circles_obj(&battery_circles_widget), 54, 216);
    } else { /* landscape: 280x240 */
        lv_obj_set_pos(zmk_widget_modifier_indicator_obj(&modifier_indicator_widget), 25, 8);
        zmk_widget_wpm_meter_set_width(&wpm_meter_widget, 260);
        lv_obj_set_pos(zmk_widget_wpm_meter_obj(&wpm_meter_widget), 10, 42);
        zmk_widget_layer_display_set_width(&layer_display_widget, 260);
        lv_obj_set_pos(zmk_widget_layer_display_obj(&layer_display_widget), 10, 142);
        lv_obj_set_pos(zmk_widget_battery_circles_obj(&battery_circles_widget), 11, 170);
        lv_obj_set_pos(zmk_widget_output_obj(&output_widget), 148, 170);
    }
}

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, 255, LV_PART_MAIN);

    zmk_widget_modifier_indicator_init(&modifier_indicator_widget, screen);
    zmk_widget_wpm_meter_init(&wpm_meter_widget, screen);
    zmk_widget_layer_display_init(&layer_display_widget, screen);
    zmk_widget_battery_circles_init(&battery_circles_widget, screen);
    zmk_widget_output_init(&output_widget, screen);
    status_screen_reflow(); /* boot orientation is landscape (ui_rot 0) */

    touch_ui_attach(screen);

    return screen;
}
