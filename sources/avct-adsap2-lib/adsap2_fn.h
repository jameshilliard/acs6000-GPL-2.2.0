#ifndef _ADSAP2_FN_H
#define _ADSAP2_FN_H

#include "adsap2.h"

/*
 * Check whether the given ADSAP2 Result indicates the user has the given target right
 *
 * @param result - The ADSAP2 Authorization Result
 * @param right  - The name of the right to check for
 *
 * @return non-zero if the result indicates the right.
 */
int  adsap2_has_target_right( adsap2_result* result, char* right );

/*
 * Check whether the given ADSAP2 Result indicates the user has the given appliance right
 *
 * @param result - The ADSAP2 Authorization Result
 * @param right  - The name of the right to check for
 *
 * @return non-zero if the result indicates the right.
 */
int  adsap2_has_appliance_right( adsap2_result* result, char* right );

#endif
