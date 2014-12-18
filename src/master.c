/*
 * master.c
 *
 *  Created on: Jul 25, 2014
 *      Author: lingyun
 */

#include "master.h"

#define LISTENQ 20

const char *local_address = "127.0.0.1";
const char *reg_ip_info = "127.0.0.1:5000";
int         port = 5000;

static void setnonblocking(int sock);
static void reg_to_monitor(int epfd);
static int connect_to_monitor(const char *target_ip, int target_port);
static int write_ipinfo_to_monitor(int connfd);


static int connect_to_monitor(const char *target_ip, int target_port) {
	int                s;
	struct sockaddr_in addr;
	int                rc;
	int                err;

	s = socket(AF_INET, SOCK_STREAM, 0);

	if(s == -1)
	{
		//log_error
		printf("build socket has error\n");
		return ERROR;
	}

	if (setnonblocking(s) == -1) {
		//log error
		printf("setnonblocking has error");
		return ERROR;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(target_port);
	addr.sin_addr.s_addr = inet_addr(target_ip);

	rc = connect(s, (struct sockaddr *)&addr, sizeof(addr);

    if (rc == -1) {
        err = errno;


        if (err != EINPROGRESS)
        {
            if (err == ECONNREFUSED
                || err == EAGAIN
                || err == ECONNRESET
                || err == ENETDOWN
                || err == ENETUNREACH
                || err == EHOSTDOWN
                || err == EHOSTUNREACH)
            {
            	return DECLINED;
            }
        }

		if (rc == -1) {

			/* EINPROGRESS */

			return AGAIN;
		}
    }

    return s;
}

static
int setnonblocking(int sock){
	int opts;
	opts=fcntl(sock,F_GETFL);
	if(opts<0){
		perror("fcntl_get");
		return -1;
	}
	opts=opts|O_NONBLOCK;
	if(fcntl(sock,F_SETFL,opts)<0){
		perror("fcntl_set");
		return -1;
	}

	return opts;
}

static
int reg_to_monitor(int epfd) {
	int                  conn_to_fd;
	struct epoll_event   ev;
	int                  rc;
	int                  error_no;


	rc = connect_to_monitor();
	if(rc == ERROR){
		//release resource
		return ERROR
	}

    if (rc == AGAIN) {
        //add to timer for re-connect
        return;
    }

	conn_to_fd = ret;

	ev.data.fd=conn_to_fd;
	ev.events = EPOLLET|EPOLLIN;
	epoll_ctl(epfd, EPOLL_CTL_ADD, conn_to_fd, &ev);
	printf("conn_to_fd %d, add 2 epoll\n", conn_to_fd);


	if(write_ipinfo_to_monitor(conn_to_fd) == -1){
		close(conn_to_fd);
		return -1;
	}

	//process write timeout

	printf("register master %s to monitor down\n");
}

void
master_start() {

	int 			   yes = 1;

	int                listenfd;
	int                nfds;
	int                conn_from_fd;
	int                conn_to_fd;

	struct epoll_event ev;
	struct epoll_event ev_new_connect;
	struct epoll_event events[20];
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;

	printf("master_start now\n");

	epfd = epoll_create(256);

	if(epfd == -1) {
		perror();
		goto failed;
	}

	reg_to_monitor(epfd);

	listenfd = socket(PF_INET,SOCK_STREAM,0);
	setnonblocking(listenfd);

	//设置套接口选项
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));

	ev.data.fd = listenfd;
	ev.events=EPOLLET | EPOLLIN;
	epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);

	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	inet_pton(AF_INET,local_address,&(serveraddr.sin_addr));
	serveraddr.sin_port=htons(port);
	bind(listenfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	listen(listenfd,LISTENQ);

	while(true){
		nfds=epoll_wait(epfd,events,20,500);
		//printf("epoll_wait execute return, widh nfds=%d\n", nfds);
		for(i=0;i<nfds;++i){
			ev = events[i];
			if(ev.data->fd==listenfd){
				clilen=sizeof(clientaddr);
				conn_from_fd=accept(listenfd,(struct sockaddr*)&clientaddr,&clilen);
				if(conn_from_fd<0){
					perror("conn_from_fd<0");
					exit(1);
				}
				printf("[ %d ] conn_from_fd accepted.\n", conn_from_fd);
				setnonblocking(conn_from_fd);
				char *str=inet_ntoa(clientaddr.sin_addr);
				printf("accept a connection from %s\n",str);

				ev.data.fd = conn_from_fd;
				ev.events=EPOLLET | EPOLLIN;
				epoll_ctl(epfd,EPOLL_CTL_ADD,conn_from_fd,&ev);
			}
			else if(events[i].events & EPOLLIN) {
				ev = events[i];
				sockfd = ev.date->fd;

				printf("fd: %d has epollin event\n", sockfd);

				/*
				while(1){
					n=read(sockfd,line,MAXLINE);
					if(n<0){
						if(errno==EAGAIN || errno==EWOULDBLOCK){
							//printf("[ %d ] EAGAIN or EWOULDBLOCK, buffer no data, n=%d, break\n", sockfd, n);

							ev.data.ptr = event_ptr;
							ev.events = EPOLLOUT|EPOLLET;
							epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
							break;
						}
					}
					else if(n==0){
						printf("[ %d ] client close socket, n=%d, break\n", sockfd, n);
						epoll_ctl(epfd,EPOLL_CTL_DEL,sockfd,&ev);
						epoll_event_ptr_free(event_ptr);
						close(sockfd);
						break;
					}
					else{
						line[n]='\0';
						event_ptr->epoll_buf = epoll_buf_append(event_ptr->epoll_buf, line);
						printf("[ %d ]client send %s\n",sockfd, line);
					}
				}
				*/
			}
			else if(events[i].events & EPOLLOUT) {
				ev = events[i];
				sockfd= ev.date->fd;
				printf("fd: %d has epollout event\n", sockfd);

				//printf("write to client %d bytes\n", edata.epoll_buf->len);
				//printf("write data is %s\n", edata.epoll_buf->data);

				/*
				nwrite = write(sockfd,event_ptr->epoll_buf->data,event_ptr->epoll_buf->len);
				if(nwrite < 0){
					perror("write");
					epoll_ctl(epfd,EPOLL_CTL_DEL,sockfd,&ev);
					epoll_event_ptr_free(event_ptr);
					close(sockfd);
					break;
				}
				epoll_buf_reset(event_ptr->epoll_buf);
				ev.data.ptr = event_ptr;
				ev.events=EPOLLIN|EPOLLET;
				epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
				*/
			}
		}
	}

	close(epfd);
}
