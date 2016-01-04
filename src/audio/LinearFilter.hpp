/* Copyright 2016 (C) Louis-Joseph Fournier 
 * louisjoseph.fournier@gmail.com
 *
 * This file is part of SailTuner.
 *
 * SailTuner is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SailTuner is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef LINEARFILTER_H
#define LINEARFILTER_H

#include <stdint.h>

template<typename A> class LinearFilter {
	private:
	int order;
	double *a, *b;
	double *backx, *backy;

	public:
	/**
	 * filter constructor
	 *
	 * @param order Order of filter (2 = biquads)
	 * @param a Array of a coefficiants
	 * @param b Array of b coefficiants
	 */
	LinearFilter(int order, double *a, double *b);
	~LinearFilter();

	/// Clear audio stream
	void Clear();
	/// Compute one audio sample
	A operator() (A x);
	/// Compute an audio buffer in place
	void operator() (A *ptr, int nbFrame);
};

#endif
