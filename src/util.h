/*
 * util.h
 *
 *  Created on: May 13, 2011
 *      Author: liheyuan
 */

#ifndef UTIL_H_
#define UTIL_H_

#define MAX_BUF_LEN 2048

/*
 * Copy file (using binary mode).
 */
int copy(char* src, char* dst);

/*
 * Add execute priv. to file.
 */
int exe_priv();

/*
 * Compare 2 file to see difference
 *
 * f1: filename of File 1
 * f2: filename of File 2
 *
 * Return 0:totally same 1:not same
 */
int filecmp(char* f1, char* f2);

/*
 * Compare 2 file to see difference without consider blank
 *
 * f1: filename of File 1
 * f2: filename of File 2
 *
 * Return 0:totally same 1:not same
 */
int filecmp_nonblank(char* f1, char* f2);

/*
 * Delete file
 *
 * path: file to be delete
 */
int delete(char* path);

/*
 * Check if file exists and premit read
 *
 * Return 1 for yes, 0 for no
 */
int isfileok(char* path);

/*
 * Check if dir exists and premit 777
 *
 * Return 1 for yes, 0 for no
 */
int isdirok(char* path);

#endif /* UTIL_H_ */
