/*
 *   pomodoro file.
 *   Copyright © 2016 Damien Riquet <d.riquet@gmail.com>
 *   All right reserved.
 *
 *   See LICENCE file.
 */

#include "pomodoro.h"

void init_pomodoro(ttymato_pomodoro_t *ctx)
{
	/* Init ttymato options
	 */
	ctx->pomodoro_duration  = DEFAULT_DURATION_POMODORI;
	ctx->pomodoro_number    = DEFAULT_NUMBER_POMODORI;
	ctx->break_duration     = DEFAULT_DURATION_BREAK;
	ctx->longbreak_duration = DEFAULT_DURATION_LONGBREAK;

	/* Init pomodoro state
	 */
	ctx->state = PAUSED;
	ctx->pomodoro_number = 0;

	/* Init time related data
	 */
}

void tick_pomodoro(ttymato_pomodoro_t *ctx, ttymato_options_t *options)
{
	time_t     lt, dt_elapsed, dt_left;

	lt = time(NULL);

	/* Determine if the current interval is finished
	 */
	if ( ctx->state != PAUSED
			&& ctx->state != POMODORI_END )
	{
		if ( lt > ctx->interval_end )
		{
			if ( options->notify )
			{
				char message[100];
				snprintf(message, sizeof(message), "%s end", get_interval_name(ctx));
				notify(NOTIFY_URGENCY_NORMAL, 30000, "ttymato", message);
			}

			if ( options->autostart )
			{
				next_interval(ctx);
				return;
			}
			else
			{
				if ( ctx->state == POMODORI )
					ctx->state = POMODORI_END;
				else
					ctx->state = PAUSED;
				ctx->interval_pause = lt;
				ctx->interval_start = lt;
				ctx->interval_end   = lt;

				if ( options->urgent )
				{
					urgent_bell();
				}
			}
		}
	}

	/* Update time related data
	 */
	localtime_r(&lt, &ctx->time);

	/* Update elapsed time, and left time
	 */
	switch ( ctx->state )
	{
		case POMODORI:
		case BREAK:
		case LONGBREAK:
			dt_elapsed = lt - ctx->interval_start;
			dt_left    = ctx->interval_end - lt;
			break;

		case PAUSED:
			dt_elapsed = ctx->interval_pause - ctx->interval_start;
			dt_left    = ctx->interval_end - ctx->interval_pause;
			break;

		case POMODORI_END:
			dt_elapsed = 0;
			dt_left    = 0;
			break;
	}

	gmtime_r(&dt_elapsed, &ctx->elapsed);
	gmtime_r(&dt_left, &ctx->left);
}

void next_interval(ttymato_pomodoro_t *ctx)
{
	switch ( ctx->state )
	{
		case PAUSED:
			ctx->state = POMODORI;
			break;

		case POMODORI:
		case POMODORI_END:
			if ( ctx->current_pomodoro == ctx->pomodoro_number - 1)
				ctx->state = LONGBREAK;
			else
				ctx->state = BREAK;
			break;

		case LONGBREAK:
			ctx->pomodoro_number = 0;
			ctx->state = POMODORI;
			break;

		case BREAK:
			ctx->pomodoro_number++;
			ctx->state = POMODORI;
			break;

	}

	ctx->interval_start = time(NULL);
	ctx->interval_end   = ctx->interval_start + get_interval_duration(ctx, ctx->state);
}

void toggle_pomodoro_state(ttymato_pomodoro_t *ctx)
{
	if ( ctx->state == POMODORI_END )
		return;

	if ( ctx->state == PAUSED )
	{
		if ( ctx->last_state == PAUSED )
		{
			ctx->state = POMODORI;
			ctx->interval_start = time(NULL);
			ctx->interval_end   = ctx->interval_start + get_interval_duration(ctx, ctx->state);
		}
		else
		{
			time_t delta = time(NULL) - ctx->interval_pause;
			ctx->state = ctx->last_state;
			ctx->interval_start += delta;
			ctx->interval_end   += delta;
		}
	}
	else
	{
		ctx->last_state     = ctx->state;
		ctx->state          = PAUSED;
		ctx->interval_pause = time(NULL);
	}
}

int get_interval_duration(ttymato_pomodoro_t *ctx, pomodoro_state_t state)
{
	if ( state == POMODORI )
		return ctx->pomodoro_duration;
	else if ( state == BREAK )
		return ctx->break_duration;
	else if ( state == LONGBREAK )
		return ctx->longbreak_duration;
	return 0;
}

const char * get_interval_name(ttymato_pomodoro_t *ctx)
{
	if ( ctx->state == POMODORI )
		return "Pomodori";
	else if ( ctx->state == POMODORI_END )
		return "Pomodori ended";
	else if ( ctx->state == BREAK )
		return "Break";
	else if ( ctx->state == LONGBREAK )
		return "Long break";
	return "Paused";
}
