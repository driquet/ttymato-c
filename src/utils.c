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
