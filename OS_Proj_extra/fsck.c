#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "fs.h"
#include "disk.h"
#include <unistd.h>
/*
bool command(char *comm, char *comm2)
{
		if(strlen(comm) == strlen(comm2) && strncmp(comm, comm2, strlen(comm)) == 0) return true;
		return false;
}*/

int main(int argc, char **argv)
{
		int i, j; 
        SuperBlock sb;
        char inodeMap[MAX_INODE];
        char blockMap[MAX_BLOCK];
        int indir_map[128];
        int i_count = 0;
        int b_count = 0;
        Inode n;

		if(argc < 2) {
				fprintf(stderr, "usage: ./fs disk_name\n");
				return -1;
		}

		if (access(argv[1], F_OK) != 0) {
				printf("\"%s\" does not exist.\n", argv[1]);
				return 0;
		}

        fs_mount(argv[1]);

        correct(); 

        fs_umount(argv[1]);
}

