#include "competitorswidget.h"
#include "ui_competitorswidget.h"
#include "competitorwidget.h"
#include "thispartwidget.h"

#include "Competitors/competitordocument.h"
#include "Competitors/competitorsplugin.h"

#include "Event/eventplugin.h"

#include <quickevent/og/siid.h>

#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>
#include <qf/qmlwidgets/toolbar.h>
#include <qf/qmlwidgets/combobox.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/transaction.h>
#include <qf/core/assert.h>

#include <QLabel>

namespace qfs = qf::core::sql;
namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfm = qf::core::model;

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin*>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Event plugin!");
	return plugin;
}

static Competitors::CompetitorsPlugin* competitorsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Competitors::CompetitorsPlugin*>(fwk->plugin("Competitors"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Competitors plugin!");
	return plugin;
}

CompetitorsWidget::CompetitorsWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::CompetitorsWidget)
{
	ui->setupUi(this);

	ui->tblCompetitorsToolBar->setTableView(ui->tblCompetitors);

	ui->tblCompetitors->setCloneRowEnabled(false);
	ui->tblCompetitors->setPersistentSettingsId("tblCompetitors");
	ui->tblCompetitors->setRowEditorMode(qfw::TableView::EditRowsMixed);
	ui->tblCompetitors->setInlineEditSaveStrategy(qfw::TableView::OnEditedValueCommit);
	qfm::SqlTableModel *m = new qfm::SqlTableModel(this);
	m->addColumn("id").setReadOnly(true);
	m->addColumn("classes.name", tr("Class"));
	m->addColumn("competitorName", tr("Name"));
	m->addColumn("registration", tr("Reg")).setReadOnly(true);
	m->addColumn("siId", tr("SI")).setReadOnly(true).setCastType(qMetaTypeId<quickevent::og::SiId>());
	m->addColumn("note", tr("Note"));
	ui->tblCompetitors->setTableModel(m);
	m_competitorsModel = m;

	//connect(ui->tblCompetitors, SIGNAL(editRowInExternalEditor(QVariant,int)), this, SLOT(editCompetitor(QVariant,int)), Qt::QueuedConnection);
	connect(ui->tblCompetitors, &qfw::TableView::editRowInExternalEditor, this, &CompetitorsWidget::editCompetitor, Qt::QueuedConnection);
	connect(ui->tblCompetitors, &qfw::TableView::editSelectedRowsInExternalEditor, this, &CompetitorsWidget::editCompetitors, Qt::QueuedConnection);

	QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

CompetitorsWidget::~CompetitorsWidget()
{
	delete ui;
}

void CompetitorsWidget::settleDownInPartWidget(ThisPartWidget *part_widget)
{
	connect(part_widget, SIGNAL(resetPartRequest()), this, SLOT(reset()));
	connect(part_widget, SIGNAL(reloadPartRequest()), this, SLOT(reload()));
	/*
	qfw::Action *a = part_widget->menuBar()->actionForPath("station", true);
	a->setText("&Station");
	a->addActionInto(m_actCommOpen);
	*/
	qfw::ToolBar *main_tb = part_widget->toolBar("main", true);
	//main_tb->addAction(m_actCommOpen);
	QLabel *lbl_class;
	{
		lbl_class = new QLabel(tr("&Class "));
		main_tb->addWidget(lbl_class);
	}
	{
		m_cbxClasses = new qfw::ForeignKeyComboBox();
		m_cbxClasses->setMinimumWidth(fontMetrics().width('X') * 10);
		m_cbxClasses->setMaxVisibleItems(100);
		m_cbxClasses->setReferencedTable("classes");
		m_cbxClasses->setReferencedField("id");
		m_cbxClasses->setReferencedCaptionField("name");
		main_tb->addWidget(m_cbxClasses);
	}
	lbl_class->setBuddy(m_cbxClasses);
}

void CompetitorsWidget::lazyInit()
{
}

void CompetitorsWidget::reset()
{
	if(!eventPlugin()->isEventOpen()) {
		m_competitorsModel->clearRows();
		return;
	}
	{
		m_cbxClasses->blockSignals(true);
		m_cbxClasses->loadItems(true);
		m_cbxClasses->insertItem(0, tr("--- all ---"), 0);
		connect(m_cbxClasses, SIGNAL(currentDataChanged(QVariant)), this, SLOT(reload()), Qt::UniqueConnection);
		m_cbxClasses->blockSignals(false);
	}
	reload();
}

void CompetitorsWidget::reload()
{
	qfs::QueryBuilder qb;
	qb.select2("competitors", "*")
			.select2("classes", "name")
			.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
			.from("competitors")
			.join("competitors.classId", "classes.id")
			.orderBy("competitors.id");//.limit(10);
	int class_id = m_cbxClasses->currentData().toInt();
	if(class_id > 0) {
		qb.where("competitors.classId=" + QString::number(class_id));
	}
	m_competitorsModel->setQueryBuilder(qb);
	m_competitorsModel->reload();
}

void CompetitorsWidget::editCompetitor(const QVariant &id, int mode)
{
	qfLogFuncFrame() << "id:" << id << "mode:" << mode;
	auto *w = new CompetitorWidget();
	w->setWindowTitle(tr("Edit Competitor"));
	qfd::Dialog dlg(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
	dlg.setDefaultButton(QDialogButtonBox::Save);
	dlg.setCentralWidget(w);
	w->load(id, mode);
	auto *doc = qobject_cast<Competitors::CompetitorDocument*>(w->dataController()->document());
	QF_ASSERT(doc != nullptr, "Document is null!", return);
	if(mode == qfm::DataDocument::ModeInsert) {
		int class_id = m_cbxClasses->currentData().toInt();
		doc->setValue("competitors.classId", class_id);
	}
	connect(doc, &Competitors::CompetitorDocument::saved, ui->tblCompetitors, &qf::qmlwidgets::TableView::rowExternallySaved, Qt::QueuedConnection);
	connect(doc, &Competitors::CompetitorDocument::saved, competitorsPlugin(), &Competitors::CompetitorsPlugin::competitorEdited, Qt::QueuedConnection);
	//connect(doc, &Competitors::CompetitorDocument::competitorSaved, ui->tblCompetitors, &qf::qmlwidgets::TableView::rowExternallySaved, Qt::QueuedConnection);
	/*
	connect(w, &CompetitorWidget::editStartListRequest, [&dlg](int stage_id, int class_id, int competitor_id) {
		dlg.accept();
		emit eventPlugin()->editStartListRequest(stage_id, class_id, competitor_id);
	});
	*/
	dlg.exec();
}

void CompetitorsWidget::editCompetitors(int mode)
{
	if(mode == qfm::DataDocument::ModeDelete) {
		if(qfd::MessageBox::askYesNo(this, tr("Realy delete all the selected competitors? This action cannot be reverted."), false)) {
			qfs::Transaction transaction;
			int n = 0;
			for(int ix : ui->tblCompetitors->selectedRowsIndexes()) {
				int id = ui->tblCompetitors->tableRow(ix).value(ui->tblCompetitors->idColumnName()).toInt();
				if(id > 0) {
					Competitors::CompetitorDocument doc;
					doc.load(id, qfm::DataDocument::ModeDelete);
					doc.drop();
					n++;
				}
			}
			if(n > 0) {
				if(qfd::MessageBox::askYesNo(this, tr("Confirm deletion of %1 competitors.").arg(n), false)) {
					transaction.commit();
					ui->tblCompetitors->reload();
				}
				else {
					transaction.rollback();
				}
			}
		}
	}
}
