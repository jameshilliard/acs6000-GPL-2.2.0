/* -*- linux-c -*- */
/*****************************************************************
File: sh_write_event.c

Description: -- write event message to /dev/event_pipe fifo 
device.

The purpose of creating this program is so that you can write 
event messages from a shell script. The messages are outputted 
to the event_pipe fifo device instead of the standard 
output. 
This program basically takes in two parameters :
- event number
- the text to be logged. 
****************************************************************/

#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/signal.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <paths.h>
#include <stdio.h>
#include "event_connect.h"

int atoi(const char *nptr);

void writeevt(int,char*,...);

int main (int argc, char *argv[])
{
	int eventn, cnt;
	char *fmt;
	union {
		char * pars;
		int    pard;
	} param[6];

	if(argc < 3){
		fprintf(stderr, "usage: %s <evt definition> <fmt> ...\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	int i=0;
        char *dup,*pli,*pe;
        static const char filename[] = "/etc/event_shell";
        FILE *file = fopen ( filename, "r" );
                if (file != NULL)
                   {
                        char line[1024]; //or other suitable maximum line size 

                        while ( fgets ( line, sizeof line, file ) != NULL ) // read a line 
                                {

                        if (strstr(line,argv[1])) {
                                pe = strstr(line,"=");
                        	if (!pe) continue;
                        	pe++;
                        	if(*pe && (isdigit(*pe))) 
                        	pli=pe;
                        	while (*pe && !(isspace(*pe))) pe++;
                        	if (*pe) *pe=0x00;
                        	dup = strdup(pli);
               			eventn= atoi(dup);
                                i++;
                                }
                        }
                fclose ( file );

                }
                else
                {
                        // why didn't the file open? 
                        syslog(4,"file open error[%s]\n",__func__);
                }

//	eventn= atoi(argv[1]);
	fmt = argv[2];

	cnt = 0;
	memset((char *) param, 0, sizeof(param));

	while (*fmt) {
		if (*fmt == 'u') {
			fmt++;
			continue;
		}
		if (argc-- <= 3) exit (-1); // No more arguments
		if (*fmt == 'i') {
			param[cnt].pard = atoi(argv[cnt+3]); //integer	
		} else {
			param[cnt].pars = argv[cnt+3];
		}
		cnt++; fmt++;
	}
//	printf("Sending event %d\n",eventn);	
	writeevt(eventn, argv[2], param[0].pard, param[1].pard, param[2].pard,
		 param[3].pard, param[4].pard, param[5].pard);
	exit(EXIT_SUCCESS);
}

