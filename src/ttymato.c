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
			fprintf(stderr, "Segmentation fault.\n");
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
	printf("usage: ttymato [-hunbN] [-D <pomodoro>,<break>,<longbreak>] [-p number] \n"
		"\t -h \t Print this help                                        \n"
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

	while ( (c = getopt(argc, argv, "hunbND:p:")) != -1 )
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
						fprintf(stderr, "Error while parsing pomodori duration\n");
						goto duration_end;
				}
				g_ctx->pomodoro.pomodoro_duration *= 60;

				/* Break duration
				 */
				if ( !parse_next_int(NULL, ",", 0, 60, &g_ctx->pomodoro.break_duration) )
				{
						fprintf(stderr, "Error while parsing break duration\n");
						goto duration_end;
				}
				g_ctx->pomodoro.break_duration *= 60;

				/* Longbreak duration
				 */
				if ( !parse_next_int(NULL, ",", 0, 60, &g_ctx->pomodoro.longbreak_duration) )
				{
						fprintf(stderr, "Error while parsing longbreak duration\n");
						goto duration_end;
				}
				g_ctx->pomodoro.longbreak_duration *= 60;

				free(values);
				break;

duration_end:
				free(values);
				exit(EXIT_FAILURE);
			}
			break;

			case 'p':
			if ( atoi(optarg) > 0 )
				g_ctx->pomodoro.pomodoro_number = atoi(optarg);
			break;
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
	init_ncurses();


	while ( g_ctx->running )
		tick();

	return 0;
}

