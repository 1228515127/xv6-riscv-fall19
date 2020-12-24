#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc,char *argv[]){
	int n = atoi(argv[1]);
	sleep(n);
	printf("nothing happens for a little while\n");
	exit();
}