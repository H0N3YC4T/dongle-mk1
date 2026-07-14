/* ------------------------------- CLIPBOARD --------------------------------- */
/* editing shortcuts; reached by holding the PAD button on HOME */

#include "../touch_ui.h"

static const struct page_cell clipboard_cells[] = {
    {0, 0, 1, 1, "UNDO",              NULL, COLOR_PRIMARY, ACT_SEND_KEY, .arg.keycode = LC(Z)},
    {0, 1, 1, 1, NULL,        &icon_up, COLOR_RED,     ACT_GO_VIEW,  .arg.view = &view_home},
    {0, 2, 1, 1, "REDO",              NULL, COLOR_PRIMARY, ACT_SEND_KEY, .arg.keycode = LC(Y)},

    {1, 0, 1, 1, NULL,       &icon_cut, COLOR_PRIMARY, ACT_SEND_KEY, .arg.keycode = LC(X)},
    {1, 1, 1, 1, NULL,     &icon_paste, COLOR_PRIMARY, ACT_SEND_KEY, .arg.keycode = LC(V)},
    {1, 2, 1, 1, NULL,      &icon_copy, COLOR_PRIMARY, ACT_SEND_KEY, .arg.keycode = LC(C)},

    {2, 0, 1, 1, NULL,                NULL, COLOR_PRIMARY, ACT_NONE},
    {2, 1, 1, 1, NULL,  &icon_newline, COLOR_ACCENT,  ACT_SEND_KEY, .arg.keycode = RETURN},
    {2, 2, 1, 1, NULL,                NULL, COLOR_PRIMARY, ACT_NONE},
    {0}
};

const struct view_def view_clipboard = {
    .cells = clipboard_cells,
    .keeps_mods = true,
};
