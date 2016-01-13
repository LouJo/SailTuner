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
#include <sailfishapp.h>
#include "ObjectSaver.hpp"
#include "TunerSailfish.hpp"

class Main {
	private:
	QGuiApplication *app;
	QQuickView *view;

	public:
	Main(int &argc, char* argv[]) {
		app = SailfishApp::application(argc, argv);
		view = SailfishApp::createView();
		view->setSource(QUrl("qrc:///qml/Sailfish.qml"));
		QObject::connect(view->engine(), SIGNAL(quit()), app, SLOT(quit()));
	}
	Main() {}
	~Main() {}

	int Launch() {
		view->show();
		return app->exec();
	}
};

Q_DECL_EXPORT int main(int argc, char* argv[])
{
	if (argc == 2) {
		PitchDetection::analyse_file(argv[1]);
		return 0;
	}
	else if (argc == 3 && strcmp(argv[1], "record") == 0) {
		TunerWorker::set_record(argv[2]);
	}

	qmlRegisterType<Tuner>("harbour.sailtuner.tuner", 1, 0, "TunerSailfish");
	qmlRegisterType<ObjectSaver>("harbour.sailtuner.objectsaver", 1, 0, "ObjectSaver");

	Main *appli = new Main(argc, argv);
	return appli->Launch();
}
