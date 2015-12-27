#include <QGuiApplication>
#include <QQuickView>
#include <QtQml>
#include <sailfishapp.h>
#include "Tuner.hpp"

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
	qmlRegisterType<Tuner>("harbour.sailtuner.tuner", 1, 0, "Tuner");
	Main *appli = new Main(argc, argv);
	return appli->Launch();
}
