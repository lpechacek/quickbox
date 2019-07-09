message(including $$PWD)

QT += core gui qml widgets
# plugin sometimes cannot find Qt libraries, CardReader needs QtSerialPort.so
QT += serialport

CONFIG += C++11

TEMPLATE = app

QF_PROJECT_TOP_SRCDIR = $$PWD/../../..
QF_PROJECT_TOP_BUILDDIR = $$OUT_PWD/../../..

PROJECT_TOP_SRCDIR = $$PWD

DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/bin
TARGET = quickevent

win32: LIB_DIR_NAME = bin
else:  LIB_DIR_NAME = lib

INCLUDEPATH += \
$$QF_PROJECT_TOP_SRCDIR/libqf/libqfcore/include \
$$QF_PROJECT_TOP_SRCDIR/libqf/libqfqmlwidgets/include \
$$QF_PROJECT_TOP_SRCDIR/libquickevent/libquickeventcore/include \
$$QF_PROJECT_TOP_SRCDIR/libquickevent/libquickeventgui/include \

message(INCLUDEPATH: $$INCLUDEPATH)

LIBS += \
	-L$$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME \

LIBS +=      \
	-lqfcore  \
	-lqfqmlwidgets  \
	-lsiut \
	-lquickeventcore \
	-lquickeventgui \

# define essential plugin search paths
include($$shadowed($$QF_PROJECT_TOP_SRCDIR/install_paths.pri))
unix {
        REL_LIB_PATH = $$relative_path($$LIBS_INSTALL_PATH, $$BINS_INSTALL_PATH)
        REL_PLUGINS_PATH = $${REL_LIB_PATH}/qml
        QMAKE_LFLAGS += -Wl,-rpath,\'\$\$ORIGIN/$${REL_LIB_PATH}:\$\$ORIGIN/$${REL_PLUGINS_PATH}\'
}
else {
}

# exception backtrace support
CONFIG(debug, debug|release): unix: QMAKE_LFLAGS += -rdynamic

RESOURCES += \
    $$PWD/images/images.qrc \

RC_FILE = $$PWD/app.rc

//OTHER_FILES += \
//	$$PWD/main.qml

include ($$QF_PROJECT_TOP_SRCDIR/crosscompile-support.pri)
include ($$QF_PROJECT_TOP_SRCDIR/appdatafiles.pri)

include($$PWD/src/src.pri)

OTHER_FILES += \
    ../plugins/qml/Core/*.qml \
    ../plugins/qml/SqlDb/*.qml \
    ../plugins/qml/Event/*.qml \
    ../plugins/qml/Help/*.qml \

QML_IMPORT_PATH += \
    $$QF_PROJECT_TOP_BUILDDIR/lib/qml \
	$$QF_PROJECT_TOP_BUILDDIR/lib/qml/quickevent \


win32:CONFIG(debug, debug|release):CONFIG += console
#CONFIG += console

TRANSLATIONS += \
    $${TARGET}.cs_CZ.ts \
    $${TARGET}.nb_NO.ts \
