#include <QGuiApplication>
#include <QQuickView>
#include <QtQml>

#include "Tuner.hpp"

Q_DECL_EXPORT int main(int argc, char* argv[])
{
	qmlRegisterType<Tuner>("LJTuner", 1, 0, "Tuner");

	QGuiApplication app(argc, argv);
	QQuickView view;
	view.setSource(QUrl("qrc:///qml/Desktop.qml"));
	view.show();
	return app.exec();
}
