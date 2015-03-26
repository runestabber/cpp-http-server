#include <unistd.h>
#include <string>
#include <stdio.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fstream>
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define LIMIT 2000
#include <stdlib.h>
#include <pthread.h>


#define LIMIT 2000
#define myfifo "/temp/FIFO"
using namespace std;struct pass
{	int thread_id;
	int client;
	struct addrinfo cli_res;
};

void * process_client(void * pth)
{	
	char buffer[LIMIT];
	pass *arg=(pass *)pth;
	cout<<"Executing "<<arg->thread_id<<endl;
	int clisockfd=arg->client;
	struct addrinfo cli=arg->cli_res;
	

	struct  addrinfo *cli_res;
	cout<<"Client Connected :";
	char *ip;
	
	cli_res=&cli;
	struct sockaddr_in *sin = (struct sockaddr_in *) cli_res;	
  	ip = inet_ntoa(sin->sin_addr);
  	cout<<ip<<endl;
  	
	int x=recv(clisockfd,buffer,LIMIT,0); //Receiving the filename
	
	
	ifstream file_cpy(buffer);	//Opening the file
	
	cout<<buffer<<endl;
	
	
	
	
	while(file_cpy.getline(buffer,LIMIT))	//Reading from file
	{
		cout<<buffer<<"\n";
		send(clisockfd,buffer,strlen(buffer),0);	//Sending file Contents
		send(clisockfd,"\n",1,0);
	}

	

	cout<<"Done\n";
	
	close(clisockfd);


}
int main()
{
	
	int sockfd,clisockfd,portno;
	char buffer[LIMIT];	
	char buff;
	struct addrinfo serv,*res,cli,*cli_res;
	memset(&serv,0,sizeof (serv));
	serv.ai_family=AF_UNSPEC;			//Creating Sockets
	serv.ai_socktype=SOCK_STREAM;
	serv.ai_flags=AI_PASSIVE;
	
	
	
	getaddrinfo("192.168.188.196","90",&serv,&res);
	sockfd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	
	
	if(bind(sockfd,res->ai_addr,res->ai_addrlen)<0)
	{
		perror("Error Binding :");
		return 0;
	}
	
	
	pthread_t threads[100];
	int clients=0;
	int th=0;
	
	
	while(1)
	{
		cout<<"Server Started\n";
		listen(sockfd,100);		//ready to accept connections
		int cli_len=sizeof(cli);
		clisockfd=accept(sockfd,(struct sockaddr *)&cli,(socklen_t *)&cli_len);
	
		pass * arg=new pass();
		arg->thread_id=th;
		arg->client=clisockfd;
		arg->cli_res=cli;
		void *ptr=(void *)arg;
	
		pthread_create(&threads[th++],NULL,process_client,ptr);		//Processing Client by creating Thread
	
	
	
	}

}
