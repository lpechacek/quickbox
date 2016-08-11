#include "dbenum.h"

#include "query.h"
#include "../core/utils.h"
#include "../core/log.h"

#include <QSqlRecord>
#include <QColor>
#include <QJsonDocument>

using namespace qf::core::sql;

//============================================================
//                              DbEnum
//============================================================
QMap<QString, int> DbEnum::s_fieldMapping;

DbEnum::DbEnum(int id, const QString & group_name, const QString & group_id, const QString & caption)
{
	setId(id);
	setGroupName(group_name);
	setGroupId(group_id);
	setCaption(caption);
}

QString DbEnum::caption() const
{
	QString ret = m_values.value(FieldCaption).toString();
	if(ret.isEmpty())
		ret = groupId();
	return ret;
}

DbEnum::DbEnum(const Query &q)
{
	qfLogFuncFrame();
	QSqlRecord rec = q.record();
	qfDebug() << "\t" << rec.value(0).toString() << rec.value(1).toString();
	if(s_fieldMapping.isEmpty()) {
		for(int i=0; i<rec.count(); i++) {
			QString fld_name = rec.fieldName(i);
			if(qf::core::Utils::fieldNameEndsWith(fld_name, QStringLiteral("id")))
				s_fieldMapping[fld_name] = FieldId;
			else if(qf::core::Utils::fieldNameEndsWith(fld_name, QStringLiteral("groupName")))
				s_fieldMapping[fld_name] = FieldGroupName;
			else if(qf::core::Utils::fieldNameEndsWith(fld_name, QStringLiteral("groupId")))
				s_fieldMapping[fld_name] = FieldGroupId;
			else if(qf::core::Utils::fieldNameEndsWith(fld_name, QStringLiteral("pos")))
				s_fieldMapping[fld_name] = FieldPos;
			else if(qf::core::Utils::fieldNameEndsWith(fld_name, QStringLiteral("abbreviation")))
				s_fieldMapping[fld_name] = FieldAbbreviation;
			else if(qf::core::Utils::fieldNameEndsWith(fld_name, QStringLiteral("value")))
				s_fieldMapping[fld_name] = FieldValue;
			else if(qf::core::Utils::fieldNameEndsWith(fld_name, QStringLiteral("typeInfo")))
				s_fieldMapping[fld_name] = FieldTypeInfo;
			else if(qf::core::Utils::fieldNameEndsWith(fld_name, QStringLiteral("caption")))
				s_fieldMapping[fld_name] = FieldCaption;
			else if(qf::core::Utils::fieldNameEndsWith(fld_name, QStringLiteral("color")))
				s_fieldMapping[fld_name] = FieldColor;
			else if(qf::core::Utils::fieldNameEndsWith(fld_name, QStringLiteral("grants")))
				s_fieldMapping[fld_name] = FieldGrants;
		}
	}
	m_values.resize(LastFieldIndex);
	for(int i=0; i<rec.count(); i++) {
		qfDebug() << "\t" << i << "field:" << rec.fieldName(i);
		QString fld_name = rec.fieldName(i);
		int fld_ix = s_fieldMapping.value(fld_name, -1);
		if(fld_ix < 0)
			continue;
		QVariant v = rec.value(i);
		if(fld_ix == FieldValue) {
			QString s = v.toString().simplified();
			if(!s.isEmpty() && (s[0] == '{' || s[0] == '[')) {
				QByteArray ba = s.toUtf8();
				QJsonParseError err;
				QJsonDocument jsd = QJsonDocument::fromJson(ba, &err);
				if(err.error == QJsonParseError::NoError) {
					v = jsd.toVariant();
					m_values[fld_ix] = v;
				}
				else {
					qfError() << "DbEnum parse value ERROR:" << s << "\n" << err.errorString();
				}
			}
		}
		else if(fld_ix == FieldColor) {
			QString s = v.toString().trimmed();
			setColor(s);
		}
		else {
			m_values[fld_ix] = v;
		}
	}
	/*
	{
		QString localized_caption = caption();
		if(localized_caption.isEmpty()) localized_caption = groupId();
		{
			/// pokud existuje lokalizace v datech ma prednost
			QFDataTranslator *dtr = dataTranslator();
			if(dtr) {
				QString domain = "enumz." + groupName();
				QString src = groupId();
				QString lc_caption = dtr->translate(src, domain);
				if(lc_caption != src) localized_caption = lc_caption;
			}
		}
		values[FieldCaption] = localized_caption;
	}
	{
		QString localized_abbreviation = abbreviation();
		//if(localized_addreviation.isEmpty()) localized_addreviation = groupId()%":abbr";
		{
			/// pokud existuje lokalizace v datech ma prednost
			QFDataTranslator *dtr = dataTranslator();
			if(dtr) {
				QString domain = "enumz." + groupName();
				QString src = groupId()%":abbr";
				QString lc_abbr = dtr->translate(src, domain);
				if(lc_abbr != src) localized_abbreviation = lc_abbr;
			}
		}
		values[FieldAbbreviation] = localized_abbreviation;
	}
	*/
}

QColor DbEnum::color() const
{
	return m_values.value(FieldColor).value<QColor>();
}

void DbEnum::setColor(const QString &s)
{
	QColor c(s);
	setColor(c);
}

void DbEnum::setColor(const QColor &c)
{
	setValue(FieldColor, c);
}

void DbEnum::setValue(DbEnum::FieldIndexes ix, const QVariant & val)
{
	if(m_values.isEmpty())
		m_values.resize(LastFieldIndex);
	m_values[ix] = val;
}

QString DbEnum::fillInPlaceholders(const QString& text_with_placeholders) const
{
	if(!isValid())
		return QString();
	QString ret = text_with_placeholders;
	ret.replace(QStringLiteral("{{groupName}}"), groupName());
	ret.replace(QStringLiteral("{{groupId}}"), groupId());
	ret.replace(QStringLiteral("{{pos}}"), QString::number(pos()));
	ret.replace(QStringLiteral("{{abbreviation}}"), abbreviation());
	ret.replace(QStringLiteral("{{value}}"), value().toString());
	ret.replace(QStringLiteral("{{caption}}"), caption());
	return ret;
}

/*
QFDataTranslator* DbEnum::dataTranslator() const
{
	QFDataTranslator *ret = NULL;
	{
		QFAppDataTranslatorInterface *appi = dynamic_cast<QFAppDataTranslatorInterface *>(QCoreApplication::instance());
		if(appi) {
			ret = appi->dataTranslator();
		}
	}
	return ret;
}

QString DbEnum::localizedCaption() const
{
	QString ret = caption();
	QFDataTranslator *dtr = dataTranslator();
	if(dtr) ret = dtr->translate(ret, "enumz."%groupName());
	return ret;
}

QString DbEnum::localizedAbbreviation() const
{
	QString ret = abbreviation();
	QFDataTranslator *dtr = dataTranslator();
	if(dtr) ret = dtr->translate(ret, "enumz."%groupName());
	return ret;
}
*/
