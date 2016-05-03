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
#include <getopt.h>
#include <signal.h>

#include "pomodoro.h"
#include "ncurses.h"
#include "options.h"


/* ttymato curses config
 */

/* ttymato global config
 */
typedef struct
{
	bool                running;
	long                delay;
	ttymato_pomodoro_t  pomodoro;
	ttymato_options_t   options;
	ttymato_curses_t    curses;
} ttymato_config_t;

ttymato_config_t *g_ttymato_config;

void init(void);
void tick(void);
void signal_handler(int signal);


#endif
