/* $Id: event_conf.h,v 1.4 2006/06/05 03:51:52 mark Exp $ */

#define MAX_TRAP_DESTINATIONS	5

#define EVENT_CONF_FILE		"/etc/event_notif.conf"
#define EVENT_PROP_FILE		"/usr/share/event_properties/events.properties.en"
// FIFO devices
#define EVENT_PIPE  "/dev/event_pipe"
#define LOG_PIPE   "/dev/log_pipe"

#define GENERATE_SNMPTRAP		1
#define GENERATE_SYSLOG_MESSAGE		2
#define GENERATE_LOGPIPE_MESSAGE	4
#define GENERATE_EMAIL_MESSAGE		8
#define GENERATE_SMS_MESSAGE		16
