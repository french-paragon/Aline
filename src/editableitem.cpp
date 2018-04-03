#include "editableitem.h"

namespace Aline {

EditableItem::EditableItem(QObject *parent) : QObject(parent)
{

}

bool EditableItem::autoSave() const {
	return false;
}

} // namespace Aline
