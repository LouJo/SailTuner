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
#include <QMetaObject>
#include <QMetaProperty>
#include <QStandardPaths>
#include <QString>
#include <iostream>

#include "ObjectSaver.hpp"

#define program_name_(x) #x
#define program_name(x) program_name_(x)
#define PROGRAM_NAME program_name(TARGET)

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
		dir.mkpath(PROGRAM_NAME);
	}
	QString path = qDataDir + "/" + PROGRAM_NAME + "/" + filename;

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

/// which types we manage
static bool type_ok(QVariant::Type type)
{
	switch (type) {
		case QVariant::Int:
		case QVariant::Double:
		case QVariant::String:
			return true;

		default:
			return false;
	}
}

void ObjectSaver::load()
{
	QFile *file = getDataFile(false);
	if (!file) return;

	// load object from file
	QMetaProperty property;
	const QMetaObject *meta = object->metaObject();
	QString title, value;
	const char *t;
	int offset;
	char c;

	while (!file->atEnd()) {
		title = file->readLine(100).trimmed();
		t = title.toStdString().c_str();
		offset = meta->indexOfProperty(t);
		if (offset != -1 && !file->atEnd() && type_ok(meta->property(offset).type())) {
			value = file->readLine(100).trimmed();
			if (!object->setProperty(t, value /*value.toInt(&ok, 10)*/)) {
				qDebug() << __func__  << " set property " << title << " to " << value << " failed";
			}
			else {
				qDebug()  << __func__  << " set property " << title << " to " << value << " ok";
			}
		}
	}

	file->close();
	delete file;

	emit objectChanged();
}

void ObjectSaver::save()
{
	qDebug() << __func__;
	QFile *file = getDataFile(true);
	if (!file) return;

	// save object in file
	QMetaProperty property;
	const QMetaObject *meta = object->metaObject();

	for (int i = meta->propertyOffset(); i < meta->propertyCount(); i++) {
		property = meta->property(i);
		if (!type_ok(property.type())) continue;
		file->write(property.name());
		file->putChar('\n');
		file->write(object->property(property.name()).toByteArray());
		file->putChar('\n');
	}

	file->close();
	delete file;
}
