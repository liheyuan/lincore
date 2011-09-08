/*
 * test.c
 *
 * Unit test for function(s).
 *
 *  Created on: 2011-5-10
 *      Author: liheyuan
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "run.h"
#include "stat.h"
#include "util.h"

void test_mydifftime()
{
	struct timeval begin, end;

	curtime(&begin);
	usleep(5);
	curtime(&end);
	printf("Time Diff:%llu\n", mydifftime(&begin, &end));
}

void test_run()
{
	struct RunItem ri;
	//Init RunItem
	ri.use_mem = ri.use_time = 0;
	strncpy(ri.prog_path, "/tmp/test.bin", RUNITEM_BUF_LEN);
	strncpy(ri.bc_in, "/home/liheyuan/code/1000.in", RUNITEM_BUF_LEN);
	strncpy(ri.bc_out, "/home/liheyuan/code/1000.out", RUNITEM_BUF_LEN);
	strncpy(ri.chroot_path, "/home/liheyuan/jail", RUNITEM_BUF_LEN);
	ri.ml = ri.ol = 65535;
	ri.sl = 8192;
	ri.tl = 1000;

	int ret = run(&ri);
	printf("Result:%d\n", ret);
	printf("Time:%lld ms\n", ri.use_time);
	printf("Memory:%lld KB\n", ri.use_mem);

}

void test_statmem()
{
	printf("Proc mem: %llu KB", statmem(4356));
}

void test_peakmem()
{
	printf("Proc peak mem: %llu KB", peakmem(4354));
}

void test_copy()
{
	int ret = copy("/home/liheyuan/workspace_cpp/LinCore/Debug/LinCore",
			"/tmp/LinCore");
	printf("Return: %d\n", ret);
}

void test_filecmp()
{
	char * f1 = "/home/liheyuan/.xsession-errors";
	char * f2 = "/tmp/.xsession-errors";
	printf("filecmp():%d\n", filecmp(f1, f2));
}

void test_filecmp_nonblank()
{
	char * f1 = "/home/liheyuan/code/1.txt";
	char * f2 = "/home/liheyuan/code/2.txt";
	printf("filecmp_nonblank():%d\n", filecmp_nonblank(f1, f2));
}

void test_init_run()
{
	struct RunItem ri;
	//Init RunItem
	strncpy(ri.prog_path, "/tmp/test.bin", RUNITEM_BUF_LEN);
	strncpy(ri.chroot_path, "/home/liheyuan/jail/", RUNITEM_BUF_LEN);

	if (run_before(&ri))
	{
		printf("Fail to init.\n");
	}
	else
	{
		printf("Init success.\n");
	}
}

void test_is_file_dir_ok()
{
	printf("test dir:%d", isdirok("/home/liheyuan/jail"));
}

/*
 int main()
 {
 //	test_mydifftime();
 test_run();
 //	test_statmem();
 //	test_peakmem();
 //	test_copy();
 //	test_filecmp();
 //	test_filecmp_nonblank();
 //	test_init_run();
 //	test_is_file_dir_ok();
 return 0;
 }
 */
