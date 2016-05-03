/*
 *   ttymato headers file.
 *   Copyright © 2016 Damien Riquet <d.riquet@gmail.com>
 *   All right reserved.
 *
 *   See LICENCE file.
 */

#ifndef __TTYMATO_H__
#define __TTYMATO_H__

#include <assert.h>
#include <ncurses.h>
#include <getopt.h>
#include <signal.h>

#include "pomodoro.h"
#include "options.h"


/* ttymato curses config
 */
#define CLOCKVIEW_WIDTH  35
#define CLOCKVIEW_HEIGHT 10
#define POMOVIEW_WIDTH  30
#define POMOVIEW_HEIGHT 10
#define TOTAL_WIDTH (CLOCKVIEW_WIDTH + POMOVIEW_WIDTH)
#define TOTAL_HEIGHT (CLOCKVIEW_HEIGHT + POMOVIEW_HEIGHT)

typedef enum { TIME, ELAPSED, LEFT } pomodoro_clock_display_t;

typedef struct
{
	WINDOW                    *clockwin;
	WINDOW                    *pomowin;
	pomodoro_clock_display_t  display;
	unsigned int              digits[4];
} ttymato_curses_t;

/* ttymato global config
 */
typedef struct
{
	bool                running;
	ttymato_pomodoro_t  pomodoro;
	ttymato_options_t   options;
	ttymato_curses_t    curses;
} ttymato_config_t;

ttymato_config_t *g_ttymato_config;

void init(void);
void tick(void);
void signal_handler(int signal);


/* Number matrix */
const bool numbers[][15] =
{
	{1,1,1,1,0,1,1,0,1,1,0,1,1,1,1}, /* 0 */
	{0,0,1,0,0,1,0,0,1,0,0,1,0,0,1}, /* 1 */
	{1,1,1,0,0,1,1,1,1,1,0,0,1,1,1}, /* 2 */
	{1,1,1,0,0,1,1,1,1,0,0,1,1,1,1}, /* 3 */
	{1,0,1,1,0,1,1,1,1,0,0,1,0,0,1}, /* 4 */
	{1,1,1,1,0,0,1,1,1,0,0,1,1,1,1}, /* 5 */
	{1,1,1,1,0,0,1,1,1,1,0,1,1,1,1}, /* 6 */
	{1,1,1,0,0,1,0,0,1,0,0,1,0,0,1}, /* 7 */
	{1,1,1,1,0,1,1,1,1,1,0,1,1,1,1}, /* 8 */
	{1,1,1,1,0,1,1,1,1,0,0,1,1,1,1}, /* 9 */
};

#endif
