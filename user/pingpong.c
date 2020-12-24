#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int main(int argc,char *argv[]){
	int pid;
	int p1[2];
	int p2[2];
	char *temp[2];
	temp[0] = "ping";
	temp[1] = "pong";
	pipe(p1);
	pipe(p2);
	pid = fork();
	if(pid < 0){
		printf("fork fail");
	}else if(pid == 0){
		write(p1[1],temp[1],sizeof(temp[1]));
		close(p1[1]);
		char buf[10];
		read(p2[0],buf,sizeof(buf));
		close(p2[0]);
		pid = getpid();
		printf("%d: received %s\n",pid,buf);
		exit();
	}else{
		write(p2[1],temp[0],sizeof(temp[0]));
		close(p2[1]);
		char buf[10];
		read(p1[0],buf,sizeof(buf));
		close(p1[0]);
		printf("%d: received %s\n",getpid(),buf);	
	}
	wait();
	exit();
}
