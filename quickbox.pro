include(utils.pri)

#version check qt
!minQtVersion(5, 11, 0) {
	message("Cannot build QuickBox with Qt version $${QT_VERSION}.")
	error("Use at least Qt 5.11.0.")
}

TEMPLATE = subdirs
CONFIG += ordered

# install paths
contains(QMAKE_HOST.arch, x86_64|ppc64|aarch64) {
	libs_target_path = /usr/lib64/quickevent
}
else {
	libs_target_path = /usr/lib/quickevent
}

LIBS_INSTALL_PATH = $${libs_target_path}
PLUGINS_INSTALL_PATH = $${LIBS_INSTALL_PATH}/qml
BINS_INSTALL_PATH = /usr/bin
NOARCH_DATA_INSTALL_PATH = /usr/share/quickevent
QML_PLUGINS_INSTALL_PATH = $${NOARCH_DATA_INSTALL_PATH}/qml
TRANSLATIONS_INSTALL_PATH = $${NOARCH_DATA_INSTALL_PATH}/translations
SOUND_FILE_INSTALL_PATH = $${NOARCH_DATA_INSTALL_PATH}/style/sound
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

