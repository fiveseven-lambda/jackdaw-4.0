#include <cstring>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <complex>
#include "wav.h"
#include "note.h"

#include <iostream>

extern std::vector<Note> score;
extern int samplerate;
extern short bitdepth, channel;
extern int ilength, filesize;

extern int verbosity;

void wav(void *dest){

	if(verbosity){
		for(const Note i : score){
			std::cout
				<< "(" << i.start << "-" << i.end << ")"
				<< " f" << i.freq
				<< " v" << i.vel
				<< " a" << i.attack
				<< " d" << i.decay
				<< " s" << i.sustain
				<< " r" << i.release
			<< std::endl;
		}
	}

	memcpy(dest, "RIFF", 4);
	((unsigned int *)dest)[1] = filesize - 8;
	memcpy((char *)dest + 8, "WAVEfmt ", 8);
	((unsigned int *)dest)[4] = 16;
	((unsigned short *)dest)[10] = 1;
	((unsigned short *)dest)[11] = channel;
	((unsigned int *)dest)[6] = samplerate;
	((unsigned int *)dest)[7] = samplerate * bitdepth / 8 * channel;
	((unsigned short *)dest)[16] = channel * bitdepth / 8;
	((unsigned short *)dest)[17] = bitdepth;
	memcpy((char *)dest + 36, "data", 4);
	((unsigned int *)dest)[10] = filesize - 44;

	double *data = new double[sizeof(double) * ilength];

	for(auto i : score){
		i.wav(data, ilength);
	}

	for(int i = 0; i < ilength; ++i){
		((short *)dest)[i + 22] = data[i] * ((1 << 15) - 1);
	}

	delete[] data;
}
