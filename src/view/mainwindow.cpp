#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "editor.h"

#include <QTabWidget>

namespace Aline {

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

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
	ui->tabWidget->removeTab(index);
}

void MainWindow::closeEditor(Editor* editor) {

	emit editorAboutToBeRemoved(editor);
	ui->tabWidget->removeTab(ui->tabWidget->indexOf(editor));

}

void MainWindow::saveCurrentEditor() {

	Editor* editor = qobject_cast<Editor*>(ui->tabWidget->currentWidget());

	if (editor != nullptr) {
		editor->saveAction();
	}

}

void MainWindow::updateTitle(Editor* editor, QString newTitle) {

	int index = ui->tabWidget->indexOf(editor);

	ui->tabWidget->setTabText(index, newTitle);

}

} // namespace Aline
