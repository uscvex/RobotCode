#ifndef __COMMON_H__
#define __COMMON_H__

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define ABS(v) (((v) < 0)?(-(v)):(v))
#define SIGN(v) ((v)<0?-1:1)
#define CLAMP(v, lower, upper) (MIN(MAX(ABS(v), (lower)), upper)*SIGN(v))
#define VALLEY(v, low, high) ((ABS(v) < (low))?0:(MIN(ABS(v), (high))*SIGN(v)))
#define DEFAULT_FEED_SPEED 90
#endif