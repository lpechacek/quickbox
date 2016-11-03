#ifndef QF_CORE_QML_SQLQUERYBUILDER_H
#define QF_CORE_QML_SQLQUERYBUILDER_H

#include <qf/core/sql/querybuilder.h>

#include <QObject>

namespace qf {
namespace core {
namespace qml {

class SqlQueryBuilder : public QObject
{
	Q_OBJECT
public:
	explicit SqlQueryBuilder(QObject *parent = nullptr);
	~SqlQueryBuilder() Q_DECL_OVERRIDE;
public:
	Q_SIGNAL void cleared();
	//static void initScriptEngine(QScriptEngine *engine);
	//static QScriptValue toScriptValue(QScriptEngine *engine, SqlQueryBuilder* const &in);
	//static void fromScriptValue(const QScriptValue &object, SqlQueryBuilder* &out);
public:
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* select(const QString &fields, const QString &flags = QString()) {
		m_builder.select(fields, flags);
		return this;
	}
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* select2(const QString &table_name, const QString &fields, const QString &flags = QString()) {
		m_builder.select2(table_name, fields, flags);
		return this;
	}
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* from(const QString &table_name) {
		m_builder.from(table_name);
		return this;
	}
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* from(qf::core::qml::SqlQueryBuilder *table) {
		m_builder.from(table->m_builder);
		return this;
	}
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* join(const QString &t1_key, const QString &t2_key, const QString  &join_kind = "LEFT JOIN") {
		m_builder.join(t1_key, t2_key, join_kind);
		return this;
	}
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* join(const QString &join_expr) {
		m_builder.join(join_expr);
		return this;
	}
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* joinQuery(const QString &t1_key, const QString &t2_select_query, const QString &t2_key_field, const QString  &join_kind = "LEFT JOIN") {
		m_builder.joinQuery(t1_key, t2_select_query, t2_key_field, join_kind);
		return this;
	}
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* joinQuery(const QString &t1_key, qf::core::qml::SqlQueryBuilder *t2_select_query, const QString &t2_key_field, const QString  &join_kind = "LEFT JOIN") {
		m_builder.joinQuery(t1_key, t2_select_query->m_builder, t2_key_field, join_kind);
		return this;
	}
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* joinRestricted(const QString &t1_key, const QString &t2_key, const QString &where_restriction = QString(), const QString  &join_kind = "LEFT JOIN") {
		m_builder.joinRestricted(t1_key, t2_key, where_restriction, join_kind);
		return this;
	}
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* joinQueryRestricted(const QString &t1_key, const QString &t2_select_query, const QString &t2_key_field, const QString  &where_restriction = QString(), const QString  &join_kind = "LEFT JOIN") {
		m_builder.joinQueryRestricted(t1_key, t2_select_query, t2_key_field, where_restriction, join_kind);
		return this;
	}
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* joinQueryRestricted(const QString &t1_key, qf::core::qml::SqlQueryBuilder *t2_select_query, const QString &t2_key_field, const QString  &where_restriction = QString(), const QString  &join_kind = "LEFT JOIN") {
		m_builder.joinQueryRestricted(t1_key, t2_select_query->m_builder, t2_key_field, where_restriction, join_kind);
		return this;
	}
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* where(const QString &cond, const QString &oper = "AND") {
		m_builder.where(cond, oper);
		return this;
	}
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* groupBy(const QString &expr) {
		m_builder.groupBy(expr);
		return this;
	}
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* having(const QString &cond, const QString &oper = "AND") {
		m_builder.having(cond, oper);
		return this;
	}
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* orderBy(const QString &field_and_order_flag) {
		m_builder.orderBy(field_and_order_flag);
		return this;
	}
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* limit(int n) {
		m_builder.limit(n);
		return this;
	}
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* offset(int n) {
		m_builder.offset(n);
		return this;
	}
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* as(const QString &alias_name) {
		m_builder.as(alias_name);
		return this;
	}
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* clear() {
		m_builder.clear();
		emit cleared();
		return this;
	}
	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* clearWhere() {
		m_builder.takeWhere();
		return this;
	}

	Q_INVOKABLE QString toString() const {
		return m_builder.toString();
	}
public:
	const qf::core::sql::QueryBuilder& coreBuilder() {return m_builder;}
private:
	qf::core::sql::QueryBuilder m_builder;
};

}}}

#endif // QF_CORE_QML_SQLQUERYBUILDER_H
