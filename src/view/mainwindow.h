#ifndef ALINE_MAINWINDOW_H
#define ALINE_MAINWINDOW_H

#include "../aline_global.h"

#include <QMainWindow>
#include <QMap>

namespace Aline {

class Editor;
class EditableItemManager;

namespace Ui {
class MainWindow;
}

class ALINE_EXPORT MainWindow : public QMainWindow
{
	Q_OBJECT

public:

	static const QString MENU_FILE_NAME;
	static const QString MENU_DISPLAY_NAME;

	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	Editor* editorAt(int index);

	EditableItemManager *currentProject() const;
	void setCurrentProject(EditableItemManager *currentProject);

	QMenu* findMenuByName(QString const& name, bool createIfNotExist = false);

	void addDockWidget(Qt::DockWidgetArea area, QDockWidget * dockwidget);
	void addDockWidget(Qt::DockWidgetArea area, QDockWidget * dockwidget, Qt::Orientation orientation);

signals:

	void editorAboutToBeRemoved(Editor* editor);
	void currentProjectChanged(EditableItemManager*);

public slots:

	void addEditor(Editor* editor);

	void switchToEditor(Editor* editor);

	void closeEditor(int index);
	void closeEditor(Editor* editor);

	void saveCurrentEditor();

	void editItem(QString const& itemRef);
	void saveAll();

protected:

	void updateTitle(Editor* editor, QString newTitle);

	EditableItemManager* _currentProject;

	QMap<QString, Aline::Editor*> _openedEditors;

	QMenu* _submenuDock;

private:
	Ui::MainWindow *ui;
};


} // namespace Aline
#endif // ALINE_MAINWINDOW_H
