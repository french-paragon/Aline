#ifndef ALINE_TESTS_REFERINGEDITABLEITEM_H
#define ALINE_TESTS_REFERINGEDITABLEITEM_H

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

#include <QString>

namespace Aline {
namespace Tests {

/*!
 * \brief The ReferingEditableItem class represent a single reference to another editable item.
 *
 * This item is meant to be used for testing purposes
 */
class ReferingEditableItem : public EditableItem
{
	Q_OBJECT
public:

	static const char* TypeId;

	explicit ReferingEditableItem(QString ref, EditableItemManager *parent = nullptr);
	explicit ReferingEditableItem(QString ref, EditableItem *parent = nullptr);

	virtual QString getTypeId() const override;
	virtual QString getTypeName() const override;

	virtual QString iconInternalUrl() const override;

	const QString &referedItemRef() const;
	void setReferedItemRef(const QString &newReferedItem);

protected:

	virtual void warnReferedRefChanges(QString oldRef, QString newRef) override;
	virtual void warnReferedRemoved(QString ref) override;

	QString _referedItemRef;
};

class ReferingEditableItemFactory : public EditableItemFactory {
	Q_OBJECT
public:
	ReferingEditableItemFactory(QObject *parent = nullptr);
	virtual EditableItem* createItem(QString ref, EditableItemManager* parent) const override;
};

} // namespace Tests
} // namespace Aline

#endif // ALINE_TESTS_REFERINGEDITABLEITEM_H
