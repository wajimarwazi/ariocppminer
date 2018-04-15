#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <sys/time.h>
#endif

#include <stdint.h>
#include <string>

#ifdef _MSC_VER
#define timeval _timeval

typedef struct timeval {
	long tv_sec;
	long tv_usec;
} _timeval;

// https://stackoverflow.com/questions/10905892/equivalent-of-gettimeday-for-windows
int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
	// Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
	// This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
	// until 00:00:00 January 1, 1970 
	static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

	SYSTEMTIME  system_time;
	FILETIME    file_time;
	uint64_t    time;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	time = ((uint64_t)file_time.dwLowDateTime);
	time += ((uint64_t)file_time.dwHighDateTime) << 32;

	tp->tv_sec = (long)((time - EPOCH) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	return 0;
}
#endif

struct timeval prev_tv = { 0, 0 };

// generates unique ID, like PHP does
std::string uniqid()
{
	int sec, usec;
	struct timeval tv;

	do {
		(void)gettimeofday((struct timeval *) &tv, (struct timezone *) NULL);
	} while (tv.tv_sec == prev_tv.tv_sec && tv.tv_usec == prev_tv.tv_usec);

	prev_tv.tv_sec = tv.tv_sec;
	prev_tv.tv_usec = tv.tv_usec;

	sec = (int)tv.tv_sec;
	usec = (int)(tv.tv_usec % 0x100000);

	char result[1024];
	snprintf(result, sizeof(result), "%08x%05x", sec, usec);

	return result;
}
