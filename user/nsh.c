
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int getcmd(char *buf, int nbuf);
void getargv(char *cmd, char* argv[],int* argc);
void runcmd(char*argv[],int argc);
void execPipe(char*argv[],int argc); 
 
#define MAXARGS 10
#define MAXWORD 30
#define MAXLINE 100
 
int getcmd(char *buf, int nbuf){
    fprintf(2, "@ ");
    memset(buf, 0, nbuf);
    gets(buf, nbuf);
    if (buf[0] == 0) // EOF
        return -1;
    return 0;
}
char whitespace[] = " \t\r\n\v";
char args[MAXARGS][MAXWORD];
 
//*****************END  from sh.c ******************
void getargv(char *cmd, char* argv[],int* argc){
    // ��argv��ÿһ��Ԫ�ض�ָ��args��ÿһ��
    for(int i=0;i<MAXARGS;i++){
        argv[i]=&args[i][0];
    }
	
    int i = 0; // ��ʾ��i��word
    int j = 0;//��ʾcmd�ĵ�i���ַ�
    for (; cmd[j] != '\n' && cmd[j] != '\0'; j++)
    {
        // ÿһ��ѭ�������ҵ�����������е�һ��word������ echo hi ,�������ҵ�echo�����ҵ�hi
        // ��argv[i]�ֱ�ָ�����ǵĿ�ͷ�����ҽ�echo��hi����Ŀո���Ϊ\0
		
        // ����֮ǰ�Ŀո�
        while (strchr(whitespace,cmd[j])){
            j++;
        }
		//argv[i]�ֱ�ָ�����ǵĿ�ͷ
        argv[i++]=cmd+j;
        //�����ǿո�
        while (!strchr(whitespace,cmd[j])){
            j++;
        }
        cmd[j]='\0';
    }
    argv[i]=0;
    *argc=i;
}
 
void runcmd(char*argv[],int argc){
	// �ܵ� | ���ض��� < > ǰ�涼Ӧ��������һ��word�����Դ�argv[1]��ʼ
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"|")){
            // ������� | ��pipe������˵�����滹��һ������Ҫִ��
            execPipe(argv,argc);
        }
    }
    
    for(int i=1;i<argc;i++){
        // ������� > ��˵����Ҫִ������ض���������Ҫ�ر�stdout
        if(!strcmp(argv[i],">")){
            close(1);
            // ��ʱ��Ҫ������ض��򵽺���������ļ�����Ӧ���ļ���
            // ��Ȼ���>�����һ�����Ǿͻ�error��������ʱ�Ȳ�����
            open(argv[i+1],O_CREATE|O_WRONLY);
            argv[i]=0;
            // break;
        }
        if(!strcmp(argv[i],"<")){
            // �������< ,��Ҫִ�������ض��򣬹ر�stdin
            close(0);
            open(argv[i+1],O_RDONLY);
            argv[i]=0;
            // break;
        }
    }
    exec(argv[0], argv);
}
 
void execPipe(char*argv[],int argc){
    int i=1;
    // �����ҵ������е�"|",Ȼ���������'\0'
    // ��ǰ�����ҵ���һ����ֹͣ�����涼�ݹ����
    for(;i<argc;i++){
        if(!strcmp(argv[i],"|")){
            argv[i]=0;
            break;
        }
    }
    int fd[2];
    pipe(fd);
    if(fork()==0){
        // �ӽ��� ִ����ߵ����� ���Լ��ı�׼����ر�
        close(1);
        dup(fd[1]);//�رձ�׼�����ʹ���ļ�������fd��������ܵ�fd
        close(fd[0]);
        close(fd[1]);
        // exec(argv[0],argv);
        runcmd(argv,i);
    }else{
        // ������ ִ���ұߵ����� ���Լ��ı�׼����ر�
        close(0);
        dup(fd[0]);//�رձ�׼���룬ʹ���ļ�������fd���ӹܵ�fd��������
        close(fd[0]);
        close(fd[1]);
        // exec(argv[i+1],argv+i+1);
        runcmd(argv+i+1,argc-i-1);
    }
}
int main()
{
    char buf[MAXLINE];
    while (getcmd(buf, sizeof(buf)) >= 0){
        if (fork() == 0){
            char* argv[MAXARGS];
            int argc=-1;
            getargv(buf, argv,&argc);
            runcmd(argv,argc);
        }
        wait(0);
    }
    exit(0);
}