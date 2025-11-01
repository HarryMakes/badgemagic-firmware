#ifndef __LEDDRV_H__
#define __LEDDRV_H__

/* Minimal LED driver definitions for host unit tests. This shadows
 * src/leddrv.h via -Itests/include appearing before -Isrc in HOST_CFLAGS.
 */

#include <stdint.h>

#define LED_COLS 44
#define LED_ROWS 11

#endif /* __LEDDRV_H__ */
