#include "action.h"

#include <qf/core/assert.h>

#include <QMenu>

using namespace qf::qmlwidgets;

Action::Action(const QIcon &icon, const QString &text, QObject *parent)
	: QAction(parent)
{
	setIcon(icon);
	setText(text);
}

void Action::setText(const QString &new_text)
{
	Super::setText(new_text);
	emit textChanged(new_text);
}

QString Action::shortcut() const
{
	QKeySequence ks = Super::shortcut();
	return ks.toString();
}

void Action::setShortcut(const QString &new_text)
{
	QKeySequence ks(new_text);
	if(ks != Super::shortcut()) {
		Super::setShortcut(ks);
		emit shortcutChanged(shortcut());
	}
}

void Action::addActionInto(QAction *action)
{
	QMenu *w = menu();
	QF_ASSERT(w!=nullptr, "bad menu", return);

	// reparent action to containing widget to allow parentMenu() functionality
	action->setParent(w);
	w->addAction(action);
}

void Action::addActionBefore(QAction *action)
{
	QWidget *w = parentMenu();
	QF_ASSERT(w!=nullptr, "bad parent", return);
	//qfInfo() << w << this << action;
	action->setParent(w);
	w->insertAction(this, action);
}

void Action::addActionAfter(QAction *new_act)
{
	QWidget *parent_w = parentMenu();
	QF_ASSERT(parent_w!=nullptr, "bad parent", return);
	new_act->setParent(parent_w);
	Action *next_act = nullptr;
	Q_FOREACH(auto a, parent_w->actions()) {
		Action *aa = qobject_cast<Action*>(a);
		if(!aa)
			continue;
		if(aa == this) {
			next_act = aa;
		}
		else if(next_act) {
			next_act = aa;
			break;
		}
	}
	//qfDebug() << "\t created menu" << new_act;
	if(next_act)
		parent_w->insertAction(next_act, new_act);
	else
		parent_w->addAction(new_act);
}

Action *Action::addMenuInto(const QString &id, const QString &text)
{
	Action *ret = nullptr;
	QWidget *parent_w = this->menu();
	if(parent_w) {
		QMenu *m = new QMenu(parent_w);
		//m->setTitle(text);
		ret = new Action(parent_w);
		ret->setMenu(m);
		ret->setOid(id);
		ret->setText(text);
		addActionInto(ret);
	}
	return ret;
}

Action* Action::addMenuAfter(const QString &id, const QString &text)
{
	QWidget *parent_w = parentMenu();
	QMenu *m = new QMenu(parent_w);
	//m->setTitle(text);
	Action *new_act = new Action(parent_w);
	new_act->setMenu(m);
	new_act->setOid(id);
	new_act->setText(text);
	addActionAfter(new_act);
	return new_act;
}

Action *Action::addMenuBefore(const QString &id, const QString &text)
{
	QWidget *parent_w = parentMenu();
	QMenu *m = new QMenu(parent_w);
	Action *new_act = new Action(parent_w);
	new_act->setMenu(m);
	new_act->setOid(id);
	new_act->setText(text);
	parent_w->insertAction(this, new_act);
	return new_act;
}

Action* Action::addSeparatorInto(const QString &id)
{
	QMenu *w = menu();
	QF_ASSERT(w != nullptr, "bad menu", return nullptr);

	Action *a = new Action(w);
	a->setObjectName(id);
	a->setSeparator(true);
	w->addAction(a);
	return a;
}

Action* Action::addSeparatorBefore(const QString &id)
{
	QWidget *w = parentMenu();
	QF_ASSERT(w != nullptr, "bad parent", return nullptr);

	Action *a = new Action(w);
	a->setObjectName(id);
	a->setSeparator(true);
	w->insertAction(this, a);
	return a;
}

QWidget *Action::parentMenu()
{
	QWidget *w = qobject_cast<QWidget*>(this->parent());
	if(!w) {
		qfWarning() << this << "Action parent is not kind of QWidget" << this->parent();
	}
	return w;
}

