#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#define MAX 1000000
#define PORT 8080
#define SA struct sockaddr
#define ll long long int
void func(int sockfd)
{
	char buff[MAX + 1];
	for (;;)
  {
    bzero(buff, sizeof(buff));
    printf("client> ");
    char str[1024], cp[1024];
    scanf("%[^\n]%*c", str);
    for(int i=0; i<1024; i++)
    {
      cp[i] = str[i];
    }
    char ** tokens = malloc(1024*sizeof(char *));
    ll pos=0;
    // separated command by delimitors to separate command from argument
    char *temp =strtok(str," ");

    // tokens will store all file names
    while(1)
    {
        if(temp==NULL)
        {
            break;
        }
        // stored
        tokens[pos]=temp;
        pos++;
        temp =strtok(NULL," ");
    }

    int i=0;
    tokens[pos]=NULL;

    while(tokens[i]!=NULL)
    {
      i++;
    }
		int totFiles = i-1;

    if(strcmp(tokens[0],"exit")==0)
    {
      write(sockfd, tokens[0], sizeof(tokens[0]));
      read(sockfd, buff, sizeof(buff));
			write(sockfd, "ack", sizeof("ack"));

      printf("From Server : %s", buff);
      if ((strncmp(buff, "exit", 4)) == 0) {
        printf("Client Exit...\n");
        break;
      }
    }

    if(strcmp(tokens[0],"get")!=0)
    {
      printf("Undefined-command\n");
      break;
    }

    // send entire input to client
    write(sockfd,cp, sizeof(cp));
    char ack[]="ack";
    for(i=0;i<totFiles;i++)
    {
      bzero(buff, sizeof(buff));
      read(sockfd, buff, sizeof(buff));
			write(sockfd, ack, sizeof(ack));
      if(strcmp(buff,"0")==0)
      {
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
				write(sockfd, "ack", sizeof("ack"));
        printf("could not open file %s\n", buff);
        continue;
      }

      if((strncmp(buff, "2", 1)) == 0)
        break;

      // else received file name and stored in buff

      int fd2 = open(buff, O_CREAT | O_RDWR | O_TRUNC, 0600);
      if(fd2 < 0)
        perror("Error opening newfile: ");
      char percent[MAX+100];
      while(1)
      {
        // int fd2 = open(filename, O_CREAT | O_RDWR | O_TRUNC, 0600);
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
				write(sockfd, "ack", sizeof("ack"));

        if(strcmp(buff, "#done#") == 0)
        {
          break;
        }
        write(fd2, buff, strlen(buff));
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
				write(sockfd, "ack", sizeof("ack"));	
        printf("\033[1;33m");			
        printf("Percentage Completed:%s\r",buff);
        printf("\033[0m");
        // fflush(stdout);
      }
      printf("\033[1;32m"); 

      // extra spaces to overwrite overall decimal
      printf("Percentage Completed:100               \r");
      
      printf("\033[0m");
      printf("\n");

      // closing after downloading and writing into it
      close(fd2);
    }
	}
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and varification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	// function for chat
	func(sockfd);

	// close the socket
	close(sockfd);
}
