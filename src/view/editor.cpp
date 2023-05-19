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

namespace Aline {

Editor::Editor(QWidget *parent) : QWidget(parent)
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

} // namespace Aline
