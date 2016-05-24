/*
 *   ncurses file.
 *   Copyright © 2016 Damien Riquet <d.riquet@gmail.com>
 *   All right reserved.
 *
 *   See LICENCE file.
 */

#include <string.h>
#include "ncurses.h"
#include "pomodoro.h"

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


void init_ncurses()
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, true);
	start_color();
	curs_set(false);
	clear();
	refresh();

	g_ncurses.clockwin = newwin(
			CLOCKVIEW_HEIGHT,
			CLOCKVIEW_WIDTH,
			LINES / 2 - CLOCKVIEW_HEIGHT / 2,
			COLS / 2 - TOTAL_WIDTH / 2
		);

	g_ncurses.pomowin = newwin(
			POMOVIEW_HEIGHT,
			POMOVIEW_WIDTH,
			LINES / 2 - CLOCKVIEW_HEIGHT / 2,
			COLS / 2 - TOTAL_WIDTH / 2 + CLOCKVIEW_WIDTH
		);

	if(use_default_colors() == OK)
		g_ncurses.bg = -1;

	nodelay(stdscr, true);
	wrefresh(g_ncurses.clockwin);

	/* Init colors
	 */
	init_pair(0, g_ncurses.bg, g_ncurses.bg);
	init_pair(1, COLOR_BLACK, g_ncurses.color);
	init_pair(2, g_ncurses.color, g_ncurses.bg);
	refresh();
}

void tick_ncurses()
{
	struct tm  *tm;

	/* Select the numbers to display in the clock
	 */
	if ( g_ncurses.display == TIME || g_pomodoro.state == PAUSED )
	{
		g_ncurses.digits[0] = g_pomodoro.time.tm_hour / 10;
		g_ncurses.digits[1] = g_pomodoro.time.tm_hour % 10;
		g_ncurses.digits[2] = g_pomodoro.time.tm_min  / 10;
		g_ncurses.digits[3] = g_pomodoro.time.tm_min  % 10;
	}
	else
	{
		if ( g_ncurses.display == ELAPSED )
			tm = &g_pomodoro.elapsed;
		else
			tm = &g_pomodoro.left;

		g_ncurses.digits[0] = tm->tm_min / 10;
		g_ncurses.digits[1] = tm->tm_min % 10;
		g_ncurses.digits[2] = tm->tm_sec / 10;
		g_ncurses.digits[3] = tm->tm_sec % 10;
	}
}

void draw_clock()
{
	chtype                    dotcolor = COLOR_PAIR(1);
	int                       total_len, str_col_start;
	pomodoro_clock_display_t  state;

	/* Cleaning window
	 */
	wbkgdset(g_ncurses.clockwin, COLOR_PAIR(0));
	werase(g_ncurses.clockwin);

	/* Digits
	 */
	draw_number(g_ncurses.digits[0], 2, 1);
	draw_number(g_ncurses.digits[1], 2, 8);
	draw_number(g_ncurses.digits[2], 2, 20);
	draw_number(g_ncurses.digits[3], 2, 27);

	/* Blinking dots
	 */
	if ( g_options.blink && time(NULL) % 2 == 0 )
		dotcolor = COLOR_PAIR(2);

	/* Dots
	 */
	wbkgdset(g_ncurses.clockwin, dotcolor);
	mvwaddstr(g_ncurses.clockwin, 3, 16, "  ");
	mvwaddstr(g_ncurses.clockwin, 5, 16, "  ");

	/* Display mode
	 */
	total_len     = strlen(TIME_STR) + strlen(ELAPSED_STR) + strlen(LEFT_STR) + 2 * 3;
	str_col_start = CLOCKVIEW_WIDTH / 2 - total_len / 2;
	state = g_ncurses.display == TIME || g_pomodoro.state == PAUSED ? TIME : g_ncurses.display;

	if ( state == TIME )
		wbkgdset(g_ncurses.clockwin, COLOR_PAIR(1));
	else
		wbkgdset(g_ncurses.clockwin, COLOR_PAIR(2));
	mvwaddstr(g_ncurses.clockwin, 8, str_col_start, TIME_STR);


	if ( state == ELAPSED )
		wbkgdset(g_ncurses.clockwin, COLOR_PAIR(1));
	else
		wbkgdset(g_ncurses.clockwin, COLOR_PAIR(2));
	mvwaddstr(g_ncurses.clockwin, 8, str_col_start + strlen(TIME_STR) + SPACING , ELAPSED_STR);

	if ( state == LEFT)
		wbkgdset(g_ncurses.clockwin, COLOR_PAIR(1));
	else
		wbkgdset(g_ncurses.clockwin, COLOR_PAIR(2));
	mvwaddstr(g_ncurses.clockwin, 8, str_col_start + strlen(TIME_STR) + strlen(ELAPSED_STR) + 2 * SPACING, LEFT_STR);

	wrefresh(g_ncurses.clockwin);
}

void draw_pomodoro()
{
	int          i;
	int          line = 3;
	char         buffer[POMOVIEW_WIDTH - 4];

	werase(g_ncurses.pomowin);

	/* separators
	 */
	wbkgdset(g_ncurses.pomowin, COLOR_PAIR(2));
	for ( i = 2 ; i < POMOVIEW_HEIGHT - 3; ++i )
		mvwaddch(g_ncurses.pomowin, i, 1, '|');

	/* status
	 */
	if ( g_pomodoro.state == POMODORI )
		snprintf(buffer, sizeof(buffer), "Status : %s (%d/%d)", get_interval_name(g_pomodoro.state),
				g_pomodoro.current_pomodoro + 1, g_pomodoro.pomodoro_number);
	else
		snprintf(buffer, sizeof(buffer), "Status : %s", get_interval_name(g_pomodoro.state));

	mvwaddstr(g_ncurses.pomowin, line++, 3, buffer);

	/* time left
	 */
	if ( g_ncurses.display == LEFT && g_pomodoro.state != PAUSED )
		strftime(buffer, sizeof(buffer), "Time   : %H:%M", &g_pomodoro.time);
	else
		strftime(buffer, sizeof(buffer), "Left   : %M:%S", &g_pomodoro.left);
	mvwaddstr(g_ncurses.pomowin, line++, 3, buffer);
	
	/* time elapsed
	 */
	if ( g_ncurses.display == ELAPSED && g_pomodoro.state != PAUSED )
		strftime(buffer, sizeof(buffer), "Time   : %H:%M", &g_pomodoro.time);
	else
		strftime(buffer, sizeof(buffer), "Elapsed: %M:%S", &g_pomodoro.elapsed);
	mvwaddstr(g_ncurses.pomowin, line++, 3, buffer);

	wrefresh(g_ncurses.pomowin);
}

void draw_ncurses()
{
	draw_clock();
	draw_pomodoro();
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

		wbkgdset(g_ncurses.clockwin, COLOR_PAIR(numbers[n][i/2]));
		mvwaddch(g_ncurses.clockwin, x, sy, ' ');
	}
}
