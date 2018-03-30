#include "editableitemeditor.h"

namespace Aline {

EditableItemEditor::EditableItemEditor(QWidget *parent) : Editor(parent)
{

}

void EditableItemEditor::setEditedItem(EditableItem* item) {

	if (effectivelySetEditedItem(item)) {
		emit editedItemChanged(item);
	}

}

} // namespace Aline
