/*
 *   pomodoro headers file.
 *   Copyright © 2016 Damien Riquet <d.riquet@gmail.com>
 *   All right reserved.
 *
 *   See LICENCE file.
 */

#ifndef __POMODORO_H__
#define __POMODORO_H__

#include <time.h>

#include "utils.h"
#include "options.h"

/* ttymato pomodoro state
 */
typedef enum { PAUSED, POMODORI, POMODORI_END, BREAK, LONGBREAK } pomodoro_state_t;

#define DEFAULT_DURATION_POMODORI   (25 * 60)
#define DEFAULT_NUMBER_POMODORI     4
#define DEFAULT_DURATION_BREAK      (5 * 60)
#define DEFAULT_DURATION_LONGBREAK  (15 * 60)

typedef struct
{
	pomodoro_state_t  state;              /*!< current state */
	pomodoro_state_t  last_state;         /*!< last state, needed when pausing/resuming */
	unsigned          current_pomodoro;   /*!< nth pomodoro */
	time_t            interval_start;     /*!< start of the current interval */
	time_t            interval_end;       /*!< end of the current interval */
	time_t            interval_pause;     /*!< pause of the current interval */
	struct tm         time;               /*!< current time */
	struct tm         elapsed;            /*!< elapsed time from beginning of the current interval */
	struct tm         left;               /*!< time left for the current interval */
	unsigned          pomodoro_duration;  /*!< duration (in minutes) of a pomodori */
	unsigned          pomodoro_number;    /*!< number of pomodori in a cycle */
	unsigned          break_duration;     /*!< duration (in minutes) of a break */
	unsigned          longbreak_duration; /*!< duration (in minutes) of a long break */
} ttymato_pomodoro_t;


/*
 * \brief Initialize the pomodoro context
 *
 * \param ctx pomodoro context
 */
void init_pomodoro(ttymato_pomodoro_t *ctx);

/*
 * \brief Update the pomodoro data
 *
 * \param ctx pomodoro context
 */
void tick_pomodoro(ttymato_pomodoro_t *ctx, ttymato_options_t *options);

/*
 * \brief Update context and go to the next interval
 *
 * \param ctx pomodoro context
 */
void next_interval(ttymato_pomodoro_t *ctx);

/*
 * \brief Pause/Resume interval
 *
 * \param ctx pomodoro context
 */
void toggle_pomodoro_state(ttymato_pomodoro_t *ctx);


/*
 * \brief Return the duration of an interval
 *
 * \param ctx pomodoro context
 * \param state interval state
 */
int get_interval_duration(ttymato_pomodoro_t *ctx, pomodoro_state_t state);

/*
 * \brief Return current interval name
 *
 * \param ctx pomodoro context
 *
 * \return name of the current interval
 */
const char * get_interval_name(ttymato_pomodoro_t *ctx);

#endif
