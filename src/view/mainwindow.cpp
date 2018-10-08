#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "editor.h"
#include "editorfactorymanager.h"
#include "model/editableitem.h"
#include "model/editableitemmanager.h"
#include "model/editableitemfactory.h"

#include <QTabWidget>
#include <QDockWidget>

namespace Aline {

const QString MainWindow::MENU_FILE_NAME = "file_menu";
const QString MainWindow::MENU_DISPLAY_NAME = "display_menu";

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	_currentProject(nullptr)
{
	ui->setupUi(this);

	//menu

	QMenu* fileMenu = menuBar()->addMenu(tr("fichier"));
	fileMenu->setObjectName(MENU_FILE_NAME);

	QMenu* viewMenu = menuBar()->addMenu(tr("affichage"));
	viewMenu->setObjectName(MENU_DISPLAY_NAME);

	_submenuDock = viewMenu->addMenu(tr("docks"));

	connect(ui->tabWidget, &QTabWidget::tabCloseRequested,
			this, static_cast<void (MainWindow::*)(int)>(&MainWindow::closeEditor));
}

MainWindow::~MainWindow()
{
	delete ui;
}

Editor* MainWindow::editorAt(int index) {

	QWidget* widget = ui->tabWidget->widget(index);

	Editor* editor = qobject_cast<Editor*>(widget);

	return editor;

}


EditableItemManager *MainWindow::currentProject() const
{
	return _currentProject;
}

void MainWindow::setCurrentProject(EditableItemManager *currentProject)
{
	if (currentProject != _currentProject) {
		_currentProject = currentProject;
		emit currentProjectChanged(_currentProject);
	}
}

QMenu* MainWindow::findMenuByName(QString const& name, bool createIfNotExist) {

	QMenu* menu = menuBar()->findChild<QMenu*>(name, Qt::FindDirectChildrenOnly);

	if (menu != nullptr) {
		return menu;
	}

	if (createIfNotExist) {
		return menuBar()->addMenu(name);
	}

	return nullptr;

}

void MainWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget * dockwidget) {

	_submenuDock->addAction(dockwidget->toggleViewAction());

	QMainWindow::addDockWidget(area, dockwidget);

}

void MainWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget * dockwidget, Qt::Orientation orientation) {

	_submenuDock->addAction(dockwidget->toggleViewAction());

	QMainWindow::addDockWidget(area, dockwidget, orientation);

}

void MainWindow::addEditor(Editor* editor) {
	ui->tabWidget->addTab(editor, editor->title());

	connect(editor, &Editor::titleChanged, this, &MainWindow::updateTitle);

	switchToEditor(editor);
}

void MainWindow::switchToEditor(Editor* editor) {
	int index = ui->tabWidget->indexOf(editor);

	if (index >= 0) {
		ui->tabWidget->setCurrentIndex(index);
	}
}

void MainWindow::closeEditor(int index) {

	QWidget* widg = ui->tabWidget->widget(index);

	Editor* editor = qobject_cast<Editor*>(widg);

	if (editor) {
		disconnect(editor, &Editor::titleChanged, this, &MainWindow::updateTitle);
	}

	emit editorAboutToBeRemoved(editor);

	if (_openedEditors.values().contains(editor)) {

		QString key = _openedEditors.key(editor);

		_openedEditors.remove(key);

	}

	ui->tabWidget->removeTab(index);
}

void MainWindow::closeEditor(Editor* editor) {

	disconnect(editor, &Editor::titleChanged, this, &MainWindow::updateTitle);

	emit editorAboutToBeRemoved(editor);

	if (_openedEditors.values().contains(editor)) {

		QString key = _openedEditors.key(editor);

		_openedEditors.remove(key);

	}

	ui->tabWidget->removeTab(ui->tabWidget->indexOf(editor));

}

void MainWindow::saveCurrentEditor() {

	Editor* editor = qobject_cast<Editor*>(ui->tabWidget->currentWidget());

	if (editor != nullptr) {
		editor->saveAction();
	}

}

void MainWindow::editItem(QString const& itemRef) {

	if (_openedEditors.contains(itemRef)) {
		switchToEditor(_openedEditors.value(itemRef));
		return;
	}

	EditableItem* item = _currentProject->loadItem(itemRef);

	Editor* editor = EditorFactoryManager::GlobalEditorFactoryManager.createItemForEditableItem(item, this);

	if (editor != nullptr) {
		addEditor(editor);
	}

	_openedEditors.insert(itemRef, editor);

}

void MainWindow::saveAll() {

	if (_currentProject != nullptr) {
		_currentProject->saveAll();
	}

}


void MainWindow::updateTitle(Editor* editor, QString newTitle) {

	int index = ui->tabWidget->indexOf(editor);

	ui->tabWidget->setTabText(index, newTitle);

}

} // namespace Aline
