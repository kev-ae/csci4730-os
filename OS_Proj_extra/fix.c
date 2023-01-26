#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "API.h"

bool command(char *comm, char *comm2)
{
		if(strlen(comm) == strlen(comm2) && strncmp(comm, comm2, strlen(comm)) == 0) return true;
		return false;
}

void helper(Inode file) {
        int j;
        Inode n;
        int indir_map[512];

        for (j = 0; j < file.blockCount && j < 15; j++) {
            set_bit(blockMap, file.directBlock[j], 1);
        }

        if (file.blockCount > 15) {
            set_bit(blockMap, file.indirectBlock, 1);
            read_disk_block(file.indirectBlock, (char*)&indir_map);
            for (j = 0; j < file.blockCount - 15; j++) {
                set_bit(blockMap, indir_map[j], 1);
            }
        }
}

void correct() {
		int i, j, d; 
        int i_count = 0;
        int b_count = 0;
		int numInodeBlock =  (sizeof(Inode)*MAX_INODE)/ BLOCK_SIZE;
        int dir[5000];
        int dir_p = 0;
        Dentry den;
        Inode n;

        for (i = 0; i < MAX_INODE; i++) {
            if (get_bit(inodeMap, i) == 0) {
                i_count++;
            }
        }

        for (i = 0; i < MAX_BLOCK; i++) {
            if (get_bit(blockMap, i) == 0) {
                b_count++;
            }
        }

        
        if (b_count != superBlock.freeBlockCount || i_count != superBlock.freeInodeCount) {
            // clear data map
            for (i = 0; i < MAX_BLOCK; i++) {
                set_bit(blockMap, i, 0);
            }

            for (i = 0; i < MAX_INODE; i++) {
                set_bit(inodeMap, i, 0);
            }

            // set bitmaps for init inode and data bitmaps
		    for(i = 0; i < MAX_INODE / 8; i++) {
			    set_bit(inodeMap, i, 0);
		    }

    		for(i = 0; i < MAX_BLOCK / 8; i++) {
			    if(i < (1+1+1+numInodeBlock)) set_bit(blockMap, i, 1);
			    else set_bit(blockMap, i, 0);
	    	}

            dir[dir_p] = superBlock.rootDirInode;

            while (dir_p >= 0) {
                n = read_inode(dir[dir_p--]);
                d = n.directBlock[0];
                read_disk_block(d, (char*)&den);

                for (i = 0; i < den.numEntry; i++) {
                    n = read_inode(den.dentry[i].inode);

                    // bitmap
                    if (!(command(den.dentry[i].name, ".") || command(den.dentry[i].name, "..")) && n.type == directory) {
                        dir[++dir_p] = den.dentry[i].inode;
                    }
                    helper(n);
                    set_bit(inodeMap, den.dentry[i].inode, 1);
                }
            }
        }
}
