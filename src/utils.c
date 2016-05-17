/*
 *   ttymato utils file.
 *   Copyright © 2016 Damien Riquet <d.riquet@gmail.com>
 *   All right reserved.
 *
 *   See LICENCE file.
 */

#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include "utils.h"
#include "context.h"

void urgent_bell(void)
{
	fprintf(stdout, "\a\n");
}

void notify(NotifyUrgency urgency, unsigned int expire, const char *summary, const char *message)
{
	NotifyNotification *n;
	notify_init("ttymato");
	n = notify_notification_new(summary, message, NULL);
	notify_notification_set_timeout(n, expire);
	notify_notification_set_urgency(n, urgency);
	notify_notification_show(n, NULL);
	g_object_unref(G_OBJECT(n));
	notify_uninit();
}

int load_config(const char *path)
{
	const char * const home = getenv("HOME");
	const char * const xdg_home = getenv("XDG_CONFIG_HOME");
	char               buffer[PATH_MAX];
	int                retval;

	DEBUG("%s", path);
	if ( strlen(path) )
		return try_parse_config(path);

	if ( home )
	{
		if ( xdg_home )
			snprintf(buffer, PATH_MAX, "%s/ttymato/config", xdg_home);
		else
			snprintf(buffer, PATH_MAX, "%s/.config/ttymato/config", home);

		retval = try_parse_config(buffer);
		if ( retval == E_PARSE_OK )
			return E_PARSE_OK;

		snprintf(buffer, PATH_MAX, "%s/.ttymato.conf", home);
		return try_parse_config(buffer);
	}

	return E_PARSE_NOFILE;
}


enum {
	OPTION_NCURSES_BLINK,
	OPTION_NCURSES_NOQUIT,
	OPTION_NOTIFICATION_URGENT,
	OPTION_NOTIFICATION_NOTIFY,
	OPTION_DURATION_POMODORO,
	OPTION_DURATION_BREAK,
	OPTION_DURATION_LONGBREAK,
	OPTION_POMODORO_NUMBER,
};

config_option_t ttymato_config_options[] =
{
	{"blink",               OPTION_NCURSES_BLINK       },
	{"noquit",              OPTION_NCURSES_NOQUIT      },
	{"urgent",              OPTION_NOTIFICATION_URGENT },
	{"notify",              OPTION_NOTIFICATION_NOTIFY },
	{"pomodoro_duration",   OPTION_DURATION_POMODORO   },
	{"break_duration",      OPTION_DURATION_BREAK      },
	{"longbreak_duration",  OPTION_DURATION_LONGBREAK  },
	{"pomodoro_number",     OPTION_POMODORO_NUMBER     },
	{NULL,                  0                          },
};

int parse_bool(const char *s_value, bool *value)
{
	if ( !strcmp(s_value, "true") )
	{
		*value = true;
		return 1;
	}
	else if ( !strcmp(s_value, "false") )
	{
		*value = false;
		return 1;
	}
	return 0;
}

int parse_int(const char *s_value, unsigned *value)
{
	uintmax_t num = strtoumax(s_value, NULL, 10);
	if ( num == UINTMAX_MAX && errno == ERANGE )
		return 0;
	*value = num;
	return 1;
}

#define TRY_PARSE_BOOL_OPT(opt_val, var)                              \
case opt_val:                                                         \
{                                                                     \
	bool value;                                                       \
	if ( parse_bool((const char *)s_value, &value) )                  \
	{                                                                 \
		DEBUG("option %s: %s", line, s_value);                        \
		var = value;                                                  \
	}                                                                 \
	else                                                              \
	{                                                                 \
		DEBUG("option %s expects bool values (true or false)", line); \
		error = true;                                                 \
	}                                                                 \
}                                                                     \
break;

#define TRY_PARSE_UNSIGNED_MULT_OPT(opt_val, var, mult)  \
case opt_val:                                            \
{                                                        \
	unsigned value;                                      \
	if ( parse_int((const char *)s_value, &value) )      \
	{                                                    \
		DEBUG("option %s: %s", line, s_value);           \
		var = value * mult;                              \
	}                                                    \
	else                                                 \
	{                                                    \
		DEBUG("option %s expects integer values", line ) \
	}                                                    \
}                                                        \
break;

#define TRY_PARSE_UNSIGNED_OPT(opt_val, val) TRY_PARSE_UNSIGNED_MULT_OPT(opt_val, val, 1)
#define TRY_PARSE_DURATION_OPT(opt_val, val) TRY_PARSE_UNSIGNED_MULT_OPT(opt_val, val, 60)

int try_parse_config(const char *path)
{
	char line[2048];
	FILE *fp = fopen(path, "r");
	bool error = false;

	DEBUG("Trying to read %s", path);
	if ( !fp )
		return E_PARSE_NOFILE;
	DEBUG("Reading %s", path);

	while ( fgets(line, sizeof(line), fp) != NULL)
	{
		int    len = strlen(line);
		char   *s_value;

		if ( line[len - 1] != '\n' )
			goto close_error;
		line[len - 1] = '\0';

		if ( *line == '#' || (len - 1 == 0) )
			continue;

		switch ( get_option(line, ttymato_config_options, &s_value) )
		{
			case E_OPT_MALFORMED:
			fprintf(stderr, "malformed option \"%s\", expecting <key>=<value>\n", line);
			error = true;
			break;

			case E_OPT_NO_MATCH:
			fprintf(stderr, "unknow option: \"%s\"\n", line);
			error = true;
			break;

			TRY_PARSE_BOOL_OPT(OPTION_NCURSES_BLINK, g_options.blink);
			TRY_PARSE_BOOL_OPT(OPTION_NCURSES_NOQUIT, g_options.noquit);
			TRY_PARSE_BOOL_OPT(OPTION_NOTIFICATION_URGENT, g_options.urgent);
			TRY_PARSE_BOOL_OPT(OPTION_NOTIFICATION_NOTIFY, g_options.notify);
			TRY_PARSE_DURATION_OPT(OPTION_DURATION_POMODORO, g_pomodoro.pomodoro_duration);
			TRY_PARSE_DURATION_OPT(OPTION_DURATION_BREAK, g_pomodoro.break_duration);
			TRY_PARSE_DURATION_OPT(OPTION_DURATION_LONGBREAK, g_pomodoro.longbreak_duration);
			TRY_PARSE_UNSIGNED_OPT(OPTION_POMODORO_NUMBER, g_pomodoro.pomodoro_number);

			default:
			DEBUG("unmanaged option: %s", line);
			error = true;
		}
	}

	if ( error )
		goto close_error;

	fclose(fp);
	return E_PARSE_OK;

close_error:
	fclose(fp);
	return E_PARSE_ERROR;
}

int get_option(char *line, config_option_t *options, char **value_p)
{
	char *key, *value, *equal;
	int  i;


	assert(line != NULL);
	assert(options != NULL);
	assert(value_p != NULL);

	*value_p = NULL;
	key = line;

	/* Skipping spaces
	 */
	while ( *key && isspace(*key) )
		key++;
	if ( !*key )
		return E_OPT_MALFORMED;

	equal = strchr(key, '=');
	if ( equal == NULL )
		return E_OPT_MALFORMED;

	value = equal + 1;

	/* Skipping spaces
	 */
	while ( *value && isspace(*value) )
		value++;
	if ( !*value )
		return E_OPT_MALFORMED;

	*equal = '\0';

	for ( i = 0 ; options[i].name ; ++i )
	{
		if ( strncmp(key, options[i].name, strlen(options[i].name))  == 0 )
		{
			*value_p = value;
			return options[i].value;
		}
	}

	return E_OPT_NO_MATCH;
}
