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

TunerSailfish::~TunerSailfish()
{
	// release resource if needed
	if (r_set) {
		resource_set_destroy(r_set);
		r_set = nullptr;
		r_granted = false;
	}
}

void TunerSailfish::g_advice_cb(resource_set_t *resource_set, uint32_t resources, void *data)
{
	((TunerSailfish*) data)->advice_cb(resource_set, resources);
}

void TunerSailfish::g_grant_cb(resource_set_t *resource_set, uint32_t resources, void *data)
{
	((TunerSailfish*) data)->grant_cb(resource_set, resources);
}

void TunerSailfish::advice_cb(resource_set_t *resource_set, uint32_t resources)
{
	(void) resource_set;
	std::cout << __func__ << " " << resources << std::endl;
}

void TunerSailfish::grant_cb(resource_set_t *resource_set, uint32_t resources)
{
	(void) resource_set;
	std::cout << __func__ << " " << resources << std::endl;

	if (resources & RESOURCE_AUDIO_PLAYBACK) {
		r_granted = true;
		if (playing) worker->SetPlaying(true);
	}
	else {
		r_granted = false;
		if (!playing) worker->SetPlaying(false);
	}
}

void TunerSailfish::get_resource()
{
	if (r_set) return;
	r_set = resource_set_create("player", RESOURCE_AUDIO_PLAYBACK, 0, 0, g_grant_cb, this);
	resource_set_configure_advice_callback(r_set, g_advice_cb, this);
	resource_set_configure_audio (r_set, "player", getpid(), "*");
	resource_set_acquire (r_set);
}

void TunerSailfish::SetPlaying(bool p)
{
	if (p == playing) return;
	playing = p;

	// get resource only the first playback start
	if (p && !r_set) get_resource();

	// if not granted, don't start, and don't stop because not started
	if (!r_granted) return;

	worker->SetPlaying(p);
	emit playingChanged();
}
