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

#include <stdlib.h>
#include <unistd.h> /* for getpid() */
#include <iostream>

#include "TunerSailfish.hpp"

TunerSailfish::TunerSailfish()
{
	r_set = audioresource_init(AUDIO_RESOURCE_MEDIA, g_grant_cb, this);
}

TunerSailfish::~TunerSailfish()
{
	audioresource_free(r_set);
}

void TunerSailfish::g_grant_cb(audioresource_t *resource_set, bool acquired, void *data)
{
	((TunerSailfish*) data)->grant_cb(resource_set, acquired);
}

void TunerSailfish::grant_cb(audioresource_t *resource_set, bool acquired)
{
	(void) resource_set;
	std::cout << __func__ << " " << acquired << std::endl;

	if (acquired) {
		r_granted = true;
		if (playing) worker->SetPlaying(true);
		emit playingChanged();
	}
	else {
		r_granted = false;
		if (playing) worker->SetPlaying(false);
	}
}

void TunerSailfish::SetPlaying(bool p)
{
	if (p == playing) return;
	playing = p;

	if (p) {
		audioresource_acquire(r_set);
	}
	else {
		if (r_granted) worker->SetPlaying(p);
		audioresource_release(r_set);
		r_granted = false;
		emit playingChanged();
	}
}
