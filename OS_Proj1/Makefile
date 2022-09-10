all: matmul matgen

matmul: matmul.c matmul.h matmul_core.c
	gcc -fcommon matmul.c matmul_core.c -g -o matmul

matgen: matgen.c
	gcc -fcommon matgen.c -g -o matgen

clean:
	rm -f wc matmul matgen
