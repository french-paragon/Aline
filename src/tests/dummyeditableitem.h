#ifndef ALINE_TESTS_DUMMYEDITABLEITEM_H
#define ALINE_TESTS_DUMMYEDITABLEITEM_H

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

#include "../model/editableitem.h"

#include "../model/editableitemfactory.h"

namespace Aline {
namespace Tests {

class DummyEditableItem : public EditableItem
{
	Q_OBJECT
public:

	static const QString TypeId;

	explicit DummyEditableItem(QString ref, EditableItemManager *parent = nullptr);
	explicit DummyEditableItem(QString ref, EditableItem *parent = nullptr);

	virtual QString getTypeId() const override;
	virtual QString getTypeName() const override;

	virtual QString iconInternalUrl() const override;
};

class DummyEditableItemFactory : public EditableItemFactory {
	Q_OBJECT
public:
	DummyEditableItemFactory(QObject *parent = nullptr);
	virtual EditableItem* createItem(QString ref, EditableItemManager* parent) const override;
};

} // namespace Tests
} // namespace Aline

#endif // ALINE_TESTS_DUMMYEDITABLEITEM_H
