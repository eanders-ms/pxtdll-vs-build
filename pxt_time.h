#ifndef __PXT_TIME_H__
#define __PXT_TIME_H__

#ifdef _MSC_VER

int gettimeofday(struct timeval* tp, struct timezone* tzp);
//int nanosleep(const struct timespec* req, struct timespec* rem);

#endif
#endif