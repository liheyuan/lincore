/*
 * memstat.c
 *
 *  Created on: 2011-5-11
 *      Author: liheyuan
 */

#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include "stat.h"

#define PROC_BUF_SIZE 4096

void curtime(struct timeval* time)
{
	gettimeofday(time, NULL);
}

unsigned long long mydifftime(struct timeval* begin, struct timeval* end)
{
	if (begin->tv_sec == 0 && begin->tv_usec == 0)
	{
		return 0;
	}
	else
	{
		long long diff = (end->tv_sec - begin->tv_sec) * 1000000
				+ (end->tv_usec - begin->tv_usec);

		if (diff < 0)
		{
			return 0;
		}

		diff /= 1000;
		return diff;
	}
}

unsigned long long statmem(pid_t pid)
{
	char filename[PROC_BUF_SIZE];
	unsigned long mem = 0;
	FILE* fp = NULL;
	int i;

	//Open proc stat
	snprintf(filename, PROC_BUF_SIZE, "/proc/%d/stat", pid);
	fp = fopen(filename, "r");
	if (!fp)
	{
		return 0;
	}

	/*
	 * Analysis the string and get vsize
	 * http://www.kernel.org/doc/man-pages/online/pages/man5/proc.5.html
	 * Under section /proc/[pid]/stat
	 * Need skip 22 fileds
	 */
	for (i = 0; i < 22; i++)
	{
		fscanf(fp, "%s", filename);
	}
	fscanf(fp, "%lu", &mem);
	mem /= 1024;

	//Close proc stat
	fclose(fp);

	return mem;
}

unsigned long long peakmem(pid_t pid)
{
	char filename[PROC_BUF_SIZE];
	unsigned long mem = 0;
	FILE* fp = NULL;
	int i;

	//Open proc stat
	snprintf(filename, PROC_BUF_SIZE, "/proc/%d/status", pid);
	fp = fopen(filename, "r");
	if (!fp)
	{
		return 0;
	}

	/*
	 * Analysis the string and get vsize
	 * http://www.kernel.org/doc/man-pages/online/pages/man5/proc.5.html
	 * Under section /proc/[pid]/status
	 * Need skip 10 line and 1 string
	 */
	for (i = 0; i < 10; i++)
	{
		if (!fgets(filename, PROC_BUF_SIZE, fp))
		{
			fclose(fp);
			return 0;
		}
	}
	fscanf(fp, "%s", filename);
	fscanf(fp, "%lu", &mem);

	//Close proc stat
	fclose(fp);

	return mem;
}

unsigned long long max(unsigned long a, unsigned long b)
{
	if (a >= b)
	{
		return a;
	}
	else
	{
		return b;
	}
}
