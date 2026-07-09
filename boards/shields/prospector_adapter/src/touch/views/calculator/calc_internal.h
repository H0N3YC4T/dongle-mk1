#ifndef CALC_INTERNAL_H
#define CALC_INTERNAL_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "../../touch_ui.h"

extern char calc_expr[40];
extern const char *calc_cp;
extern bool eval_okay;
extern bool calc_shown;
extern bool bracket_open;

extern bool bin_mode;
extern bool bin_converted;
extern int bin_word_size;

extern const struct page_cell calc_cells[];
extern const struct page_cell calc_cells_alt[];
extern const struct page_cell calc_cells_alt2[];

void calc_push(char ch);
void calc_tap_backspace(void);
void calc_ensure_binary(void);
void calc_tap_eval(void);
void calc_update_display(void);
float parse_expr(void);

// Decimal and shared callbacks
void tap_calc_0(int cell);
void tap_calc_1(int cell);
void tap_calc_2(int cell);
void tap_calc_3(int cell);
void tap_calc_4(int cell);
void tap_calc_5(int cell);
void tap_calc_6(int cell);
void tap_calc_7(int cell);
void tap_calc_8(int cell);
void tap_calc_9(int cell);
void tap_calc_10(int cell);
void tap_calc_11(int cell);
void tap_calc_13(int cell);
void tap_calc_14(int cell);
void tap_calc_15(int cell);
void tap_calc_backspace_cb(int cell);
void tap_calc_dot(int cell);
void tap_calc_bracket(int cell);
void tap_calc_percent(int cell);
void tap_calc_factorial(int cell);
void tap_calc_to_bin(int cell);
void tap_calc_to_dec(int cell);
void tap_calc_to_page2(int cell);

#endif
