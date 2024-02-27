#ifndef EDITORFACTORY_H
#define EDITORFACTORY_H

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

#include "../aline_global.h"

#include <QObject>
#include <QAbstractListModel>

namespace Aline {

class Editor;

class ALINE_EXPORT EditorFactory : public QObject
{
	Q_OBJECT
public:
	explicit EditorFactory(QObject *parent = nullptr);

	QString getItemTypeId() const;
	QString getItemTypeName() const;
	QStringList editableTypes() const;
	virtual Editor* createItem(QWidget* parent) const = 0;

Q_SIGNALS:

public Q_SLOTS:

protected:

	mutable QString _typeId;
	mutable QString _typeName;
	mutable bool _hasEditableTypesCached;
	mutable QStringList _editableTypes;

};

template<class T>
class EditorTemplateFactory : public EditorFactory {
public:
	explicit EditorTemplateFactory(QObject *parent = nullptr) :
		EditorFactory(parent)
	{

	}

	virtual Editor* createItem(QWidget* parent) const {
		return new T(parent);
	}
};

} // namespace Aline

#endif // EDITORFACTORY_H
