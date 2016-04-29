/*
 *   ttymato headers file.
 *   Copyright © 2016 Damien Riquet <d.riquet@gmail.com>
 *   All right reserved.
 *
 *   See LICENCE file.
 */

#ifndef __TTYMATO_H__
#define __TTYMATO_H__

#include <stdlib.h>
#include <assert.h>
#include <ncurses.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>

/* ttymato options
 */
#define DEFAULT_DELAY               1
#define DEFAULT_DURATION_POMODORI   (25 * 60)
#define DEFAULT_NUMBER_POMODORI     4
#define DEFAULT_DURATION_BREAK      (5 * 60)
#define DEFAULT_DURATION_LONGBREAK  (15 * 60)
typedef struct
{
	bool      date;               /*!< display date */
	bool      blink;              /*!< blink colons */
	long      delay;              /*!< sleep delay (in seconds) */
	bool      urgent;             /*!< make terminal urgent when an interval finishes */
	bool      notify;             /*!< notify at the end of intervals */
	bool      autostart;          /*!< autostart intervals */
	bool      twelve;             /*!< 12/24 meridiem */
	bool      noquit;             /*!< prevent the application from exiting on 'q' keypress */
	unsigned  pomodori_duration;  /*!< duration (in minutes) of a pomodori */
	unsigned  pomodori_number;    /*!< number of pomodori in a cycle */
	unsigned  break_duration;     /*!< duration (in minutes) of a break */
	unsigned  longbreak_duration; /*!< duration (in minutes) of a long break */
} ttymato_options_t;

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

/* ttymato pomodoro state
 */
typedef enum { PAUSED, POMODORI, POMODORI_END, BREAK, LONGBREAK } pomodoro_state_t;
typedef struct
{
	pomodoro_state_t  state, last_state;
	unsigned          pomodori_number;
	time_t            interval_start, interval_end, interval_pause;
	struct tm         time, elapsed, left;
} ttymato_pomodoro_t;

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
