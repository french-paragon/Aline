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

#include "referingeditableitem.h"

#include "model/editableitemmanager.h"

namespace Aline {
namespace Tests {

const char* ReferingEditableItem::TypeId = "ReferingEditableItem";

ReferingEditableItem::ReferingEditableItem(QString ref, EditableItemManager *parent) :
	EditableItem(ref, parent),
	_referedItemRef("")
{

}

ReferingEditableItem::ReferingEditableItem(QString ref, EditableItem *parent) :
	EditableItem(ref, parent),
	_referedItemRef("")
{

}

QString ReferingEditableItem::getTypeId() const {
	return "ReferingEditableItem";
}
QString ReferingEditableItem::getTypeName() const {
	return tr("ReferingEditableItem");
}

QString ReferingEditableItem::iconInternalUrl() const {
	return "";
}

const QString &ReferingEditableItem::referedItemRef() const
{
	return _referedItemRef;
}

void ReferingEditableItem::setReferedItemRef(const QString &newReferedItemRef)
{
	if (getManager() == nullptr) {
		return;
	}

	//disconnect
	EditableItem* item = getManager()->loadItemByUrl(_referedItemRef);

	if (item != nullptr) {
		item->warnUnrefering(getFullRefUrl());
	}

	_referedItemRef.clear();

	//connect
	item = getManager()->loadItemByUrl(newReferedItemRef);

	if (item != nullptr) {
		item->warnRefering(getFullRefUrl());
	}

	_referedItemRef = newReferedItemRef;

	if (item == nullptr) {
		_referedItemRef.clear();
	}
}

void ReferingEditableItem::warnReferedRefChanges(QString oldRef, QString newRef) {

	if (oldRef == _referedItemRef) {
		_referedItemRef = newRef;
	}

}
void ReferingEditableItem::warnReferedRemoved(QString ref) {

	if (ref == _referedItemRef) {
		_referedItemRef.clear();
	}

}

ReferingEditableItemFactory::ReferingEditableItemFactory(QObject *parent) :
	EditableItemFactory(parent)
{

}
EditableItem* ReferingEditableItemFactory::createItem(QString ref, EditableItemManager* parent) const {
	return new ReferingEditableItem(ref, parent);
}

} // namespace Tests
} // namespace Aline
