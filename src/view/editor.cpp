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

#include "editor.h"

#include "mainwindow.h"
#include "editorfactorymanager.h"

#include "../model/editableitemmanager.h"

#include <QStandardPaths>
#include <QSettings>
#include <QDir>

namespace Aline {

const char* EditorPersistentStateSaveInterface::EditorPersistentStateSaveInterfaceCode = "Aline_AppInterfaces_EditorPersistentStatesSave";

Editor::Editor(QWidget *parent) :
	QWidget(parent),
	_saveState(false),
	_title("")
{

}

void Editor::setTitle(QString title) {
	_title = title;
	updateEffectiveTitle();
}

void Editor::setHasUnsavedChanges(bool saveState) {
	_saveState = saveState;
	updateEffectiveTitle();
}

void Editor::updateEffectiveTitle() {
	Q_EMIT titleChanged(this, _title + ((_saveState) ? " *" : ""));
}

QString Editor::title() const
{
	return _title;
}

QStringList Editor::editableTypes() const {
	return QStringList();
}

void Editor::setEditedItem(EditableItem* item) {
	Q_UNUSED(item);
}

MainWindow* Editor::getEditorMainWindow() const {
	QWidget* pItem = parentWidget();
	MainWindow* mwParent = qobject_cast<MainWindow*>(pItem);

	while (mwParent == nullptr) {

		if (pItem == nullptr) {
			return nullptr;
		}

		pItem = pItem->parentWidget();
		mwParent = qobject_cast<MainWindow*>(pItem);
	}

	return mwParent;
}

QList<QAction*> Editor::getContextActions() {
	return QList<QAction*>();
}

bool Editor::isStateRestorable() const {
	return false;
}


bool Editor::restoreFromState(QString const& stateRep) {
	return false;
}


QString Editor::getEditorState() const {
	return "";
}

QString Editor::getEditorNoDuplicateClue() const {
	return "";
}

EditorPersistentStateSaveInterface::EditorPersistentStateSaveInterface(QObject *parent) :
	QObject(parent)
{

}

void EditorPersistentStateSaveInterface::saveEditorStates(MainWindow* mw) {

	if (mw == nullptr) {
		return;
	}

	EditableItemManager* manager = mw->currentProject();
	EditorFactoryManager* editorsManager = mw->editorManager();

	if (manager == nullptr) {
		return;
	}

	if (editorsManager == nullptr) {
		return;
	}

	QString projectId = manager->localProjectId();

	if (projectId.isEmpty()) {
		return;
	}

	QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

	QDir configDir(configPath);

	if (!configDir.exists()) {
		bool ok = configDir.mkpath(".");

		if (!ok) {
			return;
		}
	}

	QSettings editorsStates(configDir.absoluteFilePath("editorsStates.ini"),QSettings::Format::IniFormat);


	editorsStates.beginWriteArray(projectId);

	int arrayIndex = 0;

	for (int i = 0; i < mw->nOpenedEditors(); i++) {

		Editor* editor = mw->editorAt(i);

		if (editor == nullptr) {
			continue;
		}

		if (!editor->isStateRestorable()) {
			continue;
		}

		editorsStates.setArrayIndex(arrayIndex);
		arrayIndex++;

		QString editorType = editor->getTypeId();
		QString stateInfos = editor->getEditorState();

		editorsStates.setValue("editorType", editorType);
		editorsStates.setValue("editorState", stateInfos);
	}

	editorsStates.endArray();

}
void EditorPersistentStateSaveInterface::restoreEditorStates(MainWindow* mw) {

	if (mw == nullptr) {
		return;
	}

	// close all active editors before restoring the states.
	for (int i = mw->nOpenedEditors()-1; i >= 0; i--) {
		mw->closeEditor(i);
	}

	EditableItemManager* manager = mw->currentProject();
	EditorFactoryManager* editorsManager = mw->editorManager();

	if (manager == nullptr) {
		return;
	}

	if (editorsManager == nullptr) {
		return;
	}

	QString projectId = manager->localProjectId();

	if (projectId.isEmpty()) {
		return;
	}

	QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

	QDir configDir(configPath);

	if (!configDir.exists()) {
		bool ok = configDir.mkpath(".");

		if (!ok) {
			return;
		}
	}

	QSettings editorsStates(configDir.absoluteFilePath("editorsStates.ini"),QSettings::Format::IniFormat);


	int size = editorsStates.beginReadArray(projectId);

	for (int i = 0; i < size; i++) {
		editorsStates.setArrayIndex(i);

		QVariant editorType = editorsStates.value("editorType", QVariant());
		QVariant editorState = editorsStates.value("editorState", QVariant());

		if (!editorType.isValid() or !editorState.isValid()) {
			continue;
		}

		QString type = editorType.toString();
		QString data = editorState.toString();

		if (!editorsManager->hasFactoryInstalled(type)) {
			continue;
		}

		Editor* editor = editorsManager->createItem(type, mw);

		if (editor == nullptr) {
			continue;
		}

		if (!editor->isStateRestorable()) {
			continue;
		}

		bool ok = editor->restoreFromState(data);

		if (ok) {
			ok = mw->addEditor(editor);
			if (!ok) {
				delete editor;
			}
		} else {
			delete editor;
		}
	}

	editorsStates.endArray();

}

} // namespace Aline
