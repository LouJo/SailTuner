#include "ZeroCross.hpp"

#include <iostream>
#include <stdlib.h>
#include <math.h>

using namespace std;

/// Maximum rest of division to be a multiple of
#define EPS_DIVISOR 0.1
/// Consider it is a correct pattern to avoid multiples of
#define CORRECT_DEVIATION 1

// local functions

/**
 * Return average and standart deviation of pattern approx. interval in values
 */
pair<double,double> IsPattern(const vector<double> &values, double interval)
{
	double sum = 0, deviation = 0, current = 0;
	int nb = 0;
	double diff = interval;

	for (double v : values) {
		if (fabs(diff - v) < fabs(diff)) {
			current += v;
			diff = interval - current;
		}
		else {
			sum += current;
			deviation += diff * diff;
			current = 0;
			diff = interval;
			nb++;
		}
	}
	if (nb < 2) return pair<double,double>(0,0);
	sum /= nb;
	deviation = sqrt(deviation) / nb;
	return pair<double,double>(sum, deviation);
}

/**
 * Find best matching pattern beetween min and max
 * from an integer list
 *
 * @return best interval pattern, standart deviation for best pattern
 */
pair<double,double> FindPattern(const vector<double> &values, double pattern_min, double pattern_max)
{
	auto best = pair<double,double>(0,0);
	double interval = 0;

	for (double v : values) {
		interval += v;
		if (interval < pattern_min) continue;
		if (interval > pattern_max) break;

		auto res = IsPattern(values, interval);
//		cout << "  " << res.first << " " << res.second << endl;
		if (res.first && (res.second < best.second || best.first == 0)) {
			if (best.first && best.second < CORRECT_DEVIATION) {
				double div = res.first / best.first;
				// it is a multiple of previous
				if (fabs(div - round(div)) < EPS_DIVISOR) continue;
//				cout << "... " << div - floor(div) << endl;
			}
			best = res;
		}
	}
	cerr << " -> " << best.first << " " << best.second << endl;
	if (best.second > CORRECT_DEVIATION) return pair<double,double>(0,0);
	return best;
}

/// Methods implementation

template<typename A> ZeroCross<A>::ZeroCross(const Config &config)
{
	rate = config.rate;
	nb_frame_to_analyse = config.nb_frame;
	freq = 0;
	sample_max = (double) rate / config.tonie_min;
	sample_min = (double) rate / config.tonie_max;
	Clear();
}

template<typename sample_t> void ZeroCross<sample_t>::Clear()
{
	nb_frame_current = -1;
	nb_frame_analysed = 0;
	freq = 0;
	last_zero = false;
	pattern.clear();
}

template<typename sample_t> void ZeroCross<sample_t>::ComputeFrame(sample_t x)
{
	double delta;

	// count frame only if ever crossed
	if (nb_frame_current != -1) nb_frame_current++;
	// get only from negative to positive
	if (x > 0 && last_sample < 0) {
		if (last_zero) delta = 1;
		else delta = (double) x / (x - last_sample);

		if (nb_frame_current > 0) {
			nb_frame_current -= delta;
			pattern.push_back(nb_frame_current);
			//cout << nb_frame_current << " ";
		}
		nb_frame_current = delta;
	}
	// drop 0 values to know sign change
	if (x) last_sample = x, last_zero = false;
	else last_zero = true;
	// compute if window suffisent
	if (nb_frame_analysed++ == nb_frame_to_analyse) {
		auto res = FindPattern(pattern, sample_min, sample_max);
		if (res.first) freq = rate / res.first;
		else freq = 0;
		nb_frame_analysed = 0;
		pattern.clear();
	}
}

template<typename sample_t> double ZeroCross<sample_t>::operator() (sample_t *ptr, int nbFrame)
{
	while(nbFrame--) ComputeFrame(*ptr++);
	return freq;
}

template<typename sample_t> void ZeroCross<sample_t>::operator() (sample_t v)
{
	ComputeFrame(v);
}

template<typename sample_t> double ZeroCross<sample_t>::Freq()
{
	return freq;
}

// instanciations
template class ZeroCross<int16_t>;
