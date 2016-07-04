#ifndef COMPETITORSWIDGET_H
#define COMPETITORSWIDGET_H

#include <QFrame>

namespace Ui {
class CompetitorsWidget;
}
namespace qf {
namespace core {
namespace model {
class SqlTableModel;
}
}
namespace qmlwidgets {
class ForeignKeyComboBox;
}
}

class ThisPartWidget;

class CompetitorsWidget : public QFrame
{
	Q_OBJECT
private:
	typedef QFrame Super;
public:
	explicit CompetitorsWidget(QWidget *parent = 0);
	~CompetitorsWidget() Q_DECL_OVERRIDE;

	void settleDownInPartWidget(ThisPartWidget *part_widget);
private:
	Q_SLOT void lazyInit();
	Q_SLOT void reset();
	Q_SLOT void reload();

	Q_SLOT void editCompetitor(const QVariant &id, int mode);
	Q_SLOT void editCompetitors(int mode);
private:
	Ui::CompetitorsWidget *ui;
	qf::core::model::SqlTableModel *m_competitorsModel;
	qf::qmlwidgets::ForeignKeyComboBox *m_cbxClasses = nullptr;
};

#endif // COMPETITORSWIDGET_H
