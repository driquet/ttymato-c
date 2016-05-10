/*
 *   options headers file.
 *   Copyright © 2016 Damien Riquet <d.riquet@gmail.com>
 *   All right reserved.
 *
 *   See LICENCE file.
 */

#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

/* ttymato options
 */
typedef struct
{
	bool      date;               /*!< display date */
	bool      blink;              /*!< blink colons */
	bool      urgent;             /*!< make terminal urgent when an interval finishes */
	bool      notify;             /*!< notify at the end of intervals */
	bool      autostart;          /*!< autostart intervals */
	bool      twelve;             /*!< 12/24 meridiem */
	bool      noquit;             /*!< prevent the application from exiting on 'q' keypress */
} ttymato_options_t;

#define DEFAULT_DELAY               1

#endif
