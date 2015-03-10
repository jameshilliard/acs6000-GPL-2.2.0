#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <ctype.h>

static int got_signals = 0;

void unblock(int sig)
{
	sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, sig);
	sigprocmask(SIG_UNBLOCK, &set, NULL);
}

static void sig_handler(int sig)
{
	if (sig == SIGTERM) {
		got_signals |= 1;
		signal(sig, SIG_IGN);
	}
}

/*
 *	Main loop for WDT.
 */
int main(int argc, char **argv)
{
	int fw, count=0;
	char st[3];

	/* Disconnect from the controlling tty. */ 
	if ((fw = open("/dev/tty", O_RDWR)) >= 0) {
		signal(SIGHUP, SIG_IGN);
		ioctl(fw, TIOCNOTTY, (char *)1);
		signal(SIGHUP, SIG_DFL);
		close(fw);
	}

	if ((fw = open("/proc/sys/avocent/wdt/status", O_RDWR|O_NONBLOCK)) < 0) {
		return -1;
	}

	/* Redirect stdin, stdout, and stderr to /dev/null. */
	freopen("/dev/null", "r", stdin);
	freopen("/dev/null", "w", stdout);
	freopen("/dev/null", "w", stderr);

	signal(SIGTERM, sig_handler);
	unblock(SIGTERM);

	st[0] = '3'; st[1]= 0x00;

	while (1) {
		/* sleep half second */
		usleep(500000);

		/* service wdt each 1 second */
		if (count++ & 1) {
			write(fw, st, 1);
		}

	}
	close(fw);
	return(0);
}
