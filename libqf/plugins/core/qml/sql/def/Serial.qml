Int {
	function createSqlScript(options)
	{
		if(options.driverName.endsWith("SQLITE")) {
			var def = 'integer';
		}
		else {
			var def = (length > 32)? 'bigserial': 'serial';
		}
		if(primaryKey)
			def += " PRIMARY KEY";
		return def;
	}

	function metaTypeNameFn()
	{
		return (length > 32)? 'quint32': 'quint64';
	}
}
