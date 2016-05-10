/*
 *   ttymato utils headers file.
 *   Copyright © 2016 Damien Riquet <d.riquet@gmail.com>
 *   All right reserved.
 *
 *   See LICENCE file.
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <libnotify/notify.h>

/*!
 * \brief Generate a urgent bell to get the attention of the user
 */
void urgent_bell(void);

/*!
 * \brief Create a notification using notify-send command
 * \param urgency urgency level (low, normal, critical)
 * \param expire timeout in milliseconds
 * \param summary message summary
 * \param message message to display
 */
void notify(NotifyUrgency urgency, unsigned int expire, const char *summary, const char *message);

#endif
