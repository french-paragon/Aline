#include "editableitemeditor.h"

#include "editableitem.h"

namespace Aline {

EditableItemEditor::EditableItemEditor(QWidget *parent) : Editor(parent)
{

}

void EditableItemEditor::setEditedItem(EditableItem* item) {

	if (effectivelySetEditedItem(item)) {

		blockSignals(true);
		setTitle(item->objectName());
		blockSignals(false);
		setHasUnsavedChanges(item->getHasUnsavedChanged());

		connect(item, &EditableItem::objectNameChanged, this, &EditableItemEditor::setTitle);
		connect(item, &EditableItem::unsavedStateChanged, this, &EditableItemEditor::setHasUnsavedChanges);

		emit editedItemChanged(item);
		_editedItem = item;
	}

}

void EditableItemEditor::saveAction() {
	if (_editedItem != nullptr) {
		_editedItem->save();
	}
}

} // namespace Aline
