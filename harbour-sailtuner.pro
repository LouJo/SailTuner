QT += qml quick gui multimedia dbus
TARGET = harbour-sailtuner

CONFIG += c++11 sailfishapp sailfishapp_i18n

DEFINES += TARGET=\""$(TARGET")\"

RESOURCES += \
	qml/sailfish.qrc

SOURCES += \
	src/sailfish.cpp \
	src/Tuner.cpp \
	src/audio/LinearFilter.cpp \
	src/audio/ZeroCross.cpp \
	src/scale/Scale.cpp

HEADERS += \
	src/Tuner.hpp \
	src/audio/LinearFilter.hpp \
	src/audio/ZeroCross.hpp \
	src/scale/Scale.hpp
