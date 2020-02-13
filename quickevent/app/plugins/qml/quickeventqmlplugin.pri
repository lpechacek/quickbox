PLUGIN_TOP_SRCDIR = $$PWD/$$PLUGIN_NAME
QF_PROJECT_TOP_SRCDIR = $$PWD/../../../..
QF_PROJECT_TOP_BUILDDIR = $$OUT_PWD/../../../../..

PLUGIN_NAMESPACE_PATH = quickevent

quickevent_pure_qml_plugin {

message ( PURE QML PLUGIN $$PLUGIN_TOP_SRCDIR )

win32: LIB_DIR_NAME = bin
else:  LIB_DIR_NAME = lib

DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME/qml/$$PLUGIN_NAMESPACE_PATH

OTHER_FILES += \
	$$PLUGIN_TOP_SRCDIR/$$PLUGIN_NAME/*

QML_IMPORT_PATH += \
	$$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME/$$PLUGIN_NAME

include($$shadowed($$QF_PROJECT_TOP_SRCDIR/install_paths.pri))
SRC_DATA_DIR = $$PLUGIN_TOP_SRCDIR
DEST_DATA_DIR = $$DESTDIR/$$PLUGIN_NAME
DATA_INSTALL_PATH = $${NOARCH_DATA_INSTALL_PATH}/qml/$$PLUGIN_NAMESPACE_PATH
include ( $$QF_PROJECT_TOP_SRCDIR/datafiles.pri )

}
else {

include ( $$QF_PROJECT_TOP_SRCDIR/qmlplugin.pri )

QT += qml

INCLUDEPATH += \
	$$QF_PROJECT_TOP_SRCDIR/libqf/libqfcore/include \
	$$QF_PROJECT_TOP_SRCDIR/libqf/libqfqmlwidgets/include \
	$$QF_PROJECT_TOP_SRCDIR/libquickevent/libquickeventcore/include \
	$$QF_PROJECT_TOP_SRCDIR/libquickevent/libquickeventgui/include \

message ($$PLUGIN_NAME INCLUDEPATH $$INCLUDEPATH)

LIBS += \
    -L$$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME \

unix: LIBS +=  \
	-Wl,-rpath,\'\$\$ORIGIN\'  \


LIBS += \
	-lqfcore \
	-lqfqmlwidgets \
	-lquickeventcore \
	-lquickeventgui \

}


