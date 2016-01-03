QT += qml quick gui multimedia dbus
TARGET = Tuner

CONFIG += c++11

# PKGCONFIG += libpulse

SOURCES += \
	src/desktop.cpp \
	src/Tuner.cpp \
	src/audio/LinearFilter.cpp \
	src/audio/ZeroCross.cpp \
	src/scale/Scale.cpp \
	src/scale/Temperaments.cpp

HEADERS += \
	src/Tuner.hpp \
	src/audio/LinearFilter.hpp \
	src/audio/ZeroCross.hpp \
	src/scale/Scale.hpp \
	src/scale/Temperaments.hpp

RESOURCES += \
	qml/desktop.qrc
