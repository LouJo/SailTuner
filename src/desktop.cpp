#include <QGuiApplication>
#include <QQuickView>
#include <QtQml>

#include <iostream>
#include <fstream>

#include "Tuner.hpp"

static void analyse_file(const char *filename)
{
	using namespace std;
	cout << "analyse file " << filename << endl;
	ifstream fin;
	fin.open(filename);

	const int nb_frame = 1024;
	Tuner *tuner = new Tuner();
	int16_t buffer[nb_frame];

	while (1) {
		fin.read((char*) buffer, sizeof(buffer));
		tuner->AudioAnalyse(buffer, sizeof(buffer) >> 1);

		cout << tuner->GetFreq() << " " << tuner->GetNoteName() << " " << tuner->GetOctave() << endl;

		if (fin.eof()) break;
	}
	delete tuner;
}

Q_DECL_EXPORT int main(int argc, char* argv[])
{
	if (argc == 2) {
		analyse_file(argv[1]);
		return 0;
	}

	qmlRegisterType<Tuner>("LJTuner", 1, 0, "Tuner");

	QGuiApplication app(argc, argv);
	QQuickView view;
	view.setSource(QUrl("qrc:///qml/Desktop.qml"));
	view.show();
	return app.exec();
}
