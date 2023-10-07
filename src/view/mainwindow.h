#ifndef ALINE_MAINWINDOW_H
#define ALINE_MAINWINDOW_H

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

	/*!
	 * \brief findWidgetMainWindow find the main window a widget belongs to
	 * \param widget the widget
	 * \return the main windows
	 */
	static MainWindow* findWidgetMainWindow(QWidget* widget);

	static const QString MENU_FILE_NAME;
	static const QString MENU_DISPLAY_NAME;

	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	int nOpenedEditors() const;
	Editor* editorAt(int index);
	Editor* findEditorByName(QString const& name);
	QVector<Editor*> findAllEditorsOfType(QString const& type);

	void switchToEditor(int index);
	void switchToEditor(Editor* editor);

	EditableItemManager *currentProject() const;
	void setCurrentProject(EditableItemManager *currentProject);

	QString currentItemRef() const;
	void setCurrentItem(QString const& itemUrl);

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
	EditorFactoryManager* editorManager();

Q_SIGNALS:

	void editorAboutToBeRemoved(Editor* editor);
	void currentProjectChanged(EditableItemManager*);
	void projectLoaded(EditableItemManager*);

	void currentItemChanged(QString ref);
	void editedItemChanged(QString ref);

public Q_SLOTS:

	void addEditor(Editor* editor);

	void closeEditor(int index);
	void closeEditor(Editor* editor);

	void saveCurrentEditor();

	void editItem(QString const& itemUrl);
	void saveAll();

protected:

	void setWindowProjectFromFunc(const QString &fName);

	void updateTitle(Editor* editor, QString newTitle);

	void onCurrentEditorChanged();

	EditableItemManager* _currentProject;
	QString _currentItemUrl;

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
