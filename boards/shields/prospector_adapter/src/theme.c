/* Runtime UI theme: role -> colour lookup + category base swapping. */

#include "theme.h"

/* Classic palette; the default build renders pixel-identical to the
 * pre-theme hardcoded colours. */
static const uint32_t theme_classic[THEME_ROLE_COUNT] = {
    [THEME_PRIMARY]          = 0xc8a2c8,
    [THEME_PRIMARY_DIM]      = 0x242424,
    [THEME_SECONDARY]        = 0xa8d0e6,
    [THEME_SECONDARY_BRIGHT] = 0xb1e5f0,
    [THEME_SECONDARY_DIM]    = 0x3b527c,
    [THEME_FOCUS]            = 0xf5e08c,
    [THEME_FOCUS_BRIGHT]     = 0xffbf00,
    [THEME_ACCEPT]           = 0xa8e6b8,
    [THEME_DENY]             = 0xc2526a,
    [THEME_BACKGROUND]       = 0x000000,
    [THEME_SURFACE]          = 0x101216,
    [THEME_SURFACE_LOW]      = 0x0b0d10,
    [THEME_OUTLINE]          = 0x2e3238,
    [THEME_MUTED]            = 0x505050,
    [THEME_MUTED_DIM]        = 0x303030,
};

/* every role follows exactly one category base */
static const enum theme_category role_cat[THEME_ROLE_COUNT] = {
    [THEME_PRIMARY]          = THEME_CAT_PRIMARY,
    [THEME_PRIMARY_DIM]      = THEME_CAT_PRIMARY,
    [THEME_SECONDARY]        = THEME_CAT_SECONDARY,
    [THEME_SECONDARY_BRIGHT] = THEME_CAT_SECONDARY,
    [THEME_SECONDARY_DIM]    = THEME_CAT_SECONDARY,
    [THEME_FOCUS]            = THEME_CAT_FOCUS,
    [THEME_FOCUS_BRIGHT]     = THEME_CAT_FOCUS,
    [THEME_ACCEPT]           = THEME_CAT_ACCEPT,
    [THEME_DENY]             = THEME_CAT_DENY,
    [THEME_BACKGROUND]       = THEME_CAT_BACKGROUND,
    [THEME_SURFACE]          = THEME_CAT_BACKGROUND,
    [THEME_SURFACE_LOW]      = THEME_CAT_BACKGROUND,
    [THEME_OUTLINE]          = THEME_CAT_BACKGROUND,
    [THEME_MUTED]            = THEME_CAT_BACKGROUND,
    [THEME_MUTED_DIM]        = THEME_CAT_BACKGROUND,
};

/* the role holding each category's base colour */
static const enum theme_role cat_base[THEME_CAT_COUNT] = {
    [THEME_CAT_PRIMARY]    = THEME_PRIMARY,
    [THEME_CAT_SECONDARY]  = THEME_SECONDARY,
    [THEME_CAT_FOCUS]      = THEME_FOCUS,
    [THEME_CAT_ACCEPT]     = THEME_ACCEPT,
    [THEME_CAT_DENY]       = THEME_DENY,
    [THEME_CAT_BACKGROUND] = THEME_BACKGROUND,
};

static uint32_t palette[THEME_ROLE_COUNT] = {
    [THEME_PRIMARY]          = 0xc8a2c8,
    [THEME_PRIMARY_DIM]      = 0x242424,
    [THEME_SECONDARY]        = 0xa8d0e6,
    [THEME_SECONDARY_BRIGHT] = 0xb1e5f0,
    [THEME_SECONDARY_DIM]    = 0x3b527c,
    [THEME_FOCUS]            = 0xf5e08c,
    [THEME_FOCUS_BRIGHT]     = 0xffbf00,
    [THEME_ACCEPT]           = 0xa8e6b8,
    [THEME_DENY]             = 0xc2526a,
    [THEME_BACKGROUND]       = 0x000000,
    [THEME_SURFACE]          = 0x101216,
    [THEME_SURFACE_LOW]      = 0x0b0d10,
    [THEME_OUTLINE]          = 0x2e3238,
    [THEME_MUTED]            = 0x505050,
    [THEME_MUTED_DIM]        = 0x303030,
};

uint32_t theme_color(enum theme_role role) {
    return (role < THEME_ROLE_COUNT) ? palette[role] : 0xffffff;
}

uint32_t theme_get_base(enum theme_category cat) {
    return (cat < THEME_CAT_COUNT) ? palette[cat_base[cat]] : 0xffffff;
}

static uint8_t shift_channel(uint8_t base, uint8_t ref_base, uint8_t ref_shade) {
    int v = (int)base + ((int)ref_shade - (int)ref_base);
    return (uint8_t)(v < 0 ? 0 : v > 255 ? 255 : v);
}

/* shade = new base + (classic shade - classic base), per RGB channel */
static uint32_t shift_shade(uint32_t base, uint32_t ref_base, uint32_t ref_shade) {
    uint32_t r = shift_channel(base >> 16, ref_base >> 16, ref_shade >> 16);
    uint32_t g = shift_channel(base >> 8, ref_base >> 8, ref_shade >> 8);
    uint32_t b = shift_channel(base, ref_base, ref_shade);
    return (r << 16) | (g << 8) | b;
}

void theme_set_base(enum theme_category cat, uint32_t base_rgb) {
    if (cat >= THEME_CAT_COUNT) {
        return;
    }
    uint32_t ref_base = theme_classic[cat_base[cat]];
    for (int r = 0; r < THEME_ROLE_COUNT; r++) {
        if (role_cat[r] != cat) {
            continue;
        }
        palette[r] = (r == cat_base[cat]) ? base_rgb
                                          : shift_shade(base_rgb, ref_base, theme_classic[r]);
    }
}
