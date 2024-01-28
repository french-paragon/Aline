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

#include "projectfilterbylabelproxymodel.h"

#include "model/editableitemmanager.h"
#include "model/editableitem.h"

#include "model/labels/labelstree.h"

#include <QIcon>

namespace Aline {

ProjectFilterByLabelProxyModel::ProjectFilterByLabelProxyModel(QObject *parent) :
	QAbstractListModel(parent)
{

}

int ProjectFilterByLabelProxyModel::rowCount(const QModelIndex &parent) const {

	if (parent == QModelIndex()) {
		return _refsInLabel.size();
	}

	return 0;

}

QVariant ProjectFilterByLabelProxyModel::data(QModelIndex const& index, int role) const {

	if (_sourceProject == nullptr) {
		return QVariant();
	}

	int row = index.row();

	if (row < 0 or row >= _refsInLabel.size()) {
		return QVariant();
	}

	QString url = _refsInLabel[row];
	EditableItem* item = _sourceProject->loadItemByUrl(url);

	if (item == nullptr) {
		return QVariant();
	}

	switch (role) {
	case Qt::DisplayRole:
		return item->objectName() + (item->hasUnsavedChanges() ? " *" : "");
	case Qt::DecorationRole:
	{
		QString iconPath = item->iconInternalUrl();

		if (iconPath != "") {
			return QIcon(iconPath);
		}
	}
	break;

	case EditableItemManager::ItemRefRole:
		return item->getFullRefUrl();
	}

	return QVariant();
}


void ProjectFilterByLabelProxyModel::setRefsInLabel(QStringList const& refs) {

	_refsInLabel = refs;

	resetFilter();

}
void ProjectFilterByLabelProxyModel::addRefsInLabel(QString const& newRef) {

	_refsInLabel.push_back(newRef);

	resetFilter();

}

void ProjectFilterByLabelProxyModel::removeRefsInLabel(QString const& oldRef) {

	_refsInLabel.removeOne(oldRef);

	resetFilter();

}

void ProjectFilterByLabelProxyModel::setSourceProject(EditableItemManager *sourceModel) {

	_sourceProject = sourceModel;

	if (_connectionProject) {
		disconnect(_connectionProject);
	}

	if (sourceModel != nullptr) {
		_connectionProject = connect(sourceModel, &EditableItemManager::itemVisibleStateNeedRefresh, this, [this] (QString url) {
			int row = _refsInLabel.indexOf(url);

			if (row >= 0 and row < _refsInLabel.size()) {
				Q_EMIT dataChanged(index(row), index(row));
			}
		});
	}

	resetFilter();
}

void ProjectFilterByLabelProxyModel::connectLabelTree(LabelsTree * labelTree) {

	if (_connectionChange) {
		disconnect(_connectionChange);
	}

	if (_connectionAdd) {
		disconnect(_connectionAdd);
	}

	if (_connectionRemove) {
		disconnect(_connectionRemove);
	}

	if (labelTree == nullptr) {

		beginResetModel();
		_refsInLabel.clear();
		endResetModel();

		return;
	}

	_connectionChange = connect(labelTree, &LabelsTree::activeLabelFilterChanged,
								this, &ProjectFilterByLabelProxyModel::setRefsInLabel);

	_connectionChange = connect(labelTree, &LabelsTree::activeLabelFilterExtend,
								this, &ProjectFilterByLabelProxyModel::addRefsInLabel);

	_connectionChange = connect(labelTree, &LabelsTree::activeLabelFilterReduce,
								this, &ProjectFilterByLabelProxyModel::removeRefsInLabel);

	setRefsInLabel(labelTree->activeLabelFilter());
}

void ProjectFilterByLabelProxyModel::resetFilter() {

	beginResetModel();

	endResetModel();

}

} //namespace Sabrina
