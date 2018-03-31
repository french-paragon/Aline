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
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::addEditor(Editor* editor) {
	ui->tabWidget->addTab(editor, editor->title());
	connect(editor, &Editor::titleChanged, this, &MainWindow::updateTitle);
}

void MainWindow::closeEditor(int index) {

	QWidget* widg = ui->tabWidget->widget(index);

	Editor* editor = qobject_cast<Editor*>(widg);

	if (editor) {
		disconnect(editor, &Editor::titleChanged, this, &MainWindow::updateTitle);
	}

	ui->tabWidget->removeTab(index);
}

void MainWindow::closeEditor(Editor* editor) {

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
