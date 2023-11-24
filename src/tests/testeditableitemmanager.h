#ifndef TESTEDITABLEITEMMANAGER_H
#define TESTEDITABLEITEMMANAGER_H

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

#include "./aline_test_global.h"

#include "../model/editableitemmanager.h"

namespace Aline {
namespace Tests {

/*!
 * \brief The TestEditableItemManager class manage created items like any other manager, but won't save them, instead it will just pretend it saved them. This is usefull for testing purposes, as items needs a manager to perform some tasks.
 */
class TestEditableItemManager : public EditableItemManager
{
public:
	TestEditableItemManager(QObject *parent = nullptr);

	virtual bool hasDataSource() const;

	virtual bool saveStruct();
	virtual bool saveLabels();
	virtual bool loadStruct();

	virtual bool isNetworkShared() const;

	virtual void reset();

protected:

	virtual EditableItem* effectivelyLoadItem(QString const& ref);

	virtual bool clearItemData(QString itemRef);

	virtual bool effectivelySaveItem(QString const& ref);

	virtual void effectivelyLoadLabels();
};

} // namespace Tests
} // namespace Aline

#endif // TESTEDITABLEITEMMANAGER_H
