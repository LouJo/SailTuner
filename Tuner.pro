QT += qml quick gui multimedia
TARGET = Tuner

CONFIG += c++11

SOURCES += \
	src/desktop.cpp \
	src/Tuner.cpp \
	src/audio/LinearFilter.cpp \
	src/audio/ZeroCross.cpp

HEADERS += \
	src/Tuner.hpp \
	src/audio/LinearFilter.hpp \
	src/audio/ZeroCross.hpp

RESOURCES += \
	qml/desktop.qrc
