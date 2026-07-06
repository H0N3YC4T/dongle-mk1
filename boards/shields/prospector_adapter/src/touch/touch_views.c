/* Per-view renderers for the touch UI: build_view() lays out the current screen
 * for the current orientation. Square grids (3x3, 4x4) keep their arrangement in
 * portrait; the 2x3 screens re-arrange to 3x2 (draw_cell_l / p23_pos); HOME uses
 * explicit row spans. */

#include "touch_ui.h"

const uint32_t fkeys[12] = {F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12};
const char *const fkey_lbls[12] = {"F1", "F2", "F3",  "F4",  "F5",  "F6",
                                   "F7", "F8", "F9", "F10", "F11", "F12"};

const uint32_t symbols[32] = {
    EXCL, AT,   HASH, DLLR, PRCNT, CARET, AMPS, STAR,
    LPAR, RPAR, MINUS, UNDER, EQUAL, PLUS, LBKT, RBKT,
    LBRC, RBRC, BSLH, PIPE, SEMI, COLON, SQT, DQT,
    COMMA, DOT, LT,  GT,  FSLH, QMARK, GRAVE, TILDE,
};
const char *const sym_lbls[32] = {
    "!", "@", "#",  "$", "%", "^", "&", "*",
    "(", ")", "-",  "_", "=", "+", "[", "]",
    "{", "}", "\\", "|", ";", ":", "'", "\"",
    ",", ".", "<",  ">", "/", "?", "`", "~",
};

void build_view(enum ui_view v) {
    lv_obj_clean(touch_overlay);
    if (v == VIEW_NUMPAD) {
        grid_rows = 4; grid_cols = 4; /* square-ish grids keep their layout in portrait */
    } else if (v == VIEW_FKEYS || v == VIEW_SYMBOLS || v == VIEW_SETTINGS) {
        grid_rows = 3; grid_cols = 3;
    } else if (ui_rot & 1) {
        grid_rows = 3; grid_cols = 2; /* 2x3 views re-arrange to 3x2 in portrait */
    } else {
        grid_rows = 2; grid_cols = 3;
    }

    switch (v) {
    case VIEW_HOME: /* icons: back / trackpad / settings / keys. Portrait: back spans
                     * the top row, trackpad|settings middle, keys spans the bottom
                     * (touch remap in touch_nav.c's logical_cell()). */
        if (ui_rot & 1) {
            draw_cell(0, 0, 2, LV_SYMBOL_UP, COLOR_BACK);
            draw_cell(1, 0, 1, LV_SYMBOL_GPS, COLOR_ACCENT);
            draw_cell(1, 1, 1, LV_SYMBOL_SETTINGS, COLOR_ACCENT);
            draw_cell(2, 0, 2, LV_SYMBOL_KEYBOARD, COLOR_ACCENT);
        } else {
            draw_cell(0, 0, 3, LV_SYMBOL_UP, COLOR_BACK);
            draw_cell(1, 0, 1, LV_SYMBOL_GPS, COLOR_ACCENT);
            draw_cell(1, 1, 1, LV_SYMBOL_SETTINGS, COLOR_ACCENT);
            draw_cell(1, 2, 1, LV_SYMBOL_KEYBOARD, COLOR_ACCENT);
        }
        break;
    case VIEW_MEDIA: /* logical cells; portrait pairs vol-|vol+ / back|play / prev|next */
        draw_cell_l(0, LV_SYMBOL_VOLUME_MID, COLOR_ACCENT);
        draw_cell_l(1, LV_SYMBOL_UP, COLOR_BACK);
        draw_cell_l(2, LV_SYMBOL_VOLUME_MAX, COLOR_ACCENT);
        draw_cell_l(3, LV_SYMBOL_PREV, COLOR_ACCENT);
        draw_cell_l(4, LV_SYMBOL_PLAY, COLOR_ACCENT);
        draw_cell_l(5, LV_SYMBOL_NEXT, COLOR_ACCENT);
        break;
    case VIEW_SETTINGS: {
        /* 3x3. One setting per outer column: left = trackpad sensitivity, right =
         * display brightness. Rows 0/1 = + and - (greyed once the value is at its
         * end stop); row 2 = icon + live-value readouts (blue, not keys). Middle
         * column: back / rotate / empty. Cells 0-5 keep the 2x3 handler numbering.
         * (No sun glyph exists in LVGL's built-in set; eye-open marks brightness.) */
        char lbl[20];
        int br = prospector_brightness_get();
        int sn = prospector_touchpad_sens_get();
        if (sn >= 0) {
            draw_cell(0, 0, 1, LV_SYMBOL_PLUS,
                      sn >= SETTINGS_SENS_MAX ? COLOR_HINT_GLYPH : COLOR_ACCENT);
            draw_cell(1, 0, 1, LV_SYMBOL_MINUS, sn <= 0 ? COLOR_HINT_GLYPH : COLOR_ACCENT);
        }
        draw_cell(0, 1, 1, LV_SYMBOL_UP, COLOR_BACK);
        draw_cell(1, 1, 1, LV_SYMBOL_REFRESH, COLOR_PAGE); /* rotate 90deg cw per tap */
        draw_cell(0, 2, 1, LV_SYMBOL_PLUS,
                  br >= SETTINGS_BRIGHT_MAX ? COLOR_HINT_GLYPH : COLOR_ACCENT);
        draw_cell(1, 2, 1, LV_SYMBOL_MINUS,
                  br <= SETTINGS_BRIGHT_MIN ? COLOR_HINT_GLYPH : COLOR_ACCENT);
        /* Readout row: the two boxes split the row 50/50 (1.5 grid units each),
         * sensitivity under its column, brightness under its. Drawn on a temporary
         * 2-column grid so the normal cell math applies; restored immediately.
         * Taps still resolve on the 3x3 grid -- row 2 is no-op cells either way. */
        grid_cols = 2;
        if (sn >= 0) {
            lv_snprintf(lbl, sizeof(lbl), LV_SYMBOL_GPS " %d", sn);
            draw_cell(2, 0, 1, lbl, COLOR_PAGE);
        }
        lv_snprintf(lbl, sizeof(lbl), LV_SYMBOL_EYE_OPEN " %d%%", br);
        draw_cell(2, 1, 1, lbl, COLOR_PAGE);
        grid_cols = 3;
        break;
    }
    case VIEW_HUB: /* hub: 0 F-keys, 1 Back, 2 numpad, 3 symbols, 4 media, 5 mods.
                    * No LVGL glyphs exist for F-keys/modifiers -> short text. */
        draw_cell_l(0, "Fn", COLOR_ACCENT);
        draw_cell_l(1, LV_SYMBOL_UP, COLOR_BACK);
        draw_cell_l(2, "123", COLOR_ACCENT);
        draw_cell_l(3, "#$%", COLOR_ACCENT);
        draw_cell_l(4, LV_SYMBOL_AUDIO, COLOR_ACCENT);
        draw_cell_l(5, "MOD", COLOR_ACCENT);
        break;
    case VIEW_FKEYS:
        draw_key_page(fkey_lbls, 12, cur_page);
        break;
    case VIEW_NUMPAD: /* 4x4 calc grid; ops (blue) in col 3; cell 12 = Back, 14 = Enter */
        draw_cell(0, 0, 1, "7", COLOR_ACCENT);
        draw_cell(0, 1, 1, "8", COLOR_ACCENT);
        draw_cell(0, 2, 1, "9", COLOR_ACCENT);
        draw_cell(0, 3, 1, "+", COLOR_PAGE);
        draw_cell(1, 0, 1, "4", COLOR_ACCENT);
        draw_cell(1, 1, 1, "5", COLOR_ACCENT);
        draw_cell(1, 2, 1, "6", COLOR_ACCENT);
        draw_cell(1, 3, 1, "-", COLOR_PAGE);
        draw_cell(2, 0, 1, "1", COLOR_ACCENT);
        draw_cell(2, 1, 1, "2", COLOR_ACCENT);
        draw_cell(2, 2, 1, "3", COLOR_ACCENT);
        draw_cell(2, 3, 1, "*", COLOR_PAGE);
        draw_cell(3, 0, 1, LV_SYMBOL_UP, COLOR_BACK);
        draw_cell(3, 1, 1, "0", COLOR_ACCENT);
        draw_cell(3, 2, 1, LV_SYMBOL_NEW_LINE, COLOR_PAGE);
        draw_cell(3, 3, 1, "/", COLOR_PAGE);
        break;
    case VIEW_SYMBOLS:
        draw_key_page(sym_lbls, 32, cur_page);
        break;
    case VIEW_MODIFIERS: /* one-shot mods; armed = solid blue fill, inactive = outline */
        if (pending_mods & MOD_LCTL) { draw_cell_on_l(0, "CTRL", COLOR_PAGE); }
        else                         { draw_cell_l(0, "CTRL", COLOR_ACCENT); }
        draw_cell_l(1, LV_SYMBOL_UP, COLOR_BACK);
        if (pending_mods & MOD_LSFT) { draw_cell_on_l(2, "SHFT", COLOR_PAGE); }
        else                         { draw_cell_l(2, "SHFT", COLOR_ACCENT); }
        if (pending_mods & MOD_LALT) { draw_cell_on_l(3, "ALT", COLOR_PAGE); }
        else                         { draw_cell_l(3, "ALT", COLOR_ACCENT); }
        if (pending_mods & MOD_LGUI) { draw_cell_on_l(5, "GUI", COLOR_PAGE); }
        else                         { draw_cell_l(5, "GUI", COLOR_ACCENT); }
        break;
    case VIEW_TRACKPAD: {
        /* Whole screen = trackpad (gestures in touch_input.c): drag = move pointer,
         * 1 tap = L-click, 2 taps = R-click, top-left corner = exit. Scroll lane =
         * logical coord >= 240 along the long axis, drawn flush to the edge:
         * right-side vertical strip in landscape, bottom horizontal strip in
         * portrait (swipe right = scroll down). */
        lv_obj_t *lane = lv_obj_create(touch_overlay);
        if (lane != NULL) {
            if (ui_rot & 1) {
                lv_obj_set_size(lane, scr_w(), scr_h() - 240);
                lv_obj_set_pos(lane, 0, 240);
            } else {
                lv_obj_set_size(lane, scr_w() - 240, scr_h());
                lv_obj_set_pos(lane, 240, 0);
            }
            lv_obj_set_style_bg_color(lane, lv_color_hex(COLOR_LANE_BG), LV_PART_MAIN);
            lv_obj_set_style_bg_opa(lane, LV_OPA_COVER, LV_PART_MAIN);
            lv_obj_set_style_border_color(lane, lv_color_hex(COLOR_LANE_EDGE), LV_PART_MAIN);
            lv_obj_set_style_border_width(lane, 1, LV_PART_MAIN);
            lv_obj_set_style_radius(lane, 0, LV_PART_MAIN);
            lv_obj_set_style_pad_all(lane, 0, LV_PART_MAIN);
        }
        lv_obj_t *ex = lv_label_create(touch_overlay);
        if (ex != NULL) {
            lv_label_set_text(ex, LV_SYMBOL_CLOSE); /* X: exit the pad, not a nav level */
            lv_obj_set_style_text_font(ex, &lv_font_montserrat_20, LV_PART_MAIN);
            lv_obj_set_style_text_color(ex, lv_color_hex(COLOR_BACK), LV_PART_MAIN);
            lv_obj_set_pos(ex, 16, 12); /* corner-exit affordance, clear of the glass arc */
        }
        lv_obj_t *hint = lv_label_create(touch_overlay);
        if (hint != NULL) {
            lv_label_set_text(hint, "TRACKPAD");
            lv_obj_set_style_text_font(hint, &lv_font_montserrat_20, LV_PART_MAIN);
            lv_obj_set_style_text_color(hint, lv_color_hex(COLOR_HINT), LV_PART_MAIN);
            /* centred on the pad area (excluding the lane) */
            if (ui_rot & 1) {
                lv_obj_align(hint, LV_ALIGN_CENTER, 0, -20);
            } else {
                lv_obj_align(hint, LV_ALIGN_CENTER, -20, 0);
            }
        }
        break;
    }
    default:
        break;
    }

    /* Armed one-shot modifier -> blue frame visible from any key page. Drawn as its
     * own rounded-rect child (radius = glass) so it follows the screen's physical
     * corners. Cleaned up with the rest of the view. */
    if (pending_mods) {
        lv_obj_t *frame = lv_obj_create(touch_overlay);
        if (frame != NULL) {
            lv_obj_set_size(frame, scr_w(), scr_h());
            lv_obj_set_pos(frame, 0, 0);
            lv_obj_set_style_bg_opa(frame, LV_OPA_TRANSP, LV_PART_MAIN);
            lv_obj_set_style_border_color(frame, lv_color_hex(COLOR_PAGE), LV_PART_MAIN);
            lv_obj_set_style_border_width(frame, 3, LV_PART_MAIN);
            lv_obj_set_style_radius(frame, GLASS_RADIUS, LV_PART_MAIN);
            lv_obj_set_style_pad_all(frame, 0, LV_PART_MAIN);
        }
    }
}
