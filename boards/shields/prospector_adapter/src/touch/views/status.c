/* ------------------------------- STATUS ----------------------------------- */

#include "../touch_ui.h"

static const struct page_cell normal_cells[] = {
    {0, 0, 1, 1, NULL, NULL, 0, ACT_GO_VIEW, .arg.view = &view_home},
    {0}};

const struct view_def view_normal = {
    .cells = normal_cells,
};
