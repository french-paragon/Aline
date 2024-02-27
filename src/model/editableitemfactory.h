#ifndef ALINE_EDITABLEITEMFACTORY_H
#define ALINE_EDITABLEITEMFACTORY_H

/*This file is part of the project Aline

Copyright (C) 2022-2023 Paragon <french.paragon@gmail.com>

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
#include <QMap>
#include <QAbstractListModel>

#include "../aline_global.h"

namespace Aline {

class EditableItem;
class EditableItemManager;

class ALINE_EXPORT EditableItemFactory : public QObject
{
	Q_OBJECT
public:
	explicit EditableItemFactory(QObject *parent = nullptr);

	QString getItemTypeId() const;
	QString getItemTypeName() const;
	QString getItemTypeIconUrl() const;
	virtual EditableItem* createItem(QString ref, EditableItemManager* parent) const = 0;

	/*!
	 * \brief isSingleton indicate if the editable item should be considered a singleton in the project
	 * \return true if the item should be considered a singleton
	 *
	 * Singleton are identified by their classname rather than an id, and only one should exist per project.
	 *
	 * By default, items are not considered as singleton.
	 */
	virtual bool isSingleton() const;

Q_SIGNALS:

public Q_SLOTS:

protected:

	mutable QString _typeId;
	mutable QString _typeName;
	mutable QString _typeIconUrl;
};

class ALINE_EXPORT EditableSubItemFactory : public QObject
{
	Q_OBJECT
public:
	explicit EditableSubItemFactory(QObject *parent = nullptr);

	QString getItemTypeId() const;
	QString getItemTypeName() const;
	QString getItemTypeIconUrl() const;
	virtual EditableItem* createItem(QString ref, EditableItem* parent) const = 0;

Q_SIGNALS:

public Q_SLOTS:

protected:

	mutable QString _typeId;
	mutable QString _typeName;
	mutable QString _typeIconUrl;
};

class ALINE_EXPORT EditableItemFactoryManager : public QAbstractListModel
{
	Q_OBJECT
public:

	enum InternalDataRole{
		ItemRefRole = Qt::UserRole + 1,
		IsSingletonRole = Qt::UserRole + 2
	};

	explicit EditableItemFactoryManager(QObject *parent = nullptr);

	void installFactory(EditableItemFactory* factory, bool takeOwnership = true);
	void installSubItemFactory(EditableSubItemFactory* factory, bool takeOwnership = true);

	bool hasFactoryInstalled(QString type_id) const;
	bool hasSubItemFactoryInstalled(QString type_id) const;
	EditableItem* createItem(QString type_id, QString item_id, EditableItemManager* parent) const;
	EditableItem* createSubItem(QString type_id, QString item_id, EditableItem* parent) const;

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	QString itemIconUrl(QString type_ref);
	QString itemTypeName(QString type_ref);
	bool itemTypeIsSingleton(QString type_ref);

	QVector<QString> const& installedFactoriesKeys() const;

protected:

	QMap<QString, EditableItemFactory*> _installedItemFactories;
	QMap<QString, EditableSubItemFactory*> _installedSubItemFactories;
	QVector<QString> _installedFactoriesKeys;
	QVector<QString> _installedSubFactoriesKeys;
};

template<class T, bool singleton>
class EditableItemTemplateFactory : public EditableItemFactory {

public:
	explicit EditableItemTemplateFactory(QObject *parent = nullptr) :
		EditableItemFactory(parent) {

	}

	virtual EditableItem* createItem(QString ref, EditableItemManager* parent) const override {
		return new T(ref, parent);
	}

	virtual bool isSingleton() const override {
		return singleton;
	}

};

template<class T>
class EditableSubItemTemplateFactory : public EditableSubItemFactory {

public:
	explicit EditableSubItemTemplateFactory(QObject *parent = nullptr) :
		EditableSubItemFactory(parent) {

	}

	virtual EditableItem* createItem(QString ref, EditableItem* parent) const override {
		return new T(ref, parent);
	}

};

} // namespace Aline

#endif // EDITABLEITEMFACTORY_H
