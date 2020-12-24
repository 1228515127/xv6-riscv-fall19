#include "kernel/types.h"
#include "user/user.h"


/*
Unix������в���
����������Խ��ܱ�׼���롰stdin����Ϊ����


��cat /etc/passwd | grep root
��Ϊ����Ĵ���ʹ���˹ܵ����|�����ܵ���������ã��ǽ�������cat /etc/passwd���ı�׼���ת��Ϊ��׼���룬�ṩ���Ҳ����grep root����Ϊ������
��grep������Խ��ܱ�׼������Ϊ����
�������������ͬ������Ĵ���
grep root /etc/passwd


���ǣ��������������ܱ�׼������Ϊ������ֻ��ֱ��������������������⵼���޷��ùܵ�����ݲ�������
echo����Ͳ����ܹܵ����Ρ�
echo "hello world" | echo
����Ĵ��벻�����������Ϊ�ܵ��Ҳ��echo�����ܹܵ������ı�׼������Ϊ������
������echo�����ַ����������


xargs��������ã��ǽ���׼����תΪ�����в�����
�磺
echo "hello world" | xargs echo
����Ĵ��뽫�ܵ����ı�׼���룬תΪ�����в���hello world�������ڶ���echo����
���������
hello world


�磺
echo "one two three" | xargs mkdir
����Ĵ����ͬ��mkdir one two three���������xargs�ͻᱨ����ʾmkdirȱ�ٲ���������


���ϣ�
xargs���������ڣ�������������rm��mkdir��ls����ܵ�һ��ʹ��ʱ������Ҫxargs����׼����תΪ�����в�����


�����ʱ��xargs����Ǹ��ܵ�һ��ʹ�õġ����ǣ���Ҳ���Ե���ʹ�á�
����xargs echo���»س��Ժ������оͻ�ȴ��û����룬��Ϊ��׼���롣����������������ݣ�Ȼ����Ctrl d����ʾ�����������ʱecho����ͻ��ǰ��������ӡ������


���Դ�������
*/
int main(int argc, char *argv[]) {
	int i;
	int j = 0;
	int k;
	int l, m = 0;
	//�洢stdin��ȡ������
	char block[32];
	//�洢block����Ҫ�������Ϣ
	char buf[32];
	//pָ���ַ���buf
	char *p = buf;


	/*
	����exec���õĲ���֮һ
	exec���յĶ�ά��������argv��argv[0]�����Ǹ���������һ������argv[size-1]����Ϊ0������ִ��ʧ�ܡ�
	�磺
	char *argv[] = {"echo","hello",0};
	exec("echo",argv);
	*/
	char *true_argv[4];
	//true_argv[0]��������������echo
	//true_argv[i],i>0,���������������xargs echo hello��true_argv[1]���hello
	for (i = 1; i < argc; i++) {
		true_argv[j++] = argv[i];
	}


	//�ӹܵ�/stdin��ȡ���ݵ�block
	//��ȡ��ctrl+dʱ�˳�
	while ((k = read(0, block, sizeof(block))) > 0) {
		for (l = 0; l < k; l++) {
			//��ȡ�������ݣ���xargs echo hello�س���֮������abc�س�������ӡhello abc��������def�س������ӡdef
			//��echo abc | xargs echo abc�س����ӡhello abc
			//��ȡ�����з���ִ������
			if (block[l] == '\n') {
				//�ַ���������־
				buf[m] = 0;
				//����m=0
				m = 0;
				//true_argv[2]=buf,���������൱��true_argv[2]="abc\0"
				//��true_argv[0]="echo",[1]="hello",[2]="abc\0"
				true_argv[j] = p;
				if (fork() == 0) {
					//ִ������Ҫִ�е�����
					exec(argv[1], true_argv);
					exit();
				}
				wait();
			}
			//��ȡ���ݣ������������е�abc,def
			else {
				buf[m++] = block[l];
			}
		}
	}
	exit();
}