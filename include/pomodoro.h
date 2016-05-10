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
 */
void init_pomodoro();

/*!
 * \brief Update the pomodoro data
 */
void tick_pomodoro();

/*!
 * \brief Update context and go to the next interval
 */
void next_interval();

/*!
 * \brief Pause/Resume interval
 */
void toggle_pomodoro_state();


/*!
 * \brief Return the duration of an interval
 *
 * \param state interval state
 */
int get_interval_duration(pomodoro_state_t state);

/*!
 * \brief Return current interval name
 *
 * \param state interval state
 *
 * \return name of the current interval
 */
const char * get_interval_name(pomodoro_state_t state);

#endif
