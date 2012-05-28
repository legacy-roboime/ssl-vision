#ifndef WIN_TIME_H
#define WIN_TIME_H
#define NOGDI
#include <Windows.h>
#include <winsock.h>
#include <time.h>
const __int64 DELTA_EPOCH_IN_MICROSECS= 11644473600000000;

// ignoring the timezone
inline int gettimeofday(struct timeval* tp, void* tzp)
{
    tzp;
    __int64 freq,cnt;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    QueryPerformanceCounter((LARGE_INTEGER *)&cnt);
    tp->tv_sec = (long)((double)cnt / freq);
    tp->tv_usec = (long)(((double)cnt / freq - tp->tv_sec) * 1.0E6);
    /* 0 indicates that the call succeeded. */
    return 0;
}

inline void usleep(int waitTime)
{
    __int64 time1=0, time2=0, freq=0;
    QueryPerformanceCounter((LARGE_INTEGER *)&time1);
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    do {
        QueryPerformanceCounter((LARGE_INTEGER *) &time2);
    } while((time2 - time1) < waitTime);
}

inline struct tm* localtime_r (const time_t *clock, struct tm *result)
{
    if (!clock || !result) return NULL;
    memcpy(result,localtime(clock),sizeof(*result));
    return result;
}

#endif