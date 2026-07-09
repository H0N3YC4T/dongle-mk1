/* --------------------------------- MACROS ---------------------------------- */
/* User macro pad: up to 5 buttons bound in the consuming keyboard's keymap via a
 * zmk,prospector-touch-pad node (standard binding syntax -- &kp, &bt, macros).
 * Binding order = cells 0,2,3,4,5. Unbound cells draw greyed, tap = no-op.
 * Faces match the keyboard repo's bindings: $_ terminal / LIST task manager /
 * WIFI browser / EYE_CLOSE show desktop / EDIT notes. */

#include "../touch_ui.h"

static void tap_pad_0(int cell) { ARG_UNUSED(cell); fire_pad(0); }
static void tap_pad_1(int cell) { ARG_UNUSED(cell); fire_pad(1); }
static void tap_pad_2(int cell) { ARG_UNUSED(cell); fire_pad(2); }
static void tap_pad_3(int cell) { ARG_UNUSED(cell); fire_pad(3); }
static void tap_pad_4(int cell) { ARG_UNUSED(cell); fire_pad(4); }

static const struct page_cell pad_cells[] = {
    {0, 0, 1, 1, "$_", NULL, COLOR_PRIMARY, ACT_CUSTOM, .arg.func = tap_pad_0},
    {0, 1, 1, 1, LV_SYMBOL_UP, NULL, COLOR_RED, ACT_GO_VIEW, .arg.view = &view_home},
    {0, 2, 1, 1, LV_SYMBOL_LIST, NULL, COLOR_PRIMARY, ACT_CUSTOM, .arg.func = tap_pad_1},
    {1, 0, 1, 1, LV_SYMBOL_WIFI, NULL, COLOR_PRIMARY, ACT_CUSTOM, .arg.func = tap_pad_2},
    {1, 1, 1, 1, LV_SYMBOL_EYE_CLOSE, NULL, COLOR_PRIMARY, ACT_CUSTOM, .arg.func = tap_pad_3},
    {1, 2, 1, 1, LV_SYMBOL_EDIT, NULL, COLOR_PRIMARY, ACT_CUSTOM, .arg.func = tap_pad_4},
    {0}
};

static const struct page_cell pad_cells_portrait[] = {
    {0, 0, 1, 1, "$_", NULL, COLOR_PRIMARY, ACT_CUSTOM, .arg.func = tap_pad_0},
    {0, 1, 1, 1, LV_SYMBOL_LIST, NULL, COLOR_PRIMARY, ACT_CUSTOM, .arg.func = tap_pad_1},
    {1, 0, 1, 1, LV_SYMBOL_UP, NULL, COLOR_RED, ACT_GO_VIEW, .arg.view = &view_home},
    {1, 1, 1, 1, LV_SYMBOL_EYE_CLOSE, NULL, COLOR_PRIMARY, ACT_CUSTOM, .arg.func = tap_pad_3},
    {2, 0, 1, 1, LV_SYMBOL_WIFI, NULL, COLOR_PRIMARY, ACT_CUSTOM, .arg.func = tap_pad_2},
    {2, 1, 1, 1, LV_SYMBOL_EDIT, NULL, COLOR_PRIMARY, ACT_CUSTOM, .arg.func = tap_pad_4},
    {0}
};

static void build_pad(void)
{
  int n = touch_pad_count();
  int pads[] = {0, 2, 3, 4, 5}; // The indices of the pad buttons in pad_cells
  if (ui_rot & 1) {
      pads[0] = 0; pads[1] = 1; pads[2] = 4; pads[3] = 3; pads[4] = 5; // Portrait indices
  }
  
  for (int i = 0; i < 5; i++) {
      if (cur_view_btns[pads[i]]) {
          lv_obj_set_style_border_color(cur_view_btns[pads[i]], lv_color_hex(i < n ? COLOR_PRIMARY : COLOR_GREY), LV_PART_MAIN);
          lv_obj_t *l = lv_obj_get_child(cur_view_btns[pads[i]], 0);
          if (l) lv_obj_set_style_text_color(l, lv_color_hex(i < n ? COLOR_PRIMARY : COLOR_GREY), LV_PART_MAIN);
      }
  }
}

const struct view_def view_pad = {
    .cells = pad_cells,
    .cells_portrait = pad_cells_portrait,
    .build = build_pad,
};
