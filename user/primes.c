#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


void func(int input[], int num) {
	int p[2];
	int i;
	int prime = input[0];
	printf("prime %d\n", prime);
	if (num == 1) {
		return;
	}
	pipe(p);
	if (fork()>0) {
		write(p[1], input, num * 4);
		close(p[1]);
		wait();
	}
	else {
		int next[num];
		int counter = 0;
		read(p[0], next, num * 4);
		for (i = 0; i < num; i++) {
			if (next[i]%prime != 0) {
				next[counter] = next[i];
				counter++;
			}
		}
		close(p[0]);
		func(next, counter);
		exit();
	}
}


int main(int argc, char *argv[]) {
	int input[34];
	int i = 0;
	for (i = 0; i < 34; i++) {
		input[i] = i + 2;
	}
	func(input, 34);
	exit();
}


