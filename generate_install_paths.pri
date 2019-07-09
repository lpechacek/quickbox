defineReplace(qeRelativePath) {
	variable = $$1
	path = $$eval($$variable)
	equals(variable, BINS_INSTALL_PATH) {
		return("")
	} else {
		return("QE_$${variable}=$$relative_path($${path}, $$BINS_INSTALL_PATH)")
	}
}

target_paths = ""
for (name, $$list(LIBS_INSTALL_PATH PLUGINS_INSTALL_PATH BINS_INSTALL_PATH \
	QML_PLUGINS_INSTALL_PATH SOUND_FILE_INSTALL_PATH \
	TRANSLATIONS_INSTALL_PATH )) {
	target_paths += "$$name = $$eval($$name)"
	defines += $$qeRelativePath($$name)
}
target_paths += "DEFINES += $$defines"

write_file($$shadowed(install_paths.pri), target_paths)
