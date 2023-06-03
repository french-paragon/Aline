#ifndef ALINE_EDITABLEITEMACTIONSMANAGER_H
#define ALINE_EDITABLEITEMACTIONSMANAGER_H

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

#include <QObject>
#include <QList>
#include <QModelIndexList>

#include "../aline_global.h"

class QAction;

namespace Aline {

class EditableItemManager;
class EditableItem;

/*!
 * \brief The EditableItemActionsManager class contains methods to get a list of actions to, notably, populate menu
 */
class ALINE_EXPORT EditableItemActionsManager : public QObject
{
	Q_OBJECT
public:
	EditableItemActionsManager(QObject* parent = nullptr);

	virtual QList<QAction*> factorizeClassContextActions(QObject* parent, EditableItemManager* manager, QString editableItemType) const;
	virtual QList<QAction*> factorizeItemContextActions(QObject* parent, EditableItem* p) const;
	virtual QList<QAction*> factorizeMultiItemsContextActions(QObject* parent, EditableItemManager* manager, QModelIndexList const& projectIndices) const;
};

} // namespace Aline

#endif // ALINE_EDITABLEITEMACTIONSMANAGER_H
