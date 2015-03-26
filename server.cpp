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

using namespace std;
struct pass
{	int thread_id;
	int client;
	struct addrinfo cli_res;
};

void * process_client(void * pth)
{

	int sockfd;			//Socket descriptor

	struct addrinfo serv,*res;
	memset(&serv,0,sizeof(serv));	//Creating a socket to connect to IPC SERVER
	serv.ai_family=AF_INET;
	serv.ai_socktype=SOCK_STREAM;

	getaddrinfo("192.168.188.196","90",&serv,&res);  //Filling struct addrinfo
	sockfd=socket(res->ai_family,res->ai_socktype,res->ai_protocol); //Creating Socket

	if(connect(sockfd,res->ai_addr,res->ai_addrlen)==-1) //Connecting to proxy
	{
		perror("error connecting");
		return 0;
	}





	
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
	int x;
	x=recv(clisockfd,buffer,LIMIT,0);
	char GET[50]={'/','r','o','o','t'};
	char Host[50]={0};
	int flag=1;
	
	int j=5;
	for(int i=0;i<x;i++)
	{
		if(flag==1&&buffer[i]=='/')
		{
	
			flag=0;
		
		}
		
		if(flag==0&&buffer[i]==' ')		// GET the name of the file from the request header
		{
	
			flag=5;
			GET[j]='\0';
		}
		if(flag==0)
		{
	
			GET[j]=buffer[i];
			j++;
		}
		
	
		cout<<buffer[i];
	
	}
	
	
	
	char home[]="home.html";
	
	if(j==6)
	{
		for(int i=0;i<9;i++) //if no file is specified GET=home.html
		GET[j+i]=home[i];
	
	}
	
	
	cout<<GET<<endl;
	
	
	

	
	ifstream file_cpy(GET);
	
	if(!file_cpy.is_open())
	{	
		char buff[50]="HTTP/1.1 404 OK";
		send(sockfd,"error.html",strlen("error.html"),0); //if file is not present send error.html
		send(clisockfd,buff,strlen(buff),0);	//Sending http header
	}
	else
	{
		char buff[50]="HTTP/1.1 200 OK";
		send(sockfd,GET,strlen(GET),0);	//if file is present send the request to ipc
	}
	

	send(clisockfd,"\n",1,0);
	send(clisockfd,"\n",1,0);
	
	char buf[LIMIT];
	
	while(1)	
	{
		x=recv(sockfd,buf,LIMIT,0);	//Receiving from ipc

		send(clisockfd,buf,x,0);	//Sending file Contents
		send(clisockfd,"\n",1,0);	//Sending file Contents
		if(x==0)
		break;
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
	
	
	
	getaddrinfo("192.168.188.196","80",&serv,&res);
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
	
		pthread_create(&threads[th++],NULL,process_client,ptr); //Process Client by creating threads.
	
	
	
	}


	
}
