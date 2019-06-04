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
		setHasUnsavedChanges(item->getHasUnsavedChanged());

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

} // namespace Aline
