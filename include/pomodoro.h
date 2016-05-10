/*
 *   pomodoro headers file.
 *   Copyright © 2016 Damien Riquet <d.riquet@gmail.com>
 *   All right reserved.
 *
 *   See LICENCE file.
 */

#ifndef __POMODORO_H__
#define __POMODORO_H__

#include "context.h"

#define DEFAULT_DURATION_POMODORI   (25 * 60)
#define DEFAULT_NUMBER_POMODORI     4
#define DEFAULT_DURATION_BREAK      (5 * 60)
#define DEFAULT_DURATION_LONGBREAK  (15 * 60)

/*!
 * \brief Initialize the pomodoro context
 *
 * \param ctx pomodoro context
 */
void init_pomodoro(ttymato_pomodoro_t *ctx);

/*!
 * \brief Update the pomodoro data
 *
 * \param ctx pomodoro context
 * \param options ttymato options
 *
 */
void tick_pomodoro(ttymato_pomodoro_t *ctx, ttymato_options_t *options);

/*!
 * \brief Update context and go to the next interval
 *
 * \param ctx pomodoro context
 */
void next_interval(ttymato_pomodoro_t *ctx);

/*!
 * \brief Pause/Resume interval
 *
 * \param ctx pomodoro context
 */
void toggle_pomodoro_state(ttymato_pomodoro_t *ctx);


/*!
 * \brief Return the duration of an interval
 *
 * \param ctx pomodoro context
 * \param state interval state
 */
int get_interval_duration(ttymato_pomodoro_t *ctx, pomodoro_state_t state);

/*!
 * \brief Return current interval name
 *
 * \param ctx pomodoro context
 *
 * \return name of the current interval
 */
const char * get_interval_name(ttymato_pomodoro_t *ctx);

#endif
