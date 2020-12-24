#include "kernel/types.h"
#include "user/user.h"


/*
Unix命令都带有参数
部分命令可以接受标准输入“stdin”作为参数


如cat /etc/passwd | grep root
因为上面的代码使用了管道命令（|）。管道命令的作用，是将左侧命令（cat /etc/passwd）的标准输出转换为标准输入，提供给右侧命令（grep root）作为参数。
而grep命令可以接受标准输入作为参数
所以上述代码等同于下面的代码
grep root /etc/passwd


但是，大多数命令都不接受标准输入作为参数，只能直接在命令行输入参数，这导致无法用管道命令传递参数。如
echo命令就不接受管道传参。
echo "hello world" | echo
上面的代码不会有输出。因为管道右侧的echo不接受管道传来的标准输入作为参数。
（其中echo用于字符串的输出）


xargs命令的作用，是将标准输入转为命令行参数。
如：
echo "hello world" | xargs echo
上面的代码将管道左侧的标准输入，转为命令行参数hello world，传给第二个echo命令
所以输出：
hello world


如：
echo "one two three" | xargs mkdir
上面的代码等同于mkdir one two three。如果不加xargs就会报错，提示mkdir缺少操作参数。


综上，
xargs的作用在于，大多数命令（比如rm、mkdir、ls）与管道一起使用时，都需要xargs将标准输入转为命令行参数。


大多数时候，xargs命令都是跟管道一起使用的。但是，它也可以单独使用。
输入xargs echo按下回车以后，命令行就会等待用户输入，作为标准输入。你可以输入任意内容，然后按下Ctrl d，表示输入结束，这时echo命令就会把前面的输入打印出来。


所以代码如下
*/
int main(int argc, char *argv[]) {
	int i;
	int j = 0;
	int k;
	int l, m = 0;
	//存储stdin读取的数据
	char block[32];
	//存储block中需要输出的信息
	char buf[32];
	//p指向字符串buf
	char *p = buf;


	/*
	调用exec所用的参数之一
	exec接收的二维参数数组argv，argv[0]必须是该命令本身，最后一个参数argv[size-1]必须为0，否则将执行失败。
	如：
	char *argv[] = {"echo","hello",0};
	exec("echo",argv);
	*/
	char *true_argv[4];
	//true_argv[0]存放真正的命令，如echo
	//true_argv[i],i>0,存放其他参数，如xargs echo hello，true_argv[1]存放hello
	for (i = 1; i < argc; i++) {
		true_argv[j++] = argv[i];
	}


	//从管道/stdin读取数据到block
	//读取到ctrl+d时退出
	while ((k = read(0, block, sizeof(block))) > 0) {
		for (l = 0; l < k; l++) {
			//读取其他数据，如xargs echo hello回车，之后输入abc回车，则会打印hello abc，再输入def回车，会打印def
			//如echo abc | xargs echo abc回车会打印hello abc
			//读取到换行符，执行命令
			if (block[l] == '\n') {
				//字符串结束标志
				buf[m] = 0;
				//重置m=0
				m = 0;
				//true_argv[2]=buf,在例子中相当于true_argv[2]="abc\0"
				//即true_argv[0]="echo",[1]="hello",[2]="abc\0"
				true_argv[j] = p;
				if (fork() == 0) {
					//执行真正要执行的命令
					exec(argv[1], true_argv);
					exit();
				}
				wait();
			}
			//读取数据，如上述例子中的abc,def
			else {
				buf[m++] = block[l];
			}
		}
	}
	exit();
}