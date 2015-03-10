/*
    This file is part of llconf2

    Copyright (C) 2004  Oliver Kurth <oku@debian.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <cgi.h>

#include "modules.h"
#include "ifconf.h"

#include <netinet/in.h>
#include <net/if.h>
#include "ifaces.h"

static
struct cgiconf_module this_module = {
  name: "ifconf",
  main: ifconf_main,
};

char *ifnames[] = { "eth0", "eth1", "wlan0", "ath0", NULL };

static
void step0(void)
{
  int i;
  char *ifname;

  printf("<table border='2' cellspacing='0' align='center'>\n");

  for(ifname = ifnames[i=0]; ifname; ifname = ifnames[++i]){

    struct iface *iface;

    iface = iface_get_cfg(ifname);

    printf("<tr>\n");
    printf("<td>%s</td>", ifname);

    if(iface){
      struct sockaddr_in *addr;

      printf("<td>", inet_ntoa(addr->sin_addr));

      addr = (struct sockaddr_in *)(&iface->addr);
      printf("address: %s<br>", inet_ntoa(addr->sin_addr));
      addr = (struct sockaddr_in *)(&iface->netmask);
      printf("netmask: %s<br>", inet_ntoa(addr->sin_addr));
      addr = (struct sockaddr_in *)(&iface->broadaddr);
      printf("broadcast: %s<br>", inet_ntoa(addr->sin_addr));

      printf("</td>\n");

    }else
      printf("<td>unconfigured</td>\n");

    printf("</tr>\n");
  }
  printf("</table>\n");
}

int ifconf_main(void)
{
  char *step_name;

  step_name = cgi_param("step");
  if((step_name == NULL) || (strcmp(step_name, "0") == 0))
    step0();

}

void register_ifconf(void)
{
  register_cgiconf_module(&this_module);
}

