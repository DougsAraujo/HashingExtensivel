Main: Main.o HashingExtensivel.o
	gcc Main.o HashingExtensivel.o -o Main -lm

Main.o: Main.c HashingExtensivel.h
	gcc -c Main.c

HashingExtensivel.o: HashingExtensivel.c HashingExtensivel.h
	gcc -c HashingExtensivel.c
