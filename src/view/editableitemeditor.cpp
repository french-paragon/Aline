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

#include "editableitemeditor.h"

#include "model/editableitem.h"

namespace Aline {

EditableItemEditor::EditableItemEditor(QWidget *parent) : Editor(parent)
{

}

void EditableItemEditor::setEditedItem(EditableItem* item) {

	if (effectivelySetEditedItem(item)) {

		blockSignals(true);
		setTitle(item->objectName());
		blockSignals(false);
		setHasUnsavedChanges(item->hasUnsavedChanges());

		connect(item, &EditableItem::objectNameChanged, this, &EditableItemEditor::setTitle);
		connect(item, &EditableItem::unsavedStateChanged, this, &EditableItemEditor::setHasUnsavedChanges);
		connect(item, &QObject::destroyed,
				this, &QObject::deleteLater);

		Q_EMIT editedItemChanged(item);
		_editedItem = item;
	}

}

void EditableItemEditor::saveAction() {
	if (_editedItem != nullptr) {
		_editedItem->save();
	}
}

QString EditableItemEditor::getEditedItemRef() const {
	return _editedItem->getRef();
}
QString EditableItemEditor::getEditedItemUrl() const {
	return _editedItem->getFullRefUrl();
}

QString EditableItemEditor::getEditorNoDuplicateClue() const {
	return getEditedItemUrl();
}

} // namespace Aline
