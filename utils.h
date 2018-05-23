#ifndef __UTILS_H__
#define __UTILS_H__

#define ARRAY_SIZEOF(x) (sizeof(x)/sizeof(x[0]))

#define MIN(x, y) ((x <= y) ? x : y)
#define MAX(x, y) ((x >= y) ? x : y)

#define MSEC_TO_USEC(x) (x * 1000)
#define MSEC_TO_NSEC(x) (MSEC_TO_USEC(x) * 1000)

#endif /* __UTILS_H__ */
