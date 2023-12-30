#ifndef EDITABLEITEMTESTMODEL_H
#define EDITABLEITEMTESTMODEL_H

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

#include "../model/editableitem.h"

#include <QObject>

namespace Aline {
namespace Tests {

class TestEditableItemManager;

class ALINE_TEST_EXPORT EditableItemTestModel : public QObject
{
	Q_OBJECT
public:

protected Q_SLOTS:

	void initTestCase();

	/*!
	 * \brief StaticPropertiesTypes test that all stored static properties have a supported type.
	 */
	void StaticPropertiesTypes();

	/*!
	 * \brief StaticPropertiesChanges test if the item reach an unsaved state when a property is changed
	 */
	void StaticPropertiesChanges();

	void cleanupTestCase();

protected:

	EditableItem* _subject;
	TestEditableItemManager* _manager;

private:

	void recursivelyTestSubItemsStaticPropertiesTypes(EditableItem* subItem);
	void recursivelyTestSubItemsPropertiesChanges(EditableItem* subItem);
	QVariant updateValue(QVariant old);

};

} // namespace Tests
} // namespace Aline

#endif // EDITABLEITEMTESTMODEL_H
