#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "API.h"

int file_cat(char *name)
{
	int ret, size, i, data_num, f_size, indir_p;
	char *buf = malloc(sizeof(char) * 513);
	char *indir_buf = malloc(sizeof(char) * 513);
	//char buf[513], indir_buf[128];
	int *temp;
	Inode file_inode;

	// check for name in current directory
	ret = search_cur_dir(name);

	// if file does not exist, return -1
	if (ret == -1) {
		printf("File cat failed: file does not exist\n");
		return -1;
	}

	// get the inode
	file_inode = read_inode(ret);
	
	// determine how many pointers to reference to
	size = file_inode.blockCount;
	f_size = file_inode.size;
	
	// go through the direct pointers
	for(i = 0; i < 15 && i < size; i++) {
		data_num = file_inode.directBlock[i];
		read_disk_block(data_num, buf);
		if (f_size >= 512) {
			buf[512] = '\0';
		} else {
			buf[f_size + 1] = '\0';
		}
		printf("%s", buf);
		f_size -= 512;
	}

	// if using indirect pointers
	if (size > 15) {
		size = size - 15;
		indir_p = file_inode.indirectBlock;
		read_disk_block(indir_p, indir_buf);
		temp = (int*) indir_buf;
		for (i = 0; i < size; i++) {
			read_disk_block(temp[i], buf);
			if (f_size >= 512) {
				buf[512] = '\0';
			} else {
				buf[f_size + 1] = '\0';
			}
			printf("%s", buf);
			f_size -= 512;
		}
	}

	free(buf);
	free(indir_buf);

	return 0;
}

int file_remove(char *name)
{
	int ret, size, i, indir_p;
	int *temp;
	char *indir_buf = malloc(sizeof(char) * 513);
	Inode file_inode;

	// check if file exist
	ret = search_cur_dir(name);

	if (ret == -1) {
		printf("File remove failed: file does not exist\n");
		return -1;
	}

	// get the inode
	file_inode = read_inode(ret);

	if (file_inode.type != file) {
		printf("File remove failed: not a file\n");
		return -1;
	}

	size = file_inode.blockCount;

	// check if link is equal to 0, free block/inode if so
	file_inode.linkCount--;
	write_inode(ret, file_inode);

	if (file_inode.linkCount <= 0) {
		// go through the direct pointers
		for(i = 0; i < 15 && i < size; i++) {
			free_block(file_inode.directBlock[i]);
		}

		// if using indirect pointers
		if (size > 15) {
			size = size - 15;
			indir_p = file_inode.indirectBlock;
			read_disk_block(indir_p, indir_buf);
			temp = (int*) indir_buf;
			for (i = 0; i < size; i++) {
				free_block(temp[i]);
			}
			free_block(indir_p);
		}

		free_inode(ret);
	}

	// get index of removed file
	for(i = 0; i < curDir.numEntry; i++) {
		if(command(name, curDir.dentry[i].name)) { 
			break;
		}
	}

	// move rm file to last entry
	for(; i < curDir.numEntry - 1; i++) {
		strncpy(curDir.dentry[i].name, curDir.dentry[i + 1].name, strlen(curDir.dentry[i + 1].name));
		curDir.dentry[i].inode = curDir.dentry[i + 1].inode;
	}

	curDir.numEntry--;
	free(indir_buf);

	return 0;
}

int hard_link(char *src, char *dest)
{
	int i, ret;
	Inode file_inode;

	ret = search_cur_dir(src);

	if (ret == -1) {
		printf("Hard link create failed: src file does not exist\n");
		return -1;
	}

	if (curDir.numEntry + 1 > MAX_DIR_ENTRY) {
		printf("Hard link create failed: directory is full!\n");
		return -1;
	}

	// update link count
	file_inode = read_inode(ret);
	file_inode.linkCount++;
	write_inode(ret, file_inode);

	// create new entry in directory for hard link
	strncpy(curDir.dentry[curDir.numEntry].name, dest, strlen(dest));
	curDir.dentry[curDir.numEntry].name[strlen(dest)] = '\0';
	curDir.dentry[curDir.numEntry].inode = ret;
	curDir.numEntry++;

	return 0;
}

int file_copy(char *src, char *dest)
{
	int i, ret, inode_num, block, f_size, data_num, indir_p;
	int *temp;
	char *buf = malloc(sizeof(char) * 513);
	char *indir_buf = malloc(sizeof(char) * 513);
	//char buf[513], indir_buf[128];
	Inode file_inode, new_inode;

	ret = search_cur_dir(src);

	if (ret == -1) {
		printf("File copy failed: src file does not exist\n");
		return -1;
	}

	if (curDir.numEntry + 1 > MAX_DIR_ENTRY) {
		printf("File copy failed: directory is full!\n");
		return -1;
	}

	if(superBlock.freeInodeCount < 1) {
		printf("File copy failed: inode is full!\n");
		return -1;
	}

	file_inode = read_inode(ret);
	if (file_inode.type != file) {
		printf("File copy failed: not a file\n");
		return -1;
	}

	f_size = file_inode.size;

	if(file_inode.size > 7680) {
		if(file_inode.blockCount + 1 > superBlock.freeBlockCount)
		{
			printf("File copy failed: data block is full!\n");
			return -1;
		}
	} else {
		if(file_inode.blockCount > superBlock.freeBlockCount) {
			printf("File copy failed: data block is full!\n");
			return -1;
		}
	}

	// get inode and fill it
	inode_num = get_inode();
	if(inode_num < 0) {
		printf("File copy error: not enough inode.\n");
		return -1;
	}

	// set new inode
	new_inode.type = file;
	new_inode.size = file_inode.size;
	new_inode.blockCount = file_inode.blockCount;
	new_inode.linkCount = 1;

	// add a new file into the current directory entry
	strncpy(curDir.dentry[curDir.numEntry].name, dest, strlen(dest));
	curDir.dentry[curDir.numEntry].name[strlen(dest)] = '\0';
	curDir.dentry[curDir.numEntry].inode = inode_num;
	curDir.numEntry++;

	// copy file from data blocks
	for(i = 0; i < 15; i++) {
		if (i >= new_inode.blockCount) break;
		block = get_block();
		if(block == -1) {
			printf("File copy error: get_block failed\n");
			return -1;
		}
		//set direct block
		new_inode.directBlock[i] = block;

		// get content of src
		data_num = file_inode.directBlock[i];
		read_disk_block(data_num, buf);
		if (f_size >= 512) {
			buf[512] = '\0';
		} else {
			buf[f_size + 1] = '\0';
		}

		// copy content over
		write_disk_block(block, buf);
		f_size -= 512;
	}

	if(new_inode.size > 7680) {
		// get an indirect block
		block = get_block();
		if(block == -1) {
			printf("File copy error: get_block failed\n");
			return -1;
		}

		new_inode.indirectBlock = block;
		int indirectBlockMap[128];
		indir_p = file_inode.indirectBlock;
		read_disk_block(indir_p, indir_buf);
		temp = (int*) indir_buf;

		for(i = 15; i < new_inode.blockCount; i++)
		{
			block = get_block();
			if(block == -1) {
				printf("File copy error: get_block failed\n");
				return -1;
			}
			//set direct block
			indirectBlockMap[i - 15] = block;

			// copy
			read_disk_block(temp[i - 15], buf);
			if (f_size >= 512) {
				buf[512] = '\0';
			} else {
				buf[f_size + 1] = '\0';
			}

			write_disk_block(block, buf);
			f_size -= 512;
		}
		write_disk_block(new_inode.indirectBlock, (char*)indirectBlockMap);
	}

	// write inode
	write_inode(inode_num, new_inode);

	free(buf);
	free(indir_buf);
	
	return 0;
}


/* =============================================================*/

int file_create(char *name, int size)
{
	int i;
	int block, inodeNum, numBlock;

	if(size <= 0 || size > 73216){
		printf("File create failed: file size error\n");
		return -1;
	}

	inodeNum = search_cur_dir(name); 
	if(inodeNum >= 0) {
		printf("File create failed:  %s exist.\n", name);
		return -1;
	}

	if(curDir.numEntry + 1 > MAX_DIR_ENTRY) {
		printf("File create failed: directory is full!\n");
		return -1;
	}

	if(superBlock.freeInodeCount < 1) {
		printf("File create failed: inode is full!\n");
		return -1;
	}

	numBlock = size / BLOCK_SIZE;
	if(size % BLOCK_SIZE > 0) numBlock++;

	if(size > 7680) {
		if(numBlock+1 > superBlock.freeBlockCount)
		{
			printf("File create failed: data block is full!\n");
			return -1;
		}
	} else {
		if(numBlock > superBlock.freeBlockCount) {
			printf("File create failed: data block is full!\n");
			return -1;
		}
	}

	char *tmp = (char*) malloc(sizeof(int) * size+1);

	rand_string(tmp, size);
	printf("File contents:\n%s\n", tmp);

	// get inode and fill it
	inodeNum = get_inode();
	if(inodeNum < 0) {
		printf("File_create error: not enough inode.\n");
		return -1;
	}
	
	Inode newInode;

	newInode.type = file;
	newInode.size = size;
	newInode.blockCount = numBlock;
	newInode.linkCount = 1;

	// add a new file into the current directory entry
	strncpy(curDir.dentry[curDir.numEntry].name, name, strlen(name));
	curDir.dentry[curDir.numEntry].name[strlen(name)] = '\0';
	curDir.dentry[curDir.numEntry].inode = inodeNum;
	curDir.numEntry++;

	// get data blocks
	for(i = 0; i < 15; i++)
	{
		if (i >= numBlock) break;
		block = get_block();
		if(block == -1) {
			printf("File_create error: get_block failed\n");
			return -1;
		}
		//set direct block
		newInode.directBlock[i] = block;

		write_disk_block(block, tmp+(i*BLOCK_SIZE));
	}

	if(size > 7680) {
		// get an indirect block
		block = get_block();
		if(block == -1) {
			printf("File_create error: get_block failed\n");
			return -1;
		}

		newInode.indirectBlock = block;
		int indirectBlockMap[128];

		for(i = 15; i < numBlock; i++)
		{
			block = get_block();
			if(block == -1) {
				printf("File_create error: get_block failed\n");
				return -1;
			}
			//set direct block
			indirectBlockMap[i-15] = block;
			write_disk_block(block, tmp+(i*BLOCK_SIZE));
		}
		write_disk_block(newInode.indirectBlock, (char*)indirectBlockMap);
	}

	write_inode(inodeNum, newInode);
	printf("File created. name: %s, inode: %d, size: %d\n", name, inodeNum, size);

	free(tmp);
	return 0;
}

int file_stat(char *name)
{
	char timebuf[28];
	Inode targetInode;
	int inodeNum;
		
	inodeNum = search_cur_dir(name);
	if(inodeNum < 0) {
		printf("file cat error: file is not exist.\n");
		return -1;
	}
		
	targetInode = read_inode(inodeNum);
	printf("Inode = %d\n", inodeNum);
	if(targetInode.type == file) printf("type = file\n");
	else printf("type = directory\n");
	printf("size = %d\n", targetInode.size);
	printf("linkCount = %d\n", targetInode.linkCount);
	printf("num of block = %d\n", targetInode.blockCount);
}


