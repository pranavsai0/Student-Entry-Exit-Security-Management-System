CFLAGS = -Wno-implicit-function-declaration

run: project.o
	gcc $(CFLAGS) project.o -o final

project.o: project.c
	gcc $(CFLAGS) -c project.c

clean: 
	rm project.o final
