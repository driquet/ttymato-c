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

#define TIME_STR     " Time "
#define ELAPSED_STR  " Elapsed "
#define LEFT_STR     " Left "
#define SPACING      2

/*!
 * \brief Initialize the ncurses context
 */
void init_ncurses();

/*!
 * \brief Update the ncurses (draw stuff)
 */
void tick_ncurses();

/*!
 * \brief Draw the ttymato using ncurses
 */
void draw_ncurses();

/*!
 * \brief Draw number
 *
 * \param n number
 * \param x col
 * \param y line
 */
void draw_number(int n, int x, int y);

#endif
