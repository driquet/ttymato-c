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
#include <string.h>
#include <libnotify/notify.h>

#define DEBUG(msg, ...) \
	fprintf(stderr, "DEBUG %s:%d: " msg "\n", __func__, __LINE__, ##__VA_ARGS__);

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


typedef struct
{
	const char *name;
	int        value;
} config_option_t;


#define E_OPT_MALFORMED -1
#define E_OPT_NO_MATCH  -2


/*!
 * \brief try to parse a configuration file at several locations
 * \param path of the config file. Can be NULL
 */
int load_config(const char *path);

#define E_PARSE_OK      0
#define E_PARSE_ERROR  -1
#define E_PARSE_NOFILE -2
/*!
 * \brief try to parse a configuration file
 * \param path path of the file
 * \retval E_PARSE_ERROR if there is a parsing error
 * \retval E_PARSE_NOFILE if the file does not exist
 */
int try_parse_config(const char *path);

/*!
 * \brief parsing config option formated as follow:
 *   <key>=<value>
 * \param line line to parse
 * \param options expected options
 * \param value where to store the value
 * \retval option value if there is a match
 * \retval E_OPT_NO_MATCH if there is no matching option
 * \retval E_OPT_MALFORMED if the syntax is incorrect
 */
int get_option(char *line, config_option_t *options, char **value);
#endif
