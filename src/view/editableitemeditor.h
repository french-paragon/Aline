#ifndef EDITABLEITEMEDITOR_H
#define EDITABLEITEMEDITOR_H

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

	QString getEditedItemRef() const;
	QString getEditedItemUrl() const;

Q_SIGNALS:

	void editedItemChanged(EditableItem* new_edited_item);

public Q_SLOTS:

protected:

	virtual bool effectivelySetEditedItem(EditableItem* item) = 0;

	EditableItem* _editedItem;
};

} // namespace Aline

#endif // EDITABLEITEMEDITOR_H
