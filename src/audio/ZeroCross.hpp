#ifndef ZEROCROSS_H
#define ZEROCROSS_H

#include <stdint.h>
#include <vector>

template<typename sample_t> class ZeroCross {
	public:
	struct Config {
		uint16_t rate;
		int nb_frame;
		int tonie_min, tonie_max;
	};

	private:
	int rate, nb_frame_to_analyse, nb_frame_analysed, diff_x;
	double nb_frame_current;
	int16_t last_sample;
	double freq, sample_min, sample_max;
	bool last_zero;
	std::vector<double> pattern;

	inline void ComputeFrame(sample_t x);

	public:
	ZeroCross(const Config &config);
	double operator() (sample_t *ptr, int nbFrame);
	void operator() (sample_t v);

	void Clear();
	double Freq();
};

#endif
