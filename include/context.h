/*
 *   context headers file.
 *   Copyright © 2016 Damien Riquet <d.riquet@gmail.com>
 *   All right reserved.
 *
 *   See LICENCE file.
 */

#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <time.h>
#include <stdbool.h>
#include <curses.h>

/* pomodoro context
 */
typedef enum { PAUSED, POMODORI, POMODORI_END, BREAK, LONGBREAK } pomodoro_state_t;

typedef struct
{
	pomodoro_state_t  state;              /*!< current state */
	pomodoro_state_t  last_state;         /*!< last state, needed when pausing/resuming */
	unsigned          current_pomodoro;   /*!< nth pomodoro */
	time_t            interval_start;     /*!< start of the current interval */
	time_t            interval_end;       /*!< end of the current interval */
	time_t            interval_pause;     /*!< pause of the current interval */
	struct tm         time;               /*!< current time */
	struct tm         elapsed;            /*!< elapsed time from beginning of the current interval */
	struct tm         left;               /*!< time left for the current interval */
	unsigned          pomodoro_duration;  /*!< duration (in minutes) of a pomodori */
	unsigned          pomodoro_number;    /*!< number of pomodori in a cycle */
	unsigned          break_duration;     /*!< duration (in minutes) of a break */
	unsigned          longbreak_duration; /*!< duration (in minutes) of a long break */
} ttymato_pomodoro_t;


/* ttymato options
 */
typedef struct
{
	bool      date;               /*!< display date */
	bool      blink;              /*!< blink colons */
	bool      urgent;             /*!< make terminal urgent when an interval finishes */
	bool      notify;             /*!< notify at the end of intervals */
	bool      autostart;          /*!< autostart intervals */
	bool      twelve;             /*!< 12/24 meridiem */
	bool      noquit;             /*!< prevent the application from exiting on 'q' keypress */
} ttymato_options_t;

/* ncurses context
 */
typedef enum { TIME, ELAPSED, LEFT } pomodoro_clock_display_t;

typedef struct
{
	WINDOW                    *clockwin;
	WINDOW                    *pomowin;
	pomodoro_clock_display_t  display;
	unsigned int              digits[4];
} ttymato_curses_t;

/* global context
 */
#define DEFAULT_DELAY               1

typedef struct {
	bool                running;
	long                delay;
	ttymato_pomodoro_t  pomodoro;
	ttymato_options_t   options;
	ttymato_curses_t    curses;
} ttymato_context_t;

ttymato_context_t  *g_ttymato_context;
#define g_ctx      (g_ttymato_context)
#define g_pomodoro (g_ttymato_context->pomodoro)
#define g_ncurses  (g_ttymato_context->curses)
#define g_options  (g_ttymato_context->options)

#endif
