/*
 * run.h
 *
 * Funtions related to run a program.
 *
 *  Created on: 2011-5-10
 *      Author: liheyuan
 */

#ifndef RUN_H_
#define RUN_H_

#include <sys/time.h>
#include <sys/types.h>

#define RUNITEM_BUF_LEN 2048

/*
 * Run Param & Result
 */
struct RunItem
{
	/* Param */
	char prog_path[RUNITEM_BUF_LEN]; //Programme to be tested
	char prog_file[RUNITEM_BUF_LEN]; //Param for exec
	char prog_jail_path[RUNITEM_BUF_LEN]; //Programme in jail path
	char prog_jail_path_temp[RUNITEM_BUF_LEN]; //Programme in jail path (relative to /)
	char chroot_path[RUNITEM_BUF_LEN];//chroot path

	unsigned long long tl; //Time limit
	unsigned long long ml; //Memory limit
	unsigned long long sl; //Stack limit (optional, default 8192)
	unsigned long long ol; //Output limit(optional,default 65535KB)

	char bc_in[RUNITEM_BUF_LEN]; //Answer input
	char bc_out[RUNITEM_BUF_LEN]; //Answer output
	char tmp_out[RUNITEM_BUF_LEN]; //Run output


	/* Result */
	unsigned long long use_mem;
	unsigned long long use_time;
	int result;

	/* Other */
	struct timeval t_begin;
	struct timeval t_end;
	pid_t cpid;
	pid_t fpid;

};

/*
 * Run a executable program in limited time.
 *
 * ri: All param & result about run.
 *
 * Return:
 * 0:ok
 * 1:runtime error
 * 2:time limit exceed
 * 3:memory related error
 * 4:wrong answer
 * 5:presentation error
 * 6:copy exe to jail fail
 * 7:add exe priv to exe fail
 * 8:wait error
 * 9:fork error
 * 10:bc_in not exists
 * 11:bc_out not exists
 * 12:jail not exists or not 755
 * 13:prog not exists
 */
int run(struct RunItem* ri);

/*
 * Init struct Runresult with many params
 *
 * ri: All param & result about run.
 *
 * Return: 0:success  1:fail
 *
 */
int run_before(struct RunItem* result);

/*
 * Check if bc(in/out), prog, jail exists
 *
 * ri: All param & result about run.
 *
 * Return: 0:success  1:fail
 */
int run_check(struct RunItem* result);

/*
 * Run in father process's while
 *
 * ri: All param & result about run.
 *
 * Return: 1 for break while, 0 not.
 */
int run_father_while(struct RunItem* ri);

/*
 * Child run after fork
 *
 */
void run_fork_child(struct RunItem* ri);

/*
 * After run in limit time.
 * Check memory usage, answer...
 * Clean up
 *
 * ri: All param & result about run.
 *
 * Return: void
 */
void run_after(struct RunItem* ri);

/*
 * Limit resources for current process
 *
 * ri: All param & result about run.
 *
 * Return: 0:ok 1:fail
 *
 */
int limitres(struct RunItem* ri);

/*
 * Set signals
 */
void setsignal();

/*
 * Set signals handler for SIGCHLD
 */
void signal_usr1(int num);

/*
 * Prepare exec params
 */
void exe_param(struct RunItem* ri);

/*
 * Copy exe to jail folder
 */
int cp_exe_jail(struct RunItem* ri);

/*
 * Set uid of child process
 *
 * Return 0:ok 1:fail
 */
int change_uid();

/*
 * Chroot path to a jail
 *
 * path: the jail path
 *
 * Return 0:ok 1:fail
 */
int chrootjail(char* path);

void update_tmval(struct timeval* p);

unsigned long long get_timediff(struct RunItem* result);

void stdinout_redirect(char* input, char* output);

#endif /* RUN_H_ */
