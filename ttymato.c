/*
 *   ttymato main file.
 *   Copyright © 2016 Damien Riquet <d.riquet@gmail.com>
 *   All right reserved.
 *
 *   See LICENCE file.
 */

#include "ttymato.h"
#include "utils.h"

void init_config(void)
{
	struct sigaction sig;

	/* Init pomodoro
	 */
	init_pomodoro(&g_ttymato_config->pomodoro);

	/* ttymato config
	 */
	g_ttymato_config->running = true;
	g_ttymato_config->options.delay = DEFAULT_DELAY;

	/* Init signals
	 */
	sig.sa_handler = signal_handler;
	sig.sa_flags   = 0;
	sigaction(SIGWINCH, &sig, NULL);
	sigaction(SIGTERM,  &sig, NULL);
	sigaction(SIGINT,   &sig, NULL);
	sigaction(SIGSEGV,  &sig, NULL);
}

void init_curses(void)
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, true);
	start_color();
	curs_set(false);
	clear();
	refresh();

	g_ttymato_config->curses.clockwin = newwin(
			CLOCKVIEW_HEIGHT,
			CLOCKVIEW_WIDTH,
			LINES / 2 - CLOCKVIEW_HEIGHT / 2,
			COLS / 2 - TOTAL_WIDTH / 2
		);

	g_ttymato_config->curses.pomowin = newwin(
			POMOVIEW_HEIGHT,
			POMOVIEW_WIDTH,
			LINES / 2 - CLOCKVIEW_HEIGHT / 2,
			COLS / 2 - TOTAL_WIDTH / 2 + CLOCKVIEW_WIDTH
		);


	nodelay(stdscr, true);
	wrefresh(g_ttymato_config->curses.clockwin);

	/* Init colors
	 */
	init_pair(0, COLOR_BLACK, COLOR_BLACK);
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	refresh();
}

void signal_handler(int signal)
{
	switch(signal)
	{
		case SIGWINCH:
			endwin();
			init_curses();
			break;
			/* Interruption signal */
		case SIGINT:
		case SIGTERM:
			g_ttymato_config->running = false;
			/* Segmentation fault signal */
			break;
		case SIGSEGV:
			endwin();
			fprintf(stderr, "Segmentation fault.\n");
			exit(EXIT_FAILURE);
			break;
	}
}

static inline const char * get_display_name(void)
{
	if ( g_ttymato_config->curses.display == TIME
			|| g_ttymato_config->pomodoro.state == PAUSED )
		return "Time";
	else if ( g_ttymato_config->curses.display == LEFT )
		return "Time left";
	else if ( g_ttymato_config->curses.display == ELAPSED )
		return "Elapsed time";
	return "Undefined";
}

void cleanup(void)
{
	if ( g_ttymato_config )
		free(g_ttymato_config);

	endwin();
}

void update_time(void)
{
	struct tm  *tm;
	if ( g_ttymato_config->curses.display == TIME || g_ttymato_config->pomodoro.state == PAUSED )
	{
		g_ttymato_config->curses.digits[0] = g_ttymato_config->pomodoro.time.tm_hour / 10;
		g_ttymato_config->curses.digits[1] = g_ttymato_config->pomodoro.time.tm_hour % 10;
		g_ttymato_config->curses.digits[2] = g_ttymato_config->pomodoro.time.tm_min  / 10;
		g_ttymato_config->curses.digits[3] = g_ttymato_config->pomodoro.time.tm_min  % 10;
	}
	else
	{
		if ( g_ttymato_config->curses.display == ELAPSED )
			tm = &g_ttymato_config->pomodoro.elapsed;
		else
			tm = &g_ttymato_config->pomodoro.left;

		g_ttymato_config->curses.digits[0] = tm->tm_min / 10;
		g_ttymato_config->curses.digits[1] = tm->tm_min % 10;
		g_ttymato_config->curses.digits[2] = tm->tm_sec / 10;
		g_ttymato_config->curses.digits[3] = tm->tm_sec % 10;
	}
}

void draw_number(int n, int x, int y)
{
	int i, sy = y;

	for ( i = 0 ; i < 30; ++i, ++sy )
	{
		if ( sy == y + 6 )
		{
			sy = y;
			++x;
		}

		wbkgdset(g_ttymato_config->curses.clockwin, COLOR_PAIR(numbers[n][i/2]));
		mvwaddch(g_ttymato_config->curses.clockwin, x, sy, ' ');
	}
}

void draw_clock(void)
{
	chtype       dotcolor = COLOR_PAIR(1);

	werase(g_ttymato_config->curses.clockwin);

	/* Digits
	 */
	draw_number(g_ttymato_config->curses.digits[0], 2, 1);
	draw_number(g_ttymato_config->curses.digits[1], 2, 8);
	draw_number(g_ttymato_config->curses.digits[2], 2, 20);
	draw_number(g_ttymato_config->curses.digits[3], 2, 27);

	/* Blinking dots
	 */
	if ( g_ttymato_config->options.blink && time(NULL) % 2 == 0 )
		dotcolor = COLOR_PAIR(2);

	/* Dots
	 */
	wbkgdset(g_ttymato_config->curses.clockwin, dotcolor);
	mvwaddstr(g_ttymato_config->curses.clockwin, 3, 16, "  ");
	mvwaddstr(g_ttymato_config->curses.clockwin, 5, 16, "  ");

	/* Display mode
	 */
	if ( g_ttymato_config->curses.display == TIME )
		wbkgdset(g_ttymato_config->curses.clockwin, COLOR_PAIR(1));
	else
		wbkgdset(g_ttymato_config->curses.clockwin, COLOR_PAIR(0));
	mvwaddstr(g_ttymato_config->curses.clockwin, 8, 1, "Time");


	if ( g_ttymato_config->curses.display == ELAPSED )
		wbkgdset(g_ttymato_config->curses.clockwin, COLOR_PAIR(1));
	else
		wbkgdset(g_ttymato_config->curses.clockwin, COLOR_PAIR(0));
	mvwaddstr(g_ttymato_config->curses.clockwin, 8, CLOCKVIEW_WIDTH / 2 - 6 , "Elapsed time");

	if ( g_ttymato_config->curses.display == LEFT)
		wbkgdset(g_ttymato_config->curses.clockwin, COLOR_PAIR(1));
	else
		wbkgdset(g_ttymato_config->curses.clockwin, COLOR_PAIR(0));
	mvwaddstr(g_ttymato_config->curses.clockwin, 8, CLOCKVIEW_WIDTH - 10 , "Time left");

	wbkgdset(g_ttymato_config->curses.clockwin, COLOR_PAIR(0));
	wrefresh(g_ttymato_config->curses.clockwin);
}

void draw_pomodoro(void)
{
	int i;
	int line = 3;
	char buffer[POMOVIEW_WIDTH - 4];
	werase(g_ttymato_config->curses.pomowin);

	/* separators
	 */
	wbkgdset(g_ttymato_config->curses.pomowin, COLOR_PAIR(0));
	for ( i = 2 ; i < POMOVIEW_HEIGHT - 3; ++i )
		mvwaddch(g_ttymato_config->curses.pomowin, i, 1, '|');

	/* status
	 */
	if ( g_ttymato_config->pomodoro.state == POMODORI )
		snprintf(buffer, sizeof(buffer), "Status : %s (%d/%d)", get_interval_name(&g_ttymato_config->pomodoro),
				g_ttymato_config->pomodoro.current_pomodoro + 1, g_ttymato_config->pomodoro.pomodoro_number);
	else
		snprintf(buffer, sizeof(buffer), "Status : %s", get_interval_name(&g_ttymato_config->pomodoro));

	mvwaddstr(g_ttymato_config->curses.pomowin, line++, 3, buffer);

	/* time left
	 */
	if ( g_ttymato_config->curses.display == LEFT && g_ttymato_config->pomodoro.state != PAUSED )
		strftime(buffer, sizeof(buffer), "Time   : %H:%M", &g_ttymato_config->pomodoro.time);
	else
		strftime(buffer, sizeof(buffer), "Left   : %M:%S", &g_ttymato_config->pomodoro.left);
	mvwaddstr(g_ttymato_config->curses.pomowin, line++, 3, buffer);
	
	/* time elapsed
	 */
	if ( g_ttymato_config->curses.display == ELAPSED && g_ttymato_config->pomodoro.state != PAUSED )
		strftime(buffer, sizeof(buffer), "Time   : %H:%M", &g_ttymato_config->pomodoro.time);
	else
		strftime(buffer, sizeof(buffer), "Elapsed: %M:%S", &g_ttymato_config->pomodoro.elapsed);
	mvwaddstr(g_ttymato_config->curses.pomowin, line++, 3, buffer);

	wrefresh(g_ttymato_config->curses.pomowin);
}

void process_keys(void)
{
	switch ( wgetch(stdscr) )
	{
		case 'p':
		case 'P':
			/* Pause/Unpause
			 */
			toggle_pomodoro_state(&g_ttymato_config->pomodoro);
			break;

		case 'q':
		case 'Q':
			/* Quit
			 */
			if ( !g_ttymato_config->options.noquit )
				g_ttymato_config->running = false;
			break;


		case 's':
		case 'S':
			/* Switch time display
			 */
			g_ttymato_config->curses.display = (g_ttymato_config->curses.display + 1) % (LEFT + 1);
			break;

		case 'n':
		case 'N':
			/* Next interval
			 */
			next_interval(&g_ttymato_config->pomodoro);
			break;

		default:
			sleep(g_ttymato_config->options.delay);
			break;
	}
}

void tick(void)
{
	tick_pomodoro(&g_ttymato_config->pomodoro, &g_ttymato_config->options);
	update_time();
	draw_clock();
	draw_pomodoro();
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
			g_ttymato_config->options.urgent = true;
			break;

			case 'n':
			g_ttymato_config->options.notify = true;
			break;

			case 'N':
			g_ttymato_config->options.noquit = true;
			break;

			case 'b':
			g_ttymato_config->options.blink = true;
			break;

			case 'D':
			{
				char *values = strdup(optarg);

				/* Pomodori duration
				 */
				if ( !parse_next_int(values, ",", 0, 60, &g_ttymato_config->pomodoro.pomodoro_duration) )
				{
						fprintf(stderr, "Error while parsing pomodori duration\n");
						goto duration_end;
				}
				g_ttymato_config->pomodoro.pomodoro_duration *= 60;

				/* Break duration
				 */
				if ( !parse_next_int(NULL, ",", 0, 60, &g_ttymato_config->pomodoro.break_duration) )
				{
						fprintf(stderr, "Error while parsing break duration\n");
						goto duration_end;
				}
				g_ttymato_config->pomodoro.break_duration *= 60;

				/* Longbreak duration
				 */
				if ( !parse_next_int(NULL, ",", 0, 60, &g_ttymato_config->pomodoro.longbreak_duration) )
				{
						fprintf(stderr, "Error while parsing longbreak duration\n");
						goto duration_end;
				}
				g_ttymato_config->pomodoro.longbreak_duration *= 60;

				free(values);
				break;

duration_end:
				free(values);
				exit(EXIT_FAILURE);
			}
			break;

			case 'p':
			if ( atoi(optarg) > 0 )
				g_ttymato_config->pomodoro.pomodoro_number = atoi(optarg);
			break;
		}
	}
}


int main(int argc, char **argv)
{
	/* Allocate ttymato config
	 */
	g_ttymato_config = calloc(1, sizeof(ttymato_config_t));
	assert(g_ttymato_config != NULL);

	atexit(cleanup);

	init_config();
	parse_args(argc, argv);
	init_curses();

	while ( g_ttymato_config->running )
		tick();

	return 0;
}

