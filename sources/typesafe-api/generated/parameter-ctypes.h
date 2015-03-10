/*
 * This file defines additional structures and enumerated types used for
 * the typesafe API.
 *
 * TODO: types.conf, this file, and the mechanism for generating the typw
 * wrappers is probably about 57 times more complex than would appear to be
 * necessary.
 */

#include <netinet/in.h>

/*
 * This is the C type used for the ipAddress parameter type.
 * It may be empty (family==AF_UNSPEC), contain an IPv4 address (family=AF_INET)
 * or it may contain an IPv6 address (family=AF_INET6).
 */
struct ip_addr {
	int family; /* AF_UNSPEC, AF_INET or AF_INET6 */
	union {
		struct in_addr ip4; /* Only valid if family==AF_INET */
		struct in6_addr ip6; /* Only valid if family==AF_INET6 */
	} ip;
};
