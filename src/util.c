/*
 * util.c
 *
 *  Created on: May 13, 2011
 *      Author: liheyuan
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "util.h"

int copy(char* src, char* dst)
{
	FILE* fp_src;
	FILE* fp_dst;
	char buf[MAX_BUF_LEN];
	size_t len;

	//Open src
	fp_src = fopen(src, "rb");
	if (!fp_src)
	{
		return 1;
	}
	//Open dst
	fp_dst = fopen(dst, "wb");
	if (!fp_dst)
	{
		fclose(fp_src);
		return 1;
	}

	//Copy
	while ((len = fread(buf, 1, MAX_BUF_LEN - 1, fp_src)) != 0)
	{
		fwrite(buf, len, 1, fp_dst);
	}

	//Close all opened files.
	fclose(fp_src);
	fclose(fp_dst);

	return 0;

}

int filecmp(char* f1, char* f2)
{
	FILE* fp1;
	FILE* fp2;
	char buf1[MAX_BUF_LEN], buf2[MAX_BUF_LEN];
	size_t len1, len2;
	int flag = 0;

	//Open src
	fp1 = fopen(f1, "r");
	if (!fp1)
	{
		return 1;
	}
	//Open dst
	fp2 = fopen(f2, "r");
	if (!fp2)
	{
		fclose(fp1);
		return 1;
	}

	//Compare each block
	while (1)
	{
		len1 = fread(buf1, 1, MAX_BUF_LEN - 1, fp1);
		len2 = fread(buf2, 1, MAX_BUF_LEN - 1, fp2);
		if (len1 != len2)
		{
			flag = 1;
			break;
		}
		if (len1 == 0)
		{
			break;
		}
		if (strncmp(buf1, buf2, len1) != 0)
		{
			flag = 1;
			break;
		}
	}

	//Close and return
	fclose(fp1);
	fclose(fp2);

	return flag;
}

int filecmp_nonblank(char* f1, char* f2)
{
	FILE* fp1;
	FILE* fp2;
	char c1 = '\0', c2 = '\0';
	size_t len1, len2;
	int flag = 0;

	//Open src
	fp1 = fopen(f1, "r");
	if (!fp1)
	{
		return 1;
	}
	//Open dst
	fp2 = fopen(f2, "r");
	if (!fp2)
	{
		fclose(fp1);
		return 1;
	}

	//Compare each block
	while (1)
	{
		while (1)
		{
			len1 = fread(&c1, 1, 1, fp1);
			if (len1 == 0)
			{
				break;
			}
			if (c1 == '\n' || c1 == '\r' || c1 == '\t' || c1 == ' ')
			{
				continue;
			}
			else
			{
				break;
			}
		}
		while (1)
		{
			len2 = fread(&c2, 1, 1, fp2);
			if (len2 == 0)
			{
				break;
			}
			if (c2 == '\n' || c2 == '\r' || c2 == '\t' || c2 == ' ')
			{
				continue;
			}
			else
			{
				break;
			}
		}

		if (len1 == 0 && len2 == 0)
		{
			//EOF exit
			break;
		}

		if (len1 == 0 || len2 == 0)
		{
			flag = 1;
			break;
		}

		//Check each char
		if (c1 != c2)
		{
			flag = 1;
			break;
		}

	}

	//Close and return
	fclose(fp1);
	fclose(fp2);

	return flag;
}

int exe_priv(char* file)
{
	return chmod(file, 00777);
}

int delete(char* path)
{
	return unlink(path);
}

int isfileok(char* path)
{
	if (access(path, F_OK) == 0 && access(path, R_OK) == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int isdirok(char* path)
{
	struct stat st;
	if (lstat(path, &st) == -1)
	{
		return 0;
	}
	//755 is required for dir.
	else if (S_ISDIR(st.st_mode) && (S_IRWXU & st.st_mode) && (S_IRGRP
			& st.st_mode) && (S_IXGRP & st.st_mode) && (S_IROTH & st.st_mode)
			&& (S_IXOTH & st.st_mode))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
