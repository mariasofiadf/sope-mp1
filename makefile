xmod: xmod.c mode_t_aux.c
	gcc -Wall -o xmod xmod.c mode_t_aux.c
clean:
	rm -f xmod