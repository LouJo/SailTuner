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

#include <resource.h>
#include "Tuner.hpp"

/** Tuner object
  * with resource handling for audio playback
  */
class TunerSailfish : public Tuner {
	private:
	resource_set_t *r_set = nullptr;
	bool r_granted = false;

	/// callbacks from resource system
	static void g_advice_cb(resource_set_t *resource_set, uint32_t resources, void *data);
	static void g_grant_cb(resource_set_t *resource_set, uint32_t resources, void *data);
	void advice_cb(resource_set_t *resource_set, uint32_t resources);
	void grant_cb(resource_set_t *resource_set, uint32_t resources);

	/// main function to get resources
	void get_resource();

	public:
	~TunerSailfish();
	void SetPlaying(bool p);
};
