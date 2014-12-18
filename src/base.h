/*
 * base.h
 *
 *  Created on: Jul 25, 2014
 *      Author: lingyun
 */

#ifndef BASE_H_
#define BASE_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define  OK          0
#define  ERROR      -1
#define  AGAIN      -2
#define  BUSY       -3
#define  DONE       -4
#define  DECLINED   -5
#define  ABORT      -6


#endif /* BASE_H_ */
