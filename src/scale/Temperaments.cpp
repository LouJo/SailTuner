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

#include <fstream>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>

#include "Temperaments.hpp"

using namespace std;

Temperaments::Temperaments(const char * dirname) : current(0)
{
	GetDir(dirname);
}

void Temperaments::GetDir(const char * dirname)
{
	DIR *pdir = opendir(dirname);
	struct dirent *pent = NULL;
	struct stat filestat;
	string filepath;

	if (!pdir) {
		cerr << __func__ << ": impossible to open dir " << dirname << endl;
		return;
	}
	while((pent = readdir(pdir))) {
		// get only files
		if (pent->d_name[0] == '.') continue;
		filepath = dirname;
		filepath += "/";
		filepath	+= pent->d_name;
		if (stat(filepath.c_str(), &filestat )) continue;
		if (S_ISDIR( filestat.st_mode )) continue;

		CheckFile(filepath);
	}
}

void Temperaments::CheckFile(string filename)
{
	cout << __func__ << " " << filename << endl;
}

string Temperaments::GetCurrentName() const
{
	if (current >= list.size()) return "";
	return list[current].name;
}

string Temperaments::GetName(unsigned int n) const
{
	if (n < list.size()) return "";
	return list[n].name;
}

const double * Temperaments::NotesFrequencies() const
{
	return notes;
}
