/*
 *   ttymato utils file.
 *   Copyright © 2016 Damien Riquet <d.riquet@gmail.com>
 *   All right reserved.
 *
 *   See LICENCE file.
 */

#include "utils.h"

void urgent_bell(void)
{
	fprintf(stdout, "\a\n");
}

void notify(const char *urgency, unsigned int expire, const char *summary, const char *message)
{
	char cmd_buffer[250];
	snprintf(cmd_buffer, sizeof(cmd_buffer), "notify-send -t %u -u %s \"%s\" \"%s\"",
			expire, urgency, summary, message);
	system(cmd_buffer);
}
