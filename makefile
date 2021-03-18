xmod: xmod.c mode_t_aux.c log.c
	gcc -Wall -o xmod xmod.c mode_t_aux.c log.c
clean:
	rm -f xmod