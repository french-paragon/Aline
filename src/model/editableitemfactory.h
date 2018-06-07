#ifndef ALINE_EDITABLEITEMFACTORY_H
#define ALINE_EDITABLEITEMFACTORY_H

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

signals:

public slots:

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

signals:

public slots:

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
		ItemRefRole = Qt::UserRole + 1
	};

	static EditableItemFactoryManager GlobalEditableItemFactoryManager;

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
	QString subItemIconUrl(QString type_ref);

protected:

	QMap<QString, EditableItemFactory*> _installedItemFactories;
	QMap<QString, EditableSubItemFactory*> _installedSubItemFactories;
	QVector<QString> _installedFactoriesKeys;
	QVector<QString> _installedSubFactoriesKeys;
};

} // namespace Aline

#endif // EDITABLEITEMFACTORY_H
