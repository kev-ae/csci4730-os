all: fsck

fsck: fsck.c fs.c fs.h fs_util.c disk.c fix.c disk.h API.h
		gcc fsck.c fs.c disk.c fs_util.c fix.c -g -o fsck

clean:
		rm -f fsck
