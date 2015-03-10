#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include "ifaces.h"

static
char *_iface_parse_name(char *name, char *p)
{
  while (isspace(*p))
    p++;
  while (*p) {
    if (isspace(*p))
      break;
    if (*p == ':') {        /* could be an alias */
      char *dot = p++;
      while (*p && isdigit(*p)) p++;
      if (*p == ':') {
	/* Yes it is, backup and copy it. */
	p = dot;
	*name++ = *p++;
	while (*p && isdigit(*p)) {
	  *name++ = *p++;
	}
      } else {
	/* No, it isn't */
	p = dot;
      }
      p++;
      break;
    }
    *name++ = *p++;
  }
  *name++ = '\0';
  return p;
}

struct iface_node *iface_get_list(void)
{
  FILE *fptr;
  char buf[512];
  struct iface_node *ifnode, *ifnode_list = NULL, *ifnode_prev = NULL;

  fptr = fopen(PATH_PROCNET_DEV, "r");
  if(!fptr)
    return NULL;

  fgets(buf, sizeof(buf), fptr); /* eat line */
  fgets(buf, sizeof(buf), fptr);

  while (fgets(buf, sizeof(buf), fptr)) {
    char *s, name[IFNAMSIZ];

    s = _iface_parse_name(name, buf);

    ifnode = malloc(sizeof(struct iface_node));
    memset(ifnode, 0, sizeof(struct iface_node));
    strcpy(ifnode->name, name);

    if(ifnode_list)
      ifnode_prev->next = ifnode;
    else
      ifnode_list = ifnode;
    ifnode_prev = ifnode;
  }
  return ifnode_list;
}

struct iface *iface_get_cfg(char *ifname)
{
  struct iface *iface;
  struct ifreq ifr;
  int fd;
  
  fd = socket(AF_INET, SOCK_DGRAM, 0);

  iface = malloc(sizeof(struct iface));
  memset(iface, 0, sizeof(struct iface));

  strcpy(ifr.ifr_name, ifname);
  if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0)
    return (NULL);
  iface->flags = ifr.ifr_flags;

  strcpy(ifr.ifr_name, ifname);
  if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0)
    memset(iface->hwaddr, 0, 6);
  else
    memcpy(iface->hwaddr, ifr.ifr_hwaddr.sa_data, 6);
  
  iface->type = ifr.ifr_hwaddr.sa_family;
  
  strcpy(ifr.ifr_name, ifname);
  if (ioctl(fd, SIOCGIFMETRIC, &ifr) < 0)
    iface->metric = 0;
  else
    iface->metric = ifr.ifr_metric;
  
  strcpy(ifr.ifr_name, ifname);
  if (ioctl(fd, SIOCGIFMTU, &ifr) < 0)
    iface->mtu = 0;
  else
    iface->mtu = ifr.ifr_mtu;
  
  strcpy(ifr.ifr_name, ifname);
  if (ioctl(fd, SIOCGIFTXQLEN, &ifr) < 0)
    iface->tx_queue_len = -1; /* unknown value */
  else
    iface->tx_queue_len = ifr.ifr_qlen;
                   
  strcpy(ifr.ifr_name, ifname);
  ifr.ifr_addr.sa_family = AF_INET;
  if(ioctl(fd, SIOCGIFADDR, &ifr) == 0) {
    iface->addr = ifr.ifr_addr;
    strcpy(ifr.ifr_name, ifname);
    if (ioctl(fd, SIOCGIFDSTADDR, &ifr) < 0)
      memset(&iface->dstaddr, 0, sizeof(struct sockaddr));
    else
      iface->dstaddr = ifr.ifr_dstaddr;
    
    strcpy(ifr.ifr_name, ifname);
    if(ioctl(fd, SIOCGIFBRDADDR, &ifr) < 0)
      memset(&iface->broadaddr, 0, sizeof(struct sockaddr));
    else
      iface->broadaddr = ifr.ifr_broadaddr;
    
    strcpy(ifr.ifr_name, ifname);
    if(ioctl(fd, SIOCGIFNETMASK, &ifr) < 0)
      memset(&iface->netmask, 0, sizeof(struct sockaddr));
    else
      iface->netmask = ifr.ifr_netmask;
  }else
    memset(&iface->addr, 0, sizeof(struct sockaddr));

  return iface;
}
    
void iface_get_all_cfg(struct iface_node *iflist)
{
  struct iface_node *ifnode;

  for(ifnode = iflist; ifnode; ifnode = ifnode->next){
    if(ifnode->iface)
      free(ifnode->iface);
    ifnode->iface = iface_get_cfg(ifnode->name);
  }
}

