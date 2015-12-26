#include <QGuiApplication>
#include <QQuickView>
#include <QtQml>

int main(int argc, char* argv[])
{
//	qmlRegisterType<DBMeter>("LJDBMeter", 1, 0, "DBMeter");

	QGuiApplication app(argc, argv);
	QQuickView view;
	view.setSource(QUrl("qrc:///qml/Desktop.qml"));
	view.show();
	return app.exec();
}
