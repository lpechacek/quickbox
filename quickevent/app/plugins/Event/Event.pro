message(including plugin $$PWD)

PLUGIN_NAME = Event

include ( ../quickeventplugin.pri )

QT += widgets sql

# CONFIG += c++11 hide_symbols

include (src/src.pri)

RESOURCES += \

TRANSLATIONS += \
	$${PLUGIN_NAME}-cs_CZ.ts \
	$${PLUGIN_NAME}-pl_PL.ts \
	$${PLUGIN_NAME}-nb_NO.ts \
	$${PLUGIN_NAME}-ru_RU.ts \
	$${PLUGIN_NAME}-nl_BE.ts \

OTHER_FILES += \
	$$PWD/qml/*.qml \

