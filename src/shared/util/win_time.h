#ifndef _WIN_TIME_
#define _WIN_TIME_
#include <windows.h>
#include <time.h>
const __int64 DELTA_EPOCH_IN_MICROSECS= 11644473600000000;

/* IN UNIX the use of the timezone struct is obsolete;
I don't know why you use it. See http://linux.about.com/od/commands/l/blcmdl2_gettime.htm
But if you want to use this structure to know about GMT(UTC) diffrence from your local time
it will be next: tz_minuteswest is the real diffrence in minutes from GMT(UTC) and a tz_dsttime is a flag
indicates whether daylight is now in use
*/
struct timezone2 
{
	__int32  tz_minuteswest; /* minutes W of Greenwich */
	bool  tz_dsttime;        /* type of dst correction */
};

int gettimeofday(struct timeval *tv/*in*/, struct timezone2 *tz/*in*/)
{
	FILETIME ft;
	__int64 tmpres = 0;
	TIME_ZONE_INFORMATION tz_winapi;
	int rez=0;

	ZeroMemory(&ft,sizeof(ft));
	ZeroMemory(&tz_winapi,sizeof(tz_winapi));

	GetSystemTimeAsFileTime(&ft);

	tmpres = ft.dwHighDateTime;
	tmpres <<= 32;
	tmpres |= ft.dwLowDateTime;

	/*converting file time to unix epoch*/
	tmpres /= 10;  /*convert into microseconds*/
	tmpres -= DELTA_EPOCH_IN_MICROSECS; 
	tv->tv_sec = (__int32)(tmpres*0.000001);
	tv->tv_usec =(tmpres%1000000);


	//_tzset(),don't work properly, so we use GetTimeZoneInformation
	rez=GetTimeZoneInformation(&tz_winapi);
	tz->tz_dsttime=(rez==2)?true:false;
	tz->tz_minuteswest = tz_winapi.Bias + ((rez==2)?tz_winapi.DaylightBias:0);

	return 0;
}

inline struct tm* localtime_r (const time_t *clock, struct tm *result) {
       if (!clock || !result) return NULL;
       memcpy(result,localtime(clock),sizeof(*result));
       return result;
}

void usleep(int waitTime) {
    __int64 time1 = 0, time2 = 0, freq = 0;

    QueryPerformanceCounter((LARGE_INTEGER *) &time1);
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);

    do {
        QueryPerformanceCounter((LARGE_INTEGER *) &time2);
    } while((time2-time1) < waitTime);
}

#endif