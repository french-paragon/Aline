#ifndef PROJECTFILTERBYLABELPROXYMODEL_H
#define PROJECTFILTERBYLABELPROXYMODEL_H

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

#include <QAbstractListModel>

#include "../../aline_global.h"
#include "../editableitemmanager.h"

namespace Aline {

class LabelsTree;

class ALINE_EXPORT ProjectFilterByLabelProxyModel : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit ProjectFilterByLabelProxyModel(QObject *parent = nullptr);

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const override;

	void setSourceProject(EditableItemManager *sourceModel);

	void connectLabelTree(LabelsTree * labelTree);

Q_SIGNALS:

public Q_SLOTS:

protected:

	virtual void setRefsInLabel(QStringList const& refs);
	virtual void addRefsInLabel(QString const& newRef);
	virtual void removeRefsInLabel(QString const& oldRef);

	void resetFilter();

	QStringList _refsInLabel;
	EditableItemManager* _sourceProject;

	QMetaObject::Connection _connectionProject;

	QMetaObject::Connection _connectionChange;
	QMetaObject::Connection _connectionAdd;
	QMetaObject::Connection _connectionRemove;

};

} //namespace Aline

#endif // PROJECTFILTERBYLABELPROXYMODEL_H
