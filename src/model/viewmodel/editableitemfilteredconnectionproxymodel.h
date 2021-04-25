#ifndef EDITABLEITEMFILTEREDCONNECTIONPROXYMODEL_H
#define EDITABLEITEMFILTEREDCONNECTIONPROXYMODEL_H

#include <QSortFilterProxyModel>

namespace Aline {

class EditableItem;

class EditableItemFilteredConnectionProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
public:

	Q_PROPERTY(int refRole READ refRole CONSTANT)

	explicit EditableItemFilteredConnectionProxyModel(EditableItem* item, QString propName, int refRole, QObject* parent = nullptr);
	~EditableItemFilteredConnectionProxyModel();

	int refRole() const;

Q_SIGNALS:

public Q_SLOTS:

	void checkForbidenItemRefs();

protected:
	virtual bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;

	void setForbidenItemRefs(const QStringList &forbidenItemRefs);

	EditableItem* _trackedItem;

	char* _watchedPropertyName;
	int _watchedPropertyIndex;

	QStringList _forbidenItemRefs;

	QMetaObject::Connection _connection;

	int _refRole;
};

}

#endif // EDITABLEITEMFILTEREDCONNECTIONPROXYMODEL_H
