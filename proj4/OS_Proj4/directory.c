#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "API.h"

int dir_make(char* name)
{
    int inode_num, new_block, ret;
	Inode new_inode;
	Dentry new_dir;

	if (command(name, ".") || command(name, "..")) {
		printf("Directory create failed: cannot create with name . or ..\n");
		return -1;
	}

	ret = search_cur_dir(name);
	if (ret >= 0) {
		printf("Directory create failed: %s exist.\n", name);
		return -1;
	}

	if(curDir.numEntry + 1 > MAX_DIR_ENTRY) {
		printf("Directory create failed: current directory is full!\n");
		return -1;
	}

	if(superBlock.freeInodeCount < 1) {
		printf("Directory create failed: inode is full!\n");
		return -1;
	}

	inode_num = get_inode();
	new_block = get_block();

	new_inode.type = directory;
	new_inode.size = 1;
	new_inode.blockCount = 1;
	new_inode.directBlock[0] = new_block;
	new_inode.linkCount = 1;
	write_inode(inode_num, new_inode);

	new_dir.numEntry = 2;

	// set up for .
	strncpy(new_dir.dentry[0].name, ".", 1);
	new_dir.dentry[0].name[1] = '\0';
	new_dir.dentry[0].inode = inode_num;

	// setup for ..
	ret = search_cur_dir(".");
	if (ret == -1) {
		printf("Error: there is no .\n");
		return -1;
	}
	strncpy(new_dir.dentry[1].name, "..", 2);
	new_dir.dentry[1].name[2] = '\0';
	new_dir.dentry[1].inode = ret;

	write_disk_block(new_block, (char*)&new_dir);

	// add to current dir
	strncpy(curDir.dentry[curDir.numEntry].name, name, strlen(name));
	curDir.dentry[curDir.numEntry].name[strlen(name)] = '\0';
	curDir.dentry[curDir.numEntry].inode = inode_num;
	curDir.numEntry++;

    return 0;
}

int dir_remove(char *name)
{
	int ret, i;
	Inode node;
	Dentry dir;

	if (command(name, ".") || command(name, "..")) {
		printf("Directory remove failed: cannot remove . or ..\n");
		return -1;
	}

	ret = search_cur_dir(name);
	if (ret == -1) {
		printf("Directory remove failed: no such directory\n");
		return -1;
	}

	node = read_inode(ret);

	if (node.type != directory) {
		printf("Directory remove failed: not a directory\n");
		return -1;
	}

	read_disk_block(node.directBlock[0], (char*)&dir);
	if (dir.numEntry != 2) {
		printf("Directory remove failed: directory is not empty\n");
		return -1;
	}

	node.linkCount--;
	write_inode(ret, node);

	if (node.linkCount <= 0) {
		free_block(node.directBlock[0]);
		free_inode(ret);
	}

	// get index of removed dir
	for(i = 0; i < curDir.numEntry; i++) {
		if (command(name, curDir.dentry[i].name)) { 
			break;
		}
	}

	// move rm dir to last entry
	for(; i < curDir.numEntry - 1; i++) {
		strncpy(curDir.dentry[i].name, curDir.dentry[i + 1].name, strlen(curDir.dentry[i + 1].name));
		curDir.dentry[i].inode = curDir.dentry[i + 1].inode;
	}

	curDir.numEntry--;

	return 0;
}

int dir_change(char* name)
{
	int ret;
	Inode node;

	ret = search_cur_dir(name);
	if (ret == -1) {
		printf("Directory change failed: no such directory\n");
		return -1;
	}

	node = read_inode(ret);

	if (node.type != directory) {
		printf("Directory change failed: not a directory\n");
		return -1;
	}

	// save state of cur directory before changing
	write_disk_block(curDirBlock, (char*)&curDir);

	curDirBlock = node.directBlock[0];
	read_disk_block(curDirBlock, (char*)&curDir);

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


