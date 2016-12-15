#ifndef EVENTSTATISTICSWIDGET_H
#define EVENTSTATISTICSWIDGET_H

#include <QWidget>

namespace qf { namespace core { namespace model { class SqlTableModel; }}}

namespace Ui {
class EventStatisticsWidget;
}

class FooterView;
class FooterModel;
class EventStatisticsModel;

class EventStatisticsWidget : public QWidget
{
	Q_OBJECT
public:
	explicit EventStatisticsWidget(QWidget *parent = 0);
	~EventStatisticsWidget();

	Q_SLOT void reloadLater();
	void reload();
	Q_SLOT void onDbEventNotify(const QString &domain, const QVariant &payload);
	Q_SLOT void onVisibleChanged(bool is_visible);

	Q_SLOT void loadPersistentSettings();
	Q_SLOT void savePersistentSettings();
private slots:
	void on_btReload_clicked();
	void on_btPrintResults_clicked();
	void on_btClearNewInSelectedRows_clicked();
private:
	int currentStageId();
	QTimer* reloadLaterTimer();
	void clearNewResults(const QList<int> &classdefs_ids, const QList<int> &runners_finished);
private:
	Ui::EventStatisticsWidget *ui;
	FooterView *m_tableFooterView = nullptr;
	FooterModel *m_tableFooterModel = nullptr;
	EventStatisticsModel *m_tableModel = nullptr;
	QTimer *m_reloadLaterTimer = nullptr;
};

#endif // EVENTSTATISTICSWIDGET_H
