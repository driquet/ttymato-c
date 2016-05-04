/*
 *   ncurses file.
 *   Copyright © 2016 Damien Riquet <d.riquet@gmail.com>
 *   All right reserved.
 *
 *   See LICENCE file.
 */

#include "ncurses.h"

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


void init_ncurses(ttymato_curses_t *ctx)
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, true);
	start_color();
	curs_set(false);
	clear();
	refresh();

	ctx->clockwin = newwin(
			CLOCKVIEW_HEIGHT,
			CLOCKVIEW_WIDTH,
			LINES / 2 - CLOCKVIEW_HEIGHT / 2,
			COLS / 2 - TOTAL_WIDTH / 2
		);

	ctx->pomowin = newwin(
			POMOVIEW_HEIGHT,
			POMOVIEW_WIDTH,
			LINES / 2 - CLOCKVIEW_HEIGHT / 2,
			COLS / 2 - TOTAL_WIDTH / 2 + CLOCKVIEW_WIDTH
		);


	nodelay(stdscr, true);
	wrefresh(ctx->clockwin);

	/* Init colors
	 */
	init_pair(0, COLOR_BLACK, COLOR_BLACK);
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	refresh();
}

void tick_ncurses(ttymato_curses_t *ctx, ttymato_pomodoro_t *pomodoro, ttymato_options_t *options)
{
	struct tm  *tm;

	/* Select the numbers to display in the clock
	 */
	if ( ctx->display == TIME || pomodoro->state == PAUSED )
	{
		ctx->digits[0] = pomodoro->time.tm_hour / 10;
		ctx->digits[1] = pomodoro->time.tm_hour % 10;
		ctx->digits[2] = pomodoro->time.tm_min  / 10;
		ctx->digits[3] = pomodoro->time.tm_min  % 10;
	}
	else
	{
		if ( ctx->display == ELAPSED )
			tm = &pomodoro->elapsed;
		else
			tm = &pomodoro->left;

		ctx->digits[0] = tm->tm_min / 10;
		ctx->digits[1] = tm->tm_min % 10;
		ctx->digits[2] = tm->tm_sec / 10;
		ctx->digits[3] = tm->tm_sec % 10;
	}
}

void draw_clock(ttymato_curses_t *ctx, ttymato_pomodoro_t *pomodoro, ttymato_options_t *options)
{
	chtype                    dotcolor = COLOR_PAIR(1);
	int                       total_len, str_col_start;
	pomodoro_clock_display_t  state;

	/* Cleaning window
	 */
	werase(ctx->clockwin);

	/* Digits
	 */
	draw_number(ctx, ctx->digits[0], 2, 1);
	draw_number(ctx, ctx->digits[1], 2, 8);
	draw_number(ctx, ctx->digits[2], 2, 20);
	draw_number(ctx, ctx->digits[3], 2, 27);

	/* Blinking dots
	 */
	if ( options->blink && time(NULL) % 2 == 0 )
		dotcolor = COLOR_PAIR(2);

	/* Dots
	 */
	wbkgdset(ctx->clockwin, dotcolor);
	mvwaddstr(ctx->clockwin, 3, 16, "  ");
	mvwaddstr(ctx->clockwin, 5, 16, "  ");

	/* Display mode
	 */
	total_len     = strlen(TIME_STR) + strlen(ELAPSED_STR) + strlen(LEFT_STR) + 2 * 3;
	str_col_start = CLOCKVIEW_WIDTH / 2 - total_len / 2;
	state = ctx->display == TIME || pomodoro->state == PAUSED ? TIME : ctx->display;

	if ( state == TIME )
		wbkgdset(ctx->clockwin, COLOR_PAIR(1));
	else
		wbkgdset(ctx->clockwin, COLOR_PAIR(0));
	mvwaddstr(ctx->clockwin, 8, str_col_start, TIME_STR);


	if ( state == ELAPSED )
		wbkgdset(ctx->clockwin, COLOR_PAIR(1));
	else
		wbkgdset(ctx->clockwin, COLOR_PAIR(0));
	mvwaddstr(ctx->clockwin, 8, str_col_start + strlen(TIME_STR) + SPACING , ELAPSED_STR);

	if ( state == LEFT)
		wbkgdset(ctx->clockwin, COLOR_PAIR(1));
	else
		wbkgdset(ctx->clockwin, COLOR_PAIR(0));
	mvwaddstr(ctx->clockwin, 8, str_col_start + strlen(TIME_STR) + strlen(ELAPSED_STR) + 2 * SPACING, LEFT_STR);

	wbkgdset(ctx->clockwin, COLOR_PAIR(0));
	wrefresh(ctx->clockwin);
}

void draw_ncurses(ttymato_curses_t *ctx, ttymato_pomodoro_t *pomodoro, ttymato_options_t *options)
{
	int          i;
	int          line = 3;
	char         buffer[POMOVIEW_WIDTH - 4];


	draw_clock(ctx, pomodoro, options);

	/* ------------ POMODORO VIEW --------------
	 */
	werase(ctx->pomowin);

	/* separators
	 */
	wbkgdset(ctx->pomowin, COLOR_PAIR(0));
	for ( i = 2 ; i < POMOVIEW_HEIGHT - 3; ++i )
		mvwaddch(ctx->pomowin, i, 1, '|');

	/* status
	 */
	if ( pomodoro->state == POMODORI )
		snprintf(buffer, sizeof(buffer), "Status : %s (%d/%d)", get_interval_name(pomodoro),
				pomodoro->current_pomodoro + 1, pomodoro->pomodoro_number);
	else
		snprintf(buffer, sizeof(buffer), "Status : %s", get_interval_name(pomodoro));

	mvwaddstr(ctx->pomowin, line++, 3, buffer);

	/* time left
	 */
	if ( ctx->display == LEFT && pomodoro->state != PAUSED )
		strftime(buffer, sizeof(buffer), "Time   : %H:%M", &pomodoro->time);
	else
		strftime(buffer, sizeof(buffer), "Left   : %M:%S", &pomodoro->left);
	mvwaddstr(ctx->pomowin, line++, 3, buffer);
	
	/* time elapsed
	 */
	if ( ctx->display == ELAPSED && pomodoro->state != PAUSED )
		strftime(buffer, sizeof(buffer), "Time   : %H:%M", &pomodoro->time);
	else
		strftime(buffer, sizeof(buffer), "Elapsed: %M:%S", &pomodoro->elapsed);
	mvwaddstr(ctx->pomowin, line++, 3, buffer);

	wrefresh(ctx->pomowin);
}

void draw_number(ttymato_curses_t *ctx, int n, int x, int y)
{
	int i, sy = y;

	for ( i = 0 ; i < 30; ++i, ++sy )
	{
		if ( sy == y + 6 )
		{
			sy = y;
			++x;
		}

		wbkgdset(ctx->clockwin, COLOR_PAIR(numbers[n][i/2]));
		mvwaddch(ctx->clockwin, x, sy, ' ');
	}
}
