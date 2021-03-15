#include "./signal_aux.h"

char* pathname = NULL;
unsigned int nftot = 0, nfmod = 0;

void print_process_info(){
    int pid = getpid();
    fprintf(stderr, "\n%d ; %s ; %d ; %d\n", pid, pathname, nftot, nfmod);
    fprintf(stderr, "Do you want to terminate the program? (Y/N)\n");
    if(getc(stdin) == 'Y')
        exit(0);
}

void set_sig_action(){
	struct sigaction new, old;
	sigset_t smask;
			// defines signals to block while func() is running

		// prepare struct sigaction
	if (sigemptyset(&smask)==-1)	// block no signal
		perror ("sigsetfunctions");

	new.sa_handler = print_process_info;
	new.sa_mask = smask;
	new.sa_flags = 0;	// usually works

	if(sigaction(SIGINT, &new, &old) == -1)
		perror ("sigaction");
}