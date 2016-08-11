#include "classeswidget.h"
#include "ui_classeswidget.h"

#include "Classes/classesplugin.h"
#include "editcodeswidget.h"
#include "editcourseswidget.h"
#include "drawing/drawingganttwidget.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/toolbar.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/filedialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>

#include <qf/core/string.h>
#include <qf/core/utils.h>
#include <qf/core/collator.h>
#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/connection.h>
#include <qf/core/assert.h>

#include <QDomDocument>
#include <QComboBox>
#include <QStyledItemDelegate>

namespace qfc = qf::core;
namespace qfw = qf::qmlwidgets;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;

class CourseItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	CourseItemDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

	Q_SIGNAL void courseIdChanged();

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE
	{
		Q_UNUSED(option)
		Q_UNUSED(index)
		auto *editor = new QComboBox(parent);
		QMapIterator<QString, int> it(m_courseNameToId);
		while(it.hasNext()) {
			it.next();
			editor->addItem(it.key(), it.value());
		}
		return editor;
	}
	void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE
	{
		auto *cbx = qobject_cast<QComboBox *>(editor);
		QF_ASSERT(cbx != nullptr, "Bad combo!", return);
		QString id = index.data(Qt::EditRole).toString();
		int ix = cbx->findData(id);
		cbx->setCurrentIndex(ix);
	}
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE
	{
		qfLogFuncFrame();
		auto *cbx = qobject_cast<QComboBox *>(editor);
		QF_ASSERT(cbx != nullptr, "Bad combo!", return);
		qfDebug() << "setting model data:" << cbx->currentText() << cbx->currentData();
		model->setData(index, cbx->currentData(), Qt::EditRole);
		emit const_cast<CourseItemDelegate*>(this)->courseIdChanged();
	}

	QString displayText(const QVariant &value, const QLocale &locale) const Q_DECL_OVERRIDE
	{
		Q_UNUSED(locale);
		return m_idToCourseName.value(value.toInt(), QStringLiteral("???"));
	}

	void setCourses(const QMap<int, QString> &courses)
	{
		m_idToCourseName = courses;
		m_courseNameToId.clear();
		QMapIterator<int, QString> it(m_idToCourseName);
		while(it.hasNext()) {
			it.next();
			m_courseNameToId.insertMulti(it.value(), it.key());
		}
	}

private:
	QMap<int, QString> m_idToCourseName;
	QMultiMap<QString, int> m_courseNameToId;
};

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin*>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Event plugin!");
	return plugin;
}

ClassesWidget::ClassesWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::ClassesWidget)
{
	ui->setupUi(this);
	ui->splitter->setPersistentSettingsId(ui->splitter->objectName());
	{
		ui->tblClasses->setPersistentSettingsId("tblClasses");
		ui->tblClasses->setInsertRowEnabled(true);
		ui->tblClasses->setCloneRowEnabled(false);
		ui->tblClasses->setRemoveRowEnabled(true);

		ui->tblClassesTB->setTableView(ui->tblClasses);
		qfm::SqlTableModel *m = new qfm::SqlTableModel(this);
		//m->setObjectName("classes.classesModel");
		m->addColumn("id").setReadOnly(true);
		m->addColumn("classes.name", tr("Class"));
		m->addColumn("classdefs.drawLock", tr("DL")).setToolTip(tr("Locked for drawing"));
		m->addColumn("classdefs.startTimeMin", tr("Start"));
		m->addColumn("classdefs.startIntervalMin", tr("Interval"));
		m->addColumn("classdefs.vacantsBefore", tr("VB")).setToolTip(tr("Vacants before"));
		m->addColumn("classdefs.vacantEvery", tr("VE")).setToolTip(tr("Vacant every"));
		m->addColumn("classdefs.vacantsAfter", tr("VA")).setToolTip(tr("Vacants after"));
		m->addColumn("classdefs.lastStartTimeMin", tr("Last")).setToolTip(tr("Start time of last competitor in class."));
		m->addColumn("runsCount", tr("Count")).setToolTip(tr("Runners count"));
		m->addColumn("classdefs.mapCount", tr("Maps"));
		m->addColumn("classdefs.courseId", tr("Course")).setAlignment(Qt::AlignLeft);
		//m->addColumn("courses.name", tr("Course")).setReadOnly(true);
		m->addColumn("courses.length", tr("Length"));
		m->addColumn("courses.climb", tr("Climb"));
		ui->tblClasses->setTableModel(m);

		m_courseItemDelegate = new CourseItemDelegate(this);
		ui->tblClasses->setItemDelegateForColumn(m->columnIndex("classdefs.courseId"), m_courseItemDelegate);

		connect(m_courseItemDelegate, &CourseItemDelegate::courseIdChanged, ui->tblClasses, &qfw::TableView::reloadCurrentRow, Qt::QueuedConnection);

		m_classesModel = m;
	}
	{
		ui->tblCourseCodes->setPersistentSettingsId("tblCourseCodes");
		ui->tblCourseCodes->setInsertRowEnabled(false);
		ui->tblCourseCodes->setCloneRowEnabled(false);
		ui->tblCourseCodes->setRemoveRowEnabled(false);
		ui->tblCourseCodesTB->setTableView(ui->tblCourseCodes);
		qfm::SqlTableModel *m = new qfm::SqlTableModel(this);
		//m->setObjectName("classes.coursesModel");
		m->addColumn("coursecodes.position", tr("Pos")).setReadOnly(true);
		m->addColumn("codes.code", tr("Code")).setReadOnly(true);
		m->addColumn("codes.altCode", tr("Alt")).setToolTip(tr("Code alternative")).setReadOnly(false);
		m->addColumn("codes.outOfOrder", tr("O")).setToolTip(tr("Out of order"));
		m->addColumn("codes.radio", tr("R")).setToolTip(tr("Radio"));
		ui->tblCourseCodes->setTableModel(m);
		m_courseCodesModel = m;
	}
	connect(ui->tblClasses, SIGNAL(currentRowChanged(int)), this, SLOT(reloadCourseCodes()));
}

ClassesWidget::~ClassesWidget()
{
	delete ui;
}

int ClassesWidget::selectedStageId()
{
	return m_cbxStage->currentData().toInt();
}

void ClassesWidget::settleDownInPartWidget(ThisPartWidget *part_widget)
{
	connect(part_widget, SIGNAL(resetPartRequest()), this, SLOT(reset()));
	connect(part_widget, SIGNAL(reloadPartRequest()), this, SLOT(reload()));

	qfw::Action *a_edit = part_widget->menuBar()->actionForPath("edit", true);
	a_edit->setText("&Edit");
	{
		qfw::Action *a = new qfw::Action("Cou&rses", this);
		connect(a, &QAction::triggered, this, &ClassesWidget::edit_courses);
		a_edit->addActionInto(a);
	}
	{
		qfw::Action *a = new qfw::Action("Co&des", this);
		connect(a, &QAction::triggered, this, &ClassesWidget::edit_codes);
		a_edit->addActionInto(a);
	}
	{
		qfw::Action *a = new qfw::Action("Classes &layout");
		a->setShortcut("Ctrl+L");
		a_edit->addActionInto(a);
		connect(a, &qfw::Action::triggered, [this]()
		{
			auto *w = new drawing::DrawingGanttWidget;
			qf::qmlwidgets::dialogs::Dialog dlg(this);
			//dlg.setButtons(QDialogButtonBox::Save);
			dlg.setCentralWidget(w);
			w->load(selectedStageId());
			dlg.exec();
		});
	}

	qfw::Action *a_import = part_widget->menuBar()->actionForPath("import", true);
	a_import->setText("&Import");
	{
		qfw::Action *a = new qfw::Action("OCad v8", this);
		connect(a, &QAction::triggered, this, &ClassesWidget::import_ocad_v8);
		a_import->addActionInto(a);
	}
	{
		qfw::Action *a = new qfw::Action("OCad IOF-XML 2.0", this);
		connect(a, &QAction::triggered, this, &ClassesWidget::import_ocad_iofxml_2);
		a_import->addActionInto(a);
	}
	{
		qfw::Action *a = new qfw::Action("OCad IOF-XML 3.0", this);
		connect(a, &QAction::triggered, this, &ClassesWidget::import_ocad_iofxml_3);
		a_import->addActionInto(a);
	}

	qfw::ToolBar *main_tb = part_widget->toolBar("main", true);
	//main_tb->addAction(m_actCommOpen);
	{
		QLabel *lbl = new QLabel(tr("Stage "));
		main_tb->addWidget(lbl);
	}
	{
		m_cbxStage = new QComboBox();
		main_tb->addWidget(m_cbxStage);
	}

}

void ClassesWidget::edit_courses()
{
	qf::qmlwidgets::dialogs::Dialog dlg(QDialogButtonBox::Close, this);
	auto *w = new EditCoursesWidget();
	dlg.setCentralWidget(w);
	dlg.exec();
}

void ClassesWidget::edit_codes()
{
	qf::qmlwidgets::dialogs::Dialog dlg(QDialogButtonBox::Close, this);
	auto *w = new EditCodesWidget();
	dlg.setCentralWidget(w);
	//auto *bt_apply = dlg.buttonBox()->button(QDialogButtonBox::Apply);
	//connect(bt_apply, &QPushButton::clicked, this, &MainWindow::askUserToRestartAppServer);
	dlg.exec();
}

void ClassesWidget::reset()
{
	{
		m_cbxStage->blockSignals(true);
		m_cbxStage->clear();
		for(int i=0; i<eventPlugin()->stageCount(); i++)
			m_cbxStage->addItem(tr("E%1").arg(i+1), i+1);
		m_cbxStage->blockSignals(false);
		connect(m_cbxStage, SIGNAL(currentIndexChanged(int)), this, SLOT(reload()), Qt::UniqueConnection);
	}
	reload();
}

void ClassesWidget::reload()
{
	if(!eventPlugin()->isEventOpen()) {
		m_classesModel->clearRows();
		m_courseCodesModel->clearRows();
		return;
	}
	int stage_id = selectedStageId();
	{
		qf::core::sql::QueryBuilder qb1;
		qb1.select("COUNT(runs.isRunning)")
				.from("runs")
				.join("runs.competitorId", "competitors.id")
				.where("competitors.classId=classdefs.classId")
				.where("runs.stageId=" QF_IARG(stage_id));
		qfs::QueryBuilder qb;
		qb.select2("classes", "*")
				.select2("classdefs", "*")
				.select2("courses", "id, name, length, climb")
				.select("(" + qb1.toString() + ") AS runsCount")
				.from("classes")
				.joinRestricted("classes.id", "classdefs.classId", "classdefs.stageId=" QF_IARG(stage_id))
				.join("classdefs.courseId", "courses.id")
				.orderBy("classes.name");//.limit(10);
		/*
		int class_id = m_cbxClasses->currentData().toInt();
		if(class_id > 0) {
			qb.where("competitors.classId=" + QString::number(class_id));
		}
		*/
		m_classesModel->setQueryBuilder(qb);
		m_classesModel->reload();
	}
	{
		qf::core::sql::Query q(m_classesModel->sqlConnection());
		q.exec("SELECT COUNT(*) FROM courses");
		bool ro = true;
		if(q.next())
			ro = (q.value(0).toInt() == 0);
		if(ro) {
			qfWarning() << tr("Courses are not imported, class table is read only.");
		}
		ui->tblClasses->setReadOnly(ro);
	}
	{
		QMap<int, QString> courses;
		qf::core::sql::Query q;
		q.exec("SELECT id, name, note FROM courses ORDER BY name, note");
		while(q.next()) {
			courses[q.value(0).toInt()] = q.value(1).toString() + ' ' + q.value(2).toString();
		}
		m_courseItemDelegate->setCourses(courses);
	}
	reloadCourseCodes();
}

void ClassesWidget::reloadCourseCodes()
{
	int current_course_id = 0;
	auto row = ui->tblClasses->selectedRow();
	if(!row.isNull())
		current_course_id = row.value("classdefs.courseId").toInt();
	if(current_course_id == 0) {
		ui->lblCourseCodes->setText("---");
	}
	else {
		ui->lblCourseCodes->setText(row.value("courses.name").toString());
	}
	{
		qfs::QueryBuilder qb;
		qb.select2("codes", "*")
				.select2("coursecodes", "position")
				.from("coursecodes")
				.join("coursecodes.codeId", "codes.id")
				.where("coursecodes.courseId=" QF_IARG(current_course_id))
				.orderBy("coursecodes.position");
		m_courseCodesModel->setQueryBuilder(qb);
		m_courseCodesModel->reload();
	}
}

void ClassesWidget::importCourses(const QList<CourseDef> &course_defs)
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *event_plugin = qobject_cast<Event::EventPlugin *>(fwk->plugin("Event"));
	auto *classes_plugin = qobject_cast<Classes::ClassesPlugin *>(fwk->plugin("Classes"));
	if(event_plugin && classes_plugin) {
		QString msg = tr("Delete all courses definitions for stage %1?").arg(selectedStageId());
		if(qfd::MessageBox::askYesNo(fwk, msg, false)) {
			QVariantList courses;
			for(const auto &cd : course_defs)
				courses << cd;
			{
				QJsonDocument doc = QJsonDocument::fromVariant(courses);
				//qfInfo() << doc.toJson();
			}
			classes_plugin->createCourses(selectedStageId(), courses);
			reload();
		}
	}
}

static QString normalize_course_name(const QString &course_name)
{
	QString ret = qf::core::Collator::toAscii7(QLocale::Czech, course_name, false);
	ret.replace(' ', QString());
	ret.replace(';', '+');
	ret.replace(',', '+');
	ret.replace(':', '+');
	ret.replace('-', '+');
	return ret;
}

void ClassesWidget::import_ocad_v8()
{
	QString fn = qfd::FileDialog::getOpenFileName(this, tr("Open file"));
	if(fn.isEmpty())
		return;
	QFile f(fn);
	if(f.open(QFile::ReadOnly)) {
		QStringList lines;
		while (true) {
			QByteArray ba = f.readLine();
			if(ba.isEmpty())
				break;
			lines << QString::fromUtf8(ba).trimmed();
		}
		try {
			QMap<QString, CourseDef> defined_courses_map;
			for(QString line : lines) {
				// [classname];coursename;0;lenght_km;climb;S1;dist_1;code_1[;dist_n;code_n];dist_finish;F1
				if(line.isEmpty())
					continue;
				QStringList class_names;
				qfc::String course_name = normalize_course_name(line.section(';', 1, 1));
				QString class_name = line.section(';', 0, 0);
				if(class_name.isEmpty()) {
					for(auto ch : {'-', ',', ':', '+'}) {
						if(course_name.contains(ch)) {
							class_names = course_name.splitAndTrim(ch);
							break;
						}
					}
					if(class_names.isEmpty() && !course_name.isEmpty())
						class_names << course_name;
				}
				else {
					class_names << class_name;
				}
				if(class_names.isEmpty()) {
					//class_names << course_name;
					qfWarning() << "cannot deduce class name, skipping line:" << line;
					continue;
				}
				if(defined_courses_map.contains(course_name)) {
					CourseDef cd = defined_courses_map.value(course_name);
					QStringList classes = cd.classes() << class_names;
					defined_courses_map[course_name].setClasses(classes);
					continue;
				}
				CourseDef &cd = defined_courses_map[course_name];
				cd.setName(course_name);
				cd.setClasses(class_names);
				{
					QString s = line.section(';', 3, 3).trimmed();
					s.replace('.', QString()).replace(',', QString());
					cd.setLenght(s.toInt());
				}
				{
					QString s = line.section(';', 4, 4).trimmed();
					s.replace('.', QString()).replace(',', QString());
					cd.setClimb(s.toInt());
				}
				{
					qfc::String s = line.section(';', 6);
					QVariantList codes;
					QStringList sl = s.splitAndTrim(';');
					for (int i = 0; i < sl.count()-2; i+=2) {
						bool ok;
						int code = sl[i+1].toInt(&ok);
						if(ok)
							codes << code;
						else
							QF_EXCEPTION(QString("Invalid code definition '%1' at sequence no: %2 in '%3'\nline: %4").arg(sl[i+1]).arg(i).arg(s).arg(line));
					}
					cd.setCodes(codes);
				}
			}
			importCourses(defined_courses_map.values());
		}
		catch (const qf::core::Exception &e) {
			qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
			qf::qmlwidgets::dialogs::MessageBox::showException(fwk, e);
		}
	}
}

static QString element_text(const QDomElement &parent, const QString &tag_name)
{
	QDomElement el = parent.firstChildElement(tag_name);
	return el.text();
}

static QString dump_element(const QDomElement &el)
{
	QString ret;
	QTextStream s(&ret);
	el.save(s, QDomNode::EncodingFromDocument);
	return ret;
}

void ClassesWidget::import_ocad_iofxml_2()
{
	qfLogFuncFrame();
	QString fn = qfd::FileDialog::getOpenFileName(this, tr("Open file"), QString(), tr("XML files (*.xml);; All files (*)"));
	if(fn.isEmpty())
		return;
	try {
		QFile f(fn);
		if(f.open(QFile::ReadOnly)) {
			QDomDocument xdoc;
			QString err_str; int err_line;
			if(!xdoc.setContent(&f, &err_str, &err_line))
				QF_EXCEPTION(QString("Error parsing xml file '%1' at line: %2").arg(err_str).arg(err_line));

			QDomNodeList xml_courses = xdoc.elementsByTagName(QStringLiteral("Course"));

			QList<CourseDef> defined_courses_list;
			for (int i = 0; i < xml_courses.count(); ++i) {
				QDomElement el_course = xml_courses.at(i).toElement();
				if(el_course.isNull())
					QF_EXCEPTION(QString("Xml file format error: bad element '%1'").arg("Course"));
				CourseDef coursedef;
				QString course_name = normalize_course_name(element_text(el_course, QStringLiteral("CourseName")));
				coursedef.setName(course_name);

				QStringList class_names;
				QDomNodeList xml_classes = el_course.elementsByTagName(QStringLiteral("ClassShortName"));
				for (int j = 0; j < xml_classes.count(); ++j) {
					QString class_name = xml_classes.at(j).toElement().text().trimmed();
					class_names << class_name;
				}
				if(class_names.isEmpty())
					class_names << course_name;
				coursedef.setClasses(class_names);

				QDomElement el_course_variantion = el_course.firstChildElement(QStringLiteral("CourseVariation"));
				if(el_course_variantion.isNull())
					QF_EXCEPTION(QString("Xml file format error: missing element '%1'").arg("CourseVariation"));
				coursedef.setLenght(element_text(el_course_variantion, QStringLiteral("CourseLength")).trimmed().toInt());
				coursedef.setClimb(element_text(el_course_variantion, QStringLiteral("CourseClimb")).trimmed().toInt());

				QMap<int, QVariant> codes;
				QDomNodeList xml_controls = el_course.elementsByTagName(QStringLiteral("CourseControl"));
				for (int j = 0; j < xml_controls.count(); ++j) {
					QDomElement el_control = xml_controls.at(j).toElement();
					int no = element_text(el_control, QStringLiteral("Sequence")).trimmed().toInt();
					if(no <= 0)
						QF_EXCEPTION(QString("Xml file format error: bad sequence number %1 in %2").arg(no).arg(dump_element(el_control)));
					int code = element_text(el_control, QStringLiteral("ControlCode")).trimmed().toInt();
					if(code <= 0)
						QF_EXCEPTION(QString("Xml file format error: bad control code %1 in %2").arg(code).arg(dump_element(el_control)));
					codes[no] = code;
				}
				coursedef.setCodes(codes.values());
				defined_courses_list << coursedef;
			}
			importCourses(defined_courses_list);
		}
	}
	catch (const qf::core::Exception &e) {
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		qf::qmlwidgets::dialogs::MessageBox::showException(fwk, e);
	}
}

void ClassesWidget::import_ocad_iofxml_3()
{
	qfLogFuncFrame();
	QString fn = qfd::FileDialog::getOpenFileName(this, tr("Open file"), QString(), "XML files (*.xml);; All files (*)");
	if(fn.isEmpty())
		return;
	try {
		QFile f(fn);
		if(f.open(QFile::ReadOnly)) {
			QDomDocument xdoc;
			QString err_str; int err_line;
			if(!xdoc.setContent(&f, &err_str, &err_line))
				QF_EXCEPTION(QString("Error parsing xml file '%1' at line: %2").arg(err_str).arg(err_line));

			QMap<QString, CourseDef> defined_courses;
			{
				QDomNodeList ndlst = xdoc.elementsByTagName(QStringLiteral("Course"));
				for (int i = 0; i < ndlst.count(); ++i) {
					QDomElement el_course = ndlst.at(i).toElement();
					if(el_course.isNull())
						QF_EXCEPTION(QString("Xml file format error: bad element '%1'").arg("Course"));
					QString course_name = normalize_course_name(element_text(el_course, QStringLiteral("Name")));
					if(course_name.isEmpty())
						QF_EXCEPTION(QString("Xml file format error: empty course name in '%1'").arg(dump_element(el_course)));
					CourseDef &coursedef = defined_courses[course_name];
					coursedef.setName(course_name);
					coursedef.setLenght(element_text(el_course, QStringLiteral("Length")).trimmed().toInt());
					coursedef.setClimb(element_text(el_course, QStringLiteral("Climb")).trimmed().toInt());

					QMap<int, QVariant> codes;
					QDomNodeList xml_controls = el_course.elementsByTagName(QStringLiteral("CourseControl"));
					int no = 0;
					for (int j = 0; j < xml_controls.count(); ++j) {
						QDomElement el_control = xml_controls.at(j).toElement();
						if(el_control.attribute(QStringLiteral("type")) != QLatin1String("Control"))
								continue;
						int code = element_text(el_control, QStringLiteral("Control")).trimmed().toInt();
						if(code <= 0)
							QF_EXCEPTION(QString("Xml file format error: bad control code %1 in %2").arg(code).arg(dump_element(el_control)));
						codes[++no] = code;
					}
					coursedef.setCodes(codes.values());
				}
			}
			{
				QDomNodeList ndlst = xdoc.elementsByTagName(QStringLiteral("ClassCourseAssignment"));
				for (int i = 0; i < ndlst.count(); ++i) {
					QDomElement class_assignment = ndlst.at(i).toElement();
					QString course_name = normalize_course_name(element_text(class_assignment, QStringLiteral("CourseName")));
					QString class_name = element_text(class_assignment, QStringLiteral("ClassName"));
					if(class_name.isEmpty())
						QF_EXCEPTION(QString("Xml file format error: empty class name in '%1'").arg(dump_element(class_assignment)));
					CourseDef &coursedef = defined_courses[course_name];
					coursedef.addClass(class_name);
				}
			}
			{
				// guess empty class names from course name
				QMutableMapIterator<QString, CourseDef> it(defined_courses);
				while(it.hasNext()) {
					it.next();
					CourseDef &cd = it.value();
					if(cd.classes().isEmpty())
						cd.setClasses(QStringList() << cd.name());
				}
			}
			{
				// split combined class names
				bool split_class_names_enabled = false;
				bool split_class_names_prompted = false;
				QMutableMapIterator<QString, CourseDef> it(defined_courses);
				while(it.hasNext()) {
					it.next();
					CourseDef &cd = it.value();
					QStringList class_names = cd.classes();
					QStringList split_class_names;
					for(const QString &class_name : class_names) {
						QString normalized_class_name = normalize_course_name(class_name);
						//qfInfo() << cd.name() << ":" << class_name << normalized_class_name;
						if(class_name != normalized_class_name || class_name.contains('+')) {
							if(!split_class_names_prompted) {
								split_class_names_enabled = qf::qmlwidgets::dialogs::MessageBox::askYesNo(this, tr("Class name '%1' seems to be combined, separate it to more classes?").arg(class_name));
								split_class_names_prompted = true;
							}
						}
						if(split_class_names_enabled)
							split_class_names << normalized_class_name.split('+');
						else {
							split_class_names << class_name;
						}
					}
					if(split_class_names_enabled)
						cd.setClasses(split_class_names);
				}
			}
			importCourses(defined_courses.values());
		}
	}
	catch (const qf::core::Exception &e) {
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		qf::qmlwidgets::dialogs::MessageBox::showException(fwk, e);
	}
}

#include "classeswidget.moc"
