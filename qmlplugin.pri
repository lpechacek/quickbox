TEMPLATE = lib

CONFIG += plugin
CONFIG += c++11
CONFIG += hide_symbols

win32: LIB_DIR_NAME = bin
else:  LIB_DIR_NAME = lib

DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME/qml/$$PLUGIN_NAMESPACE_PATH
TARGET  = $${PLUGIN_NAME}plugin

include ($$QF_PROJECT_TOP_SRCDIR/crosscompile-support.pri)
include ($$PWD/qmlplugindatafiles.pri)

OTHER_FILES += \
	$$PLUGIN_TOP_SRCDIR/qml/* \

QML_IMPORT_PATH += \
	$$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME/qml \

message( PLUGIN_TOP_SRCDIR of $$PLUGIN_NAME: $$PLUGIN_TOP_SRCDIR )
message( PROJECT_TOPDIR: $$QF_PROJECT_TOP_SRCDIR )
message( PROJECT_TOP_BUILDDIR: $$QF_PROJECT_TOP_BUILDDIR )
message( OTHER_FILES: $$OTHER_FILES )
message( QML_IMPORT_PATH: $$QML_IMPORT_PATH )
message( DESTDIR: $$DESTDIR )
message( TARGET: $$TARGET )

include($$shadowed($$QF_PROJECT_TOP_SRCDIR/install_paths.pri))
target.path=$$PLUGINS_INSTALL_PATH
INSTALLS += target
