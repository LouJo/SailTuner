QT += qml quick gui multimedia dbus
TARGET = harbour-sailtuner

CONFIG += c++11 qt link_pkgconfig sailfishapp sailfishapp_i18n sailfishapp_no_deploy_qml

DEFINES += TARGET=\""$(TARGET")\"

## see rpm/Makefile for installing libpulse and libresource

PKGCONFIG += libpulse-simple libresource

RESOURCES += \
	qml/sailfish.qrc \
	data/temperaments.qrc

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
