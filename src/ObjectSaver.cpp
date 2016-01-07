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

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

#include "ObjectSaver.hpp"

#define name_(x) #x
#define name(x) name_(x)
#define NAME name(TARGET)

ObjectSaver::ObjectSaver() :
	object(NULL)
{

}

ObjectSaver::~ObjectSaver()
{
}

QString ObjectSaver::getFilename()
{
	return filename;
}

void ObjectSaver::setFilename(QString name)
{
	filename = name;
	if (object && filename != "") load();
}

QObject * ObjectSaver::getObject()
{
	return object;
}

void ObjectSaver::setObject(QObject *obj)
{
	object = obj;
	if (filename != "") load();
}

/// Get the data file object opened, or NULL
QFile * ObjectSaver::getDataFile(bool is_write)
{
	if (filename == "") {
		qDebug() << __func__ << "filename empty";
		return NULL;
	}
	if (!object) {
		qDebug() << __func__ << "object null";
		return NULL;
	}
	QString qDataDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
	if (qDataDir == "") {
		qDebug() << __func__ << "config path empty. abort";
		return NULL;
	}
	if (is_write) {
		QDir dir(qDataDir);
		dir.mkpath(NAME);
	}
	QString path = qDataDir + "/" + NAME + "/" + filename;

	QFile *file = new QFile(path);

	if (!is_write && !file->exists()) {
		qDebug() << __func__ << path << " don't exist";
		delete file;
		return NULL;
	}
	if (!file->open(is_write ? (QIODevice::WriteOnly | QIODevice::Truncate) : QIODevice::ReadOnly)) {
		qDebug() << __func__ << "file not opened";
		delete file;
		return NULL;
	}
	return file;
}

void ObjectSaver::load()
{
	QFile *file = getDataFile(false);
	if (!file) return;
	file->close();
	delete file;
}

void ObjectSaver::save()
{
	QFile *file = getDataFile(true);
	if (!file) return;
	file->close();
	delete file;
}
