#include <QGuiApplication>
#include <QQuickView>
#include <QtQml>

#include <iostream>
#include <fstream>

#include "Tuner.hpp"

Q_DECL_EXPORT int main(int argc, char* argv[])
{
	if (argc == 2) {
		Tuner::analyse_file(argv[1]);
		return 0;
	}

	qmlRegisterType<Tuner>("LJTuner", 1, 0, "Tuner");

	QGuiApplication app(argc, argv);
	QQuickView view;
	view.setSource(QUrl("qrc:///qml/Desktop.qml"));
	view.setResizeMode(view.SizeRootObjectToView);
	view.show();
	return app.exec();
}
