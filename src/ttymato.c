/*
 *   ttymato main file.
 *   Copyright © 2016 Damien Riquet <d.riquet@gmail.com>
 *   All right reserved.
 *
 *   See LICENCE file.
 */

#include <assert.h>
#include <getopt.h>
#include <signal.h>
#include <string.h>

#include "context.h"
#include "pomodoro.h"
#include "ncurses.h"
#include "utils.h"

void signal_handler(int signal)
{
	switch(signal)
	{
		case SIGWINCH:
			endwin();
			init_ncurses(&g_ctx->curses);
			break;
			/* Interruption signal */
		case SIGINT:
		case SIGTERM:
			g_ctx->running = false;
			/* Segmentation fault signal */
			break;
		case SIGSEGV:
			endwin();
			DEBUG("Segmentation fault");
			exit(EXIT_FAILURE);
			break;
	}
}

void init_config(void)
{
	struct sigaction sig;

	/* Init pomodoro
	 */
	init_pomodoro(g_ctx);

	/* ttymato config
	 */
	g_ctx->running = true;
	g_ctx->delay   = DEFAULT_DELAY;

	/* ncurses config
	 */
	g_ncurses.bg    = COLOR_BLACK;
	g_ncurses.color = COLOR_GREEN;

	/* Init signals
	 */
	sig.sa_handler = signal_handler;
	sig.sa_flags   = 0;
	sigaction(SIGWINCH, &sig, NULL);
	sigaction(SIGTERM,  &sig, NULL);
	sigaction(SIGINT,   &sig, NULL);
	sigaction(SIGSEGV,  &sig, NULL);
}

void cleanup(void)
{
	if ( g_ctx )
		free(g_ctx);

	endwin();
}

void process_keys(void)
{
	switch ( wgetch(stdscr) )
	{
		case 'p':
		case 'P':
			/* Pause/Unpause
			 */
			toggle_pomodoro_state(g_ctx);
			break;

		case 'q':
		case 'Q':
			/* Quit
			 */
			if ( !g_ctx->options.noquit )
				g_ctx->running = false;
			break;


		case 's':
		case 'S':
			/* Switch time display
			 */
			g_ctx->curses.display = (g_ctx->curses.display + 1) % (LEFT + 1);
			break;

		case 'n':
		case 'N':
			/* Next interval
			 */
			next_interval(g_ctx);
			break;

		case 'r':
		case 'R':
		g_pomodoro.state = PAUSED;
		g_pomodoro.current_pomodoro = 0;
		break;

#define HANDLE_COLOR(color, val)                                                          \
		case val:                                                                         \
		{                                                                                 \
			init_pair(1, color == COLOR_BLACK ? COLOR_WHITE : COLOR_BLACK, color); \
			init_pair(2, color, g_ncurses.bg);                                            \
		}                                                                                 \
		break;

		HANDLE_COLOR(0, '0');
		HANDLE_COLOR(1, '1');
		HANDLE_COLOR(2, '2');
		HANDLE_COLOR(3, '3');
		HANDLE_COLOR(4, '4');
		HANDLE_COLOR(5, '5');
		HANDLE_COLOR(6, '6');
		HANDLE_COLOR(7, '7');

		default:
		sleep(g_ctx->delay);
		break;
	}
}

void tick(void)
{
	tick_pomodoro(g_ctx);
	tick_ncurses(&g_ctx->curses, &g_ctx->pomodoro, &g_ctx->options);
	draw_ncurses(&g_ctx->curses, &g_ctx->pomodoro, &g_ctx->options);

	process_keys();
}

void usage(void)
{
	printf("usage: ttymato [-hunbN] [-c <config>] [-D <pomodoro>,<break>,<longbreak>] [-p number] [-C [1-7]] \n"
		"\t -h \t Print this help                                        \n"
		"\t -c \t Path to configuration file                             \n"
		"\t -C \t Set the color                                          \n"
		"\t -u \t Urgent bell on intervals end                           \n"
		"\t -n \t Notification on intervals end                          \n"
		"\t -N \t Don't quit on keypress                                 \n"
		"\t -b \t Enable blinking colon                                  \n"
		"\t -D \t Duration of pomodoro, break and longbreak (in minutes) \n"
		"\t -p \t Number of pomodori                                     \n"
		);
}

int parse_next_int(char *str, char *sep, int min, int max, unsigned int *value)
{
	char *token = strtok(str, sep);

	if ( token == NULL )
		return 0;

	*value = atoi(token);
	if ( *value >= max || *value <= min )
		return 0;

	return 1;
}

void parse_args(int argc, char **argv)
{
	int c;

	while ( (c = getopt(argc, argv, "hunbND:p:c:C:")) != -1 )
	{
		switch (c)
		{
			case 'h':
			default:
			usage();
			exit(EXIT_SUCCESS);
			break;

			case 'u':
			g_ctx->options.urgent = true;
			break;

			case 'n':
			g_ctx->options.notify = true;
			break;

			case 'N':
			g_ctx->options.noquit = true;
			break;

			case 'b':
			g_ctx->options.blink = true;
			break;

			case 'D':
			{
				char *values = strdup(optarg);

				/* Pomodori duration
				 */
				if ( !parse_next_int(values, ",", 0, 60, &g_ctx->pomodoro.pomodoro_duration) )
				{
						DEBUG("Error while parsing pomodori duration");
						goto duration_end;
				}
				g_pomodoro.pomodoro_duration *= 60;

				/* Break duration
				 */
				if ( !parse_next_int(NULL, ",", 0, 60, &g_ctx->pomodoro.break_duration) )
				{
						DEBUG("Error while parsing break duration");
						goto duration_end;
				}
				g_pomodoro.break_duration *= 60;

				/* Longbreak duration
				 */
				if ( !parse_next_int(NULL, ",", 0, 60, &g_ctx->pomodoro.longbreak_duration) )
				{
						DEBUG("Error while parsing longbreak duration");
						goto duration_end;
				}
				g_pomodoro.longbreak_duration *= 60;

				free(values);
				break;

duration_end:
				free(values);
				exit(EXIT_FAILURE);
			}
			break;

			case 'p':
			if ( atoi(optarg) > 0 )
				g_pomodoro.pomodoro_number = atoi(optarg);
			break;

			case 'c':
			strncpy(g_ctx->config, optarg, sizeof(g_ctx->config));
			break;

			case 'C':
			if ( atoi(optarg) >= 0 && atoi(optarg) < 8 )
				g_ncurses.color = atoi(optarg);
		}
	}
}


int main(int argc, char **argv)
{
	/* Allocate ttymato config
	 */
	g_ctx = calloc(1, sizeof(ttymato_context_t));
	assert(g_ctx != NULL);

	atexit(cleanup);
	init_config();

	parse_args(argc, argv);
	if ( load_config(g_ctx->config) != E_PARSE_OK )
		DEBUG("Error while loading configuration file");
	init_ncurses();

	while ( g_ctx->running )
		tick();

	return 0;
}

