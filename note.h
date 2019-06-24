class Note{
public:
	double freq, vel;
	double attack, decay, sustain, release;
	double start, end;
	void wav(double *, int);
};
