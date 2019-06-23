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

void wav(void *dest){
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
		const double t = 2 * M_PI * i.freq / samplerate;
		const std::complex<double> a(cos(t), sin(t));
		std::complex<double> x[2]{{1, 0}, {0, 0}};

		int start = i.start * samplerate;
		int length = (int)(i.end * samplerate) - start;

		for(int j = 0; j < length; ++j){
			data[start + j] += x[j&1].imag() * i.vel;
			x[~j&1] = x[j&1] * a;
		}
	}

	for(int i = 0; i < ilength; ++i){
		((short *)dest)[i + 22] = data[i] * ((1 << 15) - 1);
	}

	delete[] data;
}
