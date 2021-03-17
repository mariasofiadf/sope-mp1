/* Example for seeing and creating a process group,
 * so that all children will be in the same, new group, different from the initial.
 *  The initial process will print its pgid and pid;
 * then it will fork() and the new child process will create a new group;
 * the children of the child will belong to the same (2nd) group.
 * JMCRUZ, Mar.2020.
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *env[]) {
	pid_t ret;
printf("\n");
printf("First process: group: %d ; pid: %d\n", getpgrp(), getpid());
ret = fork();
if (ret == -1) {
	perror ("1st fork()");
} else if (ret == 0) {	// child
	printf("\tChild initially: group: %d ; pid: %d\n", getpgrp(), getpid());
	if (setpgid(0, 0) == -1) {
		perror("setpgid");
		exit(-1);
		}
	printf("\tChild finally: group: %d ; pid: %d\n", getpgrp(), getpid());
	int r = fork();
	if (r == -1) {
		perror ("2nd fork()");
	} else if (r == 0) {	// grand child
		printf("\t\tGrand child: group: %d ; pid: %d\n", getpgrp(), getpid());
	} else {	// child
		pid_t pid = wait(NULL);
		int grp = getpgid(pid);
		if (grp == -1) perror("\tgetpgid");
		printf("\tGrand child has finished. Group: %d ; pid: %d\n", grp, pid);
		}
} else {	// parent
	pid_t pid = wait(NULL);
	printf("Child has finished. Group: %d ; pid: %d\n", getpgid(pid), pid);
	}
return 0;
} // main()

