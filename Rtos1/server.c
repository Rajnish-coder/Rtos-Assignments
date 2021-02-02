#include<stdio.h>
#include<netdb.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<errno.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include<signal.h>

#define PORT "5008"
#define BACKLOG 5

void *get_in_addr(struct sockaddr *sa)
{
	if(sa->sa_family == AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main()
{
	int sockfd,new_fd;
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr;
	socklen_t sin_size;
	char s[INET6_ADDRSTRLEN];
	int rv,yes=1;

	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if((rv = getaddrinfo(NULL,PORT,&hints,&servinfo))!=0){
		fprintf(stderr,"getaddrinfo: %s\n",gai_strerror(rv));
		return 1;
	}

	for(p = servinfo; p!=NULL; p=p->ai_next){
		if((sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
			perror("server: socket");
			continue;
		}
		if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1){
			perror("setsockopt");
			exit(1);
		}
		if(bind(sockfd,p->ai_addr,p->ai_addrlen)==-1){
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}
	freeaddrinfo(servinfo);

	if(p==NULL){
		fprintf(stderr,"server: failed to bind\n");
		exit(1);
	}
	if(listen(sockfd,BACKLOG)==-1){
		perror("listen");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	char buff[50];

	
		sin_size = sizeof(their_addr);
		new_fd = accept(sockfd,(struct sockaddr *)&their_addr,&sin_size);
		if(new_fd == -1){
			perror("accept");
			continue;
		}
              
                memset(buff,0,sizeof(buff));
		inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s,sizeof(s));
		printf("server: got connection from %s\n",s);
               while(1){
		if(!fork()){
			close(sockfd);
			printf("enter the msg\n");
			scanf("%s",buff);
			if(send(new_fd,buff,sizeof(buff),0)==-1)
				perror("send");
			if(strcmp(buff,"exit")==0)
				exit(1);
			memset(buff,0,sizeof(buff));
		}
                
	       }

	return 0;
}
