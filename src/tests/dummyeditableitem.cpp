/*This file is part of the project Aline

Copyright (C) 2023 Paragon <french.paragon@gmail.com>

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

#include "dummyeditableitem.h"

namespace Aline {
namespace Tests {

const QString DummyEditableItem::TypeId = "DummyEditableItem";

DummyEditableItem::DummyEditableItem(QString ref, EditableItemManager *parent) :
	EditableItem(ref, parent)
{

}

DummyEditableItem::DummyEditableItem(QString ref, EditableItem *parent) :
	EditableItem(ref, parent)
{

}

QString DummyEditableItem::getTypeId() const {
	return TypeId;
}
QString DummyEditableItem::getTypeName() const {
	return tr("DummyEditableItem");
}

QString DummyEditableItem::iconInternalUrl() const {
	return "";
}

DummyEditableItemFactory::DummyEditableItemFactory(QObject *parent) :
	EditableItemFactory(parent)
{

}

EditableItem* DummyEditableItemFactory::createItem(QString ref, EditableItemManager* parent) const {
	return new DummyEditableItem(ref, parent);
}

} // namespace Tests
} // namespace Aline
