/*
 * memstat.h
 *
 * Functions for stat time/mem
 *
 *  Created on: 2011-5-11
 *      Author: liheyuan
 */

#ifndef MEMSTAT_H_
#define MEMSTAT_H_

/*
 * Get current time and store it in struct timeval.
 */
void curtime(struct timeval* time);

/*
 * Calculate the different time between struct timeval.
 */
unsigned long long mydifftime(struct timeval* begin, struct timeval* end);

/*
 * Stat memory usage of a process
 *
 * pid: process id of a process
 *
 * Return: Memery usage of process pid in KB
 */
unsigned long long statmem(pid_t pid);

/*
 * Stat peak memory usage of a process
 *
 * pid: process id of a process
 *
 * Return: Peak memery usage of process pid in KB
 */
unsigned long long peakmem(pid_t pid);

/*
 * Max
 */
unsigned long long max(unsigned long a, unsigned long b);

#endif /* MEMSTAT_H_ */
