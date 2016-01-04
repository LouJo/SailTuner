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

#ifndef __TEMPERAMENTS_HPP
#define __TEMPERAMENTS_HPP

#include <QString>
#include <vector>

/**
 * Temperament management
 *
 * Check temperaments in files in a dir
 * Get list and temperements
 */
class Temperaments {
	private:
	/// describe internal a temperament
	struct temp_t {
		QString name; ///< temperament name
		QString file; ///< file name
		int seek; ///< position in file
	};

	static const int nb_notes = 12;

	unsigned int current;
	std::vector<temp_t> list;
	double notes[nb_notes];

	/// Check data files in directory
	void GetDir(const QString & dirname);
	/// Check temperaments in file
	void CheckFile(const QString & filename);
	/// Load given temperament
	bool CheckoutTemperament(const temp_t & temperament);

	public:
	/// constructor with dir name to find data files
	Temperaments(const QString & dirname);
	~Temperaments() {}

	/// set current temperament
	bool SetTemperament(const QString name);
	bool SetTemperament(unsigned int index);
	/// get index of current temperament
	unsigned int GetCurrentIndex() const;
	/// get the name of current temperament
	QString GetCurrentName() const;
	/// get the name of temperament number n
	QStringList GetNames() const;
	/// get notes frequencies as double[nb_notes]
	const double * NotesFrequencies() const;
};

#endif
