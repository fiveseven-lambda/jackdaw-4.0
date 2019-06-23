#include <vector>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "load.h"
#include "wav.h"
#include "errmsg.h"

bool successful = true;

extern double end;

int samplerate = 44100;
short bitdepth = 16, channel = 1;
int ilength;
int filesize;

int main(int argc, char *argv[]){
	load(argv[1]);
	int out = open("a.wav", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	ilength = samplerate * end;
	filesize = bitdepth / 8 * channel * ilength + 44;
	ftruncate(out, filesize);
	void *map = mmap(NULL, filesize, PROT_WRITE, MAP_SHARED, out, 0);
	if(map == MAP_FAILED) return -1;

	wav(map);
	close(out);
	munmap(map, filesize);
}

