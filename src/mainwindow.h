#ifndef ALINE_MAINWINDOW_H
#define ALINE_MAINWINDOW_H

#include "aline_global.h"

#include <QMainWindow>

namespace Aline {

class Editor;

namespace Ui {
class MainWindow;
}

class ALINE_EXPORT MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

signals:

public slots:

	void addEditor(Editor* editor);

	void closeEditor(int index);
	void closeEditor(Editor* editor);

protected:

	void updateTitle(Editor* editor, QString newTitle);

private:
	Ui::MainWindow *ui;
};


} // namespace Aline
#endif // ALINE_MAINWINDOW_H
