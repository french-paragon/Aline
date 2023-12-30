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

#include <QPointF>
#include <QSizeF>
#include <QColor>
#include <QString>

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

class DummyEnrichedEditableItem : public EditableItem
{
	Q_OBJECT
public:

	enum DummyEnum { Test, Foo, Bar };
	Q_ENUM(DummyEnum)

	Q_PROPERTY(int intData MEMBER _intData NOTIFY intDataChanged);
	Q_PROPERTY(QString stringData MEMBER _stringData NOTIFY stringDataChanged);
	Q_PROPERTY(QPointF pointFData MEMBER _pointFData NOTIFY pointDataChanged);
	Q_PROPERTY(QSizeF sizeData MEMBER _sizeData NOTIFY sizeDataChanged);
	Q_PROPERTY(QColor colorData MEMBER _colorData NOTIFY colorDataChanged);
	Q_PROPERTY(Aline::Tests::DummyEnrichedEditableItem::DummyEnum enumData MEMBER _enumData NOTIFY enumDataChanged);

	static const QString TypeId;

	explicit DummyEnrichedEditableItem(QString ref, EditableItemManager *parent = nullptr);
	explicit DummyEnrichedEditableItem(QString ref, EditableItem *parent = nullptr);

	virtual QString getTypeId() const override;
	virtual QString getTypeName() const override;

	virtual QString iconInternalUrl() const override;

	int _intData;
	QString _stringData;
	QPointF _pointFData;
	QSizeF _sizeData;
	QColor _colorData;
	DummyEnum _enumData;

Q_SIGNALS:

	void intDataChanged();
	void stringDataChanged();
	void pointDataChanged();
	void sizeDataChanged();
	void colorDataChanged();
	void enumDataChanged();

};

class DummyEnrichedEditableItemFactory : public EditableItemFactory {
	Q_OBJECT
public:
	DummyEnrichedEditableItemFactory(QObject *parent = nullptr);
	virtual EditableItem* createItem(QString ref, EditableItemManager* parent) const override;
};

} // namespace Tests
} // namespace Aline

Q_DECLARE_METATYPE(Aline::Tests::DummyEnrichedEditableItem::DummyEnum);

#endif // ALINE_TESTS_DUMMYEDITABLEITEM_H
