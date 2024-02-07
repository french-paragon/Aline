#ifndef ALINE_EDITABLEITEMQMLEDITOR_H
#define ALINE_EDITABLEITEMQMLEDITOR_H

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

namespace Aline {

namespace Ui {
class EditableItemQmlEditor;
}

class EditableItemQmlEditorProxy;
class QmlEditorFactory;

class EditableItemQmlEditor : public EditableItemEditor
{
	Q_OBJECT

public:

	static const char* GENERIC_QMLBASED_EDITOR_TYPE;

	friend class QmlEditorFactory;

	explicit EditableItemQmlEditor(QWidget *parent = nullptr);
	explicit EditableItemQmlEditor(QUrl const& source, QWidget *parent = nullptr);
	~EditableItemQmlEditor();

	virtual QString getTypeId() const;
	virtual QString getTypeName() const;

	virtual QStringList editableTypes() const;

	void setQmlSource(QUrl const& source);

private:
	Ui::EditableItemQmlEditor *ui;

protected:

	virtual bool effectivelySetEditedItem(EditableItem* item);

	EditableItemQmlEditorProxy* _proxy;

	QString _shadowEditorType;
	QString _shadowEditorName;
	QStringList _editableTypes;

private:

	static int registerEditableItemCode;
};

class EditableItemQmlEditorProxy : public QObject {

	Q_OBJECT

public:
	explicit EditableItemQmlEditorProxy(EditableItemQmlEditor* parent);
	~EditableItemQmlEditorProxy();

	Q_PROPERTY(Aline::EditableItem* editedItem READ editedItem NOTIFY editedItemChanged)

	bool setEditedItem(EditableItem *editedItem);

	EditableItem* editedItem() const;

Q_SIGNALS:

	void editedItemChanged();

protected:

	EditableItem* _editedItem;

};


} // namespace Aline
#endif // ALINE_EDITABLEITEMQMLEDITOR_H
