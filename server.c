#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr
const int bufsz=1e4;
const int readBufSz=10;

void sendit(char *token, int num, int sockfd)
{
  char readBuf[readBufSz+1];
  bzero(readBuf,sizeof(readBuf));
  // char *readBuf = (char *) charalloc(readBufSz, sizeof(char));
  int fd1;
  int offset;
  fd1 = open(token, O_RDONLY);
  if (fd1 < 0)
  {
    write(sockfd, "0", sizeof("0"));
    read(sockfd, readBuf, sizeof(readBuf));
    write(sockfd, token, sizeof(token));
    read(sockfd, readBuf, sizeof(readBuf));
    return;
  }
  send(sockfd, token, sizeof(token),0);

  // hack
  // so some sends were coming empty bw this and previous
  // hence I kept reading till I receive all the empty ones
  // no idea why there are always some empty sends
  read(sockfd, readBuf, sizeof(readBuf));
  //keep reading till until I receive non empty
  // just here, other acknowledgments work fine
  while((int)readBuf[0]==0)
    read(sockfd, readBuf, sizeof(readBuf));

  offset = 0;
  int tt = (lseek(fd1, 0, SEEK_END));
  double total = tt;
  int n = tt / bufsz ;
  int rem = tt % bufsz;
  int i, j;
  // char b1[bufsz];
  lseek(fd1, 0, SEEK_SET);
  for(int i=0; i<n; i++)
  {
    char *b1 = (char *) calloc(bufsz, sizeof(char));
    read(fd1, b1, bufsz);
    send(sockfd, b1, bufsz,0);
    read(sockfd, readBuf, sizeof(readBuf));

    float per = 100*((float)i/n);
    char buf[107];
    gcvt(per, 6, buf);
    send(sockfd, buf, sizeof(buf),0);
    read(sockfd, readBuf, sizeof(readBuf));
    }

  // for remainder
  if(rem!=0)
  {
    char *r1 = (char *) calloc(rem, sizeof(char));
    read(fd1, r1, rem);
    send(sockfd, r1, rem,0);
    read(sockfd, readBuf, sizeof(readBuf));
    float per = 100;
    char buf[107];
    gcvt(per, 6, buf);
    send(sockfd, buf, sizeof(buf),0);
    read(sockfd, readBuf, sizeof(readBuf));
  }
  send(sockfd, "#done#", sizeof("#done#"),0);
  read(sockfd, readBuf, sizeof(readBuf));
  printf("sent %s\n",token);

}
// Function designed for chat between client and server.

// mx means server  will run for 1e9 times
// laptop was freezing in infinite loop
long long int cnt=0,mx=1e6;
void func(int sockfd)
{
  char *readBuf = (char *) calloc(bufsz, sizeof(char));
  
  printf("entered\n" );
  char buff[MAX];
  int n;
  // infinite loop for chat
  for (;;) {
    cnt++;
    if(cnt>mx)break;
      bzero(buff, MAX);
      read(sockfd, buff, sizeof(buff));
    if (strncmp("exit", buff, 4) == 0)
    {
      write(sockfd, buff, sizeof(buff));
      read(sockfd, readBuf, sizeof(readBuf));
      
      printf("Server Exit...\n");
      break;
    }
    char ** tokens = malloc(1024*sizeof(char *));
    long long int pos=0;
    // separated command by delimitors to separate command from argument
    char *temp =strtok(buff," ");
    while(1)
    {
      cnt++;
      if(cnt>mx)break;
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
      cnt++;
      if(cnt>mx)break;
      if(i==0)
      if(strcmp(tokens[0],"get")!=0)
      {
        write(sockfd, "Undefined command\n", sizeof("Undefined command\n"));
        read(sockfd, readBuf, sizeof(readBuf));
        break;
      }
      if(i!=0)
      if(tokens[i]!=NULL)
      {
        sendit(tokens[i], i, sockfd);
      }
      i++;
    }
  }
}

// Driver function
int main()
{
  int sockfd, connfd, len;
  struct sockaddr_in servaddr, cli;

  // socket create and verification
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
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(PORT);

  // Binding newly created socket to given IP and verification
  if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) <0) {
    printf("socket bind failed...\n");
    exit(0);
  }
  else
    printf("Socket successfully binded..\n");

  // Now server is ready to listen and verification
  if ((listen(sockfd, 5)) != 0) {
    printf("Listen failed...\n");
    exit(0);
  }
  else
    printf("Server listening..\n");
  len = sizeof(cli);

  // Accept the data packet from client and verification
  connfd = accept(sockfd, (SA*)&cli, &len);
  if (connfd < 0) {
    printf("server acccept failed...\n");
    exit(0);
  }
  else
    printf("server acccepted the client\n");

  // Function for chatting between client and server
  func(connfd);

  // After chatting close the socket
  close(sockfd);
}
