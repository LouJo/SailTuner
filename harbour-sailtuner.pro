TARGET = harbour-sailtuner

CONFIG += c++11 qt link_pkgconfig sailfishapp sailfishapp_no_deploy_qml

QT += qml quick gui
QT += multimedia
QT += dbus

DEFINES += TARGET=\""$(TARGET")\"

## see rpm/Makefile for installing audioresource

PKGCONFIG += audioresource

SAILFISHAPP_ICONS = 86x86 108x108 128x128 256x256

RESOURCES += \
	qml/sailfish.qrc \
        data/temperaments.qrc

DISTFILES += \
        rpm/harbour-sailtuner.spec

SOURCES += \
	src/sailfish.cpp \
	src/PitchDetection.cpp \
	src/ObjectSaver.cpp \
	src/Tuner.cpp \
	src/TunerSailfish.cpp \
	src/TunerWorker.cpp \
	src/audio/FreqPlayer.cpp \
	src/audio/LinearFilter.cpp \
	src/audio/ZeroCross.cpp \
	src/scale/Scale.cpp \
	src/scale/Temperaments.cpp

HEADERS += \
	src/PitchDetection.hpp \
	src/ObjectSaver.hpp \
	src/Tuner.cpp \
	src/Tuner.hpp \
	src/TunerSailfish.hpp \
	src/TunerWorker.hpp \
	src/audio/FreqPlayer.hpp \
	src/audio/LinearFilter.hpp \
	src/audio/ZeroCross.hpp \
	src/scale/Scale.hpp \
	src/scale/Temperaments.hpp
