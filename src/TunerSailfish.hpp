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

#include <audioresource.h>
#include "Tuner.hpp"

/** Tuner object
  * with resource handling for audio playback
  */
class TunerSailfish : public Tuner {
	private:
	audioresource_t *r_set;
	bool r_granted = false;

	/// callback from resource system
	static void g_grant_cb(audioresource_t *resource_set, bool acquired, void *data);
	void grant_cb(audioresource_t *resource_set, bool acquired);

	public:
	TunerSailfish();
	~TunerSailfish();
	void SetPlaying(bool p);
};
