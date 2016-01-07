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

#ifndef _OBJECT_SAVER_HPP
#define _OBJECT_SAVER_HPP

#include <QFile>
#include <QVariant>

/**
 * Class to load/save qml objects simply,
 * with limitations:
 * - only 1 depth properties
 * - double, int, string only (for the moment)
 */
class ObjectSaver : public QObject {
	Q_OBJECT

	/// object to load/save
	Q_PROPERTY(QObject* object READ getObject WRITE setObject NOTIFY objectChanged)
	/// name of save file
	Q_PROPERTY(QString filename READ getFilename WRITE setFilename)

	private:
	QObject *object;
	QString filename;

	QFile * getDataFile(bool is_write);

	public:
	ObjectSaver();
	~ObjectSaver();

	QString getFilename();
	void setFilename(QString name);
	QObject* getObject();
	void setObject(QObject *obj);

	public slots:
	/// load the object. auto called if object and filename setted
	void load();
	/// save object to disk
	void save();

	signals:
	void objectChanged();
};

#endif
