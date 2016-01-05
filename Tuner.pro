QT += qml quick gui multimedia dbus
TARGET = Tuner

CONFIG += c++11 debug

# PKGCONFIG += libpulse

SOURCES += \
	src/desktop.cpp \
	src/PitchDetection.cpp \
	src/Tuner.cpp \
	src/TunerWorker.cpp \
	src/audio/LinearFilter.cpp \
	src/audio/ZeroCross.cpp \
	src/scale/Scale.cpp \
	src/scale/Temperaments.cpp

HEADERS += \
	src/PitchDetection.hpp \
	src/Tuner.hpp \
	src/TunerWorker.hpp \
	src/audio/LinearFilter.hpp \
	src/audio/ZeroCross.hpp \
	src/scale/Scale.hpp \
	src/scale/Temperaments.hpp

RESOURCES += \
	qml/desktop.qrc \
	data/temperaments.qrc
