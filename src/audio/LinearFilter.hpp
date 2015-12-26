#ifndef LINEARFILTER_H
#define LINEARFILTER_H

#include <stdint.h>

template<typename A> class LinearFilter {
	private:
	int order;
	double *a, *b;
	double *backx, *backy;

	public:
	LinearFilter(int order, double *a, double *b);
	~LinearFilter();

	void Clear();
	A operator() (A x);
	void operator() (A *ptr, int nbFrame);
};

#endif
