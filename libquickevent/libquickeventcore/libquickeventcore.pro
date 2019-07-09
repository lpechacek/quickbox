TEMPLATE = lib
TARGET = quickeventcore

QT += core sql

CONFIG += qt dll
CONFIG += c++11 hide_symbols

DEFINES += QUICKEVENTCORE_BUILD_DLL

unix:DESTDIR = $$OUT_PWD/../../lib
win32:DESTDIR = $$OUT_PWD/../../bin

message(Target: $$TARGET)

LIBS += -L$$DESTDIR
LIBS += -lqfcore
LIBS += -lsiut

INCLUDEPATH += \
	$$PWD/../../libqf/libqfcore/include \
	$$PWD/../../libsiut/include

message(INCLUDEPATH: $$INCLUDEPATH)

include ( src/src.pri )

RESOURCES += libquickeventcore.qrc

TRANSLATIONS += \
    libquickeventcore.cs_CZ.ts \
    libquickeventcore.nb_NO.ts \

include($$shadowed(../../install_paths.pri))
target.path=$$LIBS_INSTALL_PATH
INSTALLS += target
