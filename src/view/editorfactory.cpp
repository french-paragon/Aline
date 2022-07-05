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

#include "editorfactory.h"

#include "editor.h"

namespace Aline {

EditorFactory::EditorFactory(QObject *parent) :
	QObject(parent),
	_typeId(""),
	_typeName(""),
	_hasEditableTypesCached(false)
{

}

QString EditorFactory::getItemTypeId() const {

	if (_typeId == "") {

		//get the type name.
		Editor* temp = createItem(nullptr);
		_typeId = temp->getTypeId();

		delete temp;

	}

	return _typeId;

}

QString EditorFactory::getItemTypeName() const {

	if (_typeName == "") {

		//get the type name.
		Editor* temp = createItem(nullptr);
		_typeName = temp->getTypeName();

		delete temp;

	}

	return _typeName;

}

QStringList EditorFactory::editableTypes() const {

	if (!_hasEditableTypesCached) {

		Editor* temp = createItem(nullptr);
		_hasEditableTypesCached = true;
		_editableTypes = temp->editableTypes();

		delete temp;
	}

	return _editableTypes;

}

} // namespace Aline
