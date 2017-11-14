#ifndef RUNS_RUNSPLUGIN_H
#define RUNS_RUNSPLUGIN_H

#include "../runspluginglobal.h"

#include <qf/qmlwidgets/framework/plugin.h>

#include <qf/core/utils.h>
#include <qf/core/utils/table.h>

namespace qf {
	namespace core {
		namespace utils {
			class Table;
		}
	}
	namespace qmlwidgets {
		class Action;
		namespace framework {
			class PartWidget;
			class DockWidget;
		}
	}
}

namespace Runs {

class RUNSPLUGIN_DECL_EXPORT RunsPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	Q_PROPERTY(qf::qmlwidgets::framework::PartWidget* partWidget READ partWidget FINAL)
	Q_PROPERTY(int selectedStageId READ selectedStageId WRITE setSelectedStageId NOTIFY selectedStageIdChanged)
private:
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	static constexpr int UNREAL_TIME_MS = 999 * 60 * 1000;
public:
	RunsPlugin(QObject *parent = nullptr);
	~RunsPlugin() Q_DECL_OVERRIDE;

	QF_PROPERTY_IMPL2(int, s, S, electedStageId, 1)

	qf::qmlwidgets::framework::PartWidget *partWidget() {return m_partWidget;}

	Q_SIGNAL void nativeInstalled();

	const qf::core::utils::Table& runnersTable(int stage_id);
	Q_SLOT void clearRunnersTableCache();

	Q_INVOKABLE int courseForRun(int run_id);
	Q_INVOKABLE int cardForRun(int run_id);
	Q_INVOKABLE QVariant currentStageResultsTableData(const QString &class_filter, int max_competitors_in_class = 0);
	Q_INVOKABLE QVariant stageResultsTableData(int stage_id, const QString &class_filter, int max_competitors_in_class = 0);
	qf::core::utils::Table nstagesResultsTable(int stages_count, int class_id, int places = -1, bool exclude_disq = true);
	Q_INVOKABLE QVariant nstagesResultsTableData(int stages_count, int places = -1, bool exclude_disq = true);
	Q_INVOKABLE void showRunsTable(int stage_id, int class_id, bool show_offrace, const QString &sort_column = QString(), int select_competitor_id = 0);

	Q_INVOKABLE QWidget* createReportOptionsDialog(QWidget *parent = nullptr);
	Q_INVOKABLE QWidget* createNStagesReportOptionsDialog(QWidget *parent = nullptr);

	Q_INVOKABLE bool reloadTimesFromCard(int run_id);

	Q_INVOKABLE QVariantMap printAwardsOptionsWithDialog(const QVariantMap &opts);
private:
	Q_SLOT void onInstalled();

	int courseForRun_Classic(int run_id);
private:
	qf::qmlwidgets::framework::PartWidget *m_partWidget = nullptr;
	qf::core::utils::Table m_runnersTableCache;
	int m_runnersTableCacheStageId = 0;
	qf::qmlwidgets::framework::DockWidget *m_eventStatisticsDockWidget = nullptr;
};

}

#endif // RUNS_RUNSPLUGIN_H
