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

#include <algorithm>
#include <assert.h>
#include "LinearFilter.hpp"

using namespace std;

template<typename A> LinearFilter<A>::LinearFilter(int order, double *a, double *b)
{
	assert(order > 0);
	assert(a);
	assert(b);
	assert(a[0]);

	this->a = a;
	this->b = b;
	this->order = order;

	backx = new double[order];
	backy = new double[order];

	Clear();
}

template<typename A> LinearFilter<A>::~LinearFilter()
{
	delete [] backx;
	delete [] backy;
}

template<typename A> void LinearFilter<A>::Clear()
{
	fill(backx, backx + order, 0);
	fill(backy, backy + order, 0);
}

template<typename A> A LinearFilter<A>::operator() (A x)
{
	double y = (double) x * b[0];

	for (int i = 1; i <= order; i++) {
		y += (double) b[i] * backx[i-1]; 
		y -= (double) a[i] * backy[i-1];
	}
	y /= a[0];

	for (int i = order - 1; i > 0; i--) {
		backx[i] = backx[i-1];
		backy[i] = backy[i-1];
	}
	backx[0] = x;
	backy[0] = y;

	return y;
}

template<typename A> void LinearFilter<A>::operator() (A *ptr, int nbFrame)
{
	while(nbFrame--) {
		*ptr = (*this)(*ptr);
		ptr++;
	}
}

template class LinearFilter<int16_t>;
