/* -------------------------------- HOME ------------------------------------ */
/* 3x3, every screen one tap away (the HUB sub-menu is gone). Cell 7 = the user
 * macro pad (keyboard icon; greyed if the consuming keyboard binds nothing). */

#include <zephyr/device.h>
#include "../touch_ui.h"



static void tap_home_pad(int cell)
{
  if (touch_pad_count() > 0)
  {
    show_view(&view_pad);
  }
}

static const struct page_cell home_cells[] = {
    {0, 0, 1, 1, "Fn",               &icon_fkeys,    COLOR_PRIMARY, ACT_GO_VIEW, .arg.view = &view_fkeys},
    {0, 1, 1, 1, LV_SYMBOL_UP,       NULL,           COLOR_RED,     ACT_GO_VIEW, .arg.view = &view_normal},
    {0, 2, 1, 1, "123",              &icon_numpad,   COLOR_PRIMARY, ACT_GO_VIEW, .arg.view = &view_numpad},
    {1, 0, 1, 1, "#$%",              &icon_symbols,  COLOR_PRIMARY, ACT_GO_VIEW, .arg.view = &view_symbols},
    {1, 1, 1, 1, LV_SYMBOL_SETTINGS, NULL,           COLOR_PRIMARY, ACT_GO_VIEW, .arg.view = &view_settings},
    {1, 2, 1, 1, LV_SYMBOL_GPS,      &icon_trackpad, COLOR_PRIMARY, ACT_GO_VIEW, .arg.view = &view_trackpad},
    {2, 0, 1, 1, "MOD",              &icon_modkeys,  COLOR_PRIMARY, ACT_GO_VIEW, .arg.view = &view_modifiers},
    {2, 1, 1, 1, LV_SYMBOL_KEYBOARD, NULL,           COLOR_PRIMARY, ACT_CUSTOM,  .arg.func = tap_home_pad},
    {2, 2, 1, 1, LV_SYMBOL_AUDIO,    NULL,           COLOR_PRIMARY, ACT_GO_VIEW, .arg.view = &view_media},
    {0}
};

static void build_home(void)
{
  /* The layout was drawn declaratively. Now, dynamically grey out the PAD button 
   * (cell index 7) if there are no pads bound. */
  if (touch_pad_count() == 0 && cur_view_btns[7] != NULL)
  {
    lv_obj_set_style_bg_color(cur_view_btns[7], lv_color_hex(COLOR_GREY), LV_PART_MAIN);
    lv_obj_t *label = lv_obj_get_child(cur_view_btns[7], 0);
    if (label != NULL)
    {
      lv_obj_set_style_text_color(label, lv_color_hex(COLOR_GREY), LV_PART_MAIN);
    }
  }
}

/* Long-press shortcuts on HOME: hold 123 -> calculator, hold settings -> the
 * dongle's bootloader (fires ZMK's built-in reset behavior by its DT name, so it
 * does the right thing for whatever bootloader the board is configured with). */
static void hold_home(int cell)
{
  if (cell == 2)
  {
    show_view(&view_calc);
  }
  else if (cell == 4)
  {
#if DT_NODE_EXISTS(DT_NODELABEL(bootloader))
    fire_macro(DEVICE_DT_NAME(DT_NODELABEL(bootloader)));
#endif
  }
  else
  {
    tap_declarative(cell); /* other cells: a hold is just a slow tap */
  }
}

const struct view_def view_home = {
    .cells = home_cells,
    .build = build_home,
    .keeps_mods = true,
    .on_hold = hold_home,
};
