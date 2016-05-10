/*
 *   pomodoro file.
 *   Copyright © 2016 Damien Riquet <d.riquet@gmail.com>
 *   All right reserved.
 *
 *   See LICENCE file.
 */

#include "pomodoro.h"
#include "utils.h"

void init_pomodoro()
{
	/* Init ttymato options
	 */
	g_pomodoro.pomodoro_duration  = DEFAULT_DURATION_POMODORI;
	g_pomodoro.pomodoro_number    = DEFAULT_NUMBER_POMODORI;
	g_pomodoro.break_duration     = DEFAULT_DURATION_BREAK;
	g_pomodoro.longbreak_duration = DEFAULT_DURATION_LONGBREAK;

	/* Init pomodoro state
	 */
	g_pomodoro.state = PAUSED;
	g_pomodoro.current_pomodoro = 0;
}

void tick_pomodoro()
{
	time_t     lt, dt_elapsed, dt_left;

	lt = time(NULL);

	/* Determine if the current interval is finished
	 */
	if ( g_pomodoro.state != PAUSED
			&& g_pomodoro.state != POMODORI_END )
	{
		if ( lt > g_pomodoro.interval_end )
		{
			if ( g_options.notify )
			{
				char message[100];
				snprintf(message, sizeof(message), "%s end", get_interval_name(g_pomodoro.state));
				notify(NOTIFY_URGENCY_NORMAL, 30000, "ttymato", message);
			}

			if ( g_options.autostart )
			{
				next_interval();
				return;
			}
			else
			{
				if ( g_pomodoro.state == POMODORI )
					g_pomodoro.state = POMODORI_END;
				else
					g_pomodoro.state = PAUSED;
				g_pomodoro.interval_pause = lt;
				g_pomodoro.interval_start = lt;
				g_pomodoro.interval_end   = lt;

				if ( g_options.urgent )
				{
					urgent_bell();
				}
			}
		}
	}

	/* Update time related data
	 */
	localtime_r(&lt, &g_pomodoro.time);

	/* Update elapsed time, and left time
	 */
	switch ( g_pomodoro.state )
	{
		case POMODORI:
		case BREAK:
		case LONGBREAK:
			dt_elapsed = lt - g_pomodoro.interval_start;
			dt_left    = g_pomodoro.interval_end - lt;
			break;

		case PAUSED:
			dt_elapsed = g_pomodoro.interval_pause - g_pomodoro.interval_start;
			dt_left    = g_pomodoro.interval_end - g_pomodoro.interval_pause;
			break;

		case POMODORI_END:
			dt_elapsed = 0;
			dt_left    = 0;
			break;
	}

	gmtime_r(&dt_elapsed, &g_pomodoro.elapsed);
	gmtime_r(&dt_left, &g_pomodoro.left);
}

void next_interval()
{
	switch ( g_pomodoro.state )
	{
		case PAUSED:
			g_pomodoro.state = POMODORI;
			break;

		case POMODORI:
		case POMODORI_END:
			if ( g_pomodoro.current_pomodoro == g_pomodoro.pomodoro_number - 1)
				g_pomodoro.state = LONGBREAK;
			else
				g_pomodoro.state = BREAK;
			break;

		case LONGBREAK:
			g_pomodoro.current_pomodoro = 0;
			g_pomodoro.state = POMODORI;
			break;

		case BREAK:
			g_pomodoro.current_pomodoro++;
			g_pomodoro.state = POMODORI;
			break;

	}

	g_pomodoro.interval_start = time(NULL);
	g_pomodoro.interval_end   = g_pomodoro.interval_start + get_interval_duration(g_pomodoro.state);
}

void toggle_pomodoro_state()
{
	if ( g_pomodoro.state == POMODORI_END )
		return;

	if ( g_pomodoro.state == PAUSED )
	{
		if ( g_pomodoro.last_state == PAUSED )
		{
			g_pomodoro.state = POMODORI;
			g_pomodoro.interval_start = time(NULL);
			g_pomodoro.interval_end   = g_pomodoro.interval_start + get_interval_duration(g_pomodoro.state);
		}
		else
		{
			time_t delta = time(NULL) - g_pomodoro.interval_pause;
			g_pomodoro.state = g_pomodoro.last_state;
			g_pomodoro.interval_start += delta;
			g_pomodoro.interval_end   += delta;
		}
	}
	else
	{
		g_pomodoro.last_state     = g_pomodoro.state;
		g_pomodoro.state          = PAUSED;
		g_pomodoro.interval_pause = time(NULL);
	}
}

int get_interval_duration(pomodoro_state_t state)
{
	if ( state == POMODORI )
		return g_pomodoro.pomodoro_duration;
	else if ( state == BREAK )
		return g_pomodoro.break_duration;
	else if ( state == LONGBREAK )
		return g_pomodoro.longbreak_duration;
	return 0;
}

const char * get_interval_name(pomodoro_state_t state)
{
	if ( state == POMODORI )
		return "Pomodori";
	else if ( state == POMODORI_END )
		return "Pomodori ended";
	else if ( state == BREAK )
		return "Break";
	else if ( state == LONGBREAK )
		return "Long break";
	return "Paused";
}
