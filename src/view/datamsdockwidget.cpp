

#include "datamsdockwidget.h"
#include "ui_datamsdockwidget.h"

#include "view/mainwindow.h"
#include "../model/labels/projectfilterbylabelproxymodel.h"
#include "model/editableitemmanager.h"

namespace Aline {

DataItemsDockWidget::DataItemsDockWidget(MainWindow *parent) :
	QDockWidget(parent),
	ui(new Ui::DataItemsDockWidget),
	_mw_parent(parent)
{
	ui->setupUi(this);

	_proxy = new Aline::ProjectFilterByLabelProxyModel(ui->treeView);

	
	connect(ui->treeView, &QTreeView::doubleClicked,
			this, &DataItemsDockWidget::receiveDoubleClick);

	ui->treeView->setModel(_proxy);

	ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->treeView->setDragDropMode(QAbstractItemView::DragOnly);
	ui->treeView->setDragEnabled(true);
	ui->treeView->setDropIndicatorShown(true);

	connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
			this, &DataItemsDockWidget::selectionChanged);

}


DataItemsDockWidget::~DataItemsDockWidget()
{
	delete ui;
}


void DataItemsDockWidget::receiveDoubleClick(const QModelIndex &index) {

	QVariant data = index.data(EditableItemManager::ItemRefRole);

	if (data.isValid()) {

		Q_EMIT itemDoubleClicked(data.toString());

	}

}

void DataItemsDockWidget::selectionChanged() {

	QModelIndexList selection = ui->treeView->selectionModel()->selectedIndexes();

	if (selection.size() > 0 && !_mw_parent->isEditingAnItem()) {

		QString ref = ui->treeView->model()->data(selection.first(), EditableItemManager::ItemRefRole).toString();

		_mw_parent->currentProject()->setActiveItem(ref);
	}

}

} // namespace Sabrina
