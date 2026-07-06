/* Tap routing, view state, and the idle timeout for the touch UI. Owns the
 * full-screen overlay and the display-thread timer that drains taps. */

#include "touch_ui.h"

lv_obj_t *touch_overlay;
enum ui_view cur_view = VIEW_NORMAL;
int cur_page;

static atomic_t pending_tap = ATOMIC_INIT(0); /* 0 = none, else (bit20 | sx<<10 | sy) */
static int64_t last_tap_ms;

/* Weak fallbacks: the trackpad sensitivity lives in touch_input.c; get() < 0 means
 * the driver is absent, so the settings sensitivity cells are simply not drawn. */
__weak int prospector_touchpad_sens_get(void) { return -1; }
__weak void prospector_touchpad_sens_step(int delta) { ARG_UNUSED(delta); }

/* Called from the touch workqueue thread. Stash the packed tap coords; the lv_timer
 * drains + maps them to a cell on the display thread (no LVGL off-thread). Returns
 * true = the on-screen UI owns the tap. */
bool prospector_touch_tap(int sx, int sy) {
    atomic_set(&pending_tap, (1 << 20) | ((sx & 0x3FF) << 10) | (sy & 0x3FF));
    return true;
}

/* touch_input.c streams pointer motion/clicks (instead of cell taps) while true. */
bool prospector_touchpad_active(void) {
    return cur_view == VIEW_TRACKPAD;
}

/* Map raw rendered-screen coords to a cell in the CURRENT screen's grid. */
static int cell_from_coords(int sx, int sy) {
    int cw = scr_w() / grid_cols;
    int col = cw > 0 ? sx / cw : 0;
    if (col > grid_cols - 1) { col = grid_cols - 1; }
    if (col < 0) { col = 0; }
    int rh = scr_h() / grid_rows;
    int row = rh > 0 ? sy / rh : 0;
    if (row > grid_rows - 1) { row = grid_rows - 1; }
    if (row < 0) { row = 0; }
    return row * grid_cols + col;
}

void show_view(enum ui_view v) {
    cur_view = v;
    cur_page = 0;
    /* Leaving the macro hub abandons any armed one-shot modifier, so it can't
     * silently apply to normal typing later (e.g. armed CTRL -> a stray Ctrl+A). */
    if (v < VIEW_HUB) {
        pending_mods = 0;
    }
    if (v == VIEW_NORMAL) {
        lv_obj_add_flag(touch_overlay, LV_OBJ_FLAG_HIDDEN);
    } else {
        build_view(v);
        lv_obj_clear_flag(touch_overlay, LV_OBJ_FLAG_HIDDEN);
    }
}

/* Nav + key handling shared by all paginated key screens. */
static void handle_key_page(const uint32_t *keys, int n, int cell) {
    int pages = (n + KEYS_PER_PAGE - 1) / KEYS_PER_PAGE;
    if (cell == 1) {
        if (cur_page == 0) {
            show_view(VIEW_HUB);
        } else {
            cur_page--;
            build_view(cur_view);
        }
        return;
    }
    if (cell == 7) {
        if (pages > 1) {
            cur_page = (cur_page + 1) % pages;
            build_view(cur_view);
        }
        return;
    }
    for (int i = 0; i < KEYS_PER_PAGE; i++) {
        if (key_cells[i] == cell) {
            int idx = cur_page * KEYS_PER_PAGE + i;
            if (idx < n) {
                send_key(keys[idx]);
            }
            return;
        }
    }
}

static void handle_tap(int cell) {
    last_tap_ms = k_uptime_get();

    switch (cur_view) {
    case VIEW_NORMAL:
        show_view(VIEW_HOME);
        break;
    case VIEW_HOME:
        if (cell <= 2) {
            show_view(VIEW_NORMAL);
        } else if (cell == 3) {
            show_view(VIEW_TRACKPAD);
        } else if (cell == 4) {
            show_view(VIEW_SETTINGS);
        } else {
            show_view(VIEW_HUB);
        }
        break;
    case VIEW_MEDIA:
        switch (cell) {
        case 0: fire_macro("touch_macro_0"); break;
        case 1: show_view(VIEW_HUB); break; /* media is reached from the hub */
        case 2: fire_macro("touch_macro_2"); break;
        case 3: fire_macro("touch_macro_4"); break;
        case 4: fire_macro("touch_macro_3"); break;
        case 5: fire_macro("touch_macro_5"); break;
        default: break;
        }
        break;
    case VIEW_SETTINGS: /* 3x3; cells 6-8 are the readout row -> no-ops via default */
        switch (cell) {
        case 0: prospector_touchpad_sens_step(+1); build_view(VIEW_SETTINGS); break;
        case 1: show_view(VIEW_HOME); break;
        case 2: prospector_brightness_step(+BRIGHTNESS_STEP); build_view(VIEW_SETTINGS); break;
        case 3: prospector_touchpad_sens_step(-1); build_view(VIEW_SETTINGS); break;
        case 4: ui_rot = (ui_rot + 1) & 3; settings_apply_rotation(); break; /* +90deg CW */
        case 5: prospector_brightness_step(-BRIGHTNESS_STEP); build_view(VIEW_SETTINGS); break;
        default: break;
        }
        break;
    case VIEW_HUB:
        switch (cell) {
        case 0: show_view(VIEW_FKEYS); break;
        case 1: show_view(VIEW_HOME); break;
        case 2: show_view(VIEW_NUMPAD); break;
        case 3: show_view(VIEW_SYMBOLS); break;
        case 4: show_view(VIEW_MEDIA); break;
        case 5: show_view(VIEW_MODIFIERS); break;
        default: break;
        }
        break;
    case VIEW_FKEYS:
        handle_key_page(fkeys, 12, cell);
        break;
    case VIEW_SYMBOLS:
        handle_key_page(symbols, 32, cell);
        break;
    case VIEW_NUMPAD: {
        /* 4x4; col 3 = operators. Index 12 (Back) is 0 and handled first, so the
         * np[cell] guard only skips it -- every real key code is non-zero. */
        static const uint32_t np[16] = {N7, N8, N9, PLUS,  N4, N5, N6, MINUS,
                                        N1, N2, N3, STAR,  0,  N0, RET, FSLH};
        if (cell == 12) {
            show_view(VIEW_HUB);
        } else if (cell >= 0 && cell <= 15 && np[cell]) {
            send_key(np[cell]);
        }
        break;
    }
    case VIEW_MODIFIERS:
        switch (cell) {
        case 0: pending_mods ^= MOD_LCTL; build_view(VIEW_MODIFIERS); break;
        case 1: show_view(VIEW_HUB); break;
        case 2: pending_mods ^= MOD_LSFT; build_view(VIEW_MODIFIERS); break;
        case 3: pending_mods ^= MOD_LALT; build_view(VIEW_MODIFIERS); break;
        case 5: pending_mods ^= MOD_LGUI; build_view(VIEW_MODIFIERS); break;
        default: break;
        }
        break;
    case VIEW_TRACKPAD:
        /* touch_input.c only forwards the corner-exit tap here; the trackpad is
         * entered from HOME, so leave back to HOME. */
        show_view(VIEW_HOME);
        break;
    default:
        break;
    }
}

/* Portrait 3x2 tap -> the 2x3 logical cell id the handlers use. Square grids,
 * settings, and the trackpad are identity (their layouts don't re-arrange). */
static int logical_cell(int cell) {
    if (!(ui_rot & 1) || cell < 0 || cell > 5) {
        return cell;
    }
    switch (cur_view) {
    case VIEW_HOME: { /* back spans row 0, keys spans row 2 */
        static const uint8_t m[6] = {0, 0, 3, 4, 5, 5};
        return m[cell];
    }
    case VIEW_MEDIA:
    case VIEW_HUB:
    case VIEW_MODIFIERS:
        return p23_pos[cell];
    default:
        return cell;
    }
}

/* Runs on the LVGL/display thread (lv_timer_handler). */
static void ui_timer_cb(lv_timer_t *timer) {
    ARG_UNUSED(timer);

    int v = atomic_set(&pending_tap, 0);
    if (v & (1 << 20)) {
        int sx = (v >> 10) & 0x3FF, sy = v & 0x3FF;
        handle_tap(logical_cell(cell_from_coords(sx, sy)));
    }

    /* Idle timeout returns to Normal -- but NOT while inside the macro hub. */
    if (cur_view != VIEW_NORMAL && cur_view < VIEW_HUB &&
        (k_uptime_get() - last_tap_ms) > TOUCH_TIMEOUT_MS) {
        show_view(VIEW_NORMAL);
    }
}

/* Create the (hidden) full-screen overlay + the tap-drain timer. Called once from
 * zmk_display_status_screen() in status_screen.c. */
void touch_ui_attach(lv_obj_t *screen) {
    touch_overlay = lv_obj_create(screen);
    lv_obj_set_size(touch_overlay, SCR_W, SCR_H);
    lv_obj_set_pos(touch_overlay, 0, 0);
    lv_obj_set_style_bg_color(touch_overlay, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(touch_overlay, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(touch_overlay, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(touch_overlay, 0, LV_PART_MAIN);
    lv_obj_add_flag(touch_overlay, LV_OBJ_FLAG_HIDDEN);

    last_tap_ms = k_uptime_get();
    lv_timer_create(ui_timer_cb, 30, NULL);
}
