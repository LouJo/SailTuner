QT += qml quick gui multimedia dbus
TARGET = Tuner

CONFIG += c++11 debug link_pkgconfig

DEFINES += TARGET=\""$(TARGET")\"

PKGCONFIG += libpulse-simple

SOURCES += \
	src/desktop.cpp \
	src/ObjectSaver.cpp \
	src/PitchDetection.cpp \
	src/Tuner.cpp \
	src/TunerWorker.cpp \
	src/audio/FreqPlayer.cpp \
	src/audio/LinearFilter.cpp \
	src/audio/ZeroCross.cpp \
	src/scale/Scale.cpp \
	src/scale/Temperaments.cpp

HEADERS += \
	src/PitchDetection.hpp \
	src/ObjectSaver.hpp \
	src/Tuner.hpp \
	src/TunerWorker.hpp \
	src/audio/FreqPlayer.hpp \
	src/audio/LinearFilter.hpp \
	src/audio/ZeroCross.hpp \
	src/scale/Scale.hpp \
	src/scale/Temperaments.hpp

RESOURCES += \
	qml/desktop.qrc \
	data/temperaments.qrc \
	images/desktop/images.qrc
