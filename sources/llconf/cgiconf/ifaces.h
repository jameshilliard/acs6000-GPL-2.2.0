
struct iface
{
  short type;
  short flags;                /* various flags         */
  int metric;                 /* routing metric        */
  int mtu;                    /* MTU value             */
  int tx_queue_len;           /* transmit queue length */
  unsigned char hwaddr[6];
  struct sockaddr addr;       /* IP address            */
  struct sockaddr dstaddr;    /* P-P IP address        */
  struct sockaddr broadaddr;  /* IP broadcast address  */
  struct sockaddr netmask;    /* IP network mask       */
};

struct iface_node
{
  struct iface_node *next;
  char name[IFNAMSIZ];
  struct iface *iface;
};

#define PATH_PROCNET_DEV "/proc/net/dev"

struct iface_node *iface_get_list(void);
struct iface *iface_get_cfg(char *ifname);
void iface_get_all_cfg(struct iface_node *iflist);
