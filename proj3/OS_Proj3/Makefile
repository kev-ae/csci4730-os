CFLAGS=-g
CC=/usr/bin/gcc

all: vm input_gen list_test

vm: vm.c vm.h pagetable.c disk.c list.c pt_core.c
		$(CC) $(CFLAGS) vm.c pagetable.c disk.c list.c pt_core.c -g -o vm

input_gen: input_gen.c vm.h
		$(CC) input_gen.c $(CFLAGS) -o input_gen

list_test: list_test.c list.c list.h
		$(CC) list_test.c list.c $(CFLAGS) -o list_test

clean:
		rm -f vm input_gen list_test
