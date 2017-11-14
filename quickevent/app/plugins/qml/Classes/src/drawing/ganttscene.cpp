#include "ganttitem.h"
#include "ganttscene.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/assert.h>

#include <QFontMetrics>

using namespace drawing;

static Event::EventPlugin *eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto plugin = qobject_cast<Event::EventPlugin *>(fwk->plugin("Event"));
	QF_ASSERT(plugin != nullptr, "Bad plugin", return nullptr);
	return plugin;
}

GanttScene::GanttScene(QObject * parent)
	: Super(parent), m_ganttItem()
{
	setDisplayUnit(QFontMetrics(QFont()).lineSpacing() / 2);

}

void GanttScene::load(int stage_id)
{
	clear();
	Event::StageData stage_data = eventPlugin()->stageData(stage_id);
	m_useAllMaps = stage_data.isUseAllMaps();
	m_ganttItem = new GanttItem();
	addItem(m_ganttItem);
	m_ganttItem->load(stage_id);
	m_stageId = stage_id;
}

void GanttScene::save()
{
	if(m_ganttItem)
		m_ganttItem->save(m_stageId);
}

int GanttScene::duToMin(int n) const
{
	return pxToMin(n * displayUnit());
}

int GanttScene::pxToMin(int px) const
{
	static int px_per_min = minToPx(1);
	return px / px_per_min;
}

int GanttScene::minToPx(int min) const
{
	// shoul be always natural number;
	static int px_per_min = 3 * displayUnit() / 2;
	return min * px_per_min;
}

