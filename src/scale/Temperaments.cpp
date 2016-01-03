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

#include <QDir>
#include <QDebug>

#include "Temperaments.hpp"

using namespace std;

Temperaments::Temperaments(const QString & dirname) : current(0)
{
	GetDir(dirname);
}

void Temperaments::GetDir(const QString & dirname)
{
	QDir dir(dirname);
	if (!dir.exists()) {
		qDebug() << __func__ << "dir doesn't exist: " << dirname;
		return;
	}
	dir.setFilter(QDir::Files);
	QStringList files = dir.entryList();
	for (auto &f : files) CheckFile(dirname + "/" + f);

	qDebug() << list.size() << "temperaments";
}

void Temperaments::CheckFile(const QString & filename)
{
	qDebug() << __func__ << "check " << filename;
	QFile file(filename);
	if (!file.exists()) {
		qDebug() << __func__ << "file doesn't exist: " << filename;
		return;
	}
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << __func__ << "cannot open file " << filename;
		return;
	}
	QByteArray line;
	QString name;
	int offset, f_pos;
	int n_line = 0;
	while (1) {
		f_pos = file.pos();
		line = file.readLine();
		if (line.count() == 0) break;
		n_line++;
		offset = line.indexOf(';');
		if (offset < 1) {
			qDebug() << __func__ << "cannot read line " << n_line;
			continue;
		}
		// add temperament
		name = line.left(offset);
		qDebug() << " -> add temperament" << name;
		list.push_back(temp_t{name, filename, f_pos});
	}
}

bool Temperaments::CheckoutTemperament(const temp_t &temp)
{
	QFile file(temp.file);
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << __func__ << "cannot open file " << temp.file;
		return false;
	}
	file.seek(temp.seek);
	QByteArray line = file.readLine();
	if (line.count() == 0) {
		qDebug() << __func__ << "empty line" << temp.file << "pos" << temp.seek;
		return false;
	}
	auto tab = line.trimmed().split(';');
	if (tab.count() != nb_notes + 1) {
		qDebug() << __func__ << "nb element not match" << temp.file << "pos" << temp.seek << "count" << tab.count();
		return false;
	}
	for (int i = 1; i < nb_notes + 1; i++) {
		notes[i-1] = tab[i].toDouble();
	}
	qDebug() << "temperament" << temp.name << "loaded";
	return true;
}

bool Temperaments::SetTemperament(unsigned int index)
{
	if (index >= list.size()) {
		qDebug() << __func__ << "index" << index << "out of range";
		return false;
	}
	current = index;
	return CheckoutTemperament(list[current]);
}

bool Temperaments::SetTemperament(const QString name)
{
	int idx = 0;
	for (auto &t : list) {
		if (t.name == name) {
			return SetTemperament(idx);
		}
		idx++;
	}
	return false;
}

unsigned int Temperaments::GetCurrentIndex() const
{
	return current;
}

QString Temperaments::GetCurrentName() const
{
	if (current >= list.size()) return "";
	return list[current].name;
}

QStringList  Temperaments::GetNames() const
{
	QStringList ret;
	for (auto & t : list) ret << t.name;
	return ret;
}

const double * Temperaments::NotesFrequencies() const
{
	return notes;
}
