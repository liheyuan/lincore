/*
 * main.c
 *
 *  Created on: 2011-5-10
 *      Author: liheyuan
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "run.h"

int parse_cmd(int argc, char** argv, struct RunItem* ri)
{
	int i = 1;
	int ft, fm, fi, fo, ff, fj;
	ft = fm = fi = fo = ff = fj = 1;
	unsigned long value;

	while (i < argc)
	{
		//time limit -t
		if (strlen(argv[i]) == 2 && strncmp(argv[i], "-t", 2) == 0)
		{
			i++;
			value = atoll(argv[i]);
			if (value == 0)
			{
				//error parse int value
				return 1;
			}
			else
			{
				ft = 0;
				ri->tl = value;
				i++;
				continue;
			}
		}

		//memory limit -m
		if (strlen(argv[i]) == 2 && strncmp(argv[i], "-m", 2) == 0)
		{
			i++;
			value = atoi(argv[i]);
			if (value == 0)
			{
				//error parse int value
				return 1;
			}
			else
			{
				fm = 0;
				ri->ml = value;
				i++;
				continue;
			}
		}
		//bc i -i
		if (strlen(argv[i]) == 2 && strncmp(argv[i], "-i", 2) == 0)
		{
			i++;
			strncpy(ri->bc_in, argv[i], RUNITEM_BUF_LEN);
			i++;
			fi = 0;
			continue;
		}
		//bc o -o
		if (strlen(argv[i]) == 2 && strncmp(argv[i], "-o", 2) == 0)
		{
			i++;
			strncpy(ri->bc_out, argv[i], RUNITEM_BUF_LEN);
			i++;
			fo = 0;
			continue;
		}
		//prog -f
		if (strlen(argv[i]) == 2 && strncmp(argv[i], "-f", 2) == 0)
		{
			i++;
			strncpy(ri->prog_path, argv[i], RUNITEM_BUF_LEN);
			i++;
			ff = 0;
			continue;
		}

		//jail -j
		if (strlen(argv[i]) == 2 && strncmp(argv[i], "-j", 2) == 0)
		{
			i++;
			strncpy(ri->chroot_path, argv[i], RUNITEM_BUF_LEN);
			i++;
			fj = 0;
			continue;
		}

		//Default:Unknown
		fprintf(stderr, "Unknown option:%s\n", argv[i]);
		return 1;
	}

	//Check must option
	if (ft || fm || fi || fo || ff || fj)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void print_result(struct RunItem* ri)
{
	printf("Result: %d\n", ri->result);
	printf("Time: %llu ms\n", ri->use_time);
	printf("Memory: %llu KB\n", ri->use_mem);
}

int main(int argc, char** argv)
{
	struct RunItem ri =
	{ use_mem:0, use_time:0, ol:65535, sl:8192 };

	if (parse_cmd(argc, argv, &ri))
	{
		printf("Usage:%s \n"
			"-t [d time limit (ms)] -m [d memory limit (KB)] \n"
			"-i [s bc.in file path] -o [s bc.out file path] \n"
			"-f [s prog path] -j [s jail path] \n"
			"(-sl [d stack limit (KB) ]) \n"
			"(-ol [d output limit (KB) ]) \n"
			"\n", argv[0]);
		return -1;
	}
	else
	{
		run(&ri);
		print_result(&ri);
	}

	return 0;
}
