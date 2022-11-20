#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "API.h"

int dir_make(char* name)
{
		printf("mkdir is not implemented yet.\n");
		return 0;
}

int dir_remove(char *name)
{
		printf("rmdir is not implemented yet.\n");
		return 0;
}

int dir_change(char* name)
{
		printf("cd is not implemented yet.\n");
		return 0;
}


/* ===================================================== */

int ls()
{
		int i;
		int inodeNum;
		Inode targetInode;
		for(i = 0; i < curDir.numEntry; i++)
		{
				inodeNum = curDir.dentry[i].inode;
				targetInode = read_inode(inodeNum);
				if(targetInode.type == file) printf("type: file, ");
				else printf("type: dir, ");
				printf("name \"%s\", inode %d, size %d byte\n", curDir.dentry[i].name, curDir.dentry[i].inode, targetInode.size);
		}

		return 0;
}


