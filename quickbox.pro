include(utils.pri)

#version check qt
!minQtVersion(5, 3, 1) {
	message("Cannot build QuickBox with Qt version $${QT_VERSION}.")
	error("Use at least Qt 5.3.1.")
}

TEMPLATE = subdirs
CONFIG += ordered

# install paths
unix {
    LIBS_INSTALL_PATH = /lib
    PLUGINS_INSTALL_PATH = $$LIBS_INSTALL_PATH/qml
    QML_PLUGINS_INSTALL_PATH = $$PLUGINS_INSTALL_PATH
    BINS_INSTALL_PATH = /bin
    TRANSLATIONS_INSTALL_PATH = $${BINS_INSTALL_PATH}/translations
    SOUND_FILE_INSTALL_PATH = $${BINS_INSTALL_PATH}/quickevent-data/style/sound
} else {
    LIBS_INSTALL_PATH = /
    PLUGINS_INSTALL_PATH = $$LIBS_INSTALL_PATH/qml
    QML_PLUGINS_INSTALL_PATH = $$PLUGINS_INSTALL_PATH
    BINS_INSTALL_PATH = /bin
    TRANSLATIONS_INSTALL_PATH = $${BINS_INSTALL_PATH}/translations
    SOUND_FILE_INSTALL_PATH = $${BINS_INSTALL_PATH}/quickevent-data/style/sound
}
include(generate_install_paths.pri)

#message (config: $$CONFIG)

SUBDIRS += \
	libqf \

qfsqldbfs {
	SUBDIRS += \
		tools \

}
else:qsqlmon {
	SUBDIRS += \
		tools \

}
else:quickshow-android {
	SUBDIRS += \
		quickshow \

}
else {
	SUBDIRS += \
		libsiut \
		libquickevent \
		quickevent \
		quickhttpd \
		quickshow \
		tools \

}

OTHER_FILES += \
	.travis.yml

