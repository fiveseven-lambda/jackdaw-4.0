#include <vector>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <getopt.h>
#include "load.h"
#include "wav.h"
#include "errmsg.h"

bool successful = true;

extern double end;

int samplerate = 44100;
short bitdepth = 16, channel = 1;
int ilength;
int filesize;

int verbosity;

int main(int argc, char *argv[]){
	char *out_filename = nullptr, out_filename_default[] = "a.wav";

	for(;;){
		int long_optind;
		static struct option long_options[] = {
			{
				.name = "out",
				.has_arg = required_argument,
				.val = 'o'
			},{
				.name = "version",
				.has_arg = no_argument,
				.val = 'v'
			},{
				.name = "verbose",
				.has_arg = no_argument,
			}
		};
		int c = getopt_long(argc, argv, "o:v", long_options, &long_optind);
		if(c == -1) break;
		switch(c){
			case 0:
				switch(long_optind){
					case 2:
						verbosity = 1;
						break;
				}
				break;
			case 'o':
				out_filename = optarg;
				break;
			case 'v':
				puts("jackdaw version 4.0");
				return 0;
		}
	}

	for(;optind < argc; ++optind) load(argv[optind]);

	if(successful){
		if(!out_filename) out_filename = out_filename_default;

		int out = open(out_filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		ilength = samplerate * end;
		filesize = bitdepth / 8 * channel * ilength + 44;
		ftruncate(out, filesize);
		void *map = mmap(NULL, filesize, PROT_WRITE, MAP_SHARED, out, 0);
		if(map == MAP_FAILED) return -1;

		wav(map);
		close(out);
		munmap(map, filesize);
	}
}

