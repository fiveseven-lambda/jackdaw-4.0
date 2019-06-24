#include <complex>
#include "note.h"

extern int samplerate;

void Note::wav(double *data, int length){
	const double t = 2 * M_PI * freq / samplerate;
	const std::complex<double> a(cos(t), sin(t));
	std::complex<double> x[2]{{1, 0}, {0, 0}};

	int istart = start * samplerate;
	int iattack = attack * samplerate;

	int ilength = end * samplerate - istart;

	for(int j = 0, k = 0; istart + j < length; ++j){
		double tmp = x[j&1].imag() * vel;
		double z, p;

		switch(k){
			case 0:
				tmp *= j / attack / samplerate;
				if(j == iattack){
					k = 1;
					z = 1;
					p = exp(- 1 / decay / samplerate);
				}
				break;
			case 1:
				z *= p;
				tmp *= 1 - (1 - sustain) * (1 - z);
				if(j == ilength){
					k = 2;
					z = 1;
					p = exp(- 1 / release / samplerate);
				}
				break;
			case 2:
				tmp *= z * sustain;
				z *= p;
				if(z == 0) return;
				break;
		}

		data[istart + j] += tmp;
		x[~j&1] = x[j&1] * a;
	}
}
