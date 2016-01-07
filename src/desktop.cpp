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

#include <QGuiApplication>
#include <QQuickView>
#include <QtQml>

#include <iostream>
#include <fstream>

#include "ObjectSaver.hpp"
#include "PitchDetection.hpp"
#include "Tuner.hpp"

Q_DECL_EXPORT int main(int argc, char* argv[])
{
	if (argc == 2) {
		PitchDetection::analyse_file(argv[1]);
		return 0;
	}

	qmlRegisterType<Tuner>("LJTuner", 1, 0, "Tuner");
	qmlRegisterType<ObjectSaver>("LJUtils", 1, 0, "ObjectSaver");

	QGuiApplication app(argc, argv);
	QQuickView view;
	view.setSource(QUrl("qrc:///qml/Desktop.qml"));
	view.setResizeMode(view.SizeRootObjectToView);
	view.show();
	return app.exec();
}
