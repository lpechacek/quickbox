#include "table.h"
#include "model.h"
#include "cellrenderer.h"
#include "application.h"
#include "appclioptions.h"

#include <qf/core/log.h>

#include <QTimer>

Table::Table(QWidget *parent)
	: Super(parent)
{
	qfLogFuncFrame();
	m_updateRowCountTimer = new QTimer(this);
	m_updateRowCountTimer->setSingleShot(true);
	connect(m_updateRowCountTimer, &QTimer::timeout, this, &Table::updateRowCount);

	m_scrollTimer = new QTimer(this);
	Application *app = Application::instance();
	AppCliOptions *cliopts = app->cliOptions();
	m_columnCount = cliopts->columnCount();
	m_scrollTimer->start(cliopts->refreshTime());
	connect(m_scrollTimer, &QTimer::timeout, [this]() {
		model()->shift();
		update();
	});
}

void Table::resetCellSize()
{
	m_cellSize = QSize();
	updateRowCount();
}

void Table::paintEvent(QPaintEvent *event)
{
	Super::paintEvent(event);
	if(m_rowCount == 0)
		return;
	Application *app = Application::instance();
	ClassCellRenderer cr(m_cellSize, this);
	StartListCellRenderer rr_startlist(m_cellSize, this);
	ResultsCellRenderer rr_results(m_cellSize, this);
	RunnersListCellRenderer *rr;
	if(app->cliOptions()->profile() == QLatin1String("results"))
		rr = &rr_results;
	else
		rr = &rr_startlist;
	int ix = 0;
	for (int j = 0; j < m_columnCount; ++j) {
		for (int i = 0; i < m_rowCount; ++i) {
			QPoint pos(j * m_cellSize.width(), i * m_cellSize.height());
			QVariantMap data = model()->data(ix);
			QString data_type = data.value(QStringLiteral("type")).toString();
			if(data_type == QLatin1String("classInfo"))
				cr.draw(pos, data, this);
			else
				rr->draw(pos, data, this);
			ix++;
		}
	}
}

void Table::resizeEvent(QResizeEvent *event)
{
	Super::resizeEvent(event);
	m_updateRowCountTimer->start(100);
}

void Table::updateRowCount()
{
	qfLogFuncFrame() << m_cellSize;
	if(!m_cellSize.isValid()) {
		QFontMetrics fm(font(), this);
		qfInfo() << "h:" << fm.height() << "asc:" << fm.ascent() << "desc:" << fm.descent();
		Application *app = Application::instance();
		AppCliOptions *cliopts = app->cliOptions();
		m_cellSize.setHeight(fm.height() * cliopts->fontScale() / 100);
	}
	QSize frame_size = geometry().size();
	m_rowCount = frame_size.height() / m_cellSize.height();
	int rest = frame_size.height() % m_cellSize.height();
	m_cellSize.setHeight(m_cellSize.height() + rest / m_rowCount);
	m_cellSize.setWidth(frame_size.width() / m_columnCount);
	qfDebug() << "new row count:" << m_rowCount << "cell size:" << m_cellSize;
	update();
}

Model *Table::model()
{
	if(!m_model) {
		m_model = new Model(this);
	}
	return m_model;
}
