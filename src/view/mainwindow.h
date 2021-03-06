#ifndef ALINE_MAINWINDOW_H
#define ALINE_MAINWINDOW_H

#include "../aline_global.h"

#include <QMainWindow>
#include <QMap>
#include <functional>

namespace Aline {

class Editor;
class EditableItemManager;
class EditableItemManagerFactory;
class EditorFactoryManager;

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

	bool isEditingAnItem() const;

	void registerCreateProjectFunction(QString const& fName,
									   QString const& projectType,
									   QString const& longDescr,
									   std::function<EditableItemManager*(QObject*, MainWindow*)> const& func);

	void registerOpenProjectFunction(QString const& fName,
									 QString const& projectType,
									 QString const& longDescr,
									 std::function<EditableItemManager*(QObject*, MainWindow*)> const& func);

	void setDefaultProjectCreator(const QString &defaultProjectCreator);

	QString defaultProjectCreator() const;

	QString defaultProjectOpener() const;
	void setDefaultProjectOpener(const QString &defaultProjectOpener);

	void setEditorFactoryManager(EditorFactoryManager *editorFactoryManager);

Q_SIGNALS:

	void editorAboutToBeRemoved(Editor* editor);
	void currentProjectChanged(EditableItemManager*);
	void projectLoaded(EditableItemManager*);

	void editedItemChanged(QString ref);

public Q_SLOTS:

	void addEditor(Editor* editor);

	void switchToEditor(Editor* editor);

	void closeEditor(int index);
	void closeEditor(Editor* editor);

	void saveCurrentEditor();

	void editItem(QString const& itemRef);
	void saveAll();

protected:

	void setWindowProjectFromFunc(const QString &fName);

	void updateTitle(Editor* editor, QString newTitle);

	void onCurrentEditorChanged();

	EditableItemManager* _currentProject;

	QMap<QString, Aline::Editor*> _openedEditors;

	QMenu* _submenuDock;

	void registerProjectFunction(QString const& fName,
								 QString const& projectType,
								 QString const& longDescr,
								 std::function<EditableItemManager*(QObject*, MainWindow*)> const& func,
								 QMenu* target);

	QMenu* _submenuCreateProject;
	QMenu* _submenuOpenProject;
	QString _defaultProjectCreator;
	QString _defaultProjectOpener;
	EditableItemManagerFactory* _editableItemManagerFactory;
	EditorFactoryManager* _editorFactoryManager;

private:
	Ui::MainWindow *ui;
};


} // namespace Aline
#endif // ALINE_MAINWINDOW_H
