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

#include "indexrebasedproxymodel.h"

#include <QIdentityProxyModel>

using namespace Aline::ModelViewUtils;


IndexRebasedProxyModel::IndexRebasedProxyModel(QObject *parent) :
	QAbstractItemModel(parent),
	_sourceTarget(QModelIndex())
{
	_proxyNodeTree = nullptr;
}

IndexRebasedProxyModel::~IndexRebasedProxyModel() {
	if (_proxyNodeTree != nullptr) {
		delete _proxyNodeTree;
	}
}

QModelIndex IndexRebasedProxyModel::sourceTarget() const
{
	return _sourceTarget;
}

void IndexRebasedProxyModel::setSourceTarget(const QModelIndex &sourceTarget)
{
	if (sourceTarget != (QModelIndex) _sourceTarget && (sourceTarget.model() == _sourceModel || sourceTarget == QModelIndex()) ) {

		beginResetModel();

		_sourceTarget = sourceTarget;

		if (_sourceTarget.isValid()) {
			_sourceWasValid = true;
		} else {
			_sourceWasValid = false;
		}

		rebuildTreeProxy();
		endResetModel();

	} else if (sourceTarget != _sourceTarget) {
		throw std::invalid_argument("source target must be either an invalid index or an index from the current proxied model!");
	}
}

QModelIndex IndexRebasedProxyModel::index(int row, int column, const QModelIndex &parent) const {

	Node* pNode = getNodeFromIndex(parent);
	Node* n = pNode->getChild(row, column);

	return getIndexFromNode(n);

}
QModelIndex IndexRebasedProxyModel::parent(const QModelIndex &child) const {

	Node* n = getNodeFromIndex(child);
	return getIndexFromNode(n->parent);

}

QModelIndex IndexRebasedProxyModel::mapFromSource(const QModelIndex &sourceIndex) const {

	if (_sourceModel == nullptr) {
		return QModelIndex();
	}

	if (_sourceModel == nullptr || sourceIndex.model() != _sourceModel) {
		return QModelIndex();
	}

	if (sourceIndex.isValid()) {

		if (sourceIndex == (QModelIndex) _sourceTarget) {
			return QModelIndex();
		}

		QModelIndexList path;

		for (QModelIndex id = sourceIndex; id != (QModelIndex) _sourceTarget; id = id.parent()) { //check that the _sourceTarget is in the ancestors of the index.
			path << id;

			if (id == QModelIndex()) {
				return QModelIndex();
			}
		}

		Node* n = _proxyNodeTree;

		for (int i = 0; i < path.size(); i++) {
			QModelIndex sourceIdx = path[path.size()-i-1];
			n = n->getChild(sourceIdx.row(), sourceIdx.column());
		}

		return getIndexFromNode(n);

	} else {
		return QModelIndex();
	}

}

QModelIndex IndexRebasedProxyModel::mapToSource(const QModelIndex &proxyIndex) const {

	if (_sourceModel == nullptr) {
		return QModelIndex();
	}

	if (proxyIndex == QModelIndex()) {
		return _sourceTarget;
	}

	QModelIndexList path;

	for (QModelIndex id = proxyIndex; id != QModelIndex(); id = id.parent()) {
		path << id;
	}

	QModelIndex targetIdx = _sourceTarget;

	for (int i = 0; i < path.size(); i++) {
		QModelIndex sourceIdx = path[path.size()-i-1];
		targetIdx = _sourceModel->index(sourceIdx.row(), sourceIdx.column(), targetIdx);
	}

	return targetIdx;
}

int IndexRebasedProxyModel::rowCount(const QModelIndex &parent) const {

	Node* n = getNodeFromIndex(parent);

	if (n == nullptr) {
		return 0;
	}

	return n->nRows();
}
int IndexRebasedProxyModel::columnCount(const QModelIndex &parent) const {

	Node* n = getNodeFromIndex(parent);

	if (n == nullptr) {
		return 0;
	}

	return n->nCols();
}

void IndexRebasedProxyModel::setSourceModel(QAbstractItemModel *newSourceModel) {

	if (newSourceModel == _sourceModel) {
		return;
	}


	if (_rowAboutToBeDeleteWatch) {
		disconnect(_rowAboutToBeDeleteWatch);
	}

	if (_colAboutToBeDeleteWatch) {
		disconnect(_colAboutToBeDeleteWatch);
	}

	if (_rowDeleteWatch) {
		disconnect(_rowDeleteWatch);
	}

	if (_colDeleteWatch) {
		disconnect(_colDeleteWatch);
	}


	if (_rowAboutToBeInsertWatch) {
		disconnect(_rowAboutToBeInsertWatch);
	}

	if (_colAboutToBeInsertWatch) {
		disconnect(_colAboutToBeInsertWatch);
	}

	if (_rowInsertWatch) {
		disconnect(_rowInsertWatch);
	}

	if (_colInsertWatch) {
		disconnect(_colInsertWatch);
	}

	if (_rowAboutToBeMovedWatch) {
		disconnect(_rowAboutToBeMovedWatch);
	}
	if (_colAboutToBeMovedWatch) {
		disconnect(_colAboutToBeMovedWatch);
	}
	if (_rowMovedWatch) {
		disconnect(_rowMovedWatch);
	}
	if (_colMovedWatch) {
		disconnect(_colMovedWatch);
	}


	if (_sourceModelAboutToResetWatch) {
		disconnect(_sourceModelAboutToResetWatch);
	}

	if (_sourceModelResetWatch) {
		disconnect(_sourceModelResetWatch);
	}

	if (_dataChangedWatch) {
		disconnect(_dataChangedWatch);
	}

	beginResetModel();

	_sourceModel = newSourceModel;
	_sourceTarget = QModelIndex();

	if (_sourceModel == nullptr) {
		return;
	}


	_rowAboutToBeDeleteWatch = connect(newSourceModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &IndexRebasedProxyModel::onSourceRowAboutToBeRemoved);
	_colAboutToBeDeleteWatch = connect(newSourceModel, &QAbstractItemModel::columnsAboutToBeRemoved, this, &IndexRebasedProxyModel::onSourceColAboutToBeRemoved);

	_rowDeleteWatch = connect(newSourceModel, &QAbstractItemModel::rowsRemoved, this, &IndexRebasedProxyModel::onSourceRowRemoved);
	_colDeleteWatch = connect(newSourceModel, &QAbstractItemModel::columnsRemoved, this, &IndexRebasedProxyModel::onSourceColRemoved);


	_rowAboutToBeInsertWatch = connect(newSourceModel, &QAbstractItemModel::rowsAboutToBeInserted, this, &IndexRebasedProxyModel::onSourceRowAboutToBeInserted);
	_colAboutToBeInsertWatch = connect(newSourceModel, &QAbstractItemModel::columnsAboutToBeInserted, this, &IndexRebasedProxyModel::onSourceColAboutToBeInserted);

	_rowInsertWatch = connect(newSourceModel, &QAbstractItemModel::rowsInserted, this, &IndexRebasedProxyModel::onSourceRowInserted);
	_colInsertWatch = connect(newSourceModel, &QAbstractItemModel::columnsInserted, this, &IndexRebasedProxyModel::onSourceColInserted);


	_rowAboutToBeMovedWatch = connect(newSourceModel, &QAbstractItemModel::rowsAboutToBeMoved, this, &IndexRebasedProxyModel::onSourceRowAboutToBeMoved);
	_rowAboutToBeMovedWatch = connect(newSourceModel, &QAbstractItemModel::columnsAboutToBeMoved, this, &IndexRebasedProxyModel::onSourceColAboutToBeMoved);

	_rowAboutToBeMovedWatch = connect(newSourceModel, &QAbstractItemModel::rowsMoved, this, &IndexRebasedProxyModel::onSourceRowMoved);
	_rowAboutToBeMovedWatch = connect(newSourceModel, &QAbstractItemModel::columnsMoved, this, &IndexRebasedProxyModel::onSourceColMoved);


	_sourceModelAboutToResetWatch = connect(newSourceModel, &QAbstractItemModel::modelAboutToBeReset, this, &IndexRebasedProxyModel::onSourceModelAboutToReset);
	_sourceModelResetWatch = connect(newSourceModel, &QAbstractItemModel::modelReset, this, &IndexRebasedProxyModel::reset);

	_dataChangedWatch = connect(newSourceModel, &QAbstractItemModel::dataChanged, this, &IndexRebasedProxyModel::onSourceModelDataChanged);

	rebuildTreeProxy();

	endResetModel();
}

void IndexRebasedProxyModel::setSourceModel(QAbstractItemModel *newSourceModel, QModelIndex const& targetIndex) {

	setSourceModel(newSourceModel);

	setSourceTarget(targetIndex);
}

QVariant IndexRebasedProxyModel::data(const QModelIndex &index, int role) const {

	QModelIndex sourceIndex = mapToSource(index);

	if (sourceIndex.isValid()) {
		return sourceIndex.data(role);
	}

	return QVariant();

}

QHash<int, QByteArray> IndexRebasedProxyModel::roleNames() const {

	if (_sourceModel != nullptr) {
		return _sourceModel->roleNames();
	}

	return QAbstractItemModel::roleNames();

}

void IndexRebasedProxyModel::checkSourceTarget() {

	if (!_sourceTarget.isValid() && _sourceWasValid) {
		beginResetModel();
		endResetModel();
		_sourceWasValid = false;
	}

	if (_sourceTarget.isValid() && !_sourceWasValid) {
		beginResetModel();
		endResetModel();
		_sourceWasValid = true;
	}

}

void IndexRebasedProxyModel::onSourceRowAboutToBeInserted(QModelIndex const& sourceParent, int first, int last) {

	QModelIndex mapped = mapFromSource(sourceParent);
	if (mapped != QModelIndex() || sourceParent == _sourceTarget) {

		beginInsertRows(mapped, first, last);
	}

}

void IndexRebasedProxyModel::onSourceColAboutToBeInserted(QModelIndex const& sourceParent, int first, int last) {

	QModelIndex mapped = mapFromSource(sourceParent);
	if (mapped != QModelIndex() || sourceParent == _sourceTarget) {

		beginInsertColumns(mapped, first, last);
	}

}

void IndexRebasedProxyModel::onSourceRowInserted(QModelIndex const& sourceParent) {

	QModelIndex mapped = mapFromSource(sourceParent);
	if (mapped != QModelIndex() || sourceParent == _sourceTarget) {

		rebuildTreeProxy(); //TODO make it more efficient
		endInsertRows();
	}

}
void IndexRebasedProxyModel::onSourceColInserted(QModelIndex const& sourceParent) {

	QModelIndex mapped = mapFromSource(sourceParent);
	if (mapped != QModelIndex() || sourceParent == _sourceTarget) {

		rebuildTreeProxy();
		endInsertColumns();
	}

}


void IndexRebasedProxyModel::onSourceRowAboutToBeRemoved(QModelIndex const& sourceParent, int first, int last) {

	QModelIndex mapped = mapFromSource(sourceParent);
	if (mapped != QModelIndex() || sourceParent == _sourceTarget) {

		beginRemoveRows(mapped, first, last);
	} else {
		checkSourceTarget();
	}
}

void IndexRebasedProxyModel::onSourceColAboutToBeRemoved(QModelIndex const& sourceParent, int first, int last) {
	QModelIndex mapped = mapFromSource(sourceParent);
	if (mapped != QModelIndex() || sourceParent == _sourceTarget) {

		beginRemoveColumns(mapped, first, last);
	} else {
		checkSourceTarget();
	}
}

void IndexRebasedProxyModel::onSourceRowRemoved(QModelIndex const& sourceParent) {

	QModelIndex mapped = mapFromSource(sourceParent);
	if (mapped != QModelIndex() || sourceParent == _sourceTarget) {

		rebuildTreeProxy();
		endRemoveRows();
	} else {
		checkSourceTarget();
	}

}

void IndexRebasedProxyModel::onSourceColRemoved(QModelIndex const& sourceParent) {

	QModelIndex mapped = mapFromSource(sourceParent);
	if (mapped != QModelIndex() || sourceParent == _sourceTarget) {

		rebuildTreeProxy();
		endRemoveColumns();
	} else {
		checkSourceTarget();
	}

}

void IndexRebasedProxyModel::onSourceRowAboutToBeMoved(QModelIndex const& parent, int start, int end, QModelIndex const& destination, int row) {

	QModelIndex mappedParent = mapFromSource(parent);
	QModelIndex mappedDestination = mapFromSource(destination);

	bool startedInside = false;
	bool endInside = false;

	if (mappedParent != QModelIndex() || parent == _sourceTarget) {
		startedInside = true;
	}

	if (mappedDestination != QModelIndex() || destination == _sourceTarget) {
		endInside = true;
	}

	if (startedInside && endInside) {
		beginMoveRows(mappedParent, start, end, mappedDestination, row);
	} else if (startedInside) {
		beginRemoveRows(mappedParent, start, end);
	} else if (endInside) {
		beginInsertRows(mappedDestination, row, row + end - start);
	}

}

void IndexRebasedProxyModel::onSourceColAboutToBeMoved(QModelIndex const& parent, int start, int end, QModelIndex const& destination, int col) {

	QModelIndex mappedParent = mapFromSource(parent);
	QModelIndex mappedDestination = mapFromSource(destination);

	bool startedInside = false;
	bool endInside = false;

	if (mappedParent != QModelIndex() || parent == _sourceTarget) {
		startedInside = true;
	}

	if (mappedDestination != QModelIndex() || destination == _sourceTarget) {
		endInside = true;
	}

	if (startedInside && endInside) {
		beginMoveColumns(mappedParent, start, end, mappedDestination, col);
	} else if (startedInside) {
		beginRemoveColumns(mappedParent, start, end);
	} else if (endInside) {
		beginInsertColumns(mappedDestination, col, col + end - start);
	}

}

void IndexRebasedProxyModel::onSourceRowMoved(QModelIndex const& parent, int start, int end,  QModelIndex const& destination) {

	Q_UNUSED(start)
	Q_UNUSED(end)

	QModelIndex mappedParent = mapFromSource(parent);
	QModelIndex mappedDestination = mapFromSource(destination);

	bool startedInside = false;
	bool endInside = false;

	if (mappedParent != QModelIndex() || parent == _sourceTarget) {
		startedInside = true;
	}

	if (mappedDestination != QModelIndex() || destination == _sourceTarget) {
		endInside = true;
	}

	if (startedInside && endInside) {
		rebuildTreeProxy();
		endMoveRows();
	} else if (startedInside) {
		rebuildTreeProxy();
		endRemoveRows();
	} else if (endInside) {
		rebuildTreeProxy();
		endInsertRows();
	}

}

void IndexRebasedProxyModel::onSourceColMoved(QModelIndex const& parent, int start, int end, QModelIndex const& destination) {

	Q_UNUSED(start)
	Q_UNUSED(end)

	QModelIndex mappedParent = mapFromSource(parent);
	QModelIndex mappedDestination = mapFromSource(destination);

	bool startedInside = false;
	bool endInside = false;

	if (mappedParent != QModelIndex() || parent == _sourceTarget) {
		startedInside = true;
	}

	if (mappedDestination != QModelIndex() || destination == _sourceTarget) {
		endInside = true;
	}

	if (startedInside && endInside) {
		rebuildTreeProxy();
		endMoveColumns();
	} else if (startedInside) {
		rebuildTreeProxy();
		endRemoveColumns();
	} else if (endInside) {
		rebuildTreeProxy();
		endInsertColumns();
	}

}

void IndexRebasedProxyModel::onSourceModelDataChanged(QModelIndex const& topLeft, QModelIndex const& bottomRight, QVector<int> const& roles) {

	QModelIndex mappedTopLeft = mapFromSource(topLeft);
	QModelIndex mappedBottomRight = mapFromSource(bottomRight);

	if (mappedTopLeft.parent() == mappedBottomRight.parent()) {
		if (mappedTopLeft != QModelIndex() && mappedBottomRight != QModelIndex()) {
			Q_EMIT dataChanged(mappedTopLeft, mappedBottomRight, roles);
		}
	}

}

void IndexRebasedProxyModel::onSourceModelAboutToReset() {
	beginResetModel();
}

void IndexRebasedProxyModel::reset() {

	checkSourceTarget();
	rebuildTreeProxy();
	endResetModel();

}

IndexRebasedProxyModel::Node* IndexRebasedProxyModel::setupNodeForIndex (Node* parent, QModelIndex const& index) {

	Node* node = new Node(parent, index.row(), index.column());

	int nRows = _sourceModel->rowCount(index);
	int nCols = _sourceModel->columnCount(index);

	if (nRows == 0 or nCols == 0) {
		node->childrens = {{}};
		return node;
	}

	node->childrens = QVector<QVector<Node*>>(nRows);

	for (int i = 0; i < nRows; i++) {
		node->childrens[i] = QVector<Node*>(nCols);

		for (int j = 0; j < nCols; j++) {
			QModelIndex cIndex = _sourceModel->index(i,j,index);

			Node* child = setupNodeForIndex(node, cIndex);
			node->childrens[i][j] = child;
		}
	}

	return node;
};

void IndexRebasedProxyModel::rebuildTreeProxy() {

	if (_proxyNodeTree != nullptr) {
		delete _proxyNodeTree;
	}

	_proxyNodeTree = setupNodeForIndex(nullptr, _sourceTarget);

}
