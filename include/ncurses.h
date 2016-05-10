/*
 *   ncurses headers file.
 *   Copyright © 2016 Damien Riquet <d.riquet@gmail.com>
 *   All right reserved.
 *
 *   See LICENCE file.
 */

#ifndef __NCURSES_H__
#define __NCURSES_H__

#include "context.h"

#define CLOCKVIEW_WIDTH  35
#define CLOCKVIEW_HEIGHT 10
#define POMOVIEW_WIDTH  30
#define POMOVIEW_HEIGHT 10
#define TOTAL_WIDTH (CLOCKVIEW_WIDTH + POMOVIEW_WIDTH)
#define TOTAL_HEIGHT (CLOCKVIEW_HEIGHT + POMOVIEW_HEIGHT)

#define TIME_STR     "Time"
#define ELAPSED_STR  "Elapsed"
#define LEFT_STR     "Left"
#define SPACING      2

/*!
 * \brief Initialize the ncurses context
 *
 * \param ctx ncurses context
 */
void init_ncurses(ttymato_curses_t *ctx);

/*!
 * \brief Update the ncurses (draw stuff)
 *
 * \param ctx ncurses context
 * \param pomodoro pomodoro context
 * \param options ttymato options
 */
void tick_ncurses(ttymato_curses_t *ctx, ttymato_pomodoro_t *pomodoro, ttymato_options_t *options);

/*!
 * \brief Draw the ttymato using ncurses
 *
 * \param ctx ncurses context
 * \param pomodoro pomodoro context
 * \param options ttymato options
 */
void draw_ncurses(ttymato_curses_t *ctx, ttymato_pomodoro_t *pomodoro, ttymato_options_t *options);

/*!
 * \brief Draw number
 *
 * \param ctx ncurses context
 * \param n number
 * \param x col
 * \param y line
 */
void draw_number(ttymato_curses_t *ctx, int n, int x, int y);

#endif
