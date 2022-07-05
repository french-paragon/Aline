#ifndef PROJECTTREEDOCKWIDGET_H
#define PROJECTTREEDOCKWIDGET_H

/*This file is part of the project Aline

Copyright (C) 2022 Paragon <french.paragon@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "../aline_global.h"
#include <QDockWidget>

#include "mainwindow.h"
#include "model/editableitemmanager.h"
#include <QSortFilterProxyModel>

class QMenu;

namespace Aline {

namespace Ui {
class ProjectTreeDockWidget;
}

class ALINE_EXPORT ProjectTreeDockWidget : public QDockWidget
{
	Q_OBJECT

public:
	explicit ProjectTreeDockWidget(MainWindow *parent = 0);
	~ProjectTreeDockWidget();

Q_SIGNALS:

	void itemCreationTriggered(QString itemTypeRef, QString ref);
	void itemDoubleClicked(QString itemRef);
	void itemSuppressionTriggered(QStringList itemRef);

private:

	void selectionChanged();

	void onItemCreationTriggered(QString itemTypeRef);

	void projectChanged(Aline::EditableItemManager* project);

	void reselectProject(Aline::EditableItemManager* project);
	void rebuildMenuWithoutProject();
	void rebuildMenu(Aline::EditableItemManager *project);

	void receiveDoubleClick(const QModelIndex &index);
	void supprButtonClicked();

	void buildTreeContextMenu(QPoint const& pos);

	MainWindow * _mw_parent;

	QMenu* _newItemMenu;
	QSortFilterProxyModel* _internalModel;

	Ui::ProjectTreeDockWidget *ui;

	QMetaObject::Connection _newItemFactoryWatcher;
	QMetaObject::Connection _itemCreationTrigger;
	QMetaObject::Connection _itemSuppresionTrigger;
};

} //namespace Sabrina

#endif // PROJECTTREEDOCKWIDGET_H
