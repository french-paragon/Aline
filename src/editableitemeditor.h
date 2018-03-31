#ifndef EDITABLEITEMEDITOR_H
#define EDITABLEITEMEDITOR_H

#include "editor.h"

namespace Aline {

class EditableItem;

class ALINE_EXPORT EditableItemEditor : public Editor
{
	Q_OBJECT
public:
	explicit EditableItemEditor(QWidget *parent = nullptr);

	/*!
	 * \brief setEditedItem set the item to edit
	 * \param item the item to edit
	 */
	virtual void setEditedItem(EditableItem* item);

	virtual void saveAction();

signals:

	void editedItemChanged(EditableItem* new_edited_item);

public slots:

protected:

	virtual bool effectivelySetEditedItem(EditableItem* item) = 0;

	EditableItem* _editedItem;
};

} // namespace Aline

#endif // EDITABLEITEMEDITOR_H
