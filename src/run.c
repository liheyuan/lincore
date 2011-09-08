/*
 * run.c
 *
 * Funtions related to run a program.
 *
 *  Created on: 2011-5-10
 *      Author: liheyuan
 */

#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <strings.h>
#include <signal.h>
#include <math.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>

#include "run.h"
#include "stat.h"
#include "util.h"

static struct timeval* pTV = NULL;
pthread_mutex_t mutex =
PTHREAD_MUTEX_INITIALIZER;

int run(struct RunItem *ri)
{
	//pid & runtime
	pid_t cpid;

	//Check for run environment
	if (run_check(ri))
	{
		return ri->result;
	}

	//Init for run
	if (run_before(ri))
	{
		return ri->result;
	}

	//Fork child
	ri->cpid = cpid = fork();

	if (cpid == -1)//Fork error
	{
		ri->result = 8;
		return ri->result;
	}//End Fork Error
	else if (cpid == 0)//Child
	{
		//Child run after fork
		run_fork_child(ri);

		exit(0);
	}//End Child
	else//Father
	{
		while (1)
		{
			if (run_father_while(ri))
			{
				break;
			}
		}//end while

		//Check memory,update use time and check answer
		run_after(ri);

		return ri->result;
	}//end Father
}

int run_father_while(struct RunItem* ri)
{
	useconds_t tm = 1000 * 5;//sleep 5ms
	int ret, status;

	//update end time & peak mem
	update_tmval(&ri->t_end);
	ri->use_mem = max(ri->use_mem, peakmem(ri->cpid));

	//Unblocking wait child
	ret = waitpid(ri->cpid, &status, WNOHANG);
	if (ret == -1)
	{
		//wait error -> 8:wait error
		ri->result = 8;
		return 1;
	}
	else if (ret == ri->cpid)
	{
		//Child has exit, check exit status
		if (
		WIFEXITED(status))
		{
			ri->result = 0;
			return 1;
		}
		else
		{
			//Previous error has set (tle)
			if (ri->result != 0)
			{
				return 1;
			}

			//not terminated normally -> 1:runtime error or 3:memory limit exceed
			if (WIFSIGNALED(status) &&
			WTERMSIG(status) == SIGSEGV)
			{
				ri->result = 3;
			}
			else
			{
				ri->result = 1;
			}
			return 1;
		}
	}
	else
	{
		//Not exit yet, Check if timeout
		if (get_timediff(ri) >= ri->tl)
		{
			//Timeout
			kill(ri->cpid, 9);
			ri->result = 2;
			//Next loop to wait()==cpid and exit.
			return 0;
		}
		else
		{
			//Not timeout, sleep
			usleep(tm);
			return 0;
		}
	}
}

int run_check(struct RunItem* ri)
{
	//check bc
	if (isfileok(ri->bc_in) == 0)
	{
		ri->result = 10;
		return 1;
	}
	if (isfileok(ri->bc_out) == 0)
	{
		ri->result = 11;
		return 1;
	}

	//jail
	if (isdirok(ri->chroot_path) == 0)
	{
		ri->result = 12;
		return 1;
	}

	//check prog
	if (isfileok(ri->prog_path) == 0)
	{
		ri->result = 13;
		return 1;
	}

	return 0;
}

int run_before(struct RunItem* ri)
{
	//Init RunItem
	ri->use_mem = ri->use_time = ri->cpid = ri->result = 0;
	bzero(&ri->t_begin, sizeof(struct timeval));
	bzero(&ri->t_end, sizeof(struct timeval));
	pTV = &(ri->t_begin);
	ri->fpid = getpid();

	//Init RunItem->prog_file
	exe_param(ri);

	//Copy exe to jail evn.
	if (cp_exe_jail(ri))
	{
		ri->result = 6;
		return 1;
	}

	//Change exe in jail a+rwx
	if (exe_priv(ri->prog_jail_path_temp))
	{
		ri->result = 7;
		return 1;
	}

	//Init tmp_out
	tmpnam(ri->tmp_out);

	//Registor signals
	setsignal();

	return 0;
}

void run_fork_child(struct RunItem* ri)
{
	//Set limit res/jail
	limitres(ri);
	stdinout_redirect(ri->bc_in, ri->tmp_out);
	chrootjail(ri->chroot_path);
	kill(ri->fpid, SIGUSR1);
	change_uid();

	//execute other
	if (execl(ri->prog_jail_path, ri->prog_file, NULL) == -1)
	{
		printf("Can't execl.\n");
		printf("%s\n", strerror(errno));
	}
}

void run_after(struct RunItem* ri)
{
	//Update Time result
	ri->use_time = get_timediff(ri);

	if (ri->result != 0)
	{
		return;
	}

	//Check memory usage
	if (ri->use_mem >= ri->ml)
	{
		ri->result = 3;
	}

	//Check Answer
	if (filecmp(ri->bc_out, ri->tmp_out) == 0)
	{
		ri->result = 0;
		return;
	}
	else
	{
		//WA or PE?
		if (filecmp_nonblank(ri->bc_out, ri->tmp_out) == 0)
		{
			//PE
			ri->result = 5;
		}
		else
		{
			//WA
			ri->result = 4;
		}
	}

	//Clean up fail/exe tmp out
	printf("%s\n", ri->tmp_out);
	delete(ri->tmp_out);
	delete(ri->prog_jail_path_temp);
}

int limitres(struct RunItem* ri)
{
	struct rlimit r_mem, r_stack, r_fsize;

	//Set limit of memory size
	r_mem.rlim_cur = r_mem.rlim_max = ri->ml * 1024L;
	if (setrlimit(RLIMIT_AS, &r_mem) != 0)
	{
		return 1;
	}

	//Set limit of stack size
	r_stack.rlim_cur = r_stack.rlim_max = ri->sl * 1024L;
	if (setrlimit(RLIMIT_STACK, &r_stack) != 0)
	{
		return 1;
	}

	//Set limit of RLIMIT_FSIZE to 128MB
	r_fsize.rlim_cur = r_fsize.rlim_max = ri->ol * 1024L;
	if (setrlimit(RLIMIT_FSIZE, &r_fsize) != 0)
	{
		return 1;
	}

	return 0;
}

void setsignal()
{
	signal(SIGUSR1, signal_usr1);
}

void signal_usr1(int num)
{
	update_tmval(pTV);
	return;
}

void exe_param(struct RunItem* ri)
{
	char* prog = ri->prog_path;
	int len = strlen(prog);
	int pos = 0;
	int i = 0;
	char* p = ri->prog_file;

	//find last '/'
	for (i = len - 1; i >= 0; i--)
	{
		if (prog[i] == '/')
		{
			pos = i;
			break;
		}
	}
	if (prog[pos] == '/')
	{
		pos++;
	}

	//copy file name to prog_file
	strcpy(p, &prog[pos]);

}

int change_uid()
{
	if (setuid(65534) != 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int chrootjail(char* path)
{
	if (chdir(path) == -1)
	{
		return 1;
	}
	else
	{
		if (chroot(path) == -1)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
}

int cp_exe_jail(struct RunItem* ri)
{
	int len;
	char*tmpath = ri->prog_jail_path_temp;

	//Make tmp cp dst path
	strncpy(tmpath, ri->chroot_path, RUNITEM_BUF_LEN - 1);
	len = strlen(tmpath);
	if (tmpath[len - 1] != '/')
	{
		tmpath[len] = '/';
		tmpath[len + 1] = '\0';
	}
	strncat(tmpath, ri->prog_file, RUNITEM_BUF_LEN - 1);

	//Make jail path
	ri->prog_jail_path[0] = '/';
	ri->prog_jail_path[1] = '\0';
	strncat(ri->prog_jail_path, ri->prog_file, RUNITEM_BUF_LEN - 1);

	//Copy exe to jail folder
	return copy(ri->prog_path, ri->prog_jail_path_temp);
}

void update_tmval(struct timeval* p)
{
	curtime(p);
}

unsigned long long get_timediff(struct RunItem* result)
{
	return mydifftime(&result->t_begin, &result->t_end);
}

void stdinout_redirect(char* input, char* output)
{
	//reopen stdin
	if (close(STDIN_FILENO) != -1)
	{
		open(input, O_RDONLY);
	}

	//reopen stdout
	if (close(STDOUT_FILENO) != -1)
	{
		open(output, O_WRONLY | O_TRUNC | O_CREAT, 00777);
	}

}

